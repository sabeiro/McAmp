#include "sensorLib.h"
#include "pitches.h"

float tempBase = 0.;
float lightBase = 0.;
float voltConv = 5./1024.;

int startTime = 0;
int currTime = 0;
int i = 0;
int j = 0; 
int note = 0;
bool panic = 1;

long duration=0, cm=0;

int mean = 0;
float delta = 1./(float)N_AVERAGE;

void panicButton(){
  int state = digitalRead(PIN_BUTT);
  if(state==HIGH){
    panic = !panic;
  }
}

float analogAverage(int pinN){
  mean = 0;
  for(i=0;i<N_AVERAGE;i++){
    mean += analogRead(pinN);
    delay(5);
  }
  return (float)mean*delta;
}

bool updateTime(){
  currTime = millis();
  if(currTime - startTime < N_UPDATE){
    return 0;
  }
  else{
    startTime = currTime;
    return 1;
  }
} 

void levelLed(float val, float min_val){
  mean = (val - min_val)*.5/(float)N_LED;
  j = (int)mean;
  for(i=0;i<N_LED;i++){
    if(i > j){
      digitalWrite(FIRST_LED + i,HIGH);
    }
    else{
      digitalWrite(FIRST_LED + i,HIGH);
    }
  }
}

void setBase(){
  int state = digitalRead(PIN_BUTT);
  if(state==LOW){
    tempBase = analogRead(PIN_TEMP);
    lightBase = analogRead(PIN_LIGHT);
  } 
}

void printState(float temperature, float poti, float light){
  Serial.print("temperature: ");
  Serial.print(temperature);
  Serial.print("; poti: ");
  Serial.print(poti);
  Serial.print("; light: ");
  Serial.print(light);
  Serial.print("; panic:");
  Serial.print(panic);
  Serial.print("; distance: ");
  Serial.print(cm);
  Serial.println(""); 
}

void playTone(float val){
  if(!panic){
    note = map((int)val,0,1024,0,88);
    tone(PIN_BUZZ,sNotePitches[note]);
  }
  else{
    noTone(PIN_BUZZ);
  }
}

void measureDistance(){
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT);
  duration = pulseIn(PIN_ECHO, HIGH);
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  delay(5);
}
