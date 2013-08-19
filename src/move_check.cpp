// move_check.cpp

// includes

#include "attack.h"
#include "colour.h"
#include "fen.h"
#include "list.h"
#include "move.h"
#include "move_check.h"
#include "move_do.h"
#include "move_gen.h"
#include "piece.h"
#include "square.h"
#include "util.h"
#include "movetable.h"

// prototypes

static void add_quiet_checks( list_t *list, board_t *board );
static void add_check( list_t *list, int move, board_t *board );
static void find_pins( int list [], board_t *board );

// functions

// gen_quiet_checks()

void gen_quiet_checks( list_t *list, board_t *board )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));

	LIST_CLEAR(list);

	add_quiet_checks(list, board);

	// debug

	ASSERT(list_is_ok(list));
}

// add_quiet_checks()
static void add_quiet_checks( list_t *list, board_t *board )
{
	int me, opp;
	int from, to;
	int row, col;
	int filebit, rankbit;
	int i, index;
	int start, end;

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	me = board->turn;
	opp = COLOUR_OPP(me);

	start = INDEX32_START(me);
	end   = INDEX32_END(me);

	for( i= start; i <= end; ++i)
	{
		if(board->piece[i] == SquareNone) continue;

		from = board->piece[i];
		switch(INDEX_TO_PIECE16(i))
		{
		case RedRook16:
		case BlackRook16:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit =  board->file[col];
			rankbit =  board->rank[row];

			index=0;
            to =  FileNoneCapMoves[row][col][filebit][index];
			while(to != SquareNone)
			{				
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
					LIST_ADD(list, MOVE_MAKE(from, to));

				to =  FileNoneCapMoves[row][col][filebit][++index];				
			}

			index=0;
            to = RankNoneCapMoves[col][row][rankbit][index];
			while(to != SquareNone)
			{				
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
					LIST_ADD(list, MOVE_MAKE(from, to));
				to = RankNoneCapMoves[col][row][rankbit][++index];
				
			}
			break;

		case RedCannon16:
		case BlackCannon16:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit = board->file[col];
			rankbit = board->rank[row];

			index=0;
			to = FileNoneCapMoves[row][col][filebit][index];
			while(to != SquareNone)
			{				
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
					LIST_ADD(list, MOVE_MAKE(from, to));
                to = FileNoneCapMoves[row][col][filebit][++index];
			}

			index=0;
			to = RankNoneCapMoves[col][row][rankbit][index];
			while(to != SquareNone)
			{				
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
					LIST_ADD(list, MOVE_MAKE(from, to));
                to = RankNoneCapMoves[col][row][rankbit][++index];
			}
			break;

		case RedKnight16:
		case BlackKnight16:
			
			index=0;
			to = KnightMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[HorseLegs[from][index]] == PieceNone)
				{
					if( board->square[to]  == PieceNone)
					{
                        if(is_pinned(board, MOVE_MAKE(from, to),opp))
					      LIST_ADD(list, MOVE_MAKE(from, to));
						
					}
				}
				to = KnightMoves[from][++index];
			}
			break;

		case RedPawn16:
		case BlackPawn16:
			index=0;
			to = PawnMoves[me][from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
						LIST_ADD(list, MOVE_MAKE(from, to));
				}
				to = PawnMoves[me][from][++index];
			}
			break;

		case RedAdvisor16:
		case BlackAdvisor16:
			index=0;
			to = AdvisorMoves[from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
						LIST_ADD(list, MOVE_MAKE(from, to));
				}

				to = AdvisorMoves[from][++index];
			}
			break;

		case RedBishop16:
		case BlackBishop16:
			index=0;
			to=BishopMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[ElephantEyes[from][index]] == PieceNone)
				{					
					if(board->square[to]  == PieceNone)
					{
						if(is_pinned(board, MOVE_MAKE(from, to),opp))
							LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to=BishopMoves[from][++index];
				
			}
			break;

		case RedKing16:
		case BlackKing16:
			index=0;
			to = KingMoves[from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
						LIST_ADD(list, MOVE_MAKE(from, to));
				}

                to = KingMoves[from][++index];

			}
			break;
		default:
			ASSERT(false);
			break;
			}
	}
}

// add_check()

static void add_check( list_t *list, int move, board_t *board )
{

	undo_t undo[1];

	ASSERT(list != NULL);
	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	move_do(board, move, undo);

	if( IS_IN_CHECK(board, board->turn) )
		LIST_ADD(list, move);
	move_undo(board, move, undo);
}

// move_is_check()

bool move_is_check( int move, board_t *board )
{

	undo_t undo[1];
	bool check;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	// slow test for complex moves

	move_do(board, move, undo);//move_do之后，turun会变化，这是一个将对方king的move
	check = IS_IN_CHECK(board, board->turn);
	move_undo(board, move, undo);

	return check;
}

// end of move_check.cpp
