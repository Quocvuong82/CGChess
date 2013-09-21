// value.h

#ifndef VALUE_H
#define VALUE_H

// includes

#include "piece.h"
#include "types.h"

// constants
const int ValuePawn = 125;
const int ValueKnight = 500;
const int ValueCannon = 500;
const int ValueBishop = 250;
const int ValuekAdvisor = 250;
const int ValueRook = 1200;
const int ValueKing = 10000;
const int ValueNone = -32767;
const int ValueDraw = 0;
const int ValueMate = 30000;
const int ValueWin = 3000;
const int ValuePly = 40;
const int ValueInf = ValueMate;
const int ValueEvalInf = ValueMate - 256; // handle mates upto 255 plies
const int ValueRepeatValue = 23000;

// macros

#define VALUE_MATE(height) (-ValueMate+(height))
#define VALUE_INDEX(index) (ValueIndex[index])

// variables

extern int ValueIndex[IndexNb];

// functions

extern void value_init();
extern bool value_is_ok( int value );
extern bool range_is_ok( int min, int max );
extern bool value_is_mate( int value );
extern int value_to_trans( int value, int height );
extern int value_from_trans( int value, int height );
extern int value_to_mate( int value );

#endif // !defined VALUE_H

// end of value.h
