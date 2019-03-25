#ifndef BLE_CENTRAL_HPP
#define BLE_CENTRAL_HPP

#include <BLEDevice.h>

static BLEAdvertisedDevice* myDevice;

class BleCentral {
  public:
    BleCentral(std::string _serviceUUID, std::string _charUUID);
    void open(int time);

  private:
    bool connected = false;
    bool doConnect = false;
    BLEUUID serviceUUID;
    BLEUUID charUUID;
    BLERemoteCharacteristic* pRemoteCharacteristic;
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEClient* pClient = BLEDevice::createClient();
    void openConnection();
};

class MyClientCallback: public BLEClientCallbacks {
  public:
    MyClientCallback(bool&);
    void onConnect(BLEClient* pclient) {}
    void onDisconnect(BLEClient* pclient);

  private:
  bool& connected;
};
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  public:
    MyAdvertisedDeviceCallbacks(BLEUUID&, bool&);
    // Called for each advertising BLE server.
    void onResult(BLEAdvertisedDevice advertisedDevice);

  private:
    bool& doConnect;
    BLEUUID serviceUUID;
};

#endif // BLE_CENTRAL_HPP
