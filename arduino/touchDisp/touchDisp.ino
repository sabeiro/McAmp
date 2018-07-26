#include <SPFD5408_Adafruit_GFX.h>   
#include <SPFD5408_Adafruit_TFTLCD.h> 
#include <SPFD5408_TouchScreen.h>  
#include <EEPROM.h>  
#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 

#define LCD_RESET A4 

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define YP A1 
#define XM A2 
#define YM 7  
#define XP 6 

#define TS_MINX 600
#define TS_MINY 900
#define TS_MAXX 965
#define TS_MAXY 140

TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);
int wing;
int fx, fy, fallRate;
int pillarPos, gapPos;
int score;
int highScore = 0;
bool running = false;
bool crashed = false;
bool scrPress = false;
long nextDrawLoopRunTime;
void setup() {
  
  tft.reset();
  tft.begin(0x9341); 

  tft.setRotation(3); 
  
  tft.fillScreen(BLACK);
  tft.setCursor (55, 50);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.println("Arduino");
  tft.setCursor (85, 85);
  tft.setTextSize (2);
  tft.setTextColor(WHITE);
  tft.println("team");
  tft.setCursor (55, 120);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("POLITEHNIKA");
  waitOneTouch(true);
  tft.fillScreen(BLUE);
  startGame();

}

void loop(void) {
   if (millis() > nextDrawLoopRunTime ) {
      drawLoop();
        checkCollision();
      nextDrawLoopRunTime += 50;
  }
  
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if (!running) {
      // clear text & start scrolling
      tft.fillRect(0,0,320,80,BLUE);
      running = true;
    }
    else
    {
      // fly up
      fallRate = -11;
      scrPress = true;
    }
  }
  else if (p.z == 0 && scrPress) {
    scrPress = false;
  }
 
  
}


TSPoint waitOneTouch(boolean showMessage) {
  uint8_t save = 0;
  if (showMessage) {
    save = tft.getRotation(); 
    tft.setRotation(3); 
    
    tft.setCursor (180, 200);
    tft.setTextSize (1);
    tft.setTextColor(WHITE);
    tft.println("Ceka taster!!!");

  }
  TSPoint p;
  do {
    p= ts.getPoint(); 
  
    pinMode(XM, OUTPUT); 
    pinMode(YP, OUTPUT);
  
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));

  if (showMessage) {
    tft.setRotation(save);
  }
  
  return p;
}

void drawBorder () {
  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(BLUE);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
}

