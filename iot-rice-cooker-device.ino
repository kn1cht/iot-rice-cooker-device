#include <M5Stack.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"

WebClient* client;
bool state_active = false;
float state_weight = 20;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
}

String makeJsonOne(String key, String value) {
  return String("{\"") + key + String("\": \"") + value + String("\"}");
}

void loop() {
  M5.update();
  delay(10); // for button pressing
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,20);
  String res = client->put_request("/api/cookers/0/weight", makeJsonOne("weight", String(state_weight)));
  M5.Lcd.println(res);

  if(state_active) {
    M5.Lcd.println("Cooking In Progress!");
    if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()){
      state_active = false;
      M5.Lcd.println("Cooking complete");
      res = client->put_request("/api/cookers/0/active", makeJsonOne("active", String(state_active ? "true": "false")));
      M5.Lcd.println(res);
    }
  }
  else {
    int amount = client->get_request("/api/cookers/0/amount").toInt();
    M5.Lcd.println(amount);
    if(amount > 0 && state_weight > -30 && state_weight < 30) {
      state_active = true;
      M5.Lcd.println("Start cooking"); // TODO: ugokasu
      res = client->put_request("/api/cookers/0/active", makeJsonOne("active", String(state_active ? "true": "false")));
      M5.Lcd.println(res);
    }
  }
  delay(1000);
}
