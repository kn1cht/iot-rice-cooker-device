#include <esp32-hal-log.h>
#include <M5Stack.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"

WebClient* client;

void setup() {
  m5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.println("Hello, This is M5Stack!");
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
}

void loop() {
  M5.Lcd.println(client->get_request("/api/cookers/0/amount"));
  delay(10000);
}
