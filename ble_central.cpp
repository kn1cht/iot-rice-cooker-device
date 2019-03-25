#include <M5Stack.h>
#include "ble_central.hpp"

BleCentral::BleCentral(std::string _serviceUUID, std::string _charUUID) {
  serviceUUID = BLEUUID(_serviceUUID);
  charUUID = BLEUUID(_charUUID);

  M5.Lcd.clear(); M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Starting BLE Central Client...");
  BLEDevice::init("");
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(serviceUUID, doConnect));
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  openConnection();
}

void BleCentral::open(int time = 700) {
  openConnection();
  pRemoteCharacteristic->writeValue(01,true); delay(time);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
  pRemoteCharacteristic->writeValue(02,true); delay(time);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
  pRemoteCharacteristic->writeValue(00,true);
}

void BleCentral::openForward(int time = 700) {
  openConnection();
  pRemoteCharacteristic->writeValue(01,true); delay(time);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
}
void BleCentral::openReverse(int time = 700) {
  openConnection();
  pRemoteCharacteristic->writeValue(02,true); delay(time);
  pRemoteCharacteristic->writeValue(03,true); delay(100);
  pRemoteCharacteristic->writeValue(00,true);
}

void BleCentral::openConnection() {
  while(connected == false) {
    pBLEScan->start(5, false);
    if(doConnect == true) {
      M5.Lcd.print("Forming a connection to ");
      M5.Lcd.println(myDevice->getAddress().toString().c_str());
      pClient->setClientCallbacks(new MyClientCallback(connected));
      pClient->connect(myDevice);
      M5.Lcd.println("Connected to server");

      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if(pRemoteService == nullptr) {
        M5.Lcd.print("Failed to find our service UUID: ");
        M5.Lcd.println(serviceUUID.toString().c_str());
        pClient->disconnect();
      }
      M5.Lcd.println("Found our service");

      pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
      if(pRemoteCharacteristic == nullptr) {
        M5.Lcd.print("Failed to find our characteristic UUID: ");
        M5.Lcd.println(charUUID.toString().c_str());
        pClient->disconnect();
      }
      else {
        M5.Lcd.println("We are now connected to the BLE Server.");
        connected = true;
      }
      doConnect = false;
    }
  }
}

MyClientCallback::MyClientCallback(bool& _connected):
connected(_connected){}

void MyClientCallback::onDisconnect(BLEClient* pclient) {
  connected = false;
  M5.Lcd.println("Disconnected");
}

MyAdvertisedDeviceCallbacks::MyAdvertisedDeviceCallbacks(BLEUUID& _serviceUUID, bool& _doConnect):
doConnect(_doConnect), serviceUUID(_serviceUUID) {}

void MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
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
