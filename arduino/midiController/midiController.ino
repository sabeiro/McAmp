#include "midi_code.h"

void setup()
{
  // Taken from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
  #ifdef FASTADC
    // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
  #endif
  
  // Only enable serial on the Arduino or when debugging. The Teensy board should be set as a usb-midi device so serial is not needed.
  #if defined(ARDUINO) || defined(ARDUINO_MEGA) || defined(DEBUG)
    // Enable serial I/O at 115200 kbps. This is faster than the standard MIDI rate of 31250 kbps.
    // The PC application which we connect to will automatically take the higher sample rate and send MIDI
    // messages out at the correct rate. We only send things faster in case there is any latency.
    Serial.begin(115200);
  #endif
  
  // Initialise each digital input channel.
  for (i = 0; i < NUM_DI; i++)
  {
    // Set the pin direction to input.
    pinMode(digitalInputMapping[i], INPUT);

    // Don't enable pullup resistor on LED_PIN, as the LED and resistor will always pull it low, meaning the input won't work.
    // Instead an external pulldown resistor must be used on LED_PIN.
    // NOTE: This will cause all of the high/low logic for LED_PIN to be inverted.
    if (digitalInputMapping[i] != LED_PIN)
    {
      // Enable the pull-up resistor. This call must come after the above pinMode call.
      digitalWrite(digitalInputMapping[i], HIGH);
    }
    
    // Initialise the digital state with a read to the input pin.
    digitalInputs[i] = digitalRead(digitalInputMapping[i]);
  }
  
  // Initialise each analogue input channel.
  for (i = 0; i < NUM_AI; i++)
  {
    // Set the pin direction to input.
    pinMode(analogueInputMapping[i], INPUT);
    
    // Initialise the analogue value with a read to the input pin.
    analogueInputs[i] = analogRead(analogueInputMapping[i]);
    
    // Assume no analogue inputs are active
    analogueInputChanging[i] = false;
    analogueInputTimer[i] = 0;
  }
  
  #ifdef DEBUG
    serialSendTime = millis();
  #endif
}


void loop()
{
  #ifdef DEBUG
    loopTime = micros();
  #endif
  
  for (i = 0; i < NUM_DI; i++)
  {
    #ifdef MIDI_FIGHTER
      if (i >= SKIP_ROW * 4)
      {
        digitalOffset = i + 4;
      }
      else
      {
    #endif
 
    digitalOffset = i;
    
    #ifdef MIDI_FIGHTER
      }
    #endif
    
    // Read the current state of the digital input and store it temporarily.
    tempDigitalInput = digitalRead(digitalInputMapping[i]);
    
    // Check if the last state is different to the current state.
    if (digitalInputs[i] != tempDigitalInput)
    {
      #ifdef DEBOUNCE
      // Wait for a short period of time, and then take a second reading from the input pin.
      delay(DEBOUNCE_LENGTH);
      // If the second reading is the same as the initial reading, assume it must be true.
      if (tempDigitalInput == digitalRead(digitalInputMapping[i]))
      {
      #endif
        // Record the new digital input state.
        digitalInputs[i] = tempDigitalInput;
        
        // Moved from HIGH to LOW (button pressed)
        if (digitalInputs[i] == 0)
        {
          // All the digital inputs use pullup resistors, except LED_PIN so the logic is inverted
          if (digitalInputMapping[i] != LED_PIN)
          {
            noteOn(MIDI_CHANNEL, NOTE + digitalOffset, 0x7F); // Channel 1, middle C, maximum velocity
          }
          else
          {
            noteOff(MIDI_CHANNEL, NOTE + digitalOffset); // Channel 1, middle C
          }
        }
        // Moved from LOW to HIGH (button released)
        else
        {
          // All the digital inputs use pullup resistors, except LED_PIN so the logic is inverted
          if (digitalInputMapping[i] != LED_PIN)
          {
            noteOff(MIDI_CHANNEL, NOTE + digitalOffset); // Channel 1, middle C
          }
          else
          {
            noteOn(MIDI_CHANNEL, NOTE + digitalOffset, 0x7F); // Channel 1, middle C, maximum velocity
          }
        }
      #ifdef DEBOUNCE
      }
      #endif
    }
  }
  
  /*
   * Analogue input logic:
   * The Arduino uses a 10-bit (0-1023) analogue to digital converter (ADC) on each of its analogue inputs.
   * The ADC isn't very high resolution, so if a pot is in a position such that the output voltage is 'between'
   * what it can detect (say 2.505V or about 512.5 on a scale of 0-1023) then the value read will constantly
   * fluctuate between two integers (in this case 512 and 513).
   *
   * If we're simply looking for a change in the analogue input value like in the digital case above, then
   * there will be cases where the value is always changing, even though the physical input isn't being moved.
   * This will in turn send out a constant stream of MIDI messages to the connected software which may be problematic.
   *
   * To combat this, we require that the analogue input value must change by a certain threshold amount before
   * we register that it is actually changing. This is good in avoiding a constantly fluctuating value, but has
   * the negative effect of a reduced input resolution. For example if the threshold amount was 2 and we slowly moved
   * a slider through it's full range, we would only detect every second value as a change, in effect reducing the
   * already small 7-bit MIDI value to a 6-bit MIDI value.
   *
   * To get around this problem but still use the threshold logic, a timer is used. Initially the analogue input
   * must exceed the threshold to be detected as an input. Once this occurs, we then read every value coming from the
   * analogue input (not just those exceeding a threshold) giving us full 7-bit resolution. At the same time the
   * timer is started. This timer is used to keep track of whether an input hasn't been moved for a certain time
   * period. If it has been moved, the timer is restarted. If no movement occurs the timer is just left to run. When
   * the timer expires the analogue input is assumed to be no longer moving. Subsequent movements must exceed the
   * threshold amount.
   */
  for (i = 0; i < NUM_AI; i++)
  {
    // Read the analogue input pin, dividing it by 8 so the 10-bit ADC value (0-1023) is converted to a 7-bit MIDI value (0-127).
    tempAnalogueInput = analogRead(analogueInputMapping[i]) / 8;
    
    #ifdef ANALOGUE_FILTER
    // Take the absolute value of the difference between the curent and new values 
    analogueDiff = abs(tempAnalogueInput - analogueInputs[i]);
    // Only continue if the threshold was exceeded, or the input was already changing
    if ((analogueDiff > 0 && analogueInputChanging[i] == true) || analogueDiff >= FILTER_AMOUNT)
    {
      // Only restart the timer if we're sure the input isn't 'between' a value
      // ie. It's moved more than FILTER_AMOUNT
      if (analogueInputChanging[i] == false || analogueDiff >= FILTER_AMOUNT)
      {
        // Reset the last time the input was moved
        analogueInputTimer[i] = micros();
        
        // The analogue input is moving
        analogueInputChanging[i] = true;
      }
      else if (micros() - analogueInputTimer[i] > ANALOGUE_INPUT_CHANGE_TIMEOUT)
      {
        analogueInputChanging[i] = false;
      }
      
      // Only send data if we know the analogue input is moving
      if (analogueInputChanging[i] == true)
      {
        // Record the new analogue value
        analogueInputs[i] = tempAnalogueInput;
      
        // Send the analogue value out on the general MIDI CC (see definitions at beginning of this file)
        controlChange(MIDI_CHANNEL, MIDI_CC + i, analogueInputs[i]);
      }
    }
    #else
    if (analogueInputs[i] != tempAnalogueInput)
    {
      // Record the new analogue value
      analogueInputs[i] = tempAnalogueInput;
      
      // Send the analogue value out on the general MIDI CC (see definitions at beginning of this file)
      controlChange(MIDI_CHANNEL, MIDI_CC + i, analogueInputs[i]);
    }
    #endif
  }
  
  #ifdef DEBUG
  loopTime = micros() - loopTime;
  
  // Print the loop execution time once per second
  if (millis() - serialSendTime > 1000)
  {
    Serial.print("Loop execution time (us): ");
    Serial.println(loopTime);
    
    serialSendTime = millis();
  }
  #endif
}

