#include <ESP32Servo.h>
#include <HX711.h>
#include <M5Stack.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"
#include "ble_central.hpp"

enum Pin : uint8_t {
  /* sensors */
  LoadCellDout = 23,
  LoadCellSck  = 24,
  WaterSensor1 = 2,
  WaterSensor2 = 5,
  /* actuators */
  LidWireMotor1       = 1,
  LidWireMotor2       = 3,
  RiceDeliveryServo   = 16,
  RiceWashingMotor    = 17,
  RiceWashingRodServo = 18,
  WaterDeliveryPump   = 19,
  WaterRodServo       = 21,
  WaterSuctionPump    = 22,
};

struct State{
  bool active   = false;
  bool water1   = true;
  double weight = 20;
};

class GearedMotor {
  public:
    void init(uint8_t _p1) {
      p1 = _p1;
      pinMode(p1, OUTPUT);
    }
    void init(uint8_t _p1, uint8_t _p2) {
      p1 = _p1;
      p2 = _p2;
      pinMode(p1, OUTPUT);
      pinMode(p2, OUTPUT);
    }
    void forward() { write(1, 0); }
    void reverse() { write(0, 1); }
    void stop() { write(0, 0); }
  private:
    uint8_t p1;
    uint8_t p2;
    void write(int v1, int v2) {
      digitalWrite(p1, v1);
      if(p2) digitalWrite(p2, v2);
    }
};

State state;
HX711 scale;

GearedMotor lidWireMotor;
GearedMotor riceWashingMotor;
GearedMotor waterDeliveryPump;
GearedMotor waterSuctionPump;
Servo riceDeliveryServo;
Servo riceWashingRodServo;
Servo waterRodServo;

WebClient* client;
BleCentral* lidBle;
BleCentral* buttonBle;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  /*** Sensors ***/
  pinMode(Pin::WaterSensor1, INPUT);
  pinMode(Pin::WaterSensor2, INPUT);
  scale.begin(Pin::LoadCellDout, Pin::LoadCellSck);
  scale.tare(10); // set offset
  scale.set_scale(103.5f); // set unit scale
  /*** Actuators ***/
  lidWireMotor.init(Pin::LidWireMotor1, Pin::LidWireMotor2);
  riceWashingMotor.init(Pin::RiceWashingMotor);
  waterDeliveryPump.init(Pin::WaterDeliveryPump);
  waterSuctionPump.init(Pin::WaterSuctionPump);
	riceDeliveryServo.attach(Pin::RiceDeliveryServo, 2656, 10000); // TODO: 調整
	riceWashingRodServo.attach(Pin::RiceWashingRodServo, 2656, 10000); // TODO: 調整
	waterRodServo.attach(Pin::WaterRodServo, 2656, 10000); // TODO: 調整
  for(int c = 0; c < 100; c++) { // return to home position within 3 seconds
    riceDeliveryServo.write(0);
    riceWashingRodServo.write(90);
    waterRodServo.write(90);
    delay(30);
  }
  /*** Wi-Fi and BLE Initializing ***/
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
  //lidBle = new BleCentral("12345678-9012-3456-7890-1234567890ff", "12345678-9012-3456-7890-123456789011");
  //buttonBle = new BleCentral("12345678-9012-3456-7890-1234567890aa", "12345678-9012-3456-7890-123456789022");
}

String sendPutRequest(String property, String value) {
  return client->put_request(
    "/api/cookers/0/" + property,
    String("{\"") + property + String("\": \"") + value + String("\"}")
  );
}

void sweepServo(Servo& servo, int from, int to, double speedDps = 50.0) {
  for(double angle = from; angle <= to; angle += speedDps / 50) {
    servo.write(round(angle));
    delay(20);
  }
}

void loop() {
  M5.update();
  delay(10); // for button pressing
  M5.Lcd.clear(); M5.Lcd.setCursor(0,20);

  state.weight = scale.get_units(10); // [g]
  state.water1 = digitalRead(Pin::WaterSensor1); // 0: water shortage alert

  String res = sendPutRequest("weight", String(state.weight));
  M5.Lcd.println("weight: " + String(state.weight));
  M5.Lcd.println("water1: " + String(state.water1));

  if(state.active) {
    M5.Lcd.println("Cooking In Progress!");
    if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()){
      state.active = false;
      M5.Lcd.println("Cooking complete");
      res = sendPutRequest("active", String(state.active));
      M5.Lcd.println(res);
    }
  }
  else {
    int amount = client->get_request("/api/cookers/0/amount").toInt();
    M5.Lcd.println(amount);
    if(amount > 0 && state.weight > -30 && state.weight < 30) {
      state.active = true;
      res = sendPutRequest("active", String(state.active));
      M5.Lcd.println(res);
      M5.Lcd.println("Start cooking");
      //lidBle->open(); // TODO:外す
      lidWireMotor.reverse();
      delay(5000); // TODO: 調整
      lidWireMotor.stop();
      sweepServo(waterRodServo, 0, 90);
      waterDeliveryPump.forward();
      delay(5000); //TODO: state.weight
      waterDeliveryPump.stop();
      sweepServo(waterRodServo, 90, 0);
      for(int c = 0; c <= amount * 4; c++) {
        sweepServo(riceDeliveryServo, 0, 180);
        delay(500);
        sweepServo(riceDeliveryServo, 180, 0);
        delay(500);
      }
      sweepServo(riceWashingRodServo, 0, 90);
      riceWashingMotor.forward();
      delay(5000); //TODO: 調整
      riceWashingMotor.stop();
      sweepServo(riceWashingRodServo, 90, 0);
      sweepServo(waterRodServo, 0, 90);
      waterSuctionPump.forward();
      delay(5000); //TODO: state.weight
      waterSuctionPump.stop();
      delay(1000);
      waterDeliveryPump.forward();
      delay(5000); //TODO: state.weight
      waterDeliveryPump.stop();
      sweepServo(waterRodServo, 90, 0);
      lidWireMotor.forward();
      delay(5000); // TODO: 調整
      lidWireMotor.stop();
      //buttonBle->open(); // TODO:外す
    }
  }
  delay(1000);
}
