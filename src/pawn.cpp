// pawn.cpp

// includes

#include <cstring>
#include "board.h"
#include "colour.h"
#include "hash.h"
#include "init.h"
#include "option.h"
#include "pawn.h"
#include "piece.h"
#include "protocol.h"
#include "search.h"
#include "square.h"
#include "util.h"


// constants

static const bool UseTable = true;
static const uint32 TableSize = 16384;

// types

typedef pawn_info_t entry_t;

struct pawn_t
    {
    entry_t *table;
    uint32 size;
    uint32 mask;
    uint32 used;
    sint64 read_nb;
    sint64 read_hit;
    sint64 write_nb;
    sint64 write_collision;
    };

// constants and variables

const int OutpostMatrix[2][90] =
{
	//------------------------------------------
	1,  2,  3,  5,  5,  5,  3,  2,  1,
	5,	10, 15, 20, 25, 20, 15, 10, 5,
	5,	10, 15, 20, 20, 20, 15, 10, 5,
	5,	10, 15, 15, 15, 15, 15, 10, 5,
	5,	5,  10, 10, 15, 10, 10,  5, 5,
	//-------------------------------//
	3,	 0,  5, 0,  10,  0,  5,  0, 3,
	0,	 0, -5,  0,  5,  0, -5,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,

	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0, -5,  0,  5,  0, -5,  0, 0,
	3,	 0,  5, 0,  10,  0,  5,  0, 3,
	//-------------------------------//
	5,	5,  10, 10, 15, 10, 10,  5, 5,
	5,	10, 15, 15, 15, 15, 15, 10, 5,
	5,	10, 15, 20, 20, 20, 15, 10, 5,
	5,	10, 15, 20, 25, 20, 15, 10, 5,
	1,  2,  3,  5,  5,  5,  3,  2,  1,
};

static int Bonus[RankNb];

// variables

static pawn_t PawnTable[MaxThreads][1];


// prototypes

static void pawn_comp_info( pawn_info_t *info, const board_t *board );

// functions

// pawn_init_bit()

void pawn_init_bit()
{
	int distance, ThreadId;

    // bonus

    for ( distance = 0; distance < RankNb; distance++ )
        Bonus[distance] = 0;

    Bonus[0] = 256;
	Bonus[1] = 256;
	Bonus[2] = 154;
	Bonus[3] = 77;
	Bonus[4] = 26;
	Bonus[5] = 10;
	Bonus[6] = 5;
	Bonus[7] = 2;
	Bonus[8] = 1;
	Bonus[9] = 0;
}

// pawn_init()

void pawn_init()
{

}

// pawn_alloc()

void pawn_alloc()
{

}

// pawn_clear()

void pawn_clear( int ThreadId )
{
}

// pawn_get_info()

void pawn_get_info( pawn_info_t *info, board_t *board, int ThreadId )
{
	// calculation

	pawn_comp_info(info, board);

	// store
}

// pawn_comp_info()

static void pawn_comp_info( pawn_info_t *info, const board_t *board )
{

	int colour;
	int file, rank;
	int me, opp;
	int sq;
	bool backward, candidate, doubled, isolated, open, passed;
	int opening[ColourNb], endgame[ColourNb];
	int pawn;
	int piece;
	int i;


	ASSERT(info != NULL);
	ASSERT(board != NULL);
	// init

	for ( colour = 0; colour < ColourNb; colour++ )
	{

		opening[colour] = 0;
		endgame[colour] = 0;

	}

	// store info

    info->opening = ((opening[Red] - opening[Black]) * PawnStructureWeightOpening) / 256;
    info->endgame = ((opening[Red] - opening[Black]) * PawnStructureWeightEndgame) / 256;
}

// quad()

int quad( int y_min, int y_max, int x )
{

	int y;

	ASSERT(y_min >= 0 && y_min <= y_max && y_max <= +32767);
	ASSERT(x >= Rank2 && x <= Rank7);

	y = y_min + ((y_max - y_min) * Bonus[x] + 128) / 256;
	ASSERT(y >= y_min && y <= y_max);

	return y;
}

// is_passed()
//过路兵,过河兵
bool is_passed( const board_t *board, int to, int me )
{
	if(SQUARE_COLOUR(to) != me)
		return true;
	return false;
}

// pawn_fork()
//可能会被吃掉？
bool pawn_fork( const board_t *board, int to )
{
	return false;
}

// end of pawn.cpp
