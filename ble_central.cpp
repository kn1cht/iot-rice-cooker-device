#include <M5Stack.h>
#include "ble_central.hpp"

bool BleCentral::connected = false;

bool BleCentral::doConnect = false;

BleCentral::BleCentral() {
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  openConnection();
}

void BleCentral::open() {
  openConnection();
  pRemoteCharacteristic->writeValue(01,true); delay(700);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
  pRemoteCharacteristic->writeValue(02,true); delay(700);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
  pRemoteCharacteristic->writeValue(00,true);
}

void BleCentral::openConnection() {
  while(connected == false) {
    pBLEScan->start(5, false);
    if(doConnect == true) {
      Serial.print("Forming a connection to ");
      Serial.println(myDevice->getAddress().toString().c_str());
      pClient->setClientCallbacks(new MyClientCallback());
      pClient->connect(myDevice);
      Serial.println("Connected to server");

      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if(pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
      }
      Serial.println("Found our service");

      pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
      if(pRemoteCharacteristic == nullptr) {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
      }
      else {
        Serial.println("We are now connected to the BLE Server.");
        connected = true;
      }
      doConnect = false;
    }
  }
}

void BleCentral::MyClientCallback::onDisconnect(BLEClient* pclient) {
  connected = false;
  Serial.println("Disconnected");
}

void BleCentral::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
  Serial.print("BLE Advertised Device found: ");
  Serial.println(advertisedDevice.toString().c_str());
#ifdef DEBUG
  if(advertisedDevice.getName() == "test") {
#else
  if(advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
#endif
    BLEDevice::getScan()->stop();
    myDevice = new BLEAdvertisedDevice(advertisedDevice);
    doConnect = true;
  }
}
