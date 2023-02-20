// スイッチに使用するPinの設定
#define PIR_PIN 15
volatile bool detectFlag = false;

void detected(){
  detectFlag = true;
}

void setup() {
  // タッチPinを入力に設定
  pinMode(PIR_PIN,INPUT);
  // 立ち上がりで割り込みを設定
  attachInterrupt(PIR_PIN, detected, RISING);
  // シリアルモニタの通信速度
  Serial.begin(115200);
}

void loop() {
  if(detectFlag == true){
    Serial.println("detected!");
    detectFlag = false;
  }  
}

