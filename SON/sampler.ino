#include <Audio.h>
#include <Bounce.h>
#include "sampler.h"

sampler faust;

AudioInputI2S          micInput;
AudioOutputI2S         audioOutput;
AudioMixer4            mixer1;

AudioConnection patchCord1(micInput, 0, mixer1, 0);
AudioConnection patchCord2(mixer1, 0, faust, 0);
AudioConnection patchCord3(faust, 0, audioOutput, 0);
AudioConnection patchCord4(faust, 0, audioOutput, 1);

AudioControlSGTL5000 audioShield;

const int buttonPin = 0;
Bounce button = Bounce(buttonPin, 15);

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT); // Ton bouton en Pull-down

  AudioMemory(140);
  audioShield.enable();
  audioShield.volume(0.5);
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(40);

  mixer1.gain(0, 1.0);

  usbMIDI.begin();
  Serial.println("Sampler Pret...");

  faust.setParamValue("note", 69.0f);
  faust.setParamValue("gate", 0.0f);
}

void loop() {
  button.update();

  if (button.risingEdge()) {
    faust.setParamValue("record", 1.0f);
    Serial.println("Recording...");
  }

  if (button.fallingEdge()) {
    faust.setParamValue("record", 0.0f);
    Serial.println("Stopped Recording.");
  }

  while (usbMIDI.read()) {
    byte type = usbMIDI.getType();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();

    if (type == usbMIDI.NoteOn && data2 > 0) {
      faust.setParamValue("note", (float)data1);
      faust.setParamValue("gate", 1.0f);
      Serial.print("Note On: "); Serial.println(data1);
    }
    else if (type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && data2 == 0)) {
      faust.setParamValue("gate", 0.0f);
      Serial.println("Note Off");
    }
  }
}
