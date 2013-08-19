// see.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "move.h"
#include "piece.h"
#include "see.h"
#include "util.h"
#include "value.h"
#include "data.h"

// macros

#define ALIST_CLEAR(alist) ((alist)->size=0)

// types

struct alist_t
{
	int size;
	int square[15];
};

struct alists_t
{
	alist_t alist[ColourNb][1];
};

// prototypes

static int see_rec( alists_t *alists, board_t *board, int colour, int to, int piece_value );
static void alist_build( alist_t *alist, board_t *board, int to, int colour );
static void alists_hidden( alists_t *alists, board_t *board, int from, int to );
static void alist_clear( alist_t *alist );
static void alist_add( alist_t *alist, int square, board_t *board );
static void alist_remove( alist_t *alist, int pos );
static int alist_pop( alist_t *alist, board_t *board );

// functions

// see_move()

int see_move( int move, board_t *board )
{
	return 0;
}

// see_square()

int see_square( board_t *board, int to, int colour )
{

	int att, def;
	alists_t alists[1];
	alist_t *alist;
	int piece_value;
	int piece;

	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(to));
	ASSERT(COLOUR_IS_OK(colour));

	ASSERT(INDEX_COLOUR_IS(board->square[to], COLOUR_OPP(colour)));

	// build attacker list

	att = colour;
	alist = alists->alist[att];

	ALIST_CLEAR(alist);
	alist_build(alist, board, to, att);

	if( alist->size == 0 )
		return 0; // no attacker => stop SEE

	// build defender list

	def = COLOUR_OPP(att);
	alist = alists->alist[def];

	ALIST_CLEAR(alist);
	alist_build(alist, board, to, def);

	// captured piece

	piece = board->square[to];
	ASSERT(INDEX_IS_OK(piece));
	ASSERT(INDEX_COLOUR_IS(piece, def));

	piece_value = VALUE_INDEX(piece);

	// SEE search

	return see_rec(alists, board, att, to, piece_value);
}

// see_rec()

static int see_rec( alists_t *alists, board_t *board, int colour, int to, int piece_value )
{

	int from, piece;
	int value;

	ASSERT(alists != NULL);
	ASSERT(board != NULL);
	ASSERT(COLOUR_IS_OK(colour));
	ASSERT(SQUARE_IS_OK(to));
	ASSERT(piece_value > 0);

	// find the least valuable attacker

	from = alist_pop(alists->alist[colour], board);

	if( from == SquareNone )
		return 0; // no more attackers

	// find hidden attackers

	alists_hidden(alists, board, from, to);

	// calculate the capture value

	value = +piece_value; // captured piece

	if( value == ValueKing )
		return value;     // do not allow an answer to a king capture

	piece = board->square[from];
	ASSERT(INDEX_IS_OK(piece));
	ASSERT(INDEX_COLOUR_IS(piece, colour));
	piece_value = VALUE_INDEX(piece);


	value -= see_rec(alists, board, COLOUR_OPP(colour), to, piece_value);

	if( value < 0 )
		value = 0;

	return value;
}

// alist_build()

static void alist_build( alist_t *alist, board_t *board, int to, int colour )
{

}

// alists_hidden()

static void alists_hidden( alists_t *alists, board_t *board, int from, int to )
{

	int sq, piece;
	int inc;
	int cn;
	int index;
	int square;

	ASSERT(alists != NULL);
	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(from));
	ASSERT(SQUARE_IS_OK(to));

	
}

// alist_clear()

static void alist_clear( alist_t *alist )
{

	ASSERT(alist != NULL);

	alist->size = 0;
}

// alist_add()

static void alist_add( alist_t *alist, int square, board_t *board )
{

	int piece;
	int size, pos;

	ASSERT(alist != NULL);
	ASSERT(SQUARE_IS_OK(square));
	ASSERT(board != NULL);

	// insert in MV order

	piece = board->square[square];
	size = ++alist->size;
	ASSERT(size > 0 && size < 16);

	for ( pos = size - 1; pos > 0 && INDEX_ORDER(piece) > INDEX_ORDER(board->square[alist->square[pos - 1]]); pos-- )
	{
		ASSERT(pos > 0 && pos < size);
		alist->square[pos] = alist->square[pos - 1];
	}

	ASSERT(pos >= 0 && pos < size);
	alist->square[pos] = square;
}

// alist_remove()

static void alist_remove( alist_t *alist, int pos )
{

	int size, i;

	ASSERT(alist != NULL);
	ASSERT(pos >= 0 && pos < alist->size);

	size = alist->size--;
	ASSERT(size >= 1);

	ASSERT(pos >= 0 && pos < size);

	for ( i = pos; i < size - 1; i++ )
	{
		ASSERT(i >= 0 && i < size - 1);
		alist->square[i] = alist->square[i + 1];
	}
}

// alist_pop()

static int alist_pop( alist_t *alist, board_t *board )
{

	int sq;
	int size;

	ASSERT(alist != NULL);
	ASSERT(board != NULL);

	sq = SquareNone;

	size = alist->size;

	if( size != 0 )
	{
		size--;
		ASSERT(size >= 0);
		sq = alist->square[size];
		alist->size = size;
	}

	return sq;
}

// end of see.cpp
