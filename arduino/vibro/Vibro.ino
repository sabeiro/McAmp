//#include <TimerOne.h>
#include "pitches.h"

int nNote = 12;
int nSeq1[] = {
  20,140,100,130,90,147,89,200,220,140,160,230
};
int nSeq2[] = {
  20,40,60,30,90,47,89,20,22,34,60,30
};
int nSeq3[] = {
  40,60,120,50,110,67,109,220,240,160,180,250
};
int nSeq4[] = {
  60,80,140,70,130,77,129,240,250,180,220,250
};
int tSeq[] = {
  5,4,6,9,7,8,7,3,8,10,3,7
};



#define RESOLUTION 40 //Microsecond resolution for notes

//Setup pins (Even-odd pairs for step control and direction
void setup(){
  pinMode(3, OUTPUT); // Direction 1
  pinMode(6, OUTPUT); // Direction 2
  pinMode(9, OUTPUT); // Step control 3
  pinMode(11, OUTPUT); // Step control 3

//  Timer1.initialize(RESOLUTION); // Set up a timer at the defined resolution
//  Timer1.attachInterrupt(tick); // Attach the tick function

  Serial.begin(9600);
}


void loop(){
    //for(int i = 0; i<nNote; i++){
    //Serial.println(i);
    //analogWrite(3,nSeq1[i]);
    //analogWrite(6,nSeq4[i]);
    //analogWrite(9,nSeq1[i]);
    //analogWrite(11,nSeq1[i]);
    //delay(100*tSeq[i]);
  //}
  //Serial.println(analogRead(A5)); 
  delay(500); 

  //int sensorValue = analogRead(A0)/4;
  //analogWrite(3,sensorValue);
  Serial.println(analogRead(A0));
}


/*
Called by the timer inturrupt at the specified resolution.
*/
void tick(){
}


