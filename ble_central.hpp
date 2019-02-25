#include <BLEDevice.h>

static BLEAdvertisedDevice* myDevice;
//setting about BLE starts from here
static BLERemoteCharacteristic* pRemoteCharacteristic;

class BleCentral {
  public:
    BleCentral();
    void connect();
    class MyClientCallback: public BLEClientCallbacks {
      void onConnect(BLEClient* pclient) {}
      void onDisconnect(BLEClient* pclient);
    };
    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
        // The remote service we wish to connect to.
        const char* serviceUUID = "12345678-9012-3456-7890-1234567890ff";
        // The characteristic of the remote service we are interested in.
        const char* charUUID = "12345678-9012-3456-7890-123456789011";
        // Called for each advertising BLE server.
        void onResult(BLEAdvertisedDevice advertisedDevice);
    };

  private:
    static bool connected;
    static bool doConnect;
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEClient*  pClient  = BLEDevice::createClient();
    bool connectToServer();
};
