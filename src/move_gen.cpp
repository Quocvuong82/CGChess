// move_gen.cpp

// includes
#include "attack.h"
#include "board.h"
#include "colour.h"
#include "list.h"
#include "move.h"
#include "move_do.h"
#include "move_evasion.h"
#include "move_gen.h"
#include "move_legal.h"
#include "piece.h"
#include "types.h"
#include "movetable.h"

// prototypes

static void add_moves( list_t *list, board_t *board );
static void add_captures( list_t *list, board_t *board );
static void add_quiet_moves( list_t *list, board_t *board );

// functions

// perft()

sint64 perft( board_t *board, int depth )
{
	list_t list[1];
	undo_t undo[1];
	int move;
	sint64 value = 0;

	gen_legal_moves(list, board);

	if( !depth )
		return list->size;

	for ( int pos = 0; pos < list->size; pos++ )
	{
		move = LIST_MOVE(list, pos);
		move_do(board, move, undo);
		ASSERT(board_is_ok(board));
		value += perft(board, depth - 1);
		move_undo(board, move, undo);
		assert(board_is_ok(board));
	}
	return value;
}

// gen_legal_moves()

void gen_legal_moves( list_t *list, board_t *board )
{

	attack_t attack[1];

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	attack_set(attack, board);

	if( ATTACK_IN_CHECK(attack) )
	{
		gen_legal_evasions(list, board, attack);
	}
	else
	{
		gen_moves(list, board);
		list_filter(list, board, &pseudo_is_legal, true);
	}

	// debug

	ASSERT(list_is_ok(list));
}

// gen_moves()

void gen_moves( list_t *list, board_t *board )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));

	LIST_CLEAR(list);

	add_moves(list, board);
	// debug

	ASSERT(list_is_ok(list));
}

// gen_captures()

void gen_captures( list_t *list, board_t *board )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	LIST_CLEAR(list);

	add_captures(list, board);

	// debug

	ASSERT(list_is_ok(list));
}

// gen_quiet_moves()

void gen_quiet_moves( list_t *list, board_t *board )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	ASSERT(!board_is_check(board));

	LIST_CLEAR(list);

	add_quiet_moves(list, board);

	// debug

	ASSERT(list_is_ok(list));
}

// add_moves()

static void add_moves( list_t *list, board_t *board )
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
			while(FileCapMoves[row][filebit][index])
			{
				to = FileCapMoves[row][filebit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					LIST_ADD(list, MOVE_MAKE(from, to));
				}

				++index;				
			}

			index=0;			
			while(RankCapMoves[col][rankbit][index])
			{	
				to = RankCapMoves[col][rankbit][index] + (row<<4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                   LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;				
			}

			index=0;           
			while(FileNoneCapMoves[row][filebit][index])
			{
				to =  FileNoneCapMoves[row][filebit][index] + col;
                LIST_ADD(list, MOVE_MAKE(from, to));
				++index;				
			}

			index=0;
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to = RankNoneCapMoves[col][rankbit][index] + (row<<4);
                LIST_ADD(list, MOVE_MAKE(from, to));				
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
			
			while( CannonFileCapMoves[row][filebit][index])
			{
				to = CannonFileCapMoves[row][filebit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                   LIST_ADD(list, MOVE_MAKE(from, to));
				}

				++index;				
			}

			index=0;			
			while(CannonRankCapMoves[col][rankbit][index])
			{
				to = CannonRankCapMoves[col][rankbit][index] + (row<<4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
				
				++index;
			}

			index=0;			
			while(FileNoneCapMoves[row][filebit][index])
			{	
				to = FileNoneCapMoves[row][filebit][index] + col;
                LIST_ADD(list, MOVE_MAKE(from, to));
                ++index;				
			}

			index=0;			
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to = RankNoneCapMoves[col][rankbit][index]  + (row<<4);
                LIST_ADD(list, MOVE_MAKE(from, to));
                ++index;				
			}
			break;

		case RedKnight:
		case BlackKnight:
			
			index=0;
			to = KnightMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[HorseLegs[from][index]] == PieceNone)
				{
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
                        LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to = KnightMoves[from][++index];
				
			}
			break;

		case RedPawn:
		case BlackPawn:
			index=0;
			to = PawnMoves[me][from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
				to = PawnMoves[me][from][++index];
				
			}
			break;

		case RedAdvisor:
		case BlackAdvisor:
			index=0;
			to = AdvisorMoves[from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = AdvisorMoves[from][++index];
				
			}
			break;

		case RedBishop:
		case BlackBishop:
			index=0;
			to=BishopMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[ElephantEyes[from][index]] == PieceNone)
				{					
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
                       LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to=BishopMoves[from][++index];

				
			}
			break;

		case RedKing:
		case BlackKing:
			index=0;
			to = KingMoves[from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = KingMoves[from][++index];				
			}

			to = KING_POS(board, opp);
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit = board->file[col];
			rankbit = board->rank[row];
			if(EQUAL_FILE(from, to))
			{
				if(FileCapMin[row][filebit] + col == to || FileCapMax[row][filebit] + col == to)
				{
					LIST_ADD(list, MOVE_MAKE(from, to));
				}
			}

			break;
		default:
			ASSERT(false);
			break;
			}
	}
}

