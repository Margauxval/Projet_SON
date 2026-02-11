#include <Audio.h>
#include <SD.h>
#include <Bounce.h>

// === Audio ===
AudioInputI2S          micInput;
AudioRecordQueue       recorder;
AudioPlaySdWav         playWav;     // 🔥 lecture SD ajoutée
AudioOutputI2S         audioOutput;

AudioConnection        patchCord1(micInput, 0, recorder, 0);      // enregistrement
AudioConnection        patchCord2(playWav, 0, audioOutput, 0); // gauche
AudioConnection        patchCord3(playWav, 0, audioOutput, 1); // droite
// lecture vers sortie

AudioControlSGTL5000   audioShield;

// === Bouton ===
const int buttonPin = 0;
Bounce button(buttonPin, 10);

// === SD ===
File wavFile;
const int chipSelect = 10;
bool isRecording = false;

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
  file.write((uint8_t*)&sampleRate, 4);
  file.write((uint8_t*)&byteRate, 4);
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);
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
  file.write((uint8_t*)&dataChunkSize, 4);
}

// ================= SETUP =================
void setup() {

  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  AudioMemory(60);   // 🔥 un peu plus pour SD + futur Faust

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
}

// ================= LOOP =================
void loop() {

  button.update();

  // ===== START RECORD =====
  if (button.read() == HIGH && !isRecording) {

    Serial.println("Recording...");
    SD.remove("REC1.WAV");

    wavFile = SD.open("REC1.WAV", FILE_WRITE);

    if (wavFile) {
      writeWavHeader(wavFile);
      isRecording = true;
    }
  }

  // ===== STOP RECORD =====
  if (button.read() == LOW && isRecording) {

    finalizeWavFile(wavFile);
    wavFile.close();
    isRecording = false;

    Serial.println("Stopped.");

    // 🔥 Test lecture automatique après enregistrement
    playWav.play("REC1.WAV");
  }

  // ===== WRITE AUDIO =====
  if (isRecording && recorder.available() > 0) {
    int16_t *buffer = recorder.readBuffer();
    wavFile.write((uint8_t*)buffer, 256);
    recorder.freeBuffer();
  }
}
