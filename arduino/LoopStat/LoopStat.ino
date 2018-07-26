#include <TFT.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#define cs   12
#define dc   10
#define rst  11

int val = 0;
int i = 0;
int digP[] = {2,3,4,5,6};
int anP[] = {A0,A1,A2,A3};
int Npin = 5;
TFT TFTscreen = TFT(cs, dc, rst);
char sensorPrintout[4];

//software serial
//SoftwareSerial midiSerial(2, 3); // digital pins that we'll use for soft serial RX & TX
 

void setup(){
  Serial.begin(9600);
  Serial.println("start");
  for(i=0;i<Npin;i++){
     //pinMode(digP[i],INPUT_PULLUP);
     //digitalWrite(digP[i],HIGH);
     
  }
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255,255,255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Sensor Value :\n ",0,0);
  TFTscreen.setTextSize(5);
}

void printScreen() {
  String sensorVal = String(analogRead(A0));
  sensorVal.toCharArray(sensorPrintout, 4);
  TFTscreen.stroke(255,255,255);
  TFTscreen.text(sensorPrintout, 0, 20);
  delay(250);
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(sensorPrintout, 0, 20);
  TFTscreen.setTextSize(3);
  TFTscreen.text("joy-IT",0,0);
  TFTscreen.setTextSize(2);
  TFTscreen.text("1.8 TFT LCD",0,40);
  TFTscreen.setTextSize(1);
  TFTscreen.text("ST7735",0,60);
}

void parseController(){
    for(i=0;i<Npin;i++){
    val = digitalRead(digP[i]);
    if(val<1){
      Serial.print(val);
      Serial.print(" ");
      Serial.println(i);
    }   
  }
}

void parseTrimmer(){
   for(i=0;i<4;i++){
    val = analogRead(anP[i]);
    if(val<1){
      Serial.print(val);
        Serial.print(" ");
       Serial.println(i);
    }   
  }
}


void loop(){
  //parseMidi();
  //  sendMidi(0x80,82,90);
  //  midiSeq();
  printScreen();
  delay(100);
}

