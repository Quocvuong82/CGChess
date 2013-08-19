// pst.h

#ifndef PST_H
#define PST_H

// includes

#include "types.h"

// constants

const int Opening = 0;
const int Endgame = 1;
const int StageNb = 2;

// variables
extern int Pst[16][90][StageNb];

// macros

#define PST(piece,square_90,stage) (Pst[piece][square_90][stage])

// functions

extern void pst_init();

#endif // !defined PST_H

// end of pst.h
