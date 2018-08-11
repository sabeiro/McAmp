#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif


#ifndef midi_h
#define midi_h

#include <ctype.h>

#define bit9600Delay 84  
#define halfBit9600Delay 42
#define bit4800Delay 188 
#define halfBit4800Delay 94 


void midiBackgroud();
void midiSetup();
void SWprint(int data);
int SWread();





#endif
