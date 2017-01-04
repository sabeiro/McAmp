//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License - Please reuse change and share
//Simple code to output a PWM sine wave signal on pin 9
//////////////////////////////////////////////////////////////////
#define fadePin 3
int turns = 0; // how fast the motor runs
int turnAmount = 1; // how many turns the motor makes
unsigned long currentTime;
unsigned long loopTime;
byte noteON = 144;//note on command
int potPin = A0;
byte commandByte;
byte noteByte;
byte velocityByte;
byte noteOn = 144;
int note = 60;//middle c
int velocity = 100;
int pitchbend = 224;//224 = 11100000 in binary, pitchbend command
 //pitchbend data storage:
int lsb = 0;//least siginificant bit of pitchbend message
int msb = 0;//most significant bit of pitchbend message


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


//light up led at pin 13 when receiving noteON message with note = 60

void setup(){
  Serial.begin(31250);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
//  Serial.begin(9600);
  pinMode(fadePin, OUTPUT);  
  pinMode(9, OUTPUT);
  currentTime = millis();
  loopTime = currentTime; 
  Serial.begin(31250);
  cli();//stop interrupts
  //set timer2 interrupt every 128us
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 7.8khz increments
  OCR2A = 255;// = (16*10^6) / (7812.5*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS11 bit for 8 prescaler
  TCCR2B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts
}

void checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      if (commandByte == noteOn){//if note on message
        //check if note == 60 and velocity > 0
        if (noteByte == 60 && velocityByte > 0){
          digitalWrite(13,HIGH);//turn on led
        }
      }
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}
    

void loop(){
    int velocity = 20;//set velocity to 20
  for (int note=50;note<70;note++) {//from note 50 (D3) to note 69 (A4)
    MIDImessage(noteON, note, velocity);//turn note on
    delay(300);//hold note for 300ms
    MIDImessage(noteON, note, 0);//turn note off
    delay(200);//wait 200ms until triggering next note
    velocity += 5;//ad 5 to current velocity value
  }
  checkMIDI();
  delay(100);
  digitalWrite(13,LOW);//turn led off
//  constrain((sin(rad) * 128) + 128, 0, 255);
  for(int i = 0; i<nNote; i++){
    //Serial.println(i);
    //analogWrite(3,nSeq1[i]);
    //analogWrite(6,nSeq4[i]);
    analogWrite(9,nSeq4[i]);
    analogWrite(11,nSeq1[i]);
    delay(100*tSeq[i]);
  }
  //Serial.println(analogRead(A5)); 
  delay(500); 
}


ISR(TIMER2_COMPA_vect) {//checks for incoming midi every 128us
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}
void rotation(){
//digitalWrite(fadePin, HIGH);
// delay(1000); 
// digitalWrite(fadePin, LOW);
// delay(1000);
// 
//   currentTime = millis();
//   if(currentTime >= (loopTime + 20)){
//       // set the speed of pin 9:
//       analogWrite(fadePin, turns);
//       // change the turnings for next time through the loop:
//       turns = turns + turnAmount;
//       // speed up or slow down the motor
//       if (turns == 0 || turns == 255) {
//           turnAmount = -turnAmount ;
//       }
//       if (turns == 0) {
//           delay(5000);
//       }
//       loopTime = currentTime; // Updates loopTime
//   }
//  int potVal = analogRead(potPin);//read data from potentiometer
//  //we have to scale the potentiometer data to fit between 0 and 127 (this is the range of MIDI notes)
//  byte note = map(potVal, 0, 1023, 0, 127);
//  MIDImessage(noteON, note, 100);//turn note on
//  delay(300);//hold note for 300ms
//  MIDImessage(noteON, note, 0);//turn note off (note on with velocity 0)
//  delay(200);//wait 200ms until triggering next note
  
}


//send MIDI message
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command 
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}

