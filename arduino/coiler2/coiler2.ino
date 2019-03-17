#include <LiquidCrystal.h>
#include <Servo.h> 
#include <SoftwareSerial.h>

#include "pitches.h"
#include "rotor.h"

int servoPin = 13; 
int spinPin = 9;
int spin = 0;
int val = 0;
int swit = 0;
Servo Servo1; 

long previousMillis = 0; 
long currentMillis = 0;
long interval = 1000;   

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  lcd.begin(16, 2);
  lcd.print("viudi coiler");
  Servo1.attach(servoPin); 
  Serial.begin(9600);
  pinMode(spinPin, OUTPUT);  
  pinMode(servoPin, OUTPUT);  
  pinMode(8, INPUT);  
  digitalWrite(spinPin, LOW);
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("giri ");
  lcd.print(millis() / 1000);
  val = analogRead(A0);          
  val = map(val, 0, 1023, 0, 180);   
  lcd.print(" ");
  lcd.print(val);
  Servo1.write(val);   
  spin = analogRead(A1);          
  spin = map(spin, 0, 1023, 0, 255);   
  analogWrite(spinPin, spin);
  analogWrite(spinPin, 10);
  analogWrite(spinPin, 11);
  analogWrite(spinPin, 12);
  swit = digitalRead(8);
  //rot1.spin(spin);
  lcd.print(" ");
  lcd.print(spin);
  lcd.print(" ");
  lcd.print(swit);
  tone(9,NOTE_C4);
  tone(10,NOTE_C4);
  tone(11,NOTE_C4);
  tone(12,NOTE_C4);
 
  delay(15);                         
}
