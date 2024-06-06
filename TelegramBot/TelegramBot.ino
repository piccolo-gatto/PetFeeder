#include <FastBot.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#include <ArduinoJson.h>

#define WIFI_SSID "123"
#define WIFI_PASS "11111111"
#define BOT_TOKEN "7200148672:AAFzfXtBfQUQ9nCPOi2KS3UG1VyyR0JwEt0"
#define LED_PIN D0

bool collar_mode = true;
bool shedule_mode = true;
String feeder_id = "fdr345622765";
bool reg_user = false;
bool reg_feeder = false;
bool set_shedule = false;
String comand = "";
String payload;
ESP8266WiFiMulti WiFiMulti;

FastBot bot(BOT_TOKEN);

byte feedTime[][2] = {
  {7, 0},       // часы, минуты. НЕ НАЧИНАТЬ ЧИСЛО С НУЛЯ
  {12, 0},
  {17, 0},
  {22, 11},
};

const char* comands[] = {"/reg_collar", "/collar_mode_on", "/collar_mode_off", "/collar_mode_off", "/feed", "/get_shedule"}; 

void setup() {
pinMode(LED_PIN, OUTPUT);
  delay(2000);
  Serial.begin(115200);
  Serial.println();
 // WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }
  Serial.println("Connected");

  bot.attach(newMsg);
}

void newMsg(FB_msg& msg) {
  HTTPClient http;
  WiFiClient client;
  Serial.print(msg.chatID);     // ID чата 
  Serial.print(", ");
  Serial.print(msg.userID);   // логин
  Serial.print(", ");
  Serial.println(msg.text);     // текст
  if (msg.text == "/reg_feeder"){
    
    Serial.println(client);
    if (http.begin(client, "http://172.20.10.2:8000/user_by_chat_id")){
        http.addHeader("accept", "application/json");
        http.addHeader("Content-Type", "application/json");

        StaticJsonDocument<200> TempDataJSON;
        TempDataJSON["user"] = msg.chatID;

        String TempDataString;
        serializeJson(TempDataJSON, TempDataString);
        Serial.println(TempDataString);
        int httpResponseCode = http.POST(TempDataString);
        payload = http.getString();
        if (httpResponseCode == 200) {
          Serial.println("[HTTP] Data sent successfully ");
          Serial.println(httpResponseCode);
              if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_user")){
                http.addHeader("accept", "application/json");
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(TempDataString);
                payload = http.getString();
                Serial.println(httpResponseCode);
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
            Serial.println("msg"); 
            Serial.println(payload); 
            }else {
            Serial.println("[HTTP] Unable to connect to send data");
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
          Serial.println(TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          String payload = http.getString();
          if (httpResponseCode == 200) {
            Serial.println("[HTTP] Data sent successfully");
            Serial.println(httpResponseCode);
              if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_feeder")){
                http.addHeader("accept", "application/json");
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(TempDataString);
                payload = http.getString();
                Serial.println(httpResponseCode);
                if (httpResponseCode == 200) {
                  reg_feeder = true;
                  bot.sendMessage("Ваше устройство успешно зарегестрировано", msg.chatID);
                }
              }
            else {
              reg_feeder = true;
              bot.sendMessage("Ваше устройство уже зарегестрировано", msg.chatID);
              
            }
            Serial.println("msg"); 
            Serial.println(payload); 
            }else {
            Serial.println("[HTTP] Unable to connect to send data");
            } 
          } 
        }
  if (msg.text == "/set_shedule" || msg.text == "/reg_collar" || msg.text == "/collar_mode_on" || msg.text == "/collar_mode_off" || msg.text == "/collar_mode_off" || msg.text == "/feed" || msg.text == "/get_shedule"){
      comand = msg.text;
      if(http.begin(client, "http://172.20.10.2:8000/feeders_by_user")){
          http.addHeader("accept", "application/json");
          http.addHeader("Content-Type", "application/json");
          StaticJsonDocument<200> TempDataJSON;
          TempDataJSON["user"] = msg.chatID;
          String TempDataString;
          serializeJson(TempDataJSON, TempDataString);
          Serial.println(TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          payload = http.getString();
          if (httpResponseCode == 200) {
            Serial.println("[HTTP] Data sent successfully");
            Serial.println(httpResponseCode);
            Serial.println(http.getString());
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
          Serial.println(TempDataString);
          int httpResponseCode = http.POST(TempDataString);
          payload = http.getString();
          if (httpResponseCode == 200) {
            Serial.println("[HTTP] Data sent successfully");
            Serial.println(httpResponseCode);
            Serial.println(http.getString());
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
                        Serial.println(TempDataString);
                        int httpResponseCode = http.POST(TempDataString);
                        payload = http.getString();
                        if (httpResponseCode == 200) {
                          Serial.println("[HTTP] Data sent successfully");
                          Serial.println(httpResponseCode);
                            if(payload == "{\"message\":\"None\"}" && http.begin(client, "http://172.20.10.2:8000/add_collar")){
                              http.addHeader("accept", "application/json");
                              http.addHeader("Content-Type", "application/json");
                              Serial.println("ff"+TempDataString);
                              int httpResponseCode = http.POST(TempDataString);
                              payload = http.getString();
                              Serial.println(httpResponseCode);
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
      //feed();
      bot.sendMessage("Корм подан", msg.chatID);  
    }

    if (comand == "/get_shedule"){
      String shedule = "";
      for (byte i = 0; i < sizeof(feedTime) / 2; i++) {
        shedule = shedule + int(i+1) + "-ая подача: " + feedTime[i][0] + " часов, " + feedTime[i][1] + " минут\n";
      } 
      Serial.println(shedule);
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
        Serial.println(hour+" "+minute);
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


void loop() {
  bot.tick();
  if (collar_mode == true && shedule_mode == true){}
  else if (collar_mode == false && shedule_mode == true){}
  else if (collar_mode == true && shedule_mode == false){}
  else if (collar_mode == false && shedule_mode == false){}
}

