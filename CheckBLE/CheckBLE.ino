

// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// String knownBLEAddresses[] = {"41:42:a7:00:21:5f", "6E:bc:55:18:cf:7b", "53:3c:cb:56:36:02", "40:99:4b:75:7d:2f", "5c:5b:68:6f:34:96"};
// int RSSI_THRESHOLD = -55;
// bool device_found;
// int scanTime = 5; //In seconds
// BLEScan* pBLEScan;
// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//     void onResult(BLEAdvertisedDevice advertisedDevice) {
//       for (int i = 0; i < (sizeof(knownBLEAddresses) / sizeof(knownBLEAddresses[0])); i++)
//       {
//         //Uncomment to Enable Debug Information
//         Serial.println("*************Start**************");
//         Serial.println(sizeof(knownBLEAddresses));
//         Serial.println(sizeof(knownBLEAddresses[0]));
//         Serial.println(sizeof(knownBLEAddresses)/sizeof(knownBLEAddresses[0]));
//         Serial.println(advertisedDevice.getAddress().toString().c_str());
//         Serial.println(knownBLEAddresses[i].c_str());
//         Serial.println("*************End**************");
//         if (strcmp(advertisedDevice.getAddress().toString().c_str(), knownBLEAddresses[i].c_str()) == 0)
//                         {
//           device_found = true;
//                           break;
//                         }
//         else
//           device_found = false;
//       }
//       Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
//     }
// };
// void setup() {
//   Serial.begin(115200); //Enable UART on ESP32
//   Serial.println("Scanning..."); // Print Scanning
//   pinMode(32, OUTPUT); //make BUILTIN_LED pin as output
//   BLEDevice::init("");
//   pBLEScan = BLEDevice::getScan(); //create new scan
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Init Callback Function
//   pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
//   pBLEScan->setInterval(100); // set Scan interval
//   pBLEScan->setWindow(99);  // less or equal setInterval value
// }
// void loop() {
//   // put your main code here, to run repeatedly:
//   BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
//   for (int i = 0; i < foundDevices.getCount(); i++)
//   {
//     BLEAdvertisedDevice device = foundDevices.getDevice(i);
//     int rssi = device.getRSSI();
//     Serial.print("RSSI: ");
//     Serial.println(rssi);
//     if (rssi > RSSI_THRESHOLD && device_found == true)
//       digitalWrite(32, HIGH);
//     else
//       digitalWrite(32, LOW);
//   }
//   pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
// }

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED_S 32

// #include "src/BLEDevice.h"
// #include "src/BLEUtils.h"
// #include "src/BLEScan.h"
// #include "src/BLEAdvertisedDevice.h"
String knownBLEAddresses[] = {"5c:02:1a:e0:2f:85", "de:9f:a0:f4:e0:53"};
int RSSI_THRESHOLD = -35;
bool device_found;
int scanTime = 5;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      for (int i = 0; i < (sizeof(knownBLEAddresses) / sizeof(knownBLEAddresses[0])); i++)
      {
        //Uncomment to Enable Debug Information
        // Serial.println("*************Start**************");
        // //Serial.println(sizeof(knownBLEAddresses));
        // //Serial.println(sizeof(knownBLEAddresses[0]));
        // Serial.println(sizeof(knownBLEAddresses)/sizeof(knownBLEAddresses[0]));
        // Serial.println(advertisedDevice.getAddress().toString().c_str());
        // Serial.println(knownBLEAddresses[i].c_str());
        // Serial.println("*************End**************");
        if (strcmp(advertisedDevice.getAddress().toString().c_str(), knownBLEAddresses[i].c_str()) == 0)
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
  BLEDevice::init("");
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