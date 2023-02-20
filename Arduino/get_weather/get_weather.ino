#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

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

/*******************/
/***** Weather *****/
/*******************/
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=tokyo,jp&units=metric&APPID=";
const String key = "your_api_key";

void get_weather(){
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
    const char* description = doc["weather"][0]["description"];
    long pressure = doc["main"]["pressure"];
    double temp = doc["main"]["temp"];
    Serial.println(weather);
    Serial.println(description);
    Serial.println(pressure);
    Serial.println(temp);
  }
  doc.clear();
}

void setup() {
  Serial.begin(115200);

  // WiFi 接続
  init_wifi();
  // 天気をゲット
  get_weather();
}

void loop() {}