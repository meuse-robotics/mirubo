#include <WiFi.h>
#include <WiFiClientSecure.h>

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

void setup()
{
  Serial.begin(115200);
  pinMode(TOUCH_PIN,INPUT); // タッチPinを入力に設定

  init_wifi();
  attachInterrupt(TOUCH_PIN, touched, RISING); // 立ち上がりで割り込みを設定
}

void loop(){
  if(touchFlag){
    Serial.println("touched!");
    send_line("おはよう",10858,789);
    touchFlag = false;
  } 
}

