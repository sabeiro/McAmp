#include <Arduino.h>

#ifndef SENSORLIB_H
#define SENSORLIB_H

#define N_AVERAGE 10
#define N_UPDATE 100
#define N_LED 4
#define FIRST_LED 9

#define PIN_TEMP A0
#define PIN_POTI A1
#define PIN_LIGHT A2

#define PIN_BUZZ 3
#define PIN_BUTT 2
#define PIN_TRIG 6
#define PIN_ECHO 7

extern bool updateTime();
extern void levelLed(float val, float min_val);
extern void printState(float temperature, float poti, float light);
extern void setBase();
extern float analogAverage(int pinN);
extern void playTone(float val);
extern void panicButton();
extern void measureDistance();

extern float tempBase;
extern float lightBase;
extern float voltConv;

extern int startTime;
extern int currTime;
extern int i;
extern int j; 
extern bool panic;

#endif
