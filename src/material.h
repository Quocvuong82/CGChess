// material.h

#ifndef MATERIAL_H
#define MATERIAL_H

// includes

#include "board.h"
#include "colour.h"
#include "util.h"

// constants

enum mat_dummy_t
    {
    MAT_NONE,
    MAT_KK,// king vs king
    MAT_KABKAB,
	MAT_KPKAB,
	MAT_KABKP,
	MAT_KRKAB,
	MAT_KABKR,
	MAT_KRKR,
	MAT_KNKN,
	MAT_KCKC,
	MAT_KPKP,

    MAT_NB
    };



// types

struct material_info_t
    {
    uint32 lock;
    uint8 recog;
    uint8 flags;
    uint8 cflags[ColourNb];
    uint8 mul[ColourNb];
    sint16 phase;
    sint16 opening;
    sint16 endgame;
    };

// functions

extern void material_init();
extern void material_alloc();
extern void material_clear( int ThreadId );
extern void material_get_info( material_info_t *info, const board_t *board, int ThreadId );

#endif // !defined MATERIAL_H

// end of material.h
