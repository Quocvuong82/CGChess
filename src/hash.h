// hash.h

#ifndef HASH_H
#define HASH_H

// includes

#include "board.h"
#include "types.h"


// macros
#define RANDOM_90(n) (Random90[n])
#define KEY_INDEX(key) ((uint32)(key))
#define KEY_LOCK(key)  ((uint32)((key)>>32))

// constants
const int RandomNb = 16*90 + 2;//2
const int RandomPiece = 0;       // 16 * 90
const int RandomTurn =16*90;     // 1

extern uint64 Random90[RandomNb];

// functions
extern void random_init();
extern uint64 hash_key( board_t *board );
extern uint64 hash_piece_key( int piece, int square );
extern uint64 hash_turn_key( int colour );
extern uint64 hash_material_key( board_t *board );

#endif // !defined HASH_H

// end of hash.h
