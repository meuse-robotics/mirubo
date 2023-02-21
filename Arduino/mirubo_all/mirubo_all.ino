#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP32Servo.h>

/************************/
/***** Time control *****/
/************************/
#define WAKEUP_TIME 6
#define SLEEP_TIME 15
Ticker checker;
Ticker blinker;
volatile bool fiveMinFlag = false;
bool isAwake = true;
bool morningGreeting = false;
time_t t;
struct tm *tm;

void five_min(){
  fiveMinFlag = true;
}

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

/**********************/
/***** PIR Sensor *****/
/**********************/
#define PIR_PIN 15
volatile bool detectFlag = false;

void detected(){
  detectFlag = true;
}

/************************/
/***** Touch Sensor *****/
/************************/
#define TOUCH_PIN 22
volatile bool touchFlag = false;

void touched(){
  touchFlag = true;
}

/***********************/
/***** Line Notify *****/
/***********************/
const char* host = "notify-api.line.me";
const char* token = "your_line_token";

void send_line(String message, int SI=0, int PSI=0) {
  WiFiClientSecure client;
  client.setInsecure();
  
  Serial.println("Connecting to server..");
  
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }

  String query = String("message=") + String(message);
  if(SI >0){
    query = query + "&stickerId="+SI+"&stickerPackageId="+PSI;
  }
  String request = String("") +
               "POST /api/notify HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Bearer " + token + "\r\n" +
               "Content-Length: " + String(query.length()) +  "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                query + "\r\n";
  client.print(request);
  String res = client.readString();
  Serial.println(res);
  client.stop();
}

/******************/
/***** Speech *****/
/******************/
bool isSpeaking = false;

void say_clock(){
  isSpeaking = true;
  myDFPlayer.playFolder(04,(tm->tm_hour)%12+1); // hour
  wait_speech();
  myDFPlayer.playFolder(05,(tm->tm_min)/5+1); // min
  wait_speech();
  myDFPlayer.playFolder(06,random(1,3)); // です
  wait_speech();
  isSpeaking = false;
}

void say_goodnight(){
  isSpeaking = true;
  myDFPlayer.playFolder(7,random(1,5)); // おやすみ
  wait_speech();
  isSpeaking = false;
}

void say_time(){
  static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
  isSpeaking = true;
  configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); 
  delay(500);
  
  t = time(NULL);
  tm = localtime(&t);

  Serial.printf(" %04d/%02d/%02d(%s) %02d:%02d:%02d\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        wd[tm->tm_wday],
        tm->tm_hour, tm->tm_min, tm->tm_sec);

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
  if(tm->tm_min >= 5){
    myDFPlayer.playFolder(05,(tm->tm_min)/5+1); // min
    wait_speech();
  }
  myDFPlayer.playFolder(06,random(1,3)); // です
  wait_speech();
  isSpeaking = false;
}

void say_weather(){
  isSpeaking = true;
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
    //const char* description = doc["weather"][0]["description"];
    //long pressure = doc["main"]["pressure"];
    //double temp = doc["main"]["temp"];
    Serial.println(weather);
    //Serial.println(description);
    //Serial.println(pressure);
    //Serial.println(temp);

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
    myDFPlayer.playFolder(06,random(1,3)); // です
    wait_speech();
    
  }
  doc.clear();
  isSpeaking = false;
}

void say_random(){
  isSpeaking = true;
  myDFPlayer.playMp3Folder(random(1,1932)); // ランダム
  wait_speech();
  isSpeaking = false;
}

/***********************/
/***** LCD control *****/
/***********************/
#define TFT_CS       25
#define TFT_DC       26
#define TFT_MOSI     23
#define TFT_SCLK     18
#define TFT_RST      27 

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
bool isBlinking = false;

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

void init_tft(){
  tft.init(172, 320);           // Init ST7789 172x320
  tft.setSPISpeed(40000000);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  draw_normal();
}

void fillEllipse( uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, int wb )
{
  int8_t xt, x1, yt, y1, r;
  if (rx > ry) {
    xt=r=rx; yt=0;
    while (xt>=yt) {
      x1=xt*ry/rx;
      y1=yt*ry/rx;
      tft.drawFastHLine(x-xt, y+y1, 2*xt, wb);
      tft.drawFastHLine(x-xt, y-y1, 2*xt, wb);
      tft.drawFastHLine(x-yt, y+x1, 2*yt, wb);
      tft.drawFastHLine(x-yt, y-x1, 2*yt, wb);
      if ((r -= yt++ * 2 + 1) <= 0) r += --xt * 2;
    }
  } else {
    xt=r=ry; yt=0;
    while (xt>=yt) {
      x1=xt*rx/ry;
      y1=yt*rx/ry;
      tft.drawFastVLine(x+x1, y-yt, 2*yt, wb);
      tft.drawFastVLine(x-x1, y-yt, 2*yt, wb);
      tft.drawFastVLine(x+y1, y-xt, 2*xt, wb);
      tft.drawFastVLine(x-y1, y-xt, 2*xt, wb);
      if ((r -= yt++ * 2 + 1) <= 0) r += --xt * 2;
    }
  }
}

void draw_normal(){
  tft.fillRect(0, 0, 320, 90, BLACK);
  fillEllipse(50, 50, 30, 40, GREEN);
  fillEllipse(50, 50, 20, 30, BLACK);
  fillEllipse(270, 50, 30, 40, GREEN);
  fillEllipse(270, 50, 20, 30, BLACK);
  fillEllipse(160, 120, 60, 40, GREEN);
  fillEllipse(160, 120, 50, 30, BLACK);
}

