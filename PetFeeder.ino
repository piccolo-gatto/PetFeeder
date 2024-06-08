#include <ArduinoJson.h>
#include "lib/GyverStepper.h"
#include "lib/EncButton.h"
#include <FastBot.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <WiFiClient.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

String knownBLENames[] = {"OSHEYNIK_001"};
int RSSI_THRESHOLD = -35;
bool device_found;
int scanTime = 5; 
BLEScan* pBLEScan;

#include <EEPROM.h>
#define EE_RESET 1
#include <Ds1302.h>
Ds1302 rtc(27, 14, 12);

#define BTN_PIN 32
#define LED_PIN 33

GStepper<STEPPER4WIRE> stepper(2048, 15, 0, 2, 4);
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 1000;
bool dir = true;

byte feedTime[][2] = {
  {7, 0},       
  {12, 0},
  {17, 0},
  {22, 11},
};

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASSWORD"
#define BOT_TOKEN "YOUR_TOKEN"

bool collar_mode = true;
bool shedule_mode = true;
String feeder_id = "fdr345622765";
bool reg_user = false;
bool reg_feeder = false;
bool set_shedule = false;
String comand = "";
String payload;

FastBot bot(BOT_TOKEN);
const char* comands[] = {"/reg_collar", "/collar_mode_on", "/collar_mode_off", "/collar_mode_off", "/feed", "/get_shedule"};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String deviceName = advertisedDevice.getName();
      for (int i = 0; i < (sizeof(knownBLENames) / sizeof(knownBLENames[0])); i++)
      {
        if (deviceName == knownBLENames[i].c_str()) 
        {
          device_found = true;
          break;
        }
      }
    }
};


void setup()
{
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  rtc.init();
  EEPROM.begin(4096);
  if (EEPROM.read(0) != EE_RESET) {   
    EEPROM.write(0, EE_RESET);
    EEPROM.put(1, feedAmount);
    EEPROM.commit();
    if (rtc.isHalted())
    {
        Ds1302::DateTime dt = {
            .year = 24,
            .month = Ds1302::MONTH_JUN,
            .day = 3,
            .hour = 16,
            .minute = 43,
            .second = 0,
            .dow = Ds1302::DOW_MON
        };

        rtc.setDateTime(&dt);
    }
  }
  EEPROM.get(1, feedAmount);

  stepper.setSpeed(10000);
  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(10000);
  pinMode(LED_PIN, OUTPUT);
  
  bot.attach(newMsg);
}

void loop()
{
  bot.tick();
  if (collar_mode == true && shedule_mode == true){
    
    device_found = false;
    BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);

    static uint32_t tmr = 0;
    if (millis() - tmr > 500) {          
      static byte prevMin = 0;
      tmr = millis();
      Ds1302::DateTime now;
      rtc.getDateTime(&now);
      if (prevMin != now.minute) {
        prevMin = now.minute;
        for (byte i = 0; i < sizeof(feedTime) / 2; i++)   
          if (feedTime[i][0] == now.hour && feedTime[i][1] == now.minute){
            for (int i = 0; i < foundDevices->getCount(); i++)
            {
              BLEAdvertisedDevice device = foundDevices->getDevice(i);
              //int rssi = device.getRSSI();
              if (device.getRSSI() > RSSI_THRESHOLD && device_found == true)
                {
                  feed();
                }
            }
            pBLEScan->clearResults();
          } 
      }
    }
  }
  else if (collar_mode == false && shedule_mode == true){
    static uint32_t tmr = 0;
    if (millis() - tmr > 500) {          
      static byte prevMin = 0;
      tmr = millis();
      Ds1302::DateTime now;
      rtc.getDateTime(&now);
      if (prevMin != now.minute) {
        prevMin = now.minute;
        for (byte i = 0; i < sizeof(feedTime) / 2; i++)   
          if (feedTime[i][0] == now.hour && feedTime[i][1] == now.minute){
            feed();
            
          } 
      }
    }
  }
  else if (collar_mode == true && shedule_mode == false){
    
    device_found = false;
    BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
    for (int i = 0; i < foundDevices->getCount(); i++)
    {
      BLEAdvertisedDevice device = foundDevices->getDevice(i);
      int rssi = device.getRSSI();
      if (rssi > RSSI_THRESHOLD && device_found == true)
        {
          feed();
        }
    }
    pBLEScan->clearResults();
  }


  btn.tick();
  if (btn.click()){
    feed();
  }
  if (btn.hold()) {               
    digitalWrite(LED_PIN, HIGH);
    int newAmount = 0;
    while (btn.isHold()) {
      btn.tick();
      delay(1);
      anti_jam();
      newAmount++;
    }
    Serial.println(feedAmount);
    feedAmount = newAmount;
    Serial.println(feedAmount);
    EEPROM.put(1, feedAmount);
    EEPROM.commit();
    digitalWrite(LED_PIN, LOW);
  }
}

