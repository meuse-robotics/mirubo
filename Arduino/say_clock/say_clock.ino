#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"
#include <Ticker.h>

/************************/
/***** Time control *****/
/************************/
Ticker checker;
volatile bool fiveMinFlag = false;
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
  if(digitalRead(BUSY_PIN)==HIGH){
    while(digitalRead(BUSY_PIN)==HIGH){delay(100);};
  }
  while(digitalRead(BUSY_PIN)==LOW){delay(100);};
}

/******************/
/***** Speech *****/
/******************/
void say_clock(){
  myDFPlayer.playFolder(04,(tm->tm_hour)%12+1); // hour
  wait_speech();
  myDFPlayer.playFolder(06,random(1,3)); // です
  wait_speech();
}

void setup() {
  pinMode(BUSY_PIN, INPUT);
  Serial.begin(115200);
  Serial.print("\n\nReset:\n");
  //randomSeed(analogRead(0));

  init_wifi();
  init_dfplayer();
  checker.attach(5*60, five_min); // 5分おきに割り込み
}

void loop() {
  if(fiveMinFlag){        // ５分経過
    check_time();
    fiveMinFlag = false;
    if(tm->tm_min < 5) {  // 正時～正時+５分の間
      say_clock();        // 正時を知らせる 
    }
  }
}
