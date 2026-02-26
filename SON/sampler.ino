#include <Audio.h>
#include <Bounce.h>
#include "sampler.h"

sampler faust; // création d'une instance de l'instrument

AudioInputI2S          micInput; // définit entrée 
AudioOutputI2S         audioOutput; // définit sortie 
AudioMixer4            mixer1; // mixeur équilibrant signal d'entrée 

// établissement des connexions
AudioConnection patchCord1(micInput, 0, mixer1, 0); // micro -> mixeur
AudioConnection patchCord2(mixer1, 0, faust, 0); // mixeur -> faust
AudioConnection patchCord3(faust, 0, audioOutput, 0); // faust -> canal gauche
AudioConnection patchCord4(faust, 0, audioOutput, 1); // faust -> canal droit

AudioControlSGTL5000 audioShield; // objet configuration puce audio physique 

const int buttonPin = 0;
Bounce button = Bounce(buttonPin, 15); // délai anti rebond 15 ms

void setup() {
  Serial.begin(9600); // ouverture port USB (9600 bits/s)
  pinMode(buttonPin, INPUT); // config pin 0 entrée bouton

  AudioMemory(140); // allocation mémoire vide pour stockage audio 
  audioShield.enable(); // allume puce audio 
  audioShield.volume(0.5); // volume sortie 50%
  audioShield.inputSelect(AUDIO_INPUT_MIC); // écoute micro
  audioShield.micGain(40); // amplification signal micro (40dB)

  mixer1.gain(0, 1.0); // volume entrée 0 du mixeur = 100%

  usbMIDI.begin(); // initialise pile MIDI (teensy = instrument)
  Serial.println("Sampler Pret...");

  faust.setParamValue("note", 69.0f); // envoie valeur par défaut à faust
  faust.setParamValue("gate", 0.0f); // sampler fermé  
}

void loop() {
  button.update(); // vérif état bouton

  if (button.risingEdge()) {
    faust.setParamValue("record", 1.0f); // envoie record = 1 à faust 
    Serial.println("Recording...");
  }

  if (button.fallingEdge()) {
    faust.setParamValue("record", 0.0f); // // envoie record = 1 à faust 
    Serial.println("Stopped Recording.");
  }

  while (usbMIDI.read()) { // lecture continue message MIDI entrant 
    byte type = usbMIDI.getType(); // nature message MIDI
    byte data1 = usbMIDI.getData1(); // numéro note 
    byte data2 = usbMIDI.getData2(); // vélocité (force sur touche)

    if (type == usbMIDI.NoteOn && data2 > 0) { // si une touche est pressée 
      faust.setParamValue("note", (float)data1); // envoie numéro note à faust
      faust.setParamValue("gate", 1.0f); // activation gate pour jouer note 
      Serial.print("Note On: "); Serial.println(data1);
    }
    else if (type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && data2 == 0)) { // pas de note ou note vélocité 0
      faust.setParamValue("gate", 0.0f); // ferme gate + relâche enveloppe 
      Serial.println("Note Off");
    }
  }
}
