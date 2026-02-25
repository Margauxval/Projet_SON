import("stdfaust.lib");

bufSize = 48000;

note   = nentry("note", 69, 0, 127, 1);
gate   = nentry("gate", 0, 0, 1, 1) : si.smoo;
record = nentry("record", 0, 0, 1, 1);

ratio = pow(2.0, (note - 69.0) / 12.0);

// WRITE INDEX : Revient à 0 quand record s'arrête
writeIndex = (+(1) : *(record) : min(bufSize-1)) ~ _;

// READ INDEX : On multiplie par (gate > 0.001) pour forcer le retour à 0 
// quand on ne joue pas. Ainsi, chaque NoteOn repart du début.
readIndex = (+(ratio) : *(gate > 0.001) : min(bufSize-1)) ~ _;

// TABLE
process = rwtable(
    bufSize,
    0.0,
    int(writeIndex),
    _,
    int(readIndex)
) * gate;
