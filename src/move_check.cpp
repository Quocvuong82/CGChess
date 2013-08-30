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
	int king_sq;
	int from, to;
	int start, end;
	int row, col;
	int filebit, rankbit;
	int i, index;

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	// init

	me = board->turn;
	opp = COLOUR_OPP(me);

	king_sq = KING_POS(board, opp);

	start = INDEX_START(me);
	end   = INDEX_END(me);

	for( i= start; i <= end; ++i)
	{
		if(board->piece[i] == SquareNone) continue;

		from = board->piece[i];
		switch(INDEX_TO_PIECE(i))
		{
		case RedRook:
		case BlackRook:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit =  board->file[col];
			rankbit =  board->rank[row];

			index=0;
			while(FileNoneCapMoves[row][filebit][index])
			{
				to=FileNoneCapMoves[row][filebit][index] + col;
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
				   LIST_ADD(list, MOVE_MAKE(from, to));
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to=RankNoneCapMoves[col][rankbit][index] + (row << 4);
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
				{
					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}
			break;

		case RedCannon:
		case BlackCannon:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit = board->file[col];
			rankbit = board->rank[row];

			index=0;
			while(FileNoneCapMoves[row][filebit][index])
			{
				to = FileNoneCapMoves[row][filebit][index] + col;
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
				{
					 if(!is_king_to_king(board, MOVE_MAKE(from, to)))
					   LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to = RankNoneCapMoves[col][rankbit][index] + (row << 4);
				if(is_pinned(board, MOVE_MAKE(from, to),opp))
				{
					if(!is_king_to_king(board, MOVE_MAKE(from, to))) 
					   LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}
			break;

		case RedKnight:
		case BlackKnight:
			index=0;
			while(KnightMoves[from][index])
			{
				if(board->square[HorseLegs[from][index]] == PieceNone)
				{
					to = KnightMoves[from][index];
					if(board->square[to] == PieceNone)
					{
						if(is_pinned(board, MOVE_MAKE(from, to),opp))
						{
							 if(!is_king_to_king(board, MOVE_MAKE(from, to))) 
							   LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}

				++index;
			}
			break;

		case RedPawn:
			index=0;
			while(PawnMoves[Red][from][index])
			{
				to = PawnMoves[Red][from][index];
				if(board->square[to] == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
					{
						if(!is_king_to_king(board, MOVE_MAKE(from, to)))
						   LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}
			break;

		case BlackPawn:
			index=0;
			while(PawnMoves[Black][from][index])
			{
				to = PawnMoves[Black][from][index];
				if(board->square[to] == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
					{
					    if(!is_king_to_king(board, MOVE_MAKE(from, to)))
						   LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}
			break;

		case RedAdvisor:
		case BlackAdvisor:
			index=0;
			while(AdvisorMoves[from][index])
			{
				to = AdvisorMoves[from][index];
				if(board->square[to] == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
					{
					   if(!is_king_to_king(board, MOVE_MAKE(from, to)))
						 LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}
			break;

		case RedBishop:
		case BlackBishop:
			index=0;
			while(BishopMoves[from][index])
			{
				if(board->square[ElephantEyes[from][index]] == PieceNone)
				{
					to=BishopMoves[from][index];
					if(board->square[to] == PieceNone)
					{
						if(is_pinned(board, MOVE_MAKE(from, to),opp))
						{
						   if(!is_king_to_king(board, MOVE_MAKE(from, to)))
							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}

				++index;
			}
			break;

		case RedKing:
			index=0;
			while(KingMoves[from][index])
			{
				to = KingMoves[from][index];
				if(board->square[to] == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
					{
						if(!is_king_to_king(board, MOVE_MAKE(from, to)))
						   LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}
			break;

		case BlackKing:
			index=0;
			while(KingMoves[from][index])
			{
				to = KingMoves[from][index];
				if(board->square[to] == PieceNone)
				{
					if(is_pinned(board, MOVE_MAKE(from, to),opp))
					{
						if(!is_king_to_king(board, MOVE_MAKE(from, to)))
						   LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
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

	move_do(board, move, undo);//move_do£¬turn change£¬this is check opp king move
	check = IS_IN_CHECK(board, board->turn);
	move_undo(board, move, undo);

	return check;
}

// end of move_check.cpp
