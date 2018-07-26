/*  Instructables Logo TFT

    Displays Instructables logo onto the Arduino TFT LCD screen.
    
    Make sure that you load the bitmap file onto the microSD Card before you
    run the sketch!  Open the Serial Monitor to begin.
  
    modified 14 July 13 by Quin (Qtechknow)
    created 19 April 2013 by Enrico Gueli
 */
 
// include the necessary libraries
#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library

// pin definitions
#define sd_cs  4
#define lcd_cs 10
#define dc     9
#define rst    8  

TFT TFTscreen = TFT(lcd_cs, dc, rst);

// this variable represents the image to be drawn on screen
PImage logo;

void setup() {
  
  // initialize the GLCD and show a message
  // asking the user to open the serial line
  
  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);

  TFTscreen.stroke(0, 0, 255);
  TFTscreen.println();
  TFTscreen.println("Arduino TFT Bitmap Example");
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.println("Open serial monitor");
  TFTscreen.println("to run the sketch");
  delay(2000);

  // initialize the serial port: it will be used to 
  // print some diagnostic info  
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial line to be ready
  }

  // clear the GLCD screen before starting
  TFTscreen.background(255, 255, 255);
  
  // try to access the SD card. If that fails (e.g.
  // no card present), the setup process will stop.
  Serial.print("Initializing SD card...");
  if (!SD.begin(sd_cs)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  
  // initialize and clear the GLCD screen
  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);

  // now that the SD card can be access, try to load the
  // image file.
  logo = TFTscreen.loadImage("i.bmp");
  if (!logo.isValid()) {
    Serial.println("error while loading i.bmp");
  }
  if (logo.isValid() == false) {
    return;
  }
  
  Serial.println("drawing image");

  // draw the image to the screen
  TFTscreen.image(logo, 0, 0);
}

void loop() {
  // we don't need anything here
}
