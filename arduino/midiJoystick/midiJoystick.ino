
int butt[] = {5,6,7,8,9,10,11,12};
int pott[] = {A0,A1,A2,A3};
int Nbut = 8;

void setup() {
  Serial.begin(9600);
  Nbut = (int)sizeof(butt);
  for(int i=0;i<Nbut;i++){
    pinMode(butt[i],INPUT);
  }
}

void loop() {
  for(int i=0;i<Nbut;i++){
    if(digitalRead(butt[i]) == HIGH) { // check button state
    delay(10); // software de-bounce
    if(digitalRead(butt[i]) == HIGH) { // check button state again
      Serial.print(butt[i]);
      Serial.println("Button Works!"); // log result
      delay(250);
    }
   }
  }
}
