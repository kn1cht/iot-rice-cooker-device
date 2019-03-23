#include <ESP32Servo.h>
#include <HX711.h>
#include <M5Stack.h>
#include "iot-rice-cooker-device.hpp"
#include "web_client.hpp"
#include "wifi_handler.hpp"
#include "ble_central.hpp"

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
  ledcDetachPin(SPEAKER_PIN); // disable speaker
  /*** Sensors ***/
  M5.Lcd.println("Initializing Sensors...");
  pinMode(WATER_SENSOR1_PIN, INPUT);
  pinMode(WATER_SENSOR2_PIN, INPUT);
  scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  scale.tare(10); // set offset
  scale.set_scale(103.5f); // set unit scale
  /*** Actuators ***/
  M5.Lcd.println("Initializing Actuators...");
  lidWireMotor.init(LID_WIRE_MOTOR1_PIN, LID_WIRE_MOTOR2_PIN);
  riceWashingMotor.init(RICE_WASHING_MOTOR_PIN);
  waterDeliveryPump.init(WATER_DELIVERY_PUMP_PIN);
  waterSuctionPump.init(WATER_SUCTION_PUMP_PIN);
	riceDeliveryServo.attach(RICE_DELIVERY_SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
	riceWashingRodServo.attach(RICE_WASHING_ROD_SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
	waterRodServo.attach(WATER_ROD_SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  M5.Lcd.println("Returning to Home Position");
  for(int c = 0; c < 100; c++) { // within 3 seconds
    if(c % 20 == 0) M5.Lcd.print(".");
    riceDeliveryServo.write(RICE_DELIVERY_HOME);
    riceWashingRodServo.write(RICE_WASHING_ROD_HOME);
    waterRodServo.write(WATER_ROD_HOME);
    delay(30);
  }
  M5.Lcd.println("Done");
  /*** Wi-Fi and BLE Initializing ***/
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
  //lidBle = new BleCentral("12345678-9012-3456-7890-1234567890ff", "12345678-9012-3456-7890-123456789011");
  //buttonBle = new BleCentral("12345678-9012-3456-7890-1234567890aa", "12345678-9012-3456-7890-123456789022");
  M5.Lcd.clear();
  M5.Lcd.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.setTextDatum(TC_DATUM);
}

void loop() {
  M5.Lcd.clear();
  M5.Lcd.setTextDatum(TC_DATUM);

  state.weight = scale.get_units(10); // [g]
  state.water1 = digitalRead(WATER_SENSOR1_PIN); // 0: water shortage alert

  String res = sendPutRequest(client, "weight", String(state.weight));
  M5.Lcd.println("weight: " + String(state.weight));
  M5.Lcd.println("water1: " + String(state.water1));

  if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
    state.id = STATE_COMPLETE;
    res = sendPutRequest(client, "active", "0");
    M5.Lcd.println(res);
  }

  switch(state.id) {
    case STATE_STANDBY: {
      M5.Lcd.println("Standby");
      if(state.weight >= 30) state.id = STATE_COMPLETE;
      state.amount = client->get_request("/api/cookers/0/amount").toInt();
      M5.Lcd.println("amount: " + state.amount);
      if(state.amount > 0) {
        state.id = STATE_OPENLID;
        res = sendPutRequest(client, "active", "1");
        M5.Lcd.println(res);
      }
      break;
    }
    case STATE_OPENLID: {
      M5.Lcd.println("Opening Lid");
      //lidBle->open(); // TODO:外す
      lidWireMotor.reverse();
      delay(5000); // TODO: 調整
      lidWireMotor.stop();
      state.id = STATE_POURWATER1;
      break;
    }
    case STATE_POURWATER1: {
      M5.Lcd.println("Pouring Water");
      sweepServo(waterRodServo, WATER_ROD_HOME, WATER_ROD_DOWN);
      waterDeliveryPump.forward();
      double prevWeight = state.weight;
      while(state.weight - prevWeight < 180 * state.amount) delay(100);
      waterDeliveryPump.stop();
      sweepServo(waterRodServo, WATER_ROD_DOWN, WATER_ROD_HOME);
      state.id = STATE_DROPRICE;
      break;
    }
    case STATE_DROPRICE: {
      M5.Lcd.println("Dropping Rice");
      for(int c = 0; c <= state.amount * 4; c++) {
        sweepServo(riceDeliveryServo, RICE_DELIVERY_HOME, RICE_DELIVERY_DROP);
        delay(500);
        sweepServo(riceDeliveryServo, RICE_DELIVERY_DROP, RICE_DELIVERY_HOME);
        delay(500);
      }
      state.id = STATE_WASHRICE;
      break;
    }
    case STATE_WASHRICE: {
      M5.Lcd.println("Washing Rice");
      sweepServo(riceWashingRodServo, RICE_WASHING_ROD_HOME, RICE_WASHING_ROD_DOWN);
      riceWashingMotor.forward();
      delay(5000); //TODO: 調整
      riceWashingMotor.stop();
      sweepServo(riceWashingRodServo, RICE_WASHING_ROD_DOWN, RICE_WASHING_ROD_HOME);
      state.id = STATE_SUCKWATER;
      break;
    }
    case STATE_SUCKWATER: {
      M5.Lcd.println("Sucking Water");
      sweepServo(waterRodServo, WATER_ROD_HOME, WATER_ROD_DOWN);
      waterSuctionPump.forward();
      delay(5000); //TODO: 調整
      waterSuctionPump.stop();
      state.id = STATE_POURWATER2;
      break;
    }
    case STATE_POURWATER2: {
      M5.Lcd.println("Pouring Water");
      waterDeliveryPump.forward();
      double prevWeight = state.weight;
      while(state.weight - prevWeight < 180 * state.amount) delay(100);
      waterDeliveryPump.stop();
      sweepServo(waterRodServo, WATER_ROD_DOWN, WATER_ROD_HOME);
      state.id = STATE_CLOSELID;
      break;
    }
    case STATE_CLOSELID: {
      M5.Lcd.println("Closing Lid");
      lidWireMotor.forward();
      delay(5000); // TODO: 調整
      lidWireMotor.stop();
      delay(500);
      //buttonBle->open(); // TODO:外す
      state.id = STATE_COOKING;
      break;
    }
    case STATE_COOKING: {
      M5.Lcd.println("Cooking in Progress");
      break;
    }
    case STATE_COMPLETE: {
      M5.Lcd.println("Cooking Complete");
      if(state.weight > -30 && state.weight < 30) state.id = STATE_STANDBY;
      break;
    }
  }
  M5.update();
  delay(1000);
}
