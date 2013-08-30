#ifndef MOVE_H
#define MOVE_H

// includes

#include "board.h"
#include "types.h"

const int MoveNone = 0;
const int MoveNull = 11;
const char NullMoveString [] = "null";

#define MOVE_MAKE(from,to)             (((from)<<8)|(to))
#define MOVE_FROM(move)                ((((move)>>8)&0xff))
#define MOVE_TO(move)                  (((move)&0xff))

#define MOVE_PIECE(move,board)         ((board)->square[MOVE_FROM(move)])

typedef uint16 mv_t;

extern bool move_is_ok( int move );
extern int move_order( int move );
extern bool move_is_capture( int move, board_t *board );
extern bool move_is_tactical( int move, board_t *board );
extern int move_capture( int move, board_t *board );
extern bool move_to_string( int move, char string [], int size );
extern int move_from_string( const char string [], board_t *board );

#endif