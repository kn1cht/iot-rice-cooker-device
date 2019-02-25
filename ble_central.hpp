#include "BLEDevice.h"

//setting about BLE starts from here
// The remote service we wish to connect to.
static BLEUUID serviceUUID("12345678-9012-3456-7890-1234567890ff");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("12345678-9012-3456-7890-123456789011");
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
static bool connected = false;
static bool doConnect = false;

class BleCentral {
  public:
    BleCentral();
    void connect();

  private:
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEClient*  pClient  = BLEDevice::createClient();
    bool connectToServer();
    class MyClientCallback : public BLEClientCallbacks {
      void onConnect(BLEClient* pclient) {}
      void onDisconnect(BLEClient* pclient);
    };
    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
      * Called for each advertising BLE server.
      */
      void onResult(BLEAdvertisedDevice advertisedDevice);
    };
};

