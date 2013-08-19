// fen.cpp

// includes

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include "board.h"
#include "colour.h"
#include "fen.h"
#include "piece.h"
#include "square.h"
#include "util.h"
#include "movetable.h"

#pragma warning( disable : 4800 )
// Level 2 'int' : forcing value to bool 'true' or 'false' (performance warning) ->

// "constants"

const char *const StartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";

// variables

static const bool Strict = false;

// functions

int  char_to_piece(char c);

// board_from_fen()

void board_from_fen( board_t *board, const char old_fen [] )
{

	int pos;
	int sq_90, sq_256;
	int c;
	int i;
	int piece;
	int row, col;
	char fen[256];

	ASSERT(board != NULL);
	ASSERT(fen != NULL);


	board_clear(board);
	strcpy(fen, old_fen);

	//convert_fen(fen, true);

	sq_90 = 0;
	pos   = 0;
	c     = fen[pos];

	// piece placement
	for (i = 0;sq_90 < 90 && c != '\0'; ++i)
	{
		if (c >= '0' && c <= '9')
		{
			sq_90 += c - '0';
		}
		else if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			piece = char_to_piece(c);
			if (piece != PieceNone)
			{
				sq_256 = SQUARE_FROM_90(sq_90);
				board->piece[PIECE16_TO_INDEX(piece) + board->number[piece]] = sq_256;
				board->square[sq_256] = PIECE16_TO_INDEX(piece) + board->number[piece];

				col = SQUARE_FILE(sq_256) - 3;
				row = SQUARE_RANK(sq_256) - 3;
				board->file[col] ^= 1<<row;
				board->rank[row] ^= 1<<col;

				board->number[piece]++;
				board->piece_size[PIECE16_TO_COLOUR(piece)]++;

			}
			sq_90++;
		}

		c = fen[++pos];
	}

	if( c != ' ' )
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
	c = fen[++pos]; 

	// active colour
	switch( c )
	{
	case 'w':
	case 'W':
	case 'r':
	case 'R':
		board->turn = Red;
		break;

	case 'b':
	case 'B':
		board->turn = Black;
		break;

	default:
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
		break;
	}

	c = fen[++pos];

	// castling

	if( c != ' ' )
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
	c = fen[++pos];

	if( c == '-' )
	{ // no castling rights

		c = fen[++pos];
	}
	else
	{
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
	}

	//frc_init();

	// en-passant

	if( c != ' ' )
		my_fatal("board_from_fen(): bad FEN (pos=%d),\n", pos);
	c = fen[++pos];

	if( c == '-' )
	{ // no en-passant

	}
	else
	{
		my_fatal("board_from_fen(): bad FEN (pos=%d),\n", pos);
	}


	// halfmove clock

	board->ply_nb = 0;

	c = fen[++pos];
	if( c != ' ' )
	{
		if( !Strict )
			goto update;
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
	}
	c = fen[++pos];

	if( !isdigit(c) )
	{
		if( !Strict )
			goto update;
		my_fatal("board_from_fen(): bad FEN (pos=%d)\n", pos);
	}

	board->ply_nb = atoi(&fen[pos]);

	// board update

update:
	board_init_list(board);
}

// board_to_fen()

bool board_to_fen( board_t *board, char fen [], int size )
{
	int pos;
	int i;
	int n;
	int index;

	ASSERT(board != NULL);
	ASSERT(fen != NULL);
	ASSERT(size >= 92);

	// init

	if( size < 92 )
		return false;

	pos   = 0;
	n     = 0;
	index = 0;

	// piece placement
	for(i = 0; i < 90; i++)
	{
		if(i != 0 && i % 9 == 0)
		{
			if(n != 0)
			{
				fen[index++]=n + '0';
				n=0;
			}

			fen[index++]='/';
		}

		if(board->square[SQUARE_FROM_90(i)] != PieceNone)
		{
			if(n != 0)
			{
				fen[index++] = n + '0';
				n=0;
			}

			fen[index++] = Index32ToChar[board->square[SQUARE_FROM_90(i)]];
		}
		else
			++n;
	}

	if(i == 90 && n != 0) fen[index++]=n + '0';

	fen[index++] = ' ';

	// active colour

	fen[index++] = COLOUR_IS_RED(board->turn) ? 'w' : 'b';
	fen[index++] = ' ';

	// castling
	fen[index++] = '-';
	fen[index++] = ' ';

	// en-passant
	fen[index++] = '-';
	fen[index++] = ' ';

	// halfmove clock
	sprintf(&fen[index], "%d 1", board->ply_nb);

	return true;
}

// convert_fen()

void convert_fen( char fen [], bool to_shredder )
{ // no-op if it's already in the specified format

}

int  char_to_piece(char c)
{
	switch(c)
	{
	case 'K':
		return RedKing16;

	case 'A':
		return RedAdvisor16;

	case 'B':
	case 'E':
		return RedBishop16;

	case 'N':
	case 'H':
		return RedKnight16;

	case 'R':
		return RedRook16;

	case 'C':
		return RedCannon16;

	case 'P':
		return RedPawn16;

	case 'k':
		return BlackKing16;

	case 'a':
		return BlackAdvisor16;

	case 'b':
	case 'e':
		return BlackBishop16;

	case 'n':
	case 'h':
		return BlackKnight16;

	case 'r':
		return BlackRook16;

	case 'c':
		return BlackCannon16;

	case 'p':
		return BlackPawn16;
	}

	return 0;
}

// end of fen.cpp
