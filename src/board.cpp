// board.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "fen.h"
#include "hash.h"
#include "list.h"
#include "move.h"
#include "move_do.h"
#include "move_evasion.h"
#include "move_gen.h"
#include "move_legal.h"
#include "piece.h"
#include "pst.h"
#include "util.h"
#include "value.h"

// constants

static const bool UseSlowDebug = true;

// functions

// board_is_ok()

bool board_is_ok( board_t *board )
{

	int sq, piece;
	int size;
	int i;
	int rank[10]; // 10––
	int file[9];  // 9¡–
	int row, col;
	int rking_sq, bking_sq;

	if( board == NULL )
		return false;

	// optional heavy DEBUG mode

	if( !UseSlowDebug )
		return true;


	for (i = 0; i < 10; ++i)
    {
		rank[i] = 0;
    }

	for (i = 0; i < 9; ++i)
	{
		file[i] = 0;

	}
	// squares

	for ( sq = 0; sq < SquareNb; sq++ )
	{

		piece = board->square[sq];
		if( SQUARE_IS_OK(sq) )
		{
			// inside square

			if( piece != PieceNone )
			{
				if( !INDEX_IS_OK(piece) )
					return false;

				if (board->piece[piece] != sq)
                    return false;

				switch(INDEX_TO_PIECE(piece))
				{
				case RedKing:
				case BlackKing:		
					 if(!IN_CITY(sq))
						return false;
					 break;
				case RedAdvisor:
				case BlackAdvisor:
					 if(!ADVISOR_IN_CITY(sq))
						 return false;
					 break;
				case RedBishop:
				case BlackBishop:
                     if(!BISHOP_IN_CITY(sq))
						  return false;
					 break;
				}

				col = SQUARE_FILE(sq) - 3;
				row = SQUARE_RANK(sq) - 3;
				file[col] ^= 1<<row;
				rank[row] ^= 1<<col;
			}
			else
			{
				if(board->piece[piece] != SquareNone)
					return false;
			}
		}
		else
		{
			// edge square
			if( piece != -1 )
				return false;
		}
	}

    for (i = 0; i < 10; ++i)
    {
		if(rank[i] != board->rank[i])
			return false;
    }

	for (i = 0; i < 9; ++i)
	{
		if (file[i] != board->file[i])
           return false;
	}

	// material

	if( board->piece_nb != board->piece_size[Red] + board->piece_size[Black])
	{
		return false;
	}

	size = 0;
	for (i = 0; i < 16; ++i)
	{
		size +=  board->number[i];
	}

	if (size != board->piece_nb)
	   return false;

	if( board->number[RedKing] != 1 )
		return false;

	if( board->number[BlackKing] != 1 )
		return false;

	// misc

	if( !COLOUR_IS_OK(board->turn) )
		return false;

	if( board->ply_nb < 0 )
		return false;

	if( board->sp < board->ply_nb )
		return false;

	if( board->cap_sq != SquareNone && !SQUARE_IS_OK(board->cap_sq) )
		return false;

	if( board->opening != board_opening(board) )
		return false;

	if( board->endgame != board_endgame(board) )
		return false;

	if( board->key != hash_key(board) )
		return false;

	if( board->material_key != hash_material_key(board) )
		return false;

	rking_sq = board->piece[16];
	bking_sq = board->piece[32];

	if (!SQUARE_IS_OK(rking_sq))
        return false;

	if(!SQUARE_IS_OK(bking_sq))
		return false;

	if (EQUAL_FILE(rking_sq,bking_sq))
	{
		for (sq = bking_sq + 16; sq < rking_sq && board->square[sq] == PieceNone; sq += 16)
          ;
		if(sq == rking_sq)
			return false;//∂‘¡≥
	}

	return true;
}

// board_clear()

void board_clear( board_t *board )
{

	int sq, i;

	ASSERT(board != NULL);

	// edge squares

	for ( sq = 0; sq < SquareNb; sq++ )
	{
		if(SQUARE_TO_90(sq) < 0)
		  board->square[sq] = -1;
		else
		  board->square[sq] = PieceNone;
	}
	// misc

	board->piece_material[Red] = 0;
	board->piece_material[Black] = 0;

	for (i = 0; i < 33; ++i)
	{
		board->piece[i] = SquareNone;
	}

	board->piece_size[Red] = 0;
	board->piece_size[Black] = 0;

	board->piece_nb = 0;

	for (i = 0; i < 16; ++i)
	{
		board->number[i] = 0;
	}

	for (i = 0; i < 10; ++i)
	{
		board->rank[i] = 0;
	}

	for (i = 0; i < 9; ++i)
	{
		board->file[i] = 0;
	}

	board->turn = ColourNone;

	board->ply_nb = 0;
	board->sp;

	board->cap_sq = SquareNone;
	board->moving_piece = PieceNone;

	board->opening = 0;
	board->endgame = 0;

	board->key = 0;
	board->material_key = 0;
}

