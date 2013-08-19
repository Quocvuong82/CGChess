// move_legal.cpp

// includes

#include "attack.h"
#include "colour.h"
#include "fen.h"
#include "list.h"
#include "move.h"
#include "move_do.h"
#include "move_gen.h"
#include "move_legal.h"
#include "piece.h"
#include "square.h"
#include "types.h"
#include "movetable.h"

// prototypes

static bool move_is_pseudo_debug( int move, board_t *board );
static bool castle_is_legal( int move, board_t *board );

// functions

// move_is_pseudo()

bool move_is_pseudo( int move, board_t *board )
{

	int me, opp;
	int from, to;
	int piece, capture;
	int row,col;
	int rowbit, colbit;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);
	// init

	me = board->turn;
	opp = COLOUR_OPP(board->turn);

	// from

	from = MOVE_FROM(move);
	to = MOVE_TO(move);

	ASSERT(SQUARE_IS_OK(from));
	ASSERT(SQUARE_IS_OK(to));

	piece = board->square[from];	
	if( !INDEX_COLOUR_IS(piece, me) )
		return false;

	capture = board->square[to];
	if( INDEX_COLOUR_IS(capture, me) )
		return false;

	ASSERT(INDEX_IS_OK(piece));
	ASSERT(INDEX_IS_OK(piece));

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	// move 
	switch(INDEX_TO_PIECE16(piece))
	{
	case RedKing16:
	case BlackKing16:
		if(KingAttack[from][to])  return true;
		if(INDEX_IS_KING(capture) && RookFileAttack[from][to][colbit]) return true;
		return false;

	case RedAdvisor16:
	case BlackAdvisor16:
		if(AdvisorAttack[from][to]) return true;
		return false;

	case RedBishop16:
	case BlackBishop16:
        if(BishopAttack[from][to] && board->square[BishopEyes[from][to]] == PieceNone) return true;
		return false;

	case RedKnight16:
	case BlackKnight16:
		if(KnightAttack[from][to] && board->square[KnightLegs[from][to]] == PieceNone) return true;
		return false;

	case RedRook16:
	case BlackRook16:
		if(RookFileAttack[from][to][colbit]) return true;
		if(RookRankAttack[from][to][rowbit]) return true;
		return false;

	case RedCannon16:
	case BlackCannon16:
		if(CannonFileAttack[from][to][colbit] && PinNb[SQUARE_RANK(from)][SQUARE_RANK(to)][colbit] == 1) return true;
		if(CannonRankAttack[from][to][rowbit] && PinNb[SQUARE_FILE(from)][SQUARE_FILE(to)][rowbit] == 1) return true;
		if(PinNb[SQUARE_FILE(from)][SQUARE_FILE(to)][rowbit] == 0) return true;
		if(PinNb[SQUARE_RANK(from)][SQUARE_RANK(to)][colbit] == 0) return true;
		return false;

	case BlackPawn16:
	case RedPawn16:
		if(PawnAttack[me][from][to]) return true;
		return false;

	default:
		return false;
	}

	return false;
}

// quiet_is_pseudo()

bool quiet_is_pseudo( int move, board_t *board )
{

	int me, opp;
	int from, to;
	int piece;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));
	// init

	me = board->turn;
	opp = COLOUR_OPP(board->turn);

	// from

	from = MOVE_FROM(move);
	piece = board->square[from];
	if( !INDEX_COLOUR_IS(piece, me) )
		return false;

	ASSERT(SQUARE_IS_OK(from));
	ASSERT(INDEX_IS_OK(piece));
	
	// to
	to = MOVE_TO(move);
	ASSERT(SQUARE_IS_OK(to));

	if( board->square[to] != PieceNone )
		return false; // capture
	ASSERT(SQUARE_IS_OK(to));

	return move_is_pseudo(move, board );
}

// pseudo_is_legal()

bool pseudo_is_legal( int move, board_t *board )
{

	int me, opp;
	int from, to;
	int piece;
	bool legal;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	// init

	me = board->turn;
	opp = COLOUR_OPP(me);

	from = MOVE_FROM(move);
	to = MOVE_TO(move);

	piece = board->square[from];
	ASSERT(INDEX_COLOUR_IS(piece, me));


	// king moves
	if( INDEX_IS_KING(piece) )
	{
		legal = !is_pinned(board, MOVE_MAKE(from, to), me);//!is_attacked(board,from, to, opp);

		return legal;
	}

	// pins
	if( is_pinned(board, move, me) )
	{
        return false;
	}

	return true;
}

// move_is_pseudo_debug()

static bool move_is_pseudo_debug( int move, board_t *board )
{

	list_t list[1];

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));

	gen_moves(list, board);

	return list_contain(move, list);
}

// end of move_legal.cpp
