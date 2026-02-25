//cross fade
import("stdfaust.lib");

// --- CONFIGURATION ---
bufSize = 48000; // Taille du buffer (1 seconde)

note   = nentry("note", 69, 0, 127, 1);
gate   = nentry("gate", 0, 0, 1, 1) : si.smoo;
record = nentry("record", 0, 0, 1, 1);

// Calcul du ratio de lecture (vitesse)
ratio = pow(2.0, (note - 69.0) / 12.0);

// --- 1. ENREGISTREMENT ---
// L'index d'écriture s'arrête à la fin du buffer
writeIndex = (+(1) : *(record) : min(bufSize-1)) ~ _;

// --- 2. LECTURE AVEC DOUBLE PHASOR (STRATÉGIE DE BOUCLE) ---
// On calcule la fréquence nécessaire pour lire tout le buffer à la vitesse 'ratio'
loopFreq = (ratio * ma.SR) / bufSize;

// On génère deux têtes de lecture déphasées de 180° (0.5)
phase1 = os.phasor(1.0, loopFreq) * (gate > 0.001);
phase2 = ma.modulo(phase1 + 0.5, 1.0) * (gate > 0.001);

// --- 3. FENÊTRES DE CROSSFADE ---
// On utilise une forme de sinus pour que la somme des puissances soit constante (pas de baisse de volume)
gain1 = pow(sin(ma.PI * phase1), 2);
gain2 = pow(sin(ma.PI * phase2), 2);

// --- 4. ACCÈS À LA TABLE ---
// On définit la table et on lit aux deux positions simultanément
readTable(p) = rwtable(bufSize, 0.0, int(writeIndex), _, int(p * (bufSize-1)));

// --- 5. SORTIE ---
// Mixage des deux têtes de lecture
process = (readTable(phase1) * gain1 + readTable(phase2) * gain2) * gate;
