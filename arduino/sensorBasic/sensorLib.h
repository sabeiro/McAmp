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

#define PIN_BUTT 2

extern bool updateTime();
extern void levelLed(float val, float min_val);
extern void printState(float temperature, float poti, float light);
extern void setBase();
extern float analogAverage(int pinN);

extern float tempBase;
extern float lightBase;
extern float voltConv;

extern int startTime;
extern int currTime;
extern int i;
extern int j; 

#endif
