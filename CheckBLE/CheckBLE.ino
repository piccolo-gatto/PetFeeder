#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED_S 33

//String knownBLEAddresses[] = {"5c:02:1a:e0:2f:85", "de:9f:a0:f4:e0:53"};
String knownBLENames[] = {"OSHEYNIK_001", "OSHEYNIK_002"};
int RSSI_THRESHOLD = -35;
bool device_found;
int scanTime = 5;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {

    String deviceName = advertisedDevice.getName();

      for (int i = 0; i < (sizeof(knownBLENames) / sizeof(knownBLENames[0])); i++)
      {
        //Uncomment to Enable Debug Information
        // Serial.println("*************Start**************");
        // //Serial.println(sizeof(knownBLEAddresses));
        // //Serial.println(sizeof(knownBLEAddresses[0]));
        // Serial.println(sizeof(knownBLEAddresses)/sizeof(knownBLEAddresses[0]));
        // Serial.println(advertisedDevice.getAddress().toString().c_str());
        // Serial.println(knownBLEAddresses[i].c_str());
        // Serial.println("*************End**************");

        //if (strcmp(advertisedDevice.getAddress().toString().c_str(), knownBLENames[i].c_str()) == 0)
        if (deviceName == knownBLENames[i].c_str()) 
                        {
          device_found = true;
          //Serial.println(device_found);
                          break;
                        }
        // else{
        //   Serial.println(device_found);
        //   device_found = false;
        //   Serial.println(device_found);
        // }
      }
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};
void setup() {
  Serial.begin(115200);
  //Serial.println("Scanning...");
  pinMode(LED_S, OUTPUT);
  BLEDevice::init("Feeder001");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}
void loop() {
  device_found = false;

  BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  for (int i = 0; i < foundDevices->getCount(); i++)
  {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    int rssi = device.getRSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    if (rssi > RSSI_THRESHOLD && device_found == true)
      {
        //Serial.println("its here");
        digitalWrite(LED_S, HIGH);
        delay(1000);
      }
    else{
      digitalWrite(LED_S, LOW);
    }
  }
  pBLEScan->clearResults();
}
