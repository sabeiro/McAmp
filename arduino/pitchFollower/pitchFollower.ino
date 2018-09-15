#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=104,195
AudioSynthWaveformPWM    pwm1;           //xy=108,280
AudioInputI2S            i2s1;           //xy=142,53
AudioMixer4              mixer1;         //xy=263,205
AudioAnalyzeRMS          peak1;          //xy=321,83
AudioAmplifier           amp1;           //xy=322,30
AudioEffectFreeverb      freeverb1;      //xy=421,158
AudioFilterStateVariable filter1;        //xy=467,45
AudioMixer4              mixer2;         //xy=535,230
AudioFilterStateVariable filter2;        //xy=593,44
AudioOutputI2S           i2s2;           //xy=690,234
AudioAnalyzeNoteFrequency notefreq1;      //xy=726,56
AudioConnection          patchCord1(sine1, 0, mixer1, 0);
AudioConnection          patchCord2(pwm1, 0, mixer1, 2);
AudioConnection          patchCord3(i2s1, 0, mixer1, 1);
AudioConnection          patchCord4(i2s1, 1, peak1, 0);
AudioConnection          patchCord5(i2s1, 1, amp1, 0);
AudioConnection          patchCord6(mixer1, freeverb1);
AudioConnection          patchCord7(mixer1, 0, mixer2, 1);
AudioConnection          patchCord8(amp1, 0, filter1, 0);
AudioConnection          patchCord9(freeverb1, 0, mixer2, 0);
AudioConnection          patchCord10(filter1, 0, filter2, 0);
AudioConnection          patchCord11(mixer2, 0, i2s2, 0);
AudioConnection          patchCord12(mixer2, 0, i2s2, 1);
AudioConnection          patchCord13(filter2, 2, notefreq1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=254,378
// GUItool: end automatically generated code    //// COPY PASTE THIS INTO THE WEB AUDIO TOOL   https://www.pjrc.com/teensy/gui
 
void setup() {
  AudioMemory(32); // WTF ???!!!!  doesnt work on lower sizes ,maybe related to the proccess required by the pitch tracker
 
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC); // Audio Shield Settings
  sgtl5000_1.micGain(35);   // Good gain for Cumbus
  amp1.gain(1);             // Adittional gain node to adjust the level sended to the picth tracker
 
  filter1.frequency(1000);    // FILTERING the audio to improve pitch tracking
  filter1.resonance(0);
  filter2.frequency(60);
  filter2.resonance(0);
 
    sine1.amplitude(0.5);
    pwm1.frequency(100);    //Sound generators
    pwm1.amplitude(0);
   
    notefreq1.begin(0.8);     /// Pitch tracking
   
    mixer1.gain(0,0.5);
    mixer1.gain(1,0.5);
    mixer1.gain(2,0.25);
    mixer2.gain(0, 0.4);
    mixer2.gain(1,1);
    freeverb1.roomsize(0.6);
}
void loop() {
    float note = notefreq1.read();
    notefreq1.threshold(.15);
   
  if(notefreq1.probability()> 0.70){    // set the note only when the probabulity of a good read is > than 70%
    sine1.frequency(note*0.5);
    pwm1.frequency(note*2.0);
    }      
}
