// hash.cpp

// includes

#include "board.h"
#include "hash.h"
#include "piece.h"
#include "square.h"
#include "types.h"
#include "random.h"

// functions

uint64 Random90[RandomNb] = {0};


static uint64 hash_counter_key( int piece_16, int count );

void random_init()
{
	int i;
	unsigned long randseed = 13;
	uint64 key = longrand(randseed);

	for (i = 0; i < RandomNb; ++i)
	{
		Random90[i] = (key << 32) | longrand(randseed);
		key = longrand(randseed);
	}
}

// hash_key()

uint64 hash_key( board_t *board )
{

	uint64 key;
    int sq, piece;
	int index;

	ASSERT(board != NULL);

	// init

	key = 0;

	// pieces
	for (index = 1; index <= 32; ++index)
	{		
		sq = board->piece[index];
		if (sq != SquareNone)
		{
            piece= INDEX_TO_PIECE(index);
			key ^= hash_piece_key(piece, SQUARE_TO_90(sq));
		}
		
	}

	// turn
	key ^= hash_turn_key(board->turn);

	return key;
}


// hash_piece_key()

uint64 hash_piece_key( int piece, int square )
{
	ASSERT(PIECE_IS_OK(piece));//1-7 8-15
	//ASSERT(SQUARE_IS_OK(square));//0-90
	ASSERT(RandomPiece + piece*90 + square < RandomNb);

	return RANDOM_90(RandomPiece + piece*90 + square);
}

// hash_turn_key()

uint64 hash_turn_key( int colour )
{

	ASSERT(COLOUR_IS_OK(colour));

	return (COLOUR_IS_RED(colour)) ? RANDOM_90(RandomTurn) : 0;
}

uint64 hash_material_key( board_t *board )
{

	uint64 key;
	int piece_16, count;

	ASSERT(board != NULL);

	// init

	key = 0;

	// counters

	for ( piece_16 = 0; piece_16 < 16; piece_16++ )
	{
		count = board->number[piece_16];
		key ^= hash_counter_key(piece_16, count);
	}

	return key;
}

static uint64 hash_counter_key( int piece_16, int count )
{

	uint64 key;
	int i, index;

	ASSERT(piece_16 >= 0 && piece_16 < 16);
	ASSERT(count >= 0 && count <= 10);

	// init

	key = 0;

	// counter

	index = piece_16 * 16;

	for ( i = 0; i < count; i++ )
		key ^= RANDOM_90(index + i);

	return key;
}


// end of hash.cpp
