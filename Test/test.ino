#include <Audio.h>
#include <Wire.h>
#include "sampler.h"

sampler mySampler; 
AudioInputI2S     micInput;
AudioOutputI2S    audioOut;
AudioControlSGTL5000 sgtl5000;

AudioConnection patch1(micInput, 0, mySampler, 0);
AudioConnection patch2(mySampler, 0, audioOut, 0);
AudioConnection patch3(mySampler, 1, audioOut, 1);

void setup() {
  pinMode(0, INPUT_PULLUP); 
  pinMode(13, OUTPUT);      
  
  AudioMemory(120);
  
  sgtl5000.enable();
  sgtl5000.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000.micGain(45); // Ajuste selon ton micro
  sgtl5000.volume(0.6);
}

void loop() {
  // Lecture du bouton (LOW = appuyé)
  bool isPressed = (digitalRead(0) == LOW);
  
  // Envoi à Faust
  mySampler.setParamValue("Record", isPressed ? 1.0 : 0.0);
  mySampler.setParamValue("Gain", 0.8);

  // Témoin LED
  digitalWrite(13, isPressed);
}
