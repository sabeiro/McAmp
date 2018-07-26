// This sketch for Arduino implements a very simple control surface with a MIDI interface
// for the Reaper Digital Audio Workstation from Cockos http://reaper.fm
//
// 3 dedicated buttons for Play/Pause, Stop and Record.
// 2 unassigned buttons for assigning to Actions in Reaper.
// 2x16 LCD displays track name in the top line and time code in the bottom line
// Last two chars of the bottom line for Record and Solo indicators.
//
// Emulates a Frontier AlphaTrack MIDI control surface
// (partially) so it can be used with Reaper.
// Works with DFRobot LCD Keypad Shield for Arduino and Duemilanove.
// Requires MIDI and LCD4Bit_mod libraries. Caution: you may have to modify these libs if you are building with Arduino 1.0, 
// else you will get compile errors about WConstants.h. 
// Hopefully the authors of those libraries will make them conpatible with Ardiuino 1.0 soon
//
// Tested with XP, Reaper 3.7.5, Roland Serial Midi Driver, Duemilanove and DFRobot LCD Keypad Shield.
//
// Protocol info at http://www.reaper.fm/sdk/plugin/plugin.php 
// and http://frontierdesign.com/download/pdf/AlphaTrack/AlphaTrack_Native_1.0.pdf
//
// NOTE: requires special setup on the PC to support MIDI over USB-Serial at 31250 baud
// You MUST install the FTDI driver and set it for one of COM1 to COM4, 38400 baud.
// You MUST install a USB-MIDI driver, such as the the Roland one described below:
// http://tomscarff.110mb.com/USB_MIDI/USB_MIDI.htm
//
// You MUST also 
// alter the FTDI driver to actually do 31250 baud when it is set for 38400, and configure the 
// driver for 38400 baud as described below:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1161023163/0
//
// In Reaper Options->Preference->Control Surfaces, 
// you MUST configure the control surface to be the Frontier Alpha track, set the
// Midi input and Midi Output to the Roland Serial Midi and also set the Control Surface Display 
// Update Frequency to 8Hz (not the default 15) else you will get
// display corruption due to MIDI serial buffer overruns.
//
// The Right button on the shield sends the AlphaTrack Record message (NoteOn Note 95)
// The Up button on the shield sends the AlphaTrack Play message (NoteOn Note 94)
// The Down button on the shield sends the AlphaTrack Stop message (NoteOn Note 93)
// The Left button on the shield sends NoteOn Note 2
// The Select button on the shield sends NoteOn Note 1
//
// You can use the Reaper Actions menu to assign the Select and Left buttons on the shield to any 
// Reaper action you like. In Reaper they are named 'MIDI Chan 1 Note 1' and 'MIDI Chan 1 Note 2'
//
// Copyright (C) 2011 Mike McCauley mikem@open.com.au

#include <LCD4Bit_mod.h> 
#include <MIDI.h>

int  adc_key_val[] ={ 30, 150, 360, 535, 760};
#define NUM_KEYS (sizeof(adc_key_val) / sizeof(int))
#define KEY_RIGHT  0
#define KEY_UP     1
#define KEY_DOWN   2
#define KEY_LEFT   3
#define KEY_SELECT 4

// LCD Shield is a 2 line display.
LCD4Bit_mod lcd = LCD4Bit_mod(2); 

void setup() 
{
  lcd.init();
  lcd.clear();
  lcd.printIn("Initialising...");
  MIDI.begin();
  MIDI.turnThruOff();
  delay(1000);
  lcd.clear();
}

