#include <esp32-hal-log.h>
#include <M5Stack.h>
#include "wifi_handler.hpp"

void setup() {
  m5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.println("Hello, This is M5Stack!");
  new WifiHandler(); // start Wi-Fi connection
}

void loop() {
}
