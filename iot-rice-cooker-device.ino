#include <M5Stack.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"
#include "ble_central.hpp"

const uint8_t WATER_SENSOR_1_PIN = 5;

struct State{
  bool active = false;
  bool water1 = true;
  double weight = 20;
};

State state;
WebClient* client;
BleCentral* ble;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  pinMode(WATER_SENSOR_1_PIN, INPUT);
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
  ble = new BleCentral("12345678-9012-3456-7890-1234567890aa", "12345678-9012-3456-7890-123456789022");
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
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,20);

  state.weight = 20; // TODO: load cell
  state.water1 = digitalRead(WATER_SENSOR_1_PIN);

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
      ble->open();
      res = sendPutRequest("active", String(state.active));
      M5.Lcd.println(res);
    }
  }
  delay(1000);
}
