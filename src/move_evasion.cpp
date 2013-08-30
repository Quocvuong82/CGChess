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
static bool add_multi_moves(list_t *list, board_t *board, int to, bool legal, bool stop);

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
    LIST_CLEAR(list);
	return add_multi_moves(list, board, -1, legal, stop);

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
	if(EQUAL_FILE(from, king))
	{
		if(FileCapMin[row][colbit] + col == king || FileCapMax[row][colbit] + col == king)
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

	start = INDEX_START(me);
	end   = INDEX_END(me);

	for ( i = start; i <= end; ++i)
	{
		from = board->piece[i];
		if (from != SquareNone)
		{
			switch(INDEX_TO_PIECE(i))
			{
			case RedRook:
			case BlackRook:
				{
					row = SQUARE_RANK(from) - 3;
					col = SQUARE_FILE(from) - 3;
					rowbit = board->rank[row];
					colbit = board->file[col];

					if (EQUAL_FILE(from, to))
					{
						if(FileCapMin[row][colbit] + col == to || FileCapMax[row][colbit] + col == to)
						{
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
					}
					else if (EQUAL_RANK(from, to))
					{
						if(RankCapMin[col][rowbit] + (row << 4) == to || RankCapMax[col][rowbit] + (row << 4) == to)
						{
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
					}	

					break;
				}
			case RedCannon:
			case BlackCannon:
				{
					row = SQUARE_RANK(from) - 3;
					col = SQUARE_FILE(from) - 3;
					rowbit = board->rank[row];
					colbit = board->file[col];

					if(EQUAL_FILE(from, to))
					{
						if(CannonFileCapMin[row][colbit] + col == to || CannonFileCapMax[row][colbit] + col == to)
						{	
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
					}
					else if(EQUAL_RANK(from, to))
					{
						if(CannonRankCapMin[col][rowbit] + (row << 4) == to || CannonRankCapMax[col][rowbit] + (row << 4) == to)
						{
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
					}
	
					break;
				}
			case RedKnight:
			case BlackKnight:
				{
					if(board->square[from + NLegalDt[to - from + 256]] == SquareNone)
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
			case RedAdvisor:
			case BlackAdvisor:
				{
					if(ADVISOR_IN_CITY(to) && ALegalDt[to - from + 256])
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
			case RedBishop:
			case BlackBishop:
				{
					if(BISHOP_IN_CITY(to) && BLegalDt[to - from + 256] && board->square[(to + from) / 2] == PieceNone)
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
			case RedPawn:
				if(to == from - 16)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				if(SQUARE_COLOUR(from) == Black)
				{
					if (to == from + 1)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}

					if(to == from - 1)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}
				break;
			case BlackPawn:
				{
					if(to == from + 16)
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}

					if(SQUARE_COLOUR(from) == Red)
					{
						if(to == from + 1 )
						{
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
						if(to == from - 1)
						{
							if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
							{
								if( stop )
									return true;

								LIST_ADD(list, MOVE_MAKE(from, to));
							}
						}
					}

					break;
				} 	
			case RedKing:
			case BlackKing:
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

	start = INDEX_START(me);
	end   = INDEX_END(me);

	for(i = start; i <= end; ++i)
	{
		if(board->piece[i] == SquareNone) continue;

		from = board->piece[i];
		switch(INDEX_TO_PIECE(i))
		{
		case RedRook:
		case BlackRook:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			while(FileCapMoves[row][colbit][index])
			{
				to = FileCapMoves[row][colbit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if(!legal || !is_pinned(board, MOVE_MAKE(from, to), me))
					{
						if( stop )
						  return true;
						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(RankCapMoves[col][rowbit][index])
			{
				to=RankCapMoves[col][rowbit][index] + (row << 4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if(!legal || !is_pinned(board, MOVE_MAKE(from, to), me))
					{
						if( stop )
							return true;
						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(FileNoneCapMoves[row][colbit][index])
			{
				to=FileNoneCapMoves[row][colbit][index] + col;
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rowbit][index])
			{
				to=RankNoneCapMoves[col][rowbit][index] + (row << 4);
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}
			break;

		case RedCannon:
		case BlackCannon:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			while(CannonFileCapMoves[row][colbit][index])
			{
				to = CannonFileCapMoves[row][colbit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(CannonRankCapMoves[col][rowbit][index])
			{
				to = CannonRankCapMoves[col][rowbit][index] + (row << 4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && to != checksq)
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(FileNoneCapMoves[row][colbit][index])
			{
				to=FileNoneCapMoves[row][colbit][index] + col;
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rowbit][index])
			{
				to=RankNoneCapMoves[col][rowbit][index] + (row << 4);
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

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
					to=KnightMoves[from][index];
					if((board->square[to] == PieceNone) || (to != checksq && !INDEX_SAME_COLOUR(board->square[from], board->square[to])) )
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

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
				to=PawnMoves[Red][from][index];
				if(board->square[to] == PieceNone || (to != checksq && !INDEX_SAME_COLOUR(board->square[from], board->square[to])) )
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
				to=PawnMoves[Black][from][index];
				if(board->square[to] == PieceNone || (to != checksq && !INDEX_SAME_COLOUR(board->square[from], board->square[to])) )
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
				to=AdvisorMoves[from][index];
				if(board->square[to] == PieceNone || (to != checksq && !INDEX_SAME_COLOUR(board->square[from], board->square[to])))
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
					to = BishopMoves[from][index];
					if(board->square[to] == PieceNone || (to != checksq && !INDEX_SAME_COLOUR(board->square[from], board->square[to])))
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}

				++index;
			}
			break;

		case RedKing:
		case BlackKing:
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return false;
}

static bool add_multi_moves(list_t *list, board_t *board, int sq, bool legal, bool stop)
{
	int me, opp;
	int from, to;
	int start, end;
	int row, col;
	int rowbit, colbit;
	int i;
	int index;
	int king;

	me = board->turn;
	opp = COLOUR_OPP(me);

	start = INDEX_START(me);
	end   = INDEX_END(me);

	for(i = start; i <= end; ++i)
	{
		if(board->piece[i] == SquareNone) continue;

		from = board->piece[i];
		switch(INDEX_TO_PIECE(i))
		{
		case RedRook:
		case BlackRook:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			while(FileCapMoves[row][colbit][index])
			{
				to = FileCapMoves[row][colbit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) )
				{
					if(!legal || !is_pinned(board, MOVE_MAKE(from, to), me))
					{
						if( stop )
						  return true;
						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(RankCapMoves[col][rowbit][index])
			{
				to=RankCapMoves[col][rowbit][index] + (row << 4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if(!legal || !is_pinned(board, MOVE_MAKE(from, to), me))
					{
						if( stop )
							return true;
						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(FileNoneCapMoves[row][colbit][index])
			{
				to=FileNoneCapMoves[row][colbit][index] + col;
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rowbit][index])
			{
				to=RankNoneCapMoves[col][rowbit][index] + (row << 4);
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}
			break;

		case RedCannon:
		case BlackCannon:
			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			index=0;
			while(CannonFileCapMoves[row][colbit][index])
			{
				to = CannonFileCapMoves[row][colbit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(CannonRankCapMoves[col][rowbit][index])
			{
				to = CannonRankCapMoves[col][rowbit][index] + (row << 4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) )
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

						LIST_ADD(list, MOVE_MAKE(from, to));
					}
				}

				++index;
			}

			index=0;
			while(FileNoneCapMoves[row][colbit][index])
			{
				to=FileNoneCapMoves[row][colbit][index] + col;
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

					LIST_ADD(list, MOVE_MAKE(from, to));
				}
				++index;
			}

			index=0;
			while(RankNoneCapMoves[col][rowbit][index])
			{
				to=RankNoneCapMoves[col][rowbit][index] + (row << 4);
				if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
				{
					if( stop )
						return true;

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
					to=KnightMoves[from][index];
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

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
				to=PawnMoves[Red][from][index];
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
				to=PawnMoves[Black][from][index];
				if( !INDEX_SAME_COLOUR(board->square[from], board->square[to]) )
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
				to=AdvisorMoves[from][index];
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
					{
						if( stop )
							return true;

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
					to = BishopMoves[from][index];
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						if( !legal || !is_pinned(board, MOVE_MAKE(from, to), me) )
						{
							if( stop )
								return true;

							LIST_ADD(list, MOVE_MAKE(from, to));
						}
					}
				}

				++index;
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
			if(EQUAL_FILE(from, king))
			{
				if(FileCapMin[row][colbit] + col == king || FileCapMax[row][colbit] + col == king)
				{

					if( stop )
						return true;
					LIST_ADD(list, MOVE_MAKE(from, to));

				}
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return false;
}

// end of move_evasion.cpp
