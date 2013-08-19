// move_evasion.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "list.h"
#include "move.h"
#include "move_evasion.h"
#include "move_gen.h"
#include "move_evasion.h"
#include "piece.h"
#include "types.h"
#include "movetable.h"

// prototypes

static bool gen_evasions( list_t *list, board_t *board, const attack_t *attack, bool legal, bool stop );
static bool add_capture_moves( list_t *list, board_t *board, int to, bool legal, bool stop );
static bool add_quiet_moves( list_t *list, board_t *board, int to, bool legal, bool stop );

// functions

// gen_legal_evasions()

void gen_legal_evasions( list_t *list, board_t *board, const attack_t *attack )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);
	ASSERT(attack != NULL);

	gen_evasions(list, board, attack, true, false);

	// debug

	ASSERT(list_is_ok(list));
}

// gen_pseudo_evasions()

void gen_pseudo_evasions( list_t *list, board_t *board, const attack_t *attack )
{

	ASSERT(list != NULL);
	ASSERT(board != NULL);
	ASSERT(attack != NULL);

	gen_evasions(list, board, attack, false, false);

	// debug

	ASSERT(list_is_ok(list));
}

// legal_evasion_exist()

bool legal_evasion_exist( board_t *board, const attack_t *attack )
{

	list_t list[1]; // dummy

	ASSERT(board != NULL);
	ASSERT(attack != NULL);

	return gen_evasions(list, board, attack, true, true);
}

// gen_evasions()

static bool gen_evasions( list_t *list, board_t *board, const attack_t *attack, bool legal, bool stop )
{

	int me, opp;
	int from;
	int to;
	int index;
	int row, col;
	int king;
	int rowbit, colbit;

	ASSERT(list != NULL);
	ASSERT(board != NULL);
	ASSERT(attack != NULL);
	ASSERT(legal == true || legal == false);
	ASSERT(stop == true || stop == false);

	ASSERT(board_is_check(board));
	ASSERT(ATTACK_IN_CHECK(attack));

	// init

	LIST_CLEAR(list);

	me = board->turn;
	opp = COLOUR_OPP(me);

	from = KING_POS(board, me);

	index=0;
	to = KingMoves[from][index];
	while(to != SquareNone)
	{		
		if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
		{
			//is_attacked
			if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me)/*!is_attacked(board, from, to, opp)*/ )
			{
				if( stop )
					return true;
				LIST_ADD(list, MOVE_MAKE(from, to));
			}
		}
		to = KingMoves[from][++index];
	}

	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	king = KING_POS(board, opp);
	if(RookFileAttack[from][king][colbit])
	{
		if( !legal /*|| !is_attacked(board, to, opp) */)
		{
			if( stop )
				return true;
			LIST_ADD(list, MOVE_MAKE(from, to));
		}
	}
	
	
	if( attack->dn > 11)
		return false; // more check, we are done

	// capture the checking piece
	if( add_capture_moves(list, board, attack->ds[0], legal, stop) && stop )
		return true;

	// interpose a piece
	if (add_quiet_moves(list, board, attack->ds[0], legal, stop) && stop)
	   return true;

	return false;
}

// add_piece_moves()

static bool add_capture_moves( list_t *list,board_t *board, int to, bool legal, bool stop )
{

	int me;
	int start, end;
	int row, col;
	int rowbit, colbit;
	int from;
	int i;

	ASSERT(list != NULL);
	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(to));
	ASSERT(legal == true || legal == false);
	ASSERT(stop == true || stop == false);

	me = board->turn;

	start = INDEX32_START(me);
	end   = INDEX32_END(me);

	for ( i = start; i <= end; ++i)
	{
		from = board->piece[i];
		if (from != SquareNone)
		{
			switch(INDEX_TO_PIECE16(i))
			{
			case RedRook16:
			case BlackRook16:
				{
					row = SQUARE_RANK(from) - 3;
					col = SQUARE_FILE(from) - 3;
					rowbit = board->rank[row];
					colbit = board->file[col];

					if(RookFileAttack[from][to][colbit] || RookRankAttack[from][to][rowbit])
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}	

					break;
				}
			case RedCannon16:
			case BlackCannon16:
				{
					row = SQUARE_RANK(from) - 3;
					col = SQUARE_FILE(from) - 3;
					rowbit = board->rank[row];
					colbit = board->file[col];

					if(CannonFileAttack[from][to][colbit] || CannonRankAttack[from][to][rowbit])
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
	
					break;
				}
			case RedKnight16:
			case BlackKnight16:
				{
					if(KnightAttack[from][to] && board->square[KnightLegs[from][to]] == PieceNone) 
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
					break;
				}
			case RedAdvisor16:
			case BlackAdvisor16:
				{
					if(AdvisorAttack[from][to])
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
					break;
				}
			case RedBishop16:
			case BlackBishop16:
				{
					if(BishopAttack[from][to] && board->square[BishopEyes[from][to]] == PieceNone)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
					break;
				}
			case RedPawn16:
			case BlackPawn16:
				{
					if(PawnAttack[me][from][to])
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}

					break;
				} 	
			case RedKing16:
			case BlackKing16:
				break;
			default:
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
	}

	return false;
}

bool add_quiet_moves( list_t *list, board_t *board, int checksq ,bool legal, bool stop )
{
	int me;
	int from, to;
	int start, end;
	int row, col;
	int rowbit, colbit;
	int i;
	int index;

	me = board->turn;

	start = INDEX32_START(me);
	end   = INDEX32_END(me);

	for(i = start; i <= end; ++i)
	{
		if(board->piece[i] == SquareNone) continue;

		from = board->piece[i];
		switch(INDEX_TO_PIECE16(i))
		{
		case RedRook16:
		case BlackRook16:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			to = FileCapMoves[row][col][colbit][index];
			while(to != SquareNone)
			{
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				to = FileCapMoves[row][col][colbit][++index];
				
			}

			index=0;
			to = RankCapMoves[col][row][rowbit][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				
				to = RankCapMoves[col][row][rowbit][++index];				
			}

			index=0;
            to =  FileNoneCapMoves[row][col][colbit][index];
			while(to != SquareNone)
			{				
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				to =  FileNoneCapMoves[row][col][colbit][++index];
				
			}

			index=0;
            to = RankNoneCapMoves[col][row][rowbit][index];
			while(to != SquareNone)
			{				
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}

				to = RankNoneCapMoves[col][row][rowbit][++index];
				
			}
			break;

		case RedCannon16:
		case BlackCannon16:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			to = CannonFileCapMoves[row][col][colbit][index];
			while(to != SquareNone)
			{				
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				
				to = CannonFileCapMoves[row][col][colbit][++index];
				
			}

			index=0;
			to = CannonRankCapMoves[col][row][rowbit][index];
			while(to != SquareNone)
			{
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}
				to = CannonRankCapMoves[col][row][rowbit][++index];

				
			}

			index=0;
			to = FileNoneCapMoves[row][col][colbit][index];
			while(to != SquareNone)
			{				
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = FileNoneCapMoves[row][col][colbit][++index];
				
			}

			index=0;
			to = RankNoneCapMoves[col][row][rowbit][index];
			while(to != SquareNone)
			{				
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
                to = RankNoneCapMoves[col][row][rowbit][++index];

				
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
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
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
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
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
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
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
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}
				to=BishopMoves[from][++index];

				
			}
			break;

		case RedKing16:
		case BlackKing16:
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	return false;
}

// end of move_evasion.cpp
