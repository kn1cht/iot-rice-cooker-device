#include <M5Stack.h>
#include <HX711.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"
#include "ble_central.hpp"

enum Pin : uint8_t {
  /* sensors */
  loadCellDout = 22,
  loadCellSck  = 23,
  waterSensor1 = 2,
  waterSensor2 = 5,
  /* actuators */
  lidWireMotor1       = 1,
  lidWireMotor2       = 3,
  riceWashingMotor    = 16,
  riceWashingRodServo = 17,
  waterDeliveryPump   = 18,
  waterRodServo       = 19,
  waterSuctionPump    = 21,
};

struct State{
  bool active   = false;
  bool water1   = true;
  double weight = 20;
};

State state;
HX711 scale;
WebClient* client;
BleCentral* lidBle;
BleCentral* buttonBle;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  /*** Sensors and Actuators ***/
  pinMode(Pin::waterSensor1, INPUT);
  pinMode(Pin::waterSensor2, INPUT);
  scale.begin(Pin::loadCellDout, Pin::loadCellSck);
  scale.tare(10); // set offset
  scale.set_scale(103.5f); // set unit scale
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

void loop() {
  M5.update();
  delay(10); // for button pressing
  M5.Lcd.clear(); M5.Lcd.setCursor(0,20);

  state.weight = scale.get_units(10); // [g]
  state.water1 = digitalRead(Pin::waterSensor1); // 0: water shortage alert

  String res = sendPutRequest("weight", String(state.weight));
  M5.Lcd.println(res);

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
      M5.Lcd.println("Start cooking");
      //lidBle->open(); // TODO:外す
      //buttonBle->open(); // TODO:外す
      res = sendPutRequest("active", String(state.active));
      M5.Lcd.println(res);
    }
  }
  delay(1000);
}
