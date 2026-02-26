import("stdfaust.lib");

bufSize = 48000; 

note   = nentry("note", 69, 0, 127, 1);
gate   = nentry("gate", 0, 0, 1, 1) : si.smoo;
record = nentry("record", 0, 0, 1, 1);

// Enveloppe globale
adsr = en.adsr(0.05, 0.0, 1.0, 0.1, gate);

ratio = pow(2.0, (note - 69.0) / 12.0);

// Enregistrement
writeIndex = (+(1) : *(record) : min(bufSize-1)) ~ _;

// Lecture avec double phasor
loopFreq = (ratio * ma.SR) / bufSize;
phase1 = os.phasor(1.0, loopFreq) * (gate > 0.001);
phase2 = ma.modulo(phase1 + 0.5, 1.0) * (gate > 0.001);

// Crossfade
gain1 = pow(sin(ma.PI * phase1), 2);
gain2 = pow(sin(ma.PI * phase2), 2);

// Accès table
readTable(p) = rwtable(bufSize, 0.0, int(writeIndex), _, int(p * (bufSize-1)));

// Sortie
process = (readTable(phase1) * gain1 + readTable(phase2) * gain2) * adsr;
