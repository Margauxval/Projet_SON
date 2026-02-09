#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "sampler.h" // Le fichier généré par l'export Faust

// --- Hardware Pins ---
const int buttonPin = 0;      // Bouton sur Pin 0
const int potPin = A0;        // Potentiomètre sur Pin A0
const int chipSelect = BUILTIN_SDCARD;

// --- Objets Audio ---
sampler mySampler;
AudioInputI2S            micInput;
AudioOutputI2S           audioOut;
AudioControlSGTL5000     sgtl5000;

// Connexions
AudioConnection patch1(micInput, 0, mySampler, 0);
AudioConnection patch2(mySampler, 0, audioOut, 0);
AudioConnection patch3(mySampler, 0, audioOut, 1);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  AudioMemory(120);
  
  sgtl5000.enable();
  sgtl5000.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000.micGain(30);

  // Initialisation SD (optionnel pour log ou sauvegarde)
  if (!(SD.begin(chipSelect))) {
    Serial.println("SD card failed");
  }

  // Initialisation MIDI USB
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);
}

void loop() {
  // 1. Gestion du bouton d'enregistrement (Inversé car INPUT_PULLUP)
  int recState = (digitalRead(buttonPin) == LOW) ? 1 : 0;
  mySampler.setParamValue("Record", recState);

  // 2. Gestion du potentiomètre de Gain
  float potValue = analogRead(potPin) / 1023.0;
  mySampler.setParamValue("Gain", potValue);

  // 3. Lecture MIDI USB
  usbMIDI.read();
}

// --- Callbacks MIDI ---
void handleNoteOn(byte channel, byte note, byte velocity) {
  // Convertir le numéro de note MIDI en fréquence (Hz)
  float frequency = 440.0 * pow(2.0, (note - 69) / 12.0);
  mySampler.setParamValue("freq", frequency);
  mySampler.setParamValue("gate", 1);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  mySampler.setParamValue("gate", 0);
}