// board_copy()

void board_copy( board_t *dst, board_t *src )
{

	ASSERT(dst != NULL);
	ASSERT(board_is_ok(src));

	*dst = *src;
}

// board_init_list()

void board_init_list( board_t *board )
{

	int sq_90, sq, piece;
	int i;
	

	ASSERT(board != NULL);

	// init

	// piece lists
	for (sq_90 = 0; sq_90 < 90; ++sq_90)
	{
		sq = SQUARE_FROM_90(sq_90);
		piece = board->square[sq];
		if (piece != PieceNone)
		{
			if(INDEX_TO_PIECE(piece) != 16 && INDEX_TO_PIECE(piece) != 32)
			  board->piece_material[INDEX_TO_COLOUR(piece)] += VALUE_INDEX(piece);
			board->piece_nb++;	
		}
	}

	// PST

	board->opening = board_opening(board);
	board->endgame = board_endgame(board);

	// hash key

	for ( i = 0; i < board->ply_nb; i++ )
		board->stack[i] = 0;

	board->sp = board->ply_nb;

	board->key = hash_key(board);
	board->material_key = hash_material_key(board);

	// legality

	if( !board_is_legal(board) )
		my_fatal("board_init_list(): illegal position\n");

	// debug

	ASSERT(board_is_ok(board));
}

// board_is_legal()

bool board_is_legal( board_t *board )
{

	ASSERT(board != NULL);
    ASSERT(!IS_IN_CHECK(board, COLOUR_OPP(board->turn)));
	return !IS_IN_CHECK(board, COLOUR_OPP(board->turn));
}

// board_is_check()

bool board_is_check( board_t *board )
{

	ASSERT(board != NULL);

	return IS_IN_CHECK(board, board->turn);
}

// board_is_mate()

bool board_is_mate( board_t *board )
{
    attack_t attack[1];

    ASSERT(board != NULL);

    attack_set(attack, board);

    if( !ATTACK_IN_CHECK(attack) )
        return false; // not in check => not mate

    if( legal_evasion_exist(board, attack) )
        return false; // legal move => not mate

	return true;      // in check and no legal move => mate
}

// board_is_stalemate()

bool board_is_stalemate( board_t *board )
{

	list_t list[1];
	int i, move;

	ASSERT(board != NULL);

	// init

	if( IS_IN_CHECK(board, board->turn) )
		return false; // in check => not stalemate

	// move loop

	gen_moves(list, board);

	for ( i = 0; i < LIST_SIZE(list); i++ )
	{
		move = LIST_MOVE(list, i);

		if( pseudo_is_legal(move, board) )
			return false; // legal move => not stalemate
	}

	return true;          // in check and no legal move => mate
}

// board_is_repetition()

bool board_is_repetition( board_t *board , int height, int& value)
{

	int i;

	ASSERT(board != NULL);

	value = ValueDraw;

	// 50-move rule

	if( board->ply_nb >= 100 )
	{ // potential draw
		if( board->ply_nb > 100 )
			return true;

		ASSERT(board->ply_nb == 100);
		return !board_is_mate(board);
	}

	// position repetition

	ASSERT(board->sp >= board->ply_nb);

	for ( i = 4; i <= board->ply_nb; i += 2 )
	{
		if( board->stack[board->sp - i] == board->key )
		{
			if(i&0x01)
			{
				value = ValueRepeatValue - height;
			}
			else
			{
				value = -ValueRepeatValue + height;
			}
			return true;
		}
	}

	return false;
}



// board_opening()

int board_opening( board_t *board )
{
	int opening;
	int sq, piece;
	int i;

	ASSERT(board != NULL);

	opening = 0;


	for ( i = 0 ; i < 33; ++i)
	{
		sq = board->piece[i];
		if (sq != SquareNone)
		{
			piece = INDEX_TO_PIECE(i);
            opening += PST(piece, SQUARE_TO_90(sq), Opening);
		}
	}


	return opening;
}

// board_endgame()

int board_endgame( board_t *board )
{

	int endgame;
	int sq, piece;
	int i;

	ASSERT(board != NULL);

	endgame = 0;

	for ( i = 0; i < 33; ++i)
	{
		sq = board->piece[i];
		if (sq != SquareNone)
		{
			piece = INDEX_TO_PIECE(i);
            endgame += PST(piece, SQUARE_TO_90(sq), Endgame);
		}
	}

	return endgame;
}

// frc_init()

void frc_init()
{
}

// end of board.cpp
