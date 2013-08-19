#ifndef ATTACK_H
#define ATTACK_H

#include "board.h"

//#define IS_IN_CHECK(board,colour)         (is_attacked((board),KING_POS((board),(colour)),COLOUR_OPP((colour))))
#define IS_IN_CHECK(board,colour)         (in_check(board,(colour)))
#define PIECE_ATTACK(board,piece,from,to) (PSEUDO_ATTACK((piece),(to)-(from))&&line_is_empty((board),(from),(to)))

#define ATTACK_IN_CHECK(attack)           ((attack)->dn!=0)

// types
const int IncNone = 0;

struct attack_t
{
	int dn;
	int ds[11+1];
	int di[11+1];
};

extern bool is_attacked( board_t *board, int src, int to, int colour );
extern bool is_pinned( board_t *board, int move, int colour );
extern bool attack_is_ok( const attack_t *attack );
extern void attack_set( attack_t *attack, const board_t *board );
extern bool piece_attack_king( const board_t *board, int piece, int from, int king );
extern bool in_check(board_t *board,  int colour);


#endif