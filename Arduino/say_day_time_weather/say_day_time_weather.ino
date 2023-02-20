#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

/************************/
/***** Time control *****/
/************************/
time_t t;
struct tm *tm;

void check_time(){
  static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};

  configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); 
  delay(500);
  
  t = time(NULL);
  tm = localtime(&t);

  Serial.printf(" %04d/%02d/%02d(%s) %02d:%02d:%02d\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        wd[tm->tm_wday],
        tm->tm_hour, tm->tm_min, tm->tm_sec);
}

/*******************/
/***** Weather *****/
/*******************/
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=tokyo,jp&units=metric&APPID=";
const String key = "your_api_key";

/****************/
/***** WiFi *****/
/****************/
#define WIFI_SSID       "your_wifi_ssid"
#define WIFI_PASSWORD   "your_wifi_password"

void init_wifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(200);
  }
  Serial.println();
  Serial.printf("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

/****************************/
/***** DFPlayer Setting *****/
/****************************/
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;
#define BUSY_PIN 36

void init_dfplayer(){
  mySerial.begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  while(!myDFPlayer.begin(mySerial)){
    Serial.print(".");
    delay(500);
  }

  Serial.println(F("DFPlayer Mini online."));
  delay(1000);
    
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
}

void wait_speech(){
  while(digitalRead(BUSY_PIN)==HIGH){delay(100);};
  while(digitalRead(BUSY_PIN)==LOW){delay(100);};
}

/******************/
/***** Speech *****/
/******************/
void say_time(){
  static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};

  check_time();

  myDFPlayer.playFolder(06,random(4,12)); // おはよう
  wait_speech();
  myDFPlayer.playFolder(01,tm->tm_mon+1); // month
  wait_speech();
  myDFPlayer.playFolder(01,13); // 月
  wait_speech();
  myDFPlayer.playFolder(02,tm->tm_mday); // day
  wait_speech();
  myDFPlayer.playFolder(02,32); // 日
  wait_speech();
  myDFPlayer.playFolder(03,tm->tm_wday+1); // wday
  wait_speech();
  myDFPlayer.playFolder(04,(tm->tm_hour)%12+1); // hour
  wait_speech();
  myDFPlayer.playFolder(05,(tm->tm_min)/5+1); // min
  wait_speech();
  myDFPlayer.playFolder(06,random(1,3)); // です
  wait_speech();
}

void say_weather(){
  StaticJsonDocument<1500> doc;
  HTTPClient http;
  http.begin(endpoint + key);
  int httpCode = http.GET();
  if(httpCode > 0){
    String ret = http.getString();
    Serial.println(ret);
    auto error = deserializeJson(doc, ret);
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    const char* weather = doc["weather"][0]["main"];
    Serial.println(weather);
    
    myDFPlayer.playFolder(8,random(5,6)); // 今日の天気は
    wait_speech();
    int wNum = 3;
    String w = weather;
    if(w.equals("Rain"))wNum = 1;
    else if(w.equals("Clear"))wNum = 2;
    else if(w.equals("Clouds"))wNum = 3;
    else if(w.equals("Snow"))wNum = 4;
    myDFPlayer.playFolder(8,wNum); // weather
    wait_speech();
    myDFPlayer.playFolder(6,random(1,3)); // です
    wait_speech();
    
  }
  doc.clear();
}

void setup() {
  pinMode(BUSY_PIN, INPUT);
  Serial.begin(115200);
  Serial.print("\n\nReset:\n");
  randomSeed(analogRead(0));

  init_wifi();
  init_dfplayer();
  say_time();
  say_weather();
}

void loop() {}