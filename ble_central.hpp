#ifndef BLE_CENTRAL_HPP
#define BLE_CENTRAL_HPP

#define DEBUG

#include <BLEDevice.h>

static BLEUUID serviceUUID("12345678-9012-3456-7890-1234567890aa");
static BLEUUID    charUUID("12345678-9012-3456-7890-123456789022");
static BLEAdvertisedDevice* myDevice;

class BleCentral {
  public:
    BleCentral();
    void open();
    class MyClientCallback: public BLEClientCallbacks {
      void onConnect(BLEClient* pclient) {}
      void onDisconnect(BLEClient* pclient);
    };
    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
        // Called for each advertising BLE server.
        void onResult(BLEAdvertisedDevice advertisedDevice);
    };

  private:
    static bool connected;
    static bool doConnect;
    BLERemoteCharacteristic* pRemoteCharacteristic;
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEClient* pClient = BLEDevice::createClient();
    void openConnection();
};

#endif // BLE_CENTRAL_HPP
