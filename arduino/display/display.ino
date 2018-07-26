#include <TFT.h>
#include <SPI.h>

#define cs   10
#define dc   8
#define rst  9
TFT TFTscreen = TFT(cs, dc, rst);

void setup () {
  TFTscreen.begin();
//Background: black
  TFTscreen.background(0, 0, 0);
//Textcolor: white
  TFTscreen.stroke(255, 255, 255);
}

void loop() {
//set font-size
  TFTscreen.setTextSize(3);
//print text
  TFTscreen.text("joy-IT", 0, 0);
//set font-size
  TFTscreen.setTextSize(2);
//print text
  TFTscreen.text("1.8 TFT LCD", 0, 40);
//set font-size
  TFTscreen.setTextSize(1);
//print text
  TFTscreen.text("ST7735", 0, 60);
  delay(250);
}
