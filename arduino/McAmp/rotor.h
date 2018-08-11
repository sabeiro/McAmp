#include "Arduino.h"

#ifndef Rotor_h
#define Rotor_h

class Rotor{
  public:
    Rotor(int out_pin,int in_pin,int _offset);
    bool spin(int vel);
    bool down(int note);
    void stop();
    void playPot();
    void ampPot();
  private:
    int _in_pin;
    int _out_pin;
    int potVal;
    int vel;
    int c_note;
    int amp;
    int _offset;
    bool state;
};

void setPwmFrequency(int pin, int divisor);


#endif



