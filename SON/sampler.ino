#include <Audio.h>
#include <SD.h>
#include <Bounce.h>
#include <MIDI.h>  // juste pour préparer futur flux MIDI

// === Audio ===
AudioInputI2S        micInput;
AudioRecordQueue     recorder;
AudioOutputI2S       audioOutput;  // Pour écouter le micro si tu veux
AudioConnection      patchCord1(micInput, 0, recorder, 0);
AudioConnection      patchCord2(micInput, 0, audioOutput, 0);
AudioConnection      patchCord3(micInput, 1, audioOutput, 1);

AudioControlSGTL5000 audioShield;

// === Bouton ===
const int buttonPin = 0;
Bounce button(buttonPin, 10);

// === SD / WAV ===
File wavFile;
const int chipSelect = 10;
bool isRecording = false;

// === MIDI USB natif (préparation) ===
MIDI_CREATE_DEFAULT_INSTANCE();  // pas utilisé pour l'instant

// ================= HEADER WAV =================
void writeWavHeader(File &file) {
  file.seek(0);
  file.write("RIFF", 4);
  uint32_t chunkSize = 36;
  file.write((uint8_t*)&chunkSize, 4);
  file.write("WAVE", 4);
  file.write("fmt ", 4);
  uint32_t subchunk1Size = 16;
  file.write((uint8_t*)&subchunk1Size, 4);
  uint16_t audioFormat = 1;
  uint16_t channels = 1;
  uint32_t sampleRate = 44100;
  uint16_t bitsPerSample = 16;
  uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
  uint16_t blockAlign = channels * bitsPerSample / 8;
  file.write((uint8_t*)&audioFormat, 2);
  file.write((uint8_t*)&channels, 2);
  file.write((uint32_t*)&sampleRate, 4);
  file.write((uint32_t*)&byteRate, 4);
  file.write((uint16_t*)&blockAlign, 2);
  file.write((uint16_t*)&bitsPerSample, 2);
  file.write("data", 4);
  uint32_t dataSize = 0;
  file.write((uint8_t*)&dataSize, 4);
}

void finalizeWavFile(File &file) {
  uint32_t fileSize = file.size();
  uint32_t dataChunkSize = fileSize - 44;

  file.seek(4);
  uint32_t chunkSize = fileSize - 8;
  file.write((uint8_t*)&chunkSize, 4);

  file.seek(40);
  file.write((uint32_t*)&dataChunkSize, 4);
}

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  AudioMemory(40);

  audioShield.enable();
  audioShield.volume(0.6);
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(30);

  recorder.begin();

  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur SD !");
  } else {
    Serial.println("SD OK");
  }

  // MIDI.begin(MIDI_CHANNEL_OMNI); // à activer plus tard
}

// ================= LOOP =================
void loop() {
  button.update();

  // ===== START RECORD =====
  if (button.read() == HIGH && !isRecording) {
    Serial.println("Recording...");
    SD.remove("REC1.WAV");  // Écrase l'ancien enregistrement
    wavFile = SD.open("REC1.WAV", FILE_WRITE);
    if (wavFile) {
      writeWavHeader(wavFile);
      isRecording = true;
    } else {
      Serial.println("Erreur ouverture fichier !");
    }
  }

  // ===== WRITE AUDIO =====
  if (isRecording && recorder.available() > 0) {
    int16_t *buffer = recorder.readBuffer();
    wavFile.write((uint8_t*)buffer, 256);  // Écriture mono
    recorder.freeBuffer();
  }

  // ===== STOP RECORD =====
  if (button.read() == LOW && isRecording) {
    finalizeWavFile(wavFile);
    wavFile.close();
    isRecording = false;
    Serial.println("Stopped.");
  }
}
