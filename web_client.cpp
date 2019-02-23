#include <M5Stack.h>
#include <WiFiClientSecure.h>
#include "web_client.hpp"

WebClient::WebClient() {
  client.setCACert(HOST_ROOT_CA); // enable https request
}

String WebClient::get_request(String path) {
  if(!client.connect(HOST_URI, HOST_PORT)) {
    M5.Lcd.println("Connection failed!");
    return "";
  }
  M5.Lcd.println(HOST_URI + path);

  String req_str = String("GET https://") \
    + String(HOST_URI) + path + " HTTP/1.1\r\n" \
    + "Host: " + String(HOST_URI) + "\r\n" \
    + "Connection: close\r\n\r\n" + "\0";
  client.print(req_str);
  client.flush();

  while(!client.available()) delay(50);
  while(client.connected()) {
    if(client.readStringUntil('\n') == "\r") break;
  }
  String ret = "";
  while(client.available()) {
    char c = client.read();
    ret += c;
  }
  client.stop();
  return ret;
}