// add_captures()

static void add_captures( list_t *list, board_t *board )
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
			while(FileCapMoves[row][filebit][index])
			{
				to = FileCapMoves[row][filebit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					LIST_ADD(list, MOVE_MAKE(from, to));
				}

				++index;				
			}

			index=0;			
			while(RankCapMoves[col][rankbit][index])
			{	
				to = RankCapMoves[col][rankbit][index] + (row<<4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
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
			while( CannonFileCapMoves[row][filebit][index])
			{
				to = CannonFileCapMoves[row][filebit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                   LIST_ADD(list, MOVE_MAKE(from, to));
				}

				++index;				
			}

			index=0;			
			while(CannonRankCapMoves[col][rankbit][index])
			{
				to = CannonRankCapMoves[col][rankbit][index] + (row<<4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
				
				++index;
			}

			break;

		case RedKnight:
		case BlackKnight:
			
			index=0;
			to = KnightMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[HorseLegs[from][index]] == PieceNone)
				{
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) &&  board->square[to]  != PieceNone)
					{
                        LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to = KnightMoves[from][++index];
				
			}
			break;

		case RedPawn:
		case BlackPawn:
			index=0;
			to = PawnMoves[me][from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
				to = PawnMoves[me][from][++index];
				
			}
			break;

		case RedAdvisor:
		case BlackAdvisor:
			index=0;
			to = AdvisorMoves[from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = AdvisorMoves[from][++index];
				
			}
			break;

		case RedBishop:
		case BlackBishop:
			index=0;
			to=BishopMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[ElephantEyes[from][index]] == PieceNone)
				{					
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
					{
                       LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to=BishopMoves[from][++index];
				
			}
			break;

		case RedKing:
		case BlackKing:
			index=0;
			to = KingMoves[from][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = KingMoves[from][++index];
				
			}

			to = KING_POS(board, opp);
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit = board->file[col];
			rankbit = board->rank[row];
			if(EQUAL_FILE(from, to))
			{
				if(FileCapMin[row][filebit] + col == to || FileCapMax[row][filebit] + col == to)
				{
					LIST_ADD(list, MOVE_MAKE(from, to));
				}
			}

			break;
		default:
			ASSERT(false);
			break;
			}
	}

}

// add_quiet_moves()

static void add_quiet_moves( list_t *list, board_t *board )
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
				to =  FileNoneCapMoves[row][filebit][index] + col;
                LIST_ADD(list, MOVE_MAKE(from, to));
				++index;				
			}

			index=0;
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to = RankNoneCapMoves[col][rankbit][index] + (row<<4);
                LIST_ADD(list, MOVE_MAKE(from, to));				
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
                LIST_ADD(list, MOVE_MAKE(from, to));
                ++index;				
			}

			index=0;			
			while(RankNoneCapMoves[col][rankbit][index])
			{
				to = RankNoneCapMoves[col][rankbit][index]  + (row<<4);
                LIST_ADD(list, MOVE_MAKE(from, to));
                ++index;				
			}
			break;

		case RedKnight:
		case BlackKnight:
			
			index=0;
			to = KnightMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[HorseLegs[from][index]] == PieceNone)
				{
					if( board->square[to]  == PieceNone)
					{
                        LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to = KnightMoves[from][++index];

				
			}
			break;

		case RedPawn:
		case BlackPawn:
			index=0;
			to = PawnMoves[me][from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}

				to = PawnMoves[me][from][++index];
				
			}
			break;

		case RedAdvisor:
		case BlackAdvisor:
			index=0;
			to = AdvisorMoves[from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
                    LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = AdvisorMoves[from][++index];
				
			}
			break;

		case RedBishop:
		case BlackBishop:
			index=0;
			to=BishopMoves[from][index];
			while(to != SquareNone)
			{
				if(board->square[ElephantEyes[from][index]] == PieceNone)
				{					
					if(board->square[to]  == PieceNone)
					{
                       LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to=BishopMoves[from][++index];
				
			}
			break;

		case RedKing:
		case BlackKing:
			index=0;
			to = KingMoves[from][index];
			while(to != SquareNone)
			{				
				if(board->square[to]  == PieceNone)
				{
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

// end of move_gen.cpp
