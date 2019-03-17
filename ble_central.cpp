#include <M5Stack.h>
#include "ble_central.hpp"

BleCentral::BleCentral() {
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  while(connected==false){
    pBLEScan->start(5, false);
    if (doConnect == true) {
      if (connectToServer()) Serial.println("We are now connected to the BLE Server.");
      else Serial.println("We have failed to connect to the server; there is nothin more we will do.");
      doConnect = false;
    }
  }
}

void BleCentral::open() {
  //open the rice cookers via BLE
  while(connected==false){
    pBLEScan->start(5, false);
    if (doConnect == true) {
      if (connectToServer())Serial.println("We are now connected to the BLE Server.");
      else Serial.println("We have failed to connect to the server; We will restart scan.");
      doConnect = false;
    }
  }
  pRemoteCharacteristic->writeValue(01,true);
  delay(700);
  pRemoteCharacteristic->writeValue(03,true);
  delay(100);
  pRemoteCharacteristic->writeValue(02,true);
  delay(700);
  pRemoteCharacteristic->writeValue(03,true);
  delay(100);
  pRemoteCharacteristic->writeValue(00,true);
}



bool BleCentral::connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    connected = true;
    return true;
}
