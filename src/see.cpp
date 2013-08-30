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
#include "movetable.h"

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
	int att, def;
	int from, to;
	alists_t alists[1];
	int value, piece_value;
	int piece, capture;
	alist_t *alist;
	int pos;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	// init

	from = MOVE_FROM(move);
	to = MOVE_TO(move);

	// move the piece

	piece_value = 0;

	piece = board->square[from];
	ASSERT(INDEX_IS_OK(piece));

	att = INDEX_TO_COLOUR(piece);
	def = COLOUR_OPP(att);


	piece_value += VALUE_INDEX(piece);
	// clear attacker lists

	ALIST_CLEAR(alists->alist[Black]);
	ALIST_CLEAR(alists->alist[Red]);

	// find hidden attackers
	//当走from->to这步棋后，我方可以攻击to的其他棋子
	alists_hidden(alists, board, from, to);

	// capture the piece

	value = 0;

	capture = board->square[to];

	if( capture != Empty )
	{

		ASSERT(INDEX_IS_OK(capture));
		ASSERT(INDEX_TO_COLOUR(capture) == def);

		value += VALUE_INDEX(capture);
	}


	// build defender list

	alist = alists->alist[def];

	//对方所有可以保护to的棋子
	alist_build(alist, board, to, def);

	//没有保护，返回to子的分数
	if( alist->size == 0 )
		return value; // no defender => stop SEE

	// build attacker list

	alist = alists->alist[att];
	//我方可以攻击to位置的所有棋子
	alist_build(alist, board, to, att);

	// remove the moved piece (if it's an attacker)
	for ( pos = 0; pos < alist->size && alist->square[pos] != from; pos++ )
		;

	if( pos < alist->size )
		alist_remove(alist, pos);

	// SEE search

	value -= see_rec(alists, board, def, to, piece_value);

	return value;
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

	ASSERT(INDEX_TO_COLOUR(board->square[to]) == COLOUR_OPP(colour));

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
	piece_value = VALUE_INDEX(piece);


	value -= see_rec(alists, board, COLOUR_OPP(colour), to, piece_value);

	if( value < 0 )
		value = 0;

	return value;
}

// alist_build()

