//////////////////////////////////////////////////////////////////
//©2017 viudi
//Released under the MIT License - Please reuse change and share
//Midi controller for dc motors
//////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>

#include "pitches.h"
#include "rotor.h"
#include "low_midi.h"

Rotor rot1(5,A5,200);
Rotor rot2(6,A4,200);
Rotor rot3(9,A3,200);
Rotor rot4(11,A1,200);

SoftwareSerial mySerial(2,1);// RX,TX
long previousMillis = 0; 
long interval = 1000;    


void setup(){
  Serial.begin(9600);
  Serial.println(" - midi initiated - ");
  currentTime = millis();
  loopTime = currentTime; 
  for(int i=0;i<13;i++){digitalWrite(i, LOW);}
  mySerial.begin(31250);
  setPwmFrequency(5, 8);//base freq 31250
  setPwmFrequency(9, 1);//base freq 31250
  setPwmFrequency(11,1);//base freq 31250
  setPwmFrequency(6, 1);//base freq 62500
  midiSetup();
}

void playMidi(){
  //midiChannel = commandByte & B00001111;
  //midiCommand = commandByte & B11110000;
  switch(commandByte){
    case 0x90:
      rot4.spin(noteByte);
    break;
    case 0x91:
      rot3.spin(noteByte);
    break;
    case 0x92:
      rot2.spin(noteByte);
    break;
    case 0x93:
      rot1.spin(noteByte);
    break;
    case 0x80:
      rot4.down(noteByte);
    break;
    case 0x81:
      rot3.down(noteByte);
    break;
    case 0x82:
      rot2.down(noteByte);
    break;
    case 0x83:
      rot1.down(noteByte);
    break;
    default:
      //Serial.println(commandByte,HEX);
    break;
  }
  //delay(100);
}

void parseMidi(){
  do{
    if (mySerial.available()){
      commandByte = mySerial.read();//read first byte
      noteByte = mySerial.read();//read next byte
      velocityByte = mySerial.read();//read final byte
      playMidi();
      loopTime = millis();
    }
  }
  while (mySerial.available() > 2);//when at least three bytes available
}


void sendMidi(int command, int MIDInote, int MIDIvelocity) {
  mySerial.write(command);//send note on or note off command 
  mySerial.write(MIDInote);//send pitch data
  mySerial.write(MIDIvelocity);//send velocity data
}

void exLoop(){
  for(int i = 0; i<nNote; i++){
    //Serial.println(i);
    rot1.spin(nSeq1[i]);
    rot2.spin(nSeq2[i]);
    rot3.spin(nSeq3[i]);
    rot4.spin(nSeq4[i]);
    delay(1000*tSeq[i]);
  }
}

void potLoop(){
  rot1.playPot();
  rot2.playPot();
  rot3.playPot();
  rot4.playPot();
  delay(10000);//wait 200ms until triggering next note
}

void stopAll(){
  rot1.stop();
  rot2.stop();
  rot3.stop();
  rot4.stop();
}
void tuneAll(){
  currentTime = millis();
  if(currentTime - loopTime > 1000){
    loopTime = currentTime; 
    rot1.ampPot();
    rot2.ampPot();
    rot3.ampPot();
    rot4.ampPot();
  }
}

void panicButton(){
  if(analogRead(A1)<150){
    stopAll();
  }  
}


void loop(){
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
    parseMidi();
    tuneAll();
    //potLoop();
    //exLoop();
    panicButton();
    //delay(0);
  }
}



