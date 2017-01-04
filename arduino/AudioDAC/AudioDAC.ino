//Simple Audio In w output to 8 bit DAC
//by Amanda Ghassaei
//http://www.instructables.com/id/Arduino-Audio-Input/
//Sept 2012

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

int incomingAudio;

void setup(){
  for (byte i=0;i<8;i++){
    pinMode(i,OUTPUT);//set digital pins 0-7 as outputs (DAC)
  }
}

void loop(){
  incomingAudio = analogRead(A0);//read voltage at A0
  incomingAudio = (incomingAudio+1)/4 - 1;//scale from 10 bit (0-1023) to 8 bit (0-255)
  if (incomingAudio<0){//deal with negative numbers
    incomingAudio = 0;
  }
  PORTD = incomingAudio;
}

