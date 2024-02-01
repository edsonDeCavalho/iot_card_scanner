#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

#define PIN_INFRA_ROUGE 35

DFRobotDFPlayerMini myDFPlayer;
 
void setup() {
    Serial2.begin(9600);
    Serial.begin(115200);
    pinMode(PIN_INFRA_ROUGE, INPUT);

    if (!myDFPlayer.begin(Serial2))
  {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    //für esp8266
    //while(true);
  }else Serial.println(F("DFPlayer Mini online."));
  
   myDFPlayer.volume(15);
  // myDFPlayer.play(1);
}
void mouvemmentDetetion(){
  Serial.println(digitalRead(PIN_INFRA_ROUGE));
  if (digitalRead(PIN_INFRA_ROUGE) == 1){
      Serial.println("Motion detected");
      myDFPlayer.volume(20);
      myDFPlayer.play(1);
  }
  //delay(20);  
}
void loop() {
  mouvemmentDetetion();  
}