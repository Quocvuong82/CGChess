// move_do.h

#ifndef MOVE_DO_H
#define MOVE_DO_H

// includes

#include "board.h"
#include "types.h"

// types

struct undo_t
{
	bool capture;

	int capture_square;
	int capture_piece;

	int turn;

	int ply_nb;

	int cap_sq;
	int moving_piece;

	int opening;
	int endgame;

	uint64 key;
	uint64 material_key;
};


// functions

extern void move_do( board_t *board, int move, undo_t *undo );
extern void move_undo( board_t *board, int move, const undo_t *undo );
extern void move_do_null( board_t *board, undo_t *undo );
extern void move_undo_null( board_t *board, const undo_t *undo );

extern void move_pre_do(board_t *board, int from, int to ,undo_t *undo);
extern void move_pre_undo(board_t *board, int from, int to ,undo_t *undo);

#endif // !defined MOVE_DO_H

// end of move_do.h
