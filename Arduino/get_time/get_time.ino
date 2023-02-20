#include <WiFi.h>

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

void setup() {
  Serial.begin(115200);
  
  // WiFi 接続
  init_wifi();
  // 時刻をゲット
  check_time();
}

void loop() {}