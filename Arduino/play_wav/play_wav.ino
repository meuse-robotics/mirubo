#include "DFRobotDFPlayerMini.h"

HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

void init_dfplayer(){
  mySerial.begin(9600);
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
void setup()
{
  Serial.begin(115200);
  
  init_dfplayer();
  myDFPlayer.play(1);  //Play the first mp3
}

void loop(){}