// Send a MIDI note on message
void noteOn(byte channel, byte pitch, byte velocity)
{
  // 0x90 is the first of 16 note on channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0x90 - 1;
  
  // Ensure we're between channels 1 and 16 for a note on message
  if (channel >= 0x90 && channel <= 0x9F)
  {
    #ifdef DEBUG
      Serial.print("Button pressed: ");
      Serial.println(pitch);
    #elif defined(TEENSY_PLUS_PLUS) || defined(TEENSY_2) || defined(TEENSY_PLUS_PLUS_2)
      usbMIDI.sendNoteOn(pitch, velocity, channel);
    #else
      Serial.write(channel);
      Serial.write(pitch);
      Serial.write(velocity);
    #endif
  }
}

// Send a MIDI note off message
void noteOff(byte channel, byte pitch)
{
  // 0x80 is the first of 16 note off channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0x80 - 1;
  
  // Ensure we're between channels 1 and 16 for a note off message
  if (channel >= 0x80 && channel <= 0x8F)
  {
    #ifdef DEBUG
      Serial.print("Button released: ");
      Serial.println(pitch);
    #elif defined(TEENSY_PLUS_PLUS) || defined(TEENSY_2) || defined(TEENSY_PLUS_PLUS_2)
      usbMIDI.sendNoteOff(pitch, 0x00, channel);
    #else 
      Serial.write(channel);
      Serial.write(pitch);
      Serial.write((byte)0x00);
    #endif
  }
}

// Send a MIDI control change message
void controlChange(byte channel, byte control, byte value)
{
  // 0xB0 is the first of 16 control change channels. Subtract one to go from MIDI's 1-16 channels to 0-15
  channel += 0xB0 - 1;
  
  // Ensure we're between channels 1 and 16 for a CC message
  if (channel >= 0xB0 && channel <= 0xBF)
  {
    #ifdef DEBUG
      Serial.print(control - MIDI_CC);
      Serial.print(": ");
      Serial.println(value);
    #elif defined(TEENSY_PLUS_PLUS) || defined(TEENSY_2) || defined(TEENSY_PLUS_PLUS_2)
      usbMIDI.sendControlChange(control, value, channel);
    #else
      Serial.write(channel);
      Serial.write(control);
      Serial.write(value);
    #endif
  }
}
