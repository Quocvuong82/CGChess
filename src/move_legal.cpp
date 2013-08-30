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
#include "util.h"
#include "movetable.h"


// prototypes

static bool move_is_pseudo_debug( int move, board_t *board );

// functions

// move_is_pseudo()

bool move_is_pseudo( int move, board_t *board )
{

	int me, opp;
	int from, to;
	int piece, capture;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);
	// init

	me = board->turn;
	opp = COLOUR_OPP(board->turn);

	// from

	from = MOVE_FROM(move);
	ASSERT(SQUARE_IS_OK(from));

	piece = board->square[from];	
	if(INDEX_TO_COLOUR(piece) != me)
		return false;

	// to
	to = MOVE_TO(move);
	ASSERT(SQUARE_IS_OK(to));

	capture = board->square[to];	
	if( INDEX_TO_COLOUR(capture) == me )
		return false;

	// move 
	switch(INDEX_TO_PIECE(piece))
	{
	case RedKing:
		if(KING_IN_CITY(to) && KLegalDt[to - from + 256]) return true;
		if(EQUAL_FILE(from, to) && INDEX_TO_PIECE(capture) == BlackKing)
		{
			int row=SQUARE_RANK(from) - 3;
			int col=SQUARE_FILE(from) - 3;
			if(FileCapMin[row][board->file[col]] + col == to) return true;
		}

		return false;

	case BlackKing:
		if(KING_IN_CITY(to) && KLegalDt[to - from + 256]) return true;
		if(EQUAL_FILE(from, to) && INDEX_TO_PIECE(capture) ==RedKing)
		{
			int row=SQUARE_RANK(from) - 3;
			int col=SQUARE_FILE(from) - 3;
			if(FileCapMax[row][board->file[col]] + col == to) return true;
		}

		return false;

	case RedAdvisor:
	case BlackAdvisor:
		if(ADVISOR_IN_CITY(to) && ALegalDt[to - from + 256]) return true;
		return false;

	case RedBishop:
	case BlackBishop:
		if(BISHOP_IN_CITY(to) &&
			BLegalDt[to - from + 256] &&
			board->square[(to + from) >> 1] == PieceNone)//
			return true;
		return false;

	case RedKnight:
	case BlackKnight:
		if(NLegalDt[to - from + 256] && board->square[from + NLegalDt[to - from + 256]] == PieceNone)//
			return true;
		return false;

	case RedRook:
	case BlackRook:
		if(capture)
		{
			if(EQUAL_FILE(from, to))
			{
				int row=SQUARE_RANK(from) - 3;
				int col=SQUARE_FILE(from) - 3;
				if(FileCapMax[row][board->file[col]] + col == to || FileCapMin[row][board->file[col]] + col == to)
					return true;
			}
			else if(EQUAL_RANK(from, to))
			{
				int row=SQUARE_RANK(from) - 3;
				int col=SQUARE_FILE(from) - 3;
				if(RankCapMax[col][board->rank[row]] + (row << 4) == to ||
					RankCapMin[col][board->rank[row]] + (row << 4) == to)
					return true;
			}

		}
		else
		{
			int row=SQUARE_RANK(from) - 3;
			int col=SQUARE_FILE(from) - 3;
			int to_row = SQUARE_RANK(to) - 3;
			int to_col = SQUARE_FILE(to) - 3;
			int rowbit = board->rank[row];
			int colbit = board->file[col];
			if(EQUAL_FILE(from, to))
			{
				//if(FileNonCapMin[row][board->file[col]] + col <= to && to <= FileNonCapMax[row][board->file[col]] + col)
                if(PinNb[row][to_row][colbit] == 0)
					return true;
			}
			else if(EQUAL_RANK(from, to))
			{
				//if(RankNonCapMin[col][board->rank[row]] + (row << 4) <= to && to <= RankNonCapMax[col][board->rank[row]] + (row << 4))
				 if(PinNb[col][to_col][rowbit] == 0)
				    return true;
			}

		}

		return false;

	case RedCannon:
	case BlackCannon:
		if(capture)
		{
			if(EQUAL_RANK(from, to))
			{
				int row=SQUARE_RANK(from) - 3;
				int col=SQUARE_FILE(from) - 3;
				if(CannonRankCapMax[col][board->rank[row]] + (row << 4) == to ||
					CannonRankCapMin[col][board->rank[row]] + (row << 4) == to)
					return true;
			}
			else if(EQUAL_FILE(from, to))
			{
				int row=SQUARE_RANK(from) - 3;
				int col=SQUARE_FILE(from) - 3;
				if(CannonFileCapMax[row][board->file[col]] + col == to ||
					CannonFileCapMin[row][board->file[col]] + col == to)
					return true;
			}
		}
		else
		{
			int row=SQUARE_RANK(from) - 3;
			int col=SQUARE_FILE(from) - 3;
			int to_row = SQUARE_RANK(to) - 3;
			int to_col = SQUARE_FILE(to) - 3;
			int rowbit = board->rank[row];
			int colbit = board->file[col];

			if(EQUAL_RANK(from, to))
			{
				//if(RankNonCapMin[col][board->rank[row]] + (row << 4) <= to && to <= RankNonCapMax[col][board->rank[row]] + (row << 4))
				if(PinNb[col][to_col][rowbit] == 0)
					return true;
			}
			else if(EQUAL_FILE(from, to))
			{
				//if(FileNonCapMin[row][board->file[col]] + col <= to&& to <= FileNonCapMax[row][board->file[col]] + col)
				if(PinNb[row][to_row][colbit] == 0)
					return true;
			}
		}

		return false;

	case BlackPawn:

		if((from + 16) == to) return true;
		if(SQUARE_COLOUR(from) == Red)
		{
			if((from - 1) == to || (from + 1) == to)
				return true;
		}

		return false;

	case RedPawn:

		if((from - 16) == to) return true;
		if(SQUARE_COLOUR(from) == Black)
		{
			if((from - 1) == to || (from + 1) == to)
				return true;
		}

	default:
		return false;
	}


	return false;
}

// quiet_is_pseudo()

bool quiet_is_pseudo( int move, board_t *board )
{
	int to;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));

	to = MOVE_TO(move);
	ASSERT(SQUARE_IS_OK(to));

	if( board->square[to] != Empty )
		return false; // capture

	return move_is_pseudo(move, board );
}

// pseudo_is_legal()

bool pseudo_is_legal( int move, board_t *board )
{

	int me;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	// init
	me = board->turn;

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
