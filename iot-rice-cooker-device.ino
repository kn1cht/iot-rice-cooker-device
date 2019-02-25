#include <M5Stack.h>
#include "web_client.hpp"
#include "wifi_handler.hpp"
#include "BLEDevice.h"
//#include "BLEScan.h"

WebClient* client;
bool state_active = false;
float state_weight = 20;

//setting about BLE starts from here
// The remote service we wish to connect to.
static BLEUUID serviceUUID("12345678-9012-3456-7890-1234567890ff");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("12345678-9012-3456-7890-123456789011");
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
BLEClient*  pClient  = BLEDevice::createClient();
BLEScan* pBLEScan = BLEDevice::getScan();

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    connected = true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks
//setting about bluetooth end


void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  new WifiHandler(); // start Wi-Fi connection
  client = new WebClient();
  //about BLE
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
}

String makeJsonOne(String key, String value) {
  return String("{\"") + key + String("\": \"") + value + String("\"}");
}

void loop() {
  M5.update();
  delay(10); // for button pressing
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,20);
  String res = client->put_request("/api/cookers/0/weight", makeJsonOne("weight", String(state_weight)));
  M5.Lcd.println(res);

  if(state_active) {
    M5.Lcd.println("Cooking In Progress!");
    if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()){
      state_active = false;
      M5.Lcd.println("Cooking complete");
      res = client->put_request("/api/cookers/0/active", makeJsonOne("active", String(state_active)));
      M5.Lcd.println(res);
    }
  }
  else {
    int amount = client->get_request("/api/cookers/0/amount").toInt();
    M5.Lcd.println(amount);
    if(amount > 0 && state_weight > -30 && state_weight < 30) {
      state_active = true;
      M5.Lcd.println("Start cooking"); // TODO: ugokasu

      //open the rice cookers via BLE
      int i;
      while(connected==false){
        pBLEScan->start(5, false);
        if (doConnect == true) {
          if (connectToServer())Serial.println("We are now connected to the BLE Server.");
          else Serial.println("We have failed to connect to the server; there is nothin more we will do.");
          doConnect = false;
        }
        Serial.printf("Delay 15sec.\n");
        for(i=0;i<15;i++){
          Serial.printf("%d\n",i+1);
          delay(1000);
        }
      }
        pClient->disconnect();

      res = client->put_request("/api/cookers/0/active", makeJsonOne("active", String(state_active)));
      M5.Lcd.println(res);
    }
  }
  delay(1000);
}
