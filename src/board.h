#ifndef BOARD_H
#define BOARD_H

// includes
#include "types.h"
#include "colour.h"
#include "piece.h"
#include "square.h"

const int Empty = 0;
const int StackSize = 4096;

#define KING_POS(board,colour) ((board)->piece[ (colour) ? 32 : 16])

#define INDEX_START(colour)  ((colour) ? 17 : 1)
#define INDEX_END(colour)    ((colour) ? 32 : 16)

struct board_t
{  
	int piece_material[2];
	
	int square[SquareNb];//256 square

	int piece[IndexNb];       //0 1-16 17-32
	int piece_size[ColourNb];//

	int piece_nb;
	int number[16]; // only 14 are needed

	int  rank[10]; // rankbit
	int  file[9];  // filebit

	int  turn;

	int ply_nb;//play back
	int sp; // point to stack

	int cap_sq;
	int moving_piece;

	int opening;
	int endgame;

	uint64 key;
	uint64 material_key;

	uint64 stack[StackSize];
};

extern bool board_is_ok( board_t *board );
extern void board_clear( board_t *board );
extern void board_copy( board_t *dst, board_t *src );
extern void board_init_list( board_t *board );
extern bool board_is_legal( board_t *board );
extern bool board_is_check( board_t *board );
extern bool board_is_mate( board_t *board );
extern bool board_is_stalemate( board_t *board );
extern bool board_is_repetition( board_t *board, int height, int& value);
extern int board_material( board_t *board );
extern int board_opening( board_t *board );
extern int board_endgame( board_t *board );

#endif