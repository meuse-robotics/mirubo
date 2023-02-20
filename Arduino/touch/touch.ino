// スイッチに使用するPinの設定
#define TOUCH_PIN 22
volatile bool touchFlag = false;

void touched(){ // 割り込み関数
  touchFlag = true;
}

void setup() {
  // タッチPinを入力に設定
  pinMode(TOUCH_PIN,INPUT);
  // 立ち上がりで割り込みを設定
  attachInterrupt(TOUCH_PIN, touched, RISING);
  // シリアルモニタの通信速度
  Serial.begin(115200);
}

void loop() {
  if(touchFlag){
    Serial.println("touched!");
    touchFlag = false;
  }  
}

