#ifndef BLE_CENTRAL_HPP
#define BLE_CENTRAL_HPP

#include <BLEDevice.h>

//setting about BLE starts from here
// The remote service we wish to connect to.
static BLEUUID serviceUUID("12345678-9012-3456-7890-1234567890aa");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("12345678-9012-3456-7890-123456789022");
static bool doConnect = false;
static bool connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
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
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks
//setting about bluetooth end

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

class BleCentral {
  public:
    BleCentral();
    void open();

  private:
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEClient*  pClient  = BLEDevice::createClient();
    bool connectToServer();
};

#endif // BLE_CENTRAL_HPP
