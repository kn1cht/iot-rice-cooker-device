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

void loop() {
  M5.update();
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,20);
  String json_weight = String("{\"weight\": \"") + String(state_weight) + "\"}";
  M5.Lcd.println(client->put_request("/api/cookers/0/weight", json_weight));

  if(state_active) {
    M5.Lcd.println("Cooking In Progress!");
    if(M5.BtnA.wasReleased() || M5.BtnB.wasReleased() || M5.BtnC.wasReleased()){
      state_active = false;
      M5.Lcd.println("Cooking complete");
      String json_active = String("{\"active\": \"") + String(state_active ? "true": "false") + "\"}";
      M5.Lcd.println(client->put_request("/api/cookers/0/active", json_active));
    }
  }
  else {
    int amount = client->get_request("/api/cookers/0/amount").toInt();
    M5.Lcd.println(amount);
    if(amount > 0 && state_weight > -30 && state_weight < 30) {
      state_active = true;
      M5.Lcd.println("Start cooking"); // TODO: ugokasu
      String json_active = String("{\"active\": \"") + String(state_active ? "true": "false") + "\"}";
      M5.Lcd.println(client->put_request("/api/cookers/0/active", json_active));
    }
  }
  delay(1000);
}