void loop() 
{
  // Look for a keypress on the LCD Shield and send a MIDI event
  // Reaper does not seem to need or want button up events, 
  // so we just send button down events as NoteOn
  uint8_t key = read_key();
  switch (key)
  {
    case KEY_RIGHT:
      MIDI.sendNoteOn(0x5f, 127, 1); // AlphaTrack Record
      break;
      
    case KEY_UP:
      MIDI.sendNoteOn(0x5e, 127, 1); // AlphaTrack Play
      break;
        
    case KEY_DOWN:
      MIDI.sendNoteOn(0x5d, 127, 1); // AlphaTrack Stop
      break;
              
    case KEY_LEFT:
      MIDI.sendNoteOn(0x02, 127, 1); // 'MIDI Chan 1 Note 2' For custom actions
      break;
      
    case KEY_SELECT:
      MIDI.sendNoteOn(0x01, 127, 1); // 'MIDI Chan 1 Note 1' For custom actions
      break;
  }

  // Look for MIDI events from the host
  if (MIDI.read())
  {
    uint8_t type = MIDI.getType() & 0xf0; // Low order bits ignored
    switch (type)
    {
      case SystemExclusive:
        {
          byte* p = MIDI.getSysExArray();
          if (p[1] == 0x00 && p[2] == 0x01 && p[3] == 0x40 && p[4] == 0x20 && p[5] == 0x00)
          {
            // Its an AlphaTrack LCD display message. Fully supported.
            uint8_t i;
            uint8_t length = MIDI.getData1();
            uint8_t col = p[6]; // Starting position, 0 to 31
            uint8_t line = 1;
            if (col >= 16)
            {
              // Wrap to next line
              line++;
              col -= 16;
            }
            // Display the text
            lcd.cursorTo(line, col);
            // The rest of the SysEx is the LCD display text
            for (i = 7; i < (length-1); i++)
            {
              lcd.print(p[i]);
              // It can wrap
              if (++col >= 16)
              {
                line++;
                col -= 16;
                lcd.cursorTo(line, col);                delay(10);
              }
            }
          }
          else
          {
#if 0
            // Unrecognised, Unsupported SysEx
            char buf[100];
            lcd.cursorTo(1, 0);
            sprintf(buf, "s:%x %x %x %x       ", p[1], p[2], p[3], p[4]);
            lcd.printIn(buf);
#endif
          }
        }
        break;
     
     case PitchBend: // Fader movement, with low nibble. Not supported.
       break;
       
     case NoteOn: // LED output, partially supported
       // Place these indicators at the end of the second line which is not used by Reaper
       if (MIDI.getData1() == 0x5f)
       {
         // Record LED
         lcd.cursorTo(2, 15);
         if (MIDI.getData2() == 0x7f)
           lcd.print('R');
         else
           lcd.print(' ');
       }
       else if (MIDI.getData1() == 0x73)
       {
         // Solo LED
         lcd.cursorTo(2, 14);
         if (MIDI.getData2() == 0x7f)
           lcd.print('s');
         else
           lcd.print(' ');
       }
#if 0
       else
       {
         // Some other unsupported LED
         char buf[100];
         lcd.cursorTo(1, 0);
         sprintf(buf, "n:%x %x %x %x       ", MIDI.getType(), MIDI.getChannel(), MIDI.getData1(), MIDI.getData2());
         lcd.printIn(buf);
       }
#endif
       break;
       
     default: // Unexpected and unsupported event type
#if 0
        char buf[100];
        lcd.cursorTo(1, 0);
        sprintf(buf, "e:%x %x %x %x       ", MIDI.getType(), MIDI.getChannel(), MIDI.getData1(), MIDI.getData2());
        lcd.printIn(buf);
#endif
        break;
     
    }
  }
}

// Return a keycode for the current debounced keypress, 0 to 4, else -1
uint8_t read_key()
{
  static uint8_t oldkey = -1;
  int adc_key_in = analogRead(0);    // read the value from the sensor  
  uint8_t key = get_key(adc_key_in); // convert into key press	
  if (key != oldkey)		     // if keypress is detected
  {
    delay(50);		             // wait for debounce time
    adc_key_in = analogRead(0);      // read the value from the sensor  
    key = get_key(adc_key_in);       // convert into key press
    if (key != oldkey)				
    {			
      oldkey = key;
      if (key >=0)
         return key;
    }
  }
  return -1;  
}

// Convert ADC value to key number
uint8_t get_key(unsigned int input)
{
  uint8_t k;
    
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])  
      return k;
  }
  return -1; // No valid key
}
