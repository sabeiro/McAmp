/* This is the code for the Arduino Uno, it will not work on the Leonardo. */

/* These are constants: whenever we put one of these 3 words in capitals in our code, the precompiler will replace it by the 0xsomething after the word. 
 This makes our code more readable, instead of typing the meaningless 0x90, we can now just type NOTE_ON, if we want a note to be on. */
#define NOTE_OFF       0x80
#define NOTE_ON        0x90
#define CC             0xB0

/* This is also a constant. If you want to change the number of analog inputs, you can simply do it once on this line, instead of changing it everywhere in your code.*/ 
#define NUMBER_OF_ANALOG_INPUTS  6 // The Uno has 6 analog inputs, we'll use all of them in this example. If you only need 4, change this to 4, and you'll be able to use A4 & A5 as normal I/O pins.

#define NUMBER_OF_DIGITAL_INPUTS 11  // If you use all 6 analog inputs, the Uno has 14 digital pins left. We'll be using pin 13 for the status LED, and pin 0 and 1 are needed for the serial communication, so we can connect 11 switches. Adapt this to your own needs.

#define CHANNEL  1 // Send all messages on channel 1.

/* The list with the corresponding controller numbers: for example, the values of the potentiometer on A0 will be sent as the first controller number in this list, A1 as the second, etc... 
 Here's the list with all controller numbers:  http://midi.org/techspecs/midimessages.php#3  You can change them if you want.*/
int controllers[NUMBER_OF_ANALOG_INPUTS] = { 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15
}; 

/* The list with the corresponding note numbers: for example, the values of the first button will be sent as the first note number in this list, the second switch as the second note, etc... 0x3C is defined in the MIDI implementation as the middle C.
 Here's the list with all note numbers:  http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_note_numbers_for_octaves.htm  You can change them if you want.*/
int notes[NUMBER_OF_DIGITAL_INPUTS] = { 
  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

/* The list with the numbers of the pins that have a switch or push button connected. Make sure they do not interfere with the analog inputs. Adapt to your own needs.*/
int switches[NUMBER_OF_DIGITAL_INPUTS] = { 
  2,3,4,5,6,7,8,9,10,11,12
};


int analogVal[NUMBER_OF_ANALOG_INPUTS];  // We declare an array for the values from the analog inputs

int analogOld[NUMBER_OF_ANALOG_INPUTS]; // We declare an array for the previous analog values. 


int digitalVal[NUMBER_OF_DIGITAL_INPUTS];  // We declare an array for the values from the digital inputs.

int digitalOld[NUMBER_OF_DIGITAL_INPUTS];  // We declare an array for the previous digital values. 


/* The format of the message to send via serial. We create a new data type, that can store 3 bytes at once.  This will be easier to send as MIDI. */
typedef struct {
  uint8_t status;   // first  byte   : status message (NOTE_ON, NOTE_OFF or CC (controlchange) and midi channel (0-15)
  uint8_t data1;    // second byte   : first value (0-127), controller number or note number
  uint8_t data2;    // third  byte   : second value (0-127), controller value or velocity
} 
t_midiMsg;          // We call this data type 't_midiMsg'

void setup() // The setup runs only once, at startup.
{
  pinMode(13, OUTPUT);   // Set pin 13 (the one with the LED) to output
  digitalWrite(13, LOW); // Turn off the LED
  for(int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++){  // We make all values of analogOld -1, so it will always be different from any possible analog reading.
    analogOld[i]=-1;
  }
  for(int i = 0; i < NUMBER_OF_DIGITAL_INPUTS; i++){  // We make the values of digitalOld -1, so it will always be different from any possible digital reading.
    digitalOld[i]=-1;
  }
  
  for(int i = 0; i < NUMBER_OF_DIGITAL_INPUTS; i++){  // Set all switch-pins as input, and enable the internal pull-up resistor.
    pinMode(switches[i], INPUT_PULLUP);
  }
  //Serial.begin(31250);    // Start a serial connection @31250 baud or bits per second on digital pin 0 and 1, this is the connection to the ATmega16U2, which runs the MIDI firmware. (31250 baud is the original MIDI speed.)
  Serial.begin(38400);
  delay(1000);            // Wait a second before sending messages, to be sure everything is set up, and to make uploading new sketches easier.
  digitalWrite(13, HIGH); // Turn on the LED, when the loop is about to start.
}

void loop() // The loop keeps on repeating forever.
{
  digitalWrite(13, HIGH);
  t_midiMsg msg;                                     // create a variable 'msg' of data type 't_midiMsg' we just created
  for(int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++){  // Repeat this procedure for every analog input.

    analogVal[i] = analogRead(i)/8;                  // The resolution of the Arduino's ADC is 10 bit, and the MIDI message has only 7 bits, 10 - 7 = 3, so we divide by 2^3, or 8.
    if(abs(analogVal[i] - analogOld[i]) < 8){                // Only send the value, if it is a different value than last time.
      msg.status = CC;                               // Controll Change
      msg.status = msg.status | CHANNEL-1;           // Channels are zero based (0 = ch1, and F = ch16). Bitwise or to add the status message and channel together: 
                                                    /* status     = 0bssss0000 
                                                     * channel    = 0b0000cccc 
                                                     * | ------------------ (bitwise or)
                                                     * msg.status = 0bsssscccc       
                                                     */
      msg.data1   = controllers[i];                  // Get the controller number from the array above.
      msg.data2   = analogVal[i];                    // Get the value of the analog input from the analogVal array.
      //Serial.write((uint8_t *)&msg, sizeof(msg));    // Send the MIDI message.
      Serial.print(i);
      Serial.println(" qui");
      digitalWrite(13, LOW);
    }
    analogOld[i] = analogVal[i];                     // Put the analog values in the array for old analog values, so we can compare the new values with the previous ones.
  }
  
  for(int i = 0; i < NUMBER_OF_DIGITAL_INPUTS; i++){ // Repeat this procedure for every digital input.
    digitalVal[i] = digitalRead(switches[i]);        // Read the switch and store the value (1 or 0) in the digitalVal array.
    if(digitalVal[i] != digitalOld[i]){              // Only send the value, if it is a different value than last time.
      if(digitalVal[i] == 0){                        // If the i'th switch is pressed:
        msg.status = NOTE_ON;                        // Note on
        msg.status = msg.status | CHANNEL-1;         // Channels are zero based (0 = ch1, and F = ch16). Bitwise or to add the status message and channel together: 
        msg.data1   = notes[i];                      // Get the note number from the array above.
        msg.data2   = 127;                           // Set the velocity to 127 (100%).
        //Serial.write((uint8_t *)&msg, sizeof(msg));  // Send the MIDI message.
      } else {                                       // If the i'th switch is released:
        msg.status = NOTE_OFF;                       // Note off
        msg.status = msg.status | CHANNEL-1;         // Channels are zero based (0 = ch1, and F = ch16). Bitwise or to add the status message and channel together: 
        msg.data1   = notes[i];                      // Get the note number from the array above.
        msg.data2   = 127;                           // Set the velocity to 127 (100%).
        //Serial.write((uint8_t *)&msg, sizeof(msg));  // Send the MIDI message.
      }
      //digitalWrite(13, LOW);
    }
    digitalOld[i] = digitalVal[i];                   // Put the digital values in the array for old digital values, so we can compare the new values with the previous ones.
    delay(10);                                       // Wait for 10ms, so it doesn't flood the computer with MIDI-messages
  }

}