void feed() {

  digitalWrite(LED_PIN, HIGH);
  int tmp_fa = feedAmount;
  while (tmp_fa>0){
    delay(1);
    anti_jam();
    tmp_fa--;
  }
  digitalWrite(LED_PIN, LOW);
}

void anti_jam() {
  if (!stepper.tick()) {
    dir = !dir;
    stepper.setTarget(dir ? -12 : 19, RELATIVE);
  }
}

void newMsg(FB_msg& msg) {
  HTTPClient http;
  WiFiClient client;
  if (msg.text == "/reg_feeder"){
    
    if (http.begin(client, "http://172.20.10.2:8000/user_by_chat_id")){
        http.addHeader("accept", "application/json");
        http.addHeader("Content-Type", "application/json");

        StaticJsonDocument<200> TempDataJSON;
        TempDataJSON["user"] = msg.chatID;

        String TempDataString;
        serializeJson(TempDataJSON, TempDataString);
        int httpResponseCode = http.POST(TempDataString);
        payload = http.getString();
        if (httpResponseCode == 200) {
              if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_user")){
                http.addHeader("accept", "application/json");
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(TempDataString);
                payload = http.getString();
                if (httpResponseCode == 200) {
                  bot.sendMessage("Добро пожаловать!", msg.chatID);
                  reg_user = true; 
                  reg_feeder = false;
                  bot.sendMessage("Введите уникальный номер устройства, указанный на коробке или в инструкции", msg.chatID);
                }
              }
            else {
              bot.sendMessage("Рады снова Вас видеть!", msg.chatID);
              reg_user = true; 
              reg_feeder = false;
              bot.sendMessage("Введите уникальный номер устройства, указанный на коробке или в инструкции", msg.chatID);
            }
            }
            }
      
      }
      if (reg_user == true && reg_feeder == false){
        Serial.println(msg.text + " " + feeder_id);
        if(msg.text == feeder_id && http.begin(client, "http://172.20.10.2:8000/feeder_by_user_id")){
          http.addHeader("accept", "application/json");
          http.addHeader("Content-Type", "application/json");
          StaticJsonDocument<200> TempDataJSON;
          StaticJsonDocument<200> data;
          deserializeJson(data, payload);
          TempDataJSON["user_id"] = data["id"];
          TempDataJSON["feeder"] = msg.text;
          String TempDataString;
          serializeJson(TempDataJSON, TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          String payload = http.getString();
          if (httpResponseCode == 200) {
              if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_feeder")){
                http.addHeader("accept", "application/json");
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(TempDataString);
                payload = http.getString();
                if (httpResponseCode == 200) {
                  reg_feeder = true;
                  bot.sendMessage("Ваше устройство успешно зарегистрировано", msg.chatID);
                }
              }
            else {
              reg_feeder = true;
              bot.sendMessage("Ваше устройство уже зарегистрировано", msg.chatID);
              
            } 
            
          } 
        }}
  if (msg.text == "/set_shedule" || msg.text == "/reg_collar" || msg.text == "/collar_mode_on" || msg.text == "/collar_mode_off" || msg.text == "/collar_mode_off" || msg.text == "/feed" || msg.text == "/get_shedule"){
      comand = msg.text;
      if(http.begin(client, "http://172.20.10.2:8000/feeders_by_user")){
          http.addHeader("accept", "application/json");
          http.addHeader("Content-Type", "application/json");
          StaticJsonDocument<200> TempDataJSON;
          TempDataJSON["user"] = msg.chatID;
          String TempDataString;
          serializeJson(TempDataJSON, TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          payload = http.getString();
          if (httpResponseCode == 200) {
            bot.sendMessage("Введите устройство из списка: " + http.getString(), msg.chatID);
        }
      }
  }
  if (msg.text == feeder_id && comand == "/reg_collar"){
    bot.sendMessage("Введите уникальный номер ошейника, указанный на коробке или в инструкции:", msg.chatID);
  }
  if (msg.text[0] != 'f' && msg.text[1] != 'd' && msg.text[2] != 'r' && msg.text[0] != '/' && set_shedule == false){
  if (http.begin(client, "http://172.20.10.2:8000/get_feeder_id")){
          http.addHeader("accept", "application/json");
          http.addHeader("Content-Type", "application/json");
          StaticJsonDocument<200> TempDataJSON;
          TempDataJSON["feeder"] = feeder_id;
          String TempDataString;
          serializeJson(TempDataJSON, TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          payload = http.getString();
          if (httpResponseCode == 200) {
            if (http.begin(client, "http://172.20.10.2:8000/collar_by_feeder_id")){
                        http.addHeader("accept", "application/json");
                        http.addHeader("Content-Type", "application/json");
                        StaticJsonDocument<200> TempDataJSON;
                        StaticJsonDocument<200> data;
                        deserializeJson(data, payload);
                        TempDataJSON["feeder_id"] = data["feeder_id"];
                        TempDataJSON["collar"] = msg.text;
                        String TempDataString;
                        serializeJson(TempDataJSON, TempDataString);
                        int httpResponseCode = http.POST(TempDataString);
                        payload = http.getString();
                        if (httpResponseCode == 200) {
                            if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_collar")){
                              http.addHeader("accept", "application/json");
                              http.addHeader("Content-Type", "application/json");
                              int httpResponseCode = http.POST(TempDataString);
                              payload = http.getString();
                              if (httpResponseCode == 200) {
                                bot.sendMessage("Ваш ошейник успешно зарегестрирован", msg.chatID);
                              }
                            }
                          else {
                            bot.sendMessage("Ваше ошейник уже зарегестрирован", msg.chatID);
                            
                          } 
            }  
  }}}}
  if (msg.text == feeder_id &&comand != "/reg_collar" && comand != "/set_shedule"){
    if (comand == "/collar_mode_on"){
      if (collar_mode == true) bot.sendMessage("Режим распознавания ошейника уже включен", msg.chatID); 
      else {
        collar_mode = true;
        bot.sendMessage("Режим распознавания ошейника включен", msg.chatID); 
      }
    }
    if (comand == "/collar_mode_off"){
      if (collar_mode == false) {
        bot.sendMessage("Режим распознавания ошейника уже отключен", msg.chatID); 
        }
      else {
        collar_mode = false;
        bot.sendMessage("Режим распознавания ошейника отключен", msg.chatID); 
      }
    }
      if (comand == "/shedule_mode_on"){
      if (shedule_mode == true) {
        bot.sendMessage("Режим режим подачи по расписанию уже включен", msg.chatID); 
        } 
      else {
        shedule_mode = true;
        bot.sendMessage("Режим режим подачи по расписанию включен", msg.chatID);  
      }
    }
    if (comand == "/shedule_mode_off"){
      if (shedule_mode == false){ 
        bot.sendMessage("Режим режим подачи по расписанию уже отключен", msg.chatID); 
        }
      else {
        shedule_mode = false;
        bot.sendMessage("Режим режим подачи по расписанию отключен", msg.chatID);  
      }
    }
    if (comand == "/feed"){
      bot.sendMessage("Идёт подача корма", msg.chatID); 
      feed();
      bot.sendMessage("Корм подан", msg.chatID);  
    }

    if (comand == "/get_shedule"){
      String shedule = "";
      for (byte i = 0; i < sizeof(feedTime) / 2; i++) {
        shedule = shedule + int(i+1) + "-ая подача: " + feedTime[i][0] + " часов, " + feedTime[i][1] + " минут\n";
      } 
      bot.sendMessage(String(shedule), msg.chatID); 
    }
  }
  if (msg.text == feeder_id && comand == "/set_shedule"){
    bot.sendMessage("Введите время подачи еды через запятую в формате Ч:М (Например: 8:0, 10:30, 17:0, 21:15):", msg.chatID);
    set_shedule = true;
  }
  if (set_shedule == true && msg.text != feeder_id){
    String hour = "";
    String minute = "";
    bool check_hour = true;
    bool check_minute = false;
    int time = 0;
    for (byte i = 0; i < msg.text.length(); i++){
      if(msg.text[i] == ':'){
        check_hour = false;
        check_minute = true;
        continue;
      }
      if (check_hour == true && check_minute == false){
        hour += msg.text[i];
      }
      if (check_hour == false && check_minute == true){
        minute += msg.text[i];
      }
      if ((msg.text[i] == ',' && msg.text[i+1] == ' ') || i == msg.text.length()-1){
        
        feedTime[time][0] = byte(hour.toInt());
        feedTime[time][1] = byte(minute.toInt());
        hour = "";
        minute = "";
        check_hour = false;
        check_minute = false;
        time++;
        continue;
      }
      if(msg.text[i] == ' '){
        check_hour = true;
        check_minute = false;
        continue;
      }


    }
    bot.sendMessage("Новое расписание установлено", msg.chatID);
    set_shedule = false;
  }
}