static void alist_build( alist_t *alist, board_t *board, int to, int colour )
{

	// int from;
	//int piece;
	//int delta;
	//int inc;
	//int sq;
	//int pawn;

	ASSERT(alist != NULL);
	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(to));
	ASSERT(COLOUR_IS_OK(colour));

	// piece attacks
	// colour方所有攻击to的位置
	//for ( ptr = &board->piece[colour][0]; (from = *ptr) != SquareNone; ptr++ )
	//    {

	//    piece = board->square[from];
	//    delta = to - from;

	//    if( PSEUDO_ATTACK(piece, delta) )
	//        {

	//        inc = DELTA_INC_ALL(delta);
	//        ASSERT(inc != IncNone);

	//        sq = from;

	//        do
	//            {
	//            sq += inc;

	//            if( sq == to )
	//                { // attack
	//                alist_add(alist, from, board);
	//                break;
	//                }
	//            } while ( board->square[sq] == Empty );
	//        }
	//    }

	//// pawn attacks

	//inc = PAWN_MOVE_INC(colour);
	//pawn = PAWN_MAKE(colour);

	//from = to - (inc - 1);

	//if( board->square[from] == pawn )
	//    alist_add(alist, from, board);

	//from = to - (inc + 1);

	//if( board->square[from] == pawn )
	//    alist_add(alist, from, board);
	int i;
	int sq;
	int start, end;
	int row, col;
	int rowbit, colbit;

	start = INDEX_START(colour);
	end   = INDEX_END(colour);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	for ( i = start; i <= end; ++i)
	{
		sq = board->piece[i];
		if (sq != SquareNone && sq != to)
		{

			switch(INDEX_TO_PIECE(i))
			{
			case RedRook:
			case BlackRook:
				{
					if (EQUAL_FILE(sq, to))
					{
						if(FileCapMin[row][colbit] + col == sq || FileCapMax[row][colbit] + col == sq)
						{
							alist_add(alist, sq, board);
						}
					}
					else if (EQUAL_RANK(sq, to))
					{
						if(RankCapMin[col][rowbit] + (row << 4) == sq || RankCapMax[col][rowbit] + (row << 4) == sq)
						{
							alist_add(alist, sq, board);
						}
					}
					break;
				}
			case RedCannon:
			case BlackCannon:
				{
					if(EQUAL_FILE(sq, to))
					{
						if(CannonFileCapMin[row][colbit] + col == sq || CannonFileCapMax[row][colbit] + col == sq)
						{
							alist_add(alist, sq, board);
						}
					}
					else if(EQUAL_RANK(sq, to))
					{
						if(CannonRankCapMin[col][rowbit] + (row << 4) == sq || CannonRankCapMax[col][rowbit] + (row << 4) == sq)
						{
							alist_add(alist, sq, board);
						}
					}
					break;
				}
			case RedKnight:
			case BlackKnight:
				{
					//马腿上没有子，这里有两层含义：1 马腿没子 2 马脚在to
					if(board->square[sq + NLegalDt[to - sq + 256]] == SquareNone)
					{
						alist_add(alist, sq, board);
					}

					break;
				}
			case RedAdvisor:
			case BlackAdvisor:
				{
					if(ADVISOR_IN_CITY(to) && ALegalDt[to - sq + 256])
					{
						alist_add(alist, sq, board);
					}
					break;
				}
			case RedBishop:
			case BlackBishop:
				{
					if(BISHOP_IN_CITY(to) && BLegalDt[to - sq + 256] && board->square[(to + sq) / 2] == 0)
					{
						alist_add(alist, sq, board);
					}
					break;
				}
			case RedPawn:
				{
					if(to+16 == sq) alist_add(alist, sq, board);
					if(SQUARE_COLOUR(sq) == Black)
					  if (to-1 == sq || to+1 == sq) alist_add(alist, sq, board);
					break;
				} 
			case BlackPawn:
				{
					if(to - 16 == sq) alist_add(alist, sq, board);
					if(SQUARE_COLOUR(sq) == Red)
					  if(to - 1 == sq || to + 1 == sq) alist_add(alist, sq, board);
					break;
				}
			case RedKing:
				{
					if(IN_CITY(to) && KLegalDt[to - board->piece[16] + 256]) alist_add(alist, sq, board);
					break;
				}
			case BlackKing:
				{
					if(IN_CITY(to) && KLegalDt[to - board->piece[32] + 256]) alist_add(alist, sq, board);
					break;
				}
			default:
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
	}
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


	if (EQUAL_RANK(from, to))
	{
		inc = (to > from) ? (1) : (-1);
	}
	else if (EQUAL_FILE(from, to))
	{
		inc = (to > from) ? (16) : (-16);
	}
	else
	{
		inc = 0;
	}

	if(inc != 0)
	{
		piece = INDEX_TO_PIECE(board->square[from]);
		switch(piece)
		{
		case RedPawn:
		case BlackPawn://只能查看兵后面是否有车

			sq = from - inc;
			while (SQUARE_TO_90(sq) >= 0 && board->square[sq] == SquareNone)
			{
				sq -= inc;//无子，继续查找
			}

			if(SQUARE_TO_90(sq) >= 0)
			{
				int index1 = board->square[sq];
				if (( INDEX_TO_PIECE(index1) == RedRook || INDEX_TO_PIECE(index1) == BlackRook))
				{
					alist_add(alists->alist[INDEX_TO_COLOUR(index1)], sq, board);
					
				}

				sq = sq - inc;
				while (SQUARE_TO_90(sq) >= 0 && board->square[sq] == SquareNone)
				{
					sq -= inc;//无子，继续查找
				}

				if(SQUARE_TO_90(sq) >= 0)//车后面有炮
				{
					int index1 = board->square[sq];
					if (( INDEX_TO_PIECE(index1) == RedCannon || INDEX_TO_PIECE(index1) == BlackCannon))
					{
						alist_add(alists->alist[INDEX_TO_COLOUR(index1)], sq, board);
						break;
					}
				}
			}
			break;
		case RedRook:
		case BlackRook:
			sq = from - inc;
			while (SQUARE_TO_90(sq) >= 0 && board->square[sq] == SquareNone)
			{
				sq -= inc;//无子，继续查找
			}

			if(SQUARE_TO_90(sq) >= 0)
			{
				int index0 = board->square[sq];
				if(INDEX_TO_PIECE(index0) == RedRook || INDEX_TO_PIECE(index0) == BlackRook)
				{
					alist_add(alists->alist[INDEX_TO_COLOUR(index0)], sq, board);

					//square = sq -inc;
					//while(SQUARE_TO_90(square) >= 0 &&  board->square[square] == SquareNone)
					//{
					//	square -= inc;
					//}

					//if(SQUARE_TO_90(square) >= 0)//车后面还有炮
					//{
					//	int index1 = board->square[square];
					//	if (( INDEX_TO_PIECE(index1) == RedCannon || INDEX_TO_PIECE(index1) == BlackCannon))
					//	{
					//		alist_add(alists->alist[INDEX_TO_COLOUR(index1)], square, board);

					//	}
					//}
					//break;
				}

				square = sq -inc;//上面子后面有炮
				while(SQUARE_TO_90(square) >= 0 &&  board->square[square] == SquareNone)
				{
					square -= inc;
				}

				if (SQUARE_TO_90(square) >= 0)
				{
					int index1 = board->square[square];
					if (( INDEX_TO_PIECE(index1) == RedCannon || INDEX_TO_PIECE(index1) == BlackCannon))
					{
						alist_add(alists->alist[INDEX_TO_COLOUR(index1)], square, board);
						break;
					}
				}

			}			

			break;
		case RedCannon:
		case BlackCannon://炮权重小于车，如果炮是隐藏攻击子，则只有重炮这种情况，因炮优先于车处理
			sq = from - inc;
			while (SQUARE_TO_90(sq) >= 0 && board->square[sq] == SquareNone)
			{
				sq -= inc;//无子，继续查找
			}

			if (SQUARE_TO_90(sq) >= 0)
			{
				int index1 = board->square[sq];
				if (( INDEX_TO_PIECE(index1) == RedCannon || INDEX_TO_PIECE(index1) == BlackCannon))
				{
					alist_add(alists->alist[INDEX_TO_COLOUR(index1)], sq, board);
					break;
				}
			}			

			break;
		}
	}
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
