import("stdfaust.lib");

bufSize = 48000; // taille buffer

note   = nentry("note", 69, 0, 127, 1); // reçoit note 
gate   = nentry("gate", 0, 0, 1, 1) : si.smoo; // déclenche son + lisse anti click
record = nentry("record", 0, 0, 1, 1); // active ou désactive écriture buffer

adsr = en.adsr(0.05, 0.0, 1.0, 0.1, gate); // enveloppe 

ratio = pow(2.0, (note - 69.0) / 12.0); // calcul du ratio

writeIndex = (+(1) : *(record) : min(bufSize-1)) ~ _; // tant que record +1 à chaque échantillon + mémoire position précédente + anti dépassement buffer 

// crossfade
loopFreq = (ratio * ma.SR) / bufSize; // calcul nombre de note / sec pour avoir la bonne note 
phase1 = os.phasor(1.0, loopFreq) * (gate > 0.001); // rampe : index lecture principal (cycle 0 à 1)
phase2 = ma.modulo(phase1 + 0.5, 1.0) * (gate > 0.001); // rampe décalée de 50%

gain1 = pow(sin(ma.PI * phase1), 2); // sin pour le volume pour éviter arrêt brusque 
gain2 = pow(sin(ma.PI * phase2), 2);

readTable(p) = rwtable(bufSize, 0.0, int(writeIndex), _, int(p * (bufSize-1))); // table de lecture prend entrée _ + écrit + lit à position p (curseur) 

process = (readTable(phase1) * gain1 + readTable(phase2) * gain2) * adsr; // sortie : addition têtes lecture + enveloppe
