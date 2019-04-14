#include "sensorLib.h"

float temperature = 0.;
float poti = 0.;
float light = 0.;

void setup() {
  Serial.begin(9600);
  startTime = millis();
  for(int i=0;i<N_LED;i++){
    pinMode(FIRST_LED+i,OUTPUT);
    digitalWrite(FIRST_LED+i,LOW);
  }
  pinMode(2,INPUT); // button
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
}

void loop() {
  temperature = analogAverage(PIN_TEMP);
  poti = analogAverage(PIN_POTI);
  light = analogAverage(PIN_LIGHT);
  temperature = (temperature*voltConv - 0.5)*100.;
  measureDistance();
  if(updateTime()){
    panicButton();
    levelLed(temperature,tempBase);
    //levelLed(light,lightBase);
    printState(temperature,poti,light);
    setBase();
    playTone(light);
  }
  delay(5);
}