void draw_smile(){
  tft.fillRect(0, 0, 320, 90, BLACK);
  fillEllipse(50, 50, 30, 40, GREEN);
  fillEllipse(50, 50, 20, 30, BLACK);
  fillEllipse(270, 50, 30, 40, GREEN);
  fillEllipse(270, 50, 20, 30, BLACK);
  tft.fillRect(0, 50, 320, 90, BLACK);
  fillEllipse(160, 120, 60, 40, GREEN);
  fillEllipse(160, 120, 50, 30, BLACK);
}

void draw_sleep(){
  tft.fillRect(0, 0, 320, 90, BLACK);
  fillEllipse(50, 50, 30, 40, GREEN);
  fillEllipse(50, 50, 20, 30, BLACK);
  fillEllipse(270, 50, 30, 40, GREEN);
  fillEllipse(270, 50, 20, 30, BLACK);
  fillEllipse(160, 120, 60, 40, GREEN);
  fillEllipse(160, 120, 50, 30, BLACK);
  tft.fillRect(0, 0, 320, 50, BLACK);
}

void draw_close(){
  tft.fillRect(0, 0, 320, 90, BLACK);
  draw_angled_line(20,20,80,50,10,GREEN);
  draw_angled_line(20,80,80,50,10,GREEN);
  draw_angled_line(300,20,240,50,10,GREEN);
  draw_angled_line(300,80,240,50,10,GREEN);
  fillEllipse(160, 120, 60, 40, GREEN);
  fillEllipse(160, 120, 50, 30, BLACK);
}

void blink(){
  if(isAwake && !isSpeaking){
    if(random(20)==0){
      tft.fillRect(0, 0, 320, 50, BLACK);
      isBlinking = true;
    }else if(isBlinking){  
      fillEllipse(50, 50, 30, 40, GREEN);
      fillEllipse(50, 50, 20, 30, BLACK);
      fillEllipse(270, 50, 30, 40, GREEN);
      fillEllipse(270, 50, 20, 30, BLACK);
      isBlinking = false;
    }
  }
}

void draw_angled_line(int x, int y, int x1, int y1, int size, int color) {
  float dx = (size / 2.0) * (y - y1) / sqrt(sq(x - x1) + sq(y - y1));
  float dy = (size / 2.0) * (x - x1) / sqrt(sq(x - x1) + sq(y - y1));
  tft.fillTriangle(x + dx, y - dy, x - dx,  y + dy,  x1 + dx, y1 - dy, color);
  tft.fillTriangle(x - dx, y + dy, x1 - dx, y1 + dy, x1 + dx, y1 - dy, color);
}

/*****************/
/***** Servo *****/
/*****************/
#define SV_PIN 13
Servo myServo;
int angle = 90;
int delta = 1;

void servo_up(){
  while(angle < 110){
    angle++;
    myServo.write(angle);
    delay(30);
  }
}
void servo_down(){
  while(angle > 60){
    angle--;
    myServo.write(angle);
    delay(30);
  }
}
void servo_down_and_up(){
  while(angle > 60){
    angle--;
    myServo.write(angle);
    delay(30);
  }
  while(angle < 110){
    angle++;
    myServo.write(angle);
    delay(30);
  }
}

void setup() {
  pinMode(BUSY_PIN, INPUT);
  pinMode(PIR_PIN,INPUT);
  pinMode(TOUCH_PIN,INPUT);
  myServo.attach(SV_PIN);
  myServo.write(90);
  Serial.begin(115200);
  Serial.print("\n\nReset:\n");
  randomSeed(analogRead(0));

  init_wifi();
  init_dfplayer();
  init_tft();
  checker.attach(5*60, five_min);
  blinker.attach(0.2, blink);
  attachInterrupt(PIR_PIN, detected, RISING);
  attachInterrupt(TOUCH_PIN, touched, RISING);
}

void loop() {
  if(fiveMinFlag){
    while(isSpeaking){};
    if (WiFi.status() != WL_CONNECTED){
      Serial.print("disconnected");
      WiFi.disconnect();
      WiFi.reconnect();
    }
    check_time();
    fiveMinFlag = false;
    if(tm->tm_min < 5) {
      if(tm->tm_hour >= WAKEUP_TIME 
        && tm->tm_hour < SLEEP_TIME
        && !isAwake){
        isAwake = true;
        draw_normal();
        servo_up();
      }else if((tm->tm_hour >= SLEEP_TIME 
        || tm->tm_hour < WAKEUP_TIME)
        && isAwake){
        Serial.println("sleep");
        isAwake = false;
        say_goodnight();
        draw_sleep();
        servo_down();
        morningGreeting = false;
      }else if(isAwake){
        say_clock();
      }
    }
  }
  if(detectFlag && isAwake){
    while(isSpeaking){};
    Serial.println("detected!");
    if(!morningGreeting){
      say_time();
      say_weather();
      morningGreeting = true;
    }else{
      int rnd = random(3);
      if(rnd == 0)draw_normal();
      else if(rnd == 1)draw_smile();
      else draw_close();
      say_random();
      draw_normal();
      servo_down_and_up();
    }
    detectFlag = false;
  }
  if(touchFlag){
    while(isSpeaking){};
    Serial.println("touched!");
    send_line("おはよう",10858,789);
    touchFlag = false;
  }
}