void drawFlappy(int x, int y) {
  // Upper & lower body
  tft.fillRect(x+2, y+8, 2, 10, BLACK);
  tft.fillRect(x+4, y+6, 2, 2, BLACK);
  tft.fillRect(x+6, y+4, 2, 2, BLACK);
  tft.fillRect(x+8, y+2, 4, 2, BLACK);
  tft.fillRect(x+12, y, 12, 2, BLACK);
  tft.fillRect(x+24, y+2, 2, 2, BLACK);
  tft.fillRect(x+26, y+4, 2, 2, BLACK);
  tft.fillRect(x+28, y+6, 2, 6, BLACK);
  tft.fillRect(x+10, y+22, 10, 2, BLACK);
  tft.fillRect(x+4, y+18, 2, 2, BLACK);
  tft.fillRect(x+6, y+20, 4, 2, BLACK);
  
  // Body fill
  tft.fillRect(x+12, y+2, 6, 2, YELLOW);
  tft.fillRect(x+8, y+4, 8, 2, YELLOW);
  tft.fillRect(x+6, y+6, 10, 2, YELLOW);
  tft.fillRect(x+4, y+8, 12, 2, YELLOW);
  tft.fillRect(x+4, y+10, 14, 2, YELLOW);
  tft.fillRect(x+4, y+12, 16, 2, YELLOW);
  tft.fillRect(x+4, y+14, 14, 2, YELLOW);
  tft.fillRect(x+4, y+16, 12, 2, YELLOW);
  tft.fillRect(x+6, y+18, 12, 2, YELLOW);
  tft.fillRect(x+10, y+20, 10, 2, YELLOW);
  
  // Eye
  tft.fillRect(x+18, y+2, 2, 2, BLACK);
  tft.fillRect(x+16, y+4, 2, 6, BLACK);
  tft.fillRect(x+18, y+10, 2, 2, BLACK);
  tft.fillRect(x+18, y+4, 2, 6, WHITE);
  tft.fillRect(x+20, y+2, 4, 10, WHITE);
  tft.fillRect(x+24, y+4, 2, 8, WHITE);
  tft.fillRect(x+26, y+6, 2, 6, WHITE);
  tft.fillRect(x+24, y+6, 2, 4, BLACK);
  
  // Beak
  tft.fillRect(x+20, y+12, 12, 2, BLACK);
  tft.fillRect(x+18, y+14, 2, 2, BLACK);
  tft.fillRect(x+20, y+14, 12, 2, RED);
  tft.fillRect(x+32, y+14, 2, 2, BLACK);
  tft.fillRect(x+16, y+16, 2, 2, BLACK);
  tft.fillRect(x+18, y+16, 2, 2, RED);
  tft.fillRect(x+20, y+16, 12, 2, BLACK);
  tft.fillRect(x+18, y+18, 2, 2, BLACK);
  tft.fillRect(x+20, y+18, 10, 2, RED);
  tft.fillRect(x+30, y+18, 2, 2, BLACK);
  tft.fillRect(x+20, y+20, 10, 2, BLACK);
}
void startGame() {
  int fx=50;
  int fy=125;
  int fallRate=1;
  int pillarPos = 320;
  int gapPos = 60;
  bool crashed = false;
  int score = 0;
  
  tft.fillScreen(BLUE); 
  int ty = 230;
  for (int tx = 0; tx <= 300; tx +=20) {
    tft.fillTriangle(tx,ty, tx+10,ty, tx,ty+10, GREEN);
    tft.fillTriangle(tx+10,ty+10, tx+10,ty, tx,ty+10, YELLOW);
    tft.fillTriangle(tx+10,ty, tx+20,ty, tx+10,ty+10, YELLOW);
    tft.fillTriangle(tx+20,ty+10, tx+20,ty, tx+10,ty+10, GREEN);
  }
}
void drawPillar(int x, int gap) {
  tft.fillRect(x+2, 2, 46, gap-4, GREEN);
  tft.fillRect(x+2, gap+92, 46, 136-gap, GREEN);
  
  tft.drawRect(x,0,50,gap,BLACK);
  tft.drawRect(x+1,1,48,gap-2,BLACK);
  tft.drawRect(x, gap+90, 50, 140-gap, BLACK);
  tft.drawRect(x+1,gap+91 ,48, 138-gap, BLACK);
}

void clearPillar(int x, int gap) {
  tft.fillRect(x+45, 0, 5, gap, BLUE);
  tft.fillRect(x+45, gap+90, 5, 140-gap, BLUE);
}

void clearFlappy(int x, int y) {
 tft.fillRect(x, y, 34, 24, BLUE); 
}

void checkCollision() {
  // Collision with ground
  if (fy > 206) crashed = true;
  
  // Collision with pillar
  if (fx + 34 > pillarPos && fx < pillarPos + 50)
    if (fy < gapPos || fy + 24 > gapPos + 90)
      crashed = true;
  
  if (crashed) {
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(75, 75);
    tft.print("Game Over!");
    tft.setCursor(75, 125);
    tft.print("Score:");
    tft.setCursor(220, 125);
    tft.print(score);
    
    // stop animation
    running = false;
    
    // delay to stop any last minute clicks from restarting immediately
    delay(1000);
  }
}
void drawLoop() {
  // clear moving items
  clearPillar(pillarPos, gapPos);
  clearFlappy(fx, fy);

  // move items
  if (running) {
    fy += fallRate;
    fallRate++;
  
    pillarPos -=5;
    if (pillarPos == 0) {
      score++;
    }
    else if (pillarPos < -50) {
      pillarPos = 320;
      gapPos = random(20, 120);
    }
  }

  // draw moving items & animate
  drawPillar(pillarPos, gapPos);
  drawFlappy(fx, fy);
 
}

