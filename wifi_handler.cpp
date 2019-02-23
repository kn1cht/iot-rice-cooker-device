#include <M5Stack.h>
#include <Preferences.h>
#include <WiFi.h>
#include "wifi_handler.hpp"

WifiHandler::WifiHandler() {
  if(!restoreConfig() || !checkConnection())
    serveSmartConfig();
}

bool WifiHandler::checkConnection() {
  M5.Lcd.print("Waiting for Wi-Fi connection");
  for(int count = 0; count < 30; count++) {
    if (WiFi.status() == WL_CONNECTED) {
      M5.Lcd.println("\nConnected!");
      return true;
    }
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("Timed out.");
  return false;
}

bool WifiHandler::restoreConfig() {
  preferences.begin("wifi-config");
  String wifi_ssid = preferences.getString("WIFI_SSID");
  String wifi_password = preferences.getString("WIFI_PASS");
  preferences.end();
  M5.Lcd.printf("WIFI-SSID: %s\n", wifi_ssid.c_str());
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  return wifi_ssid.length() > 0;
}

void WifiHandler::serveSmartConfig() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  M5.Lcd.println("Waiting for SmartConfig.");
  while(!WiFi.smartConfigDone()) {
    delay(500);
    M5.Lcd.print(".");
  }
  while(!checkConnection()) {}
  preferences.begin("wifi-config", false);
  preferences.putString("WIFI_SSID", WiFi.SSID());
  preferences.putString("WIFI_PASS", WiFi.psk());
  preferences.end();
}
