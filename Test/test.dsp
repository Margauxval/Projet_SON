import("stdfaust.lib");

// --- Contrôles ---
rec = checkbox("v:Sampler/Record"); 
vol = hslider("v:Sampler/Gain", 0.5, 0, 1, 0.01);

// --- Paramètres ---
size = 65536; // Env 1.5s à 44.1kHz

// Index d'écriture (ne tourne que si rec est actif)
// On utilise une rampe qui se remet à 0 quand on commence à enregistrer
write_index = (+(1) : %(size)) ~ *(rec);

// Index de lecture (tourne tout le temps)
read_index = os.phasor(size, 1.0) : int;

// La table de données
// On enregistre l'entrée (_) à l'emplacement 'write_index'
looper = rwtable(size, 0.0, write_index, _, read_index);

// Logique de sortie :
// On ne veut entendre QUE la table de lecture, multipliée par le volume.
// Si on veut entendre le micro pendant qu'on enregistre (monitoring), 
// on peut décommenter la partie (+ _)
process = looper : *(vol) <: _,_;
