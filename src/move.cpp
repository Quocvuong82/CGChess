// move.cpp

// includes

#include <cctype>
#include <cstring>

#include "colour.h"
#include "list.h"
#include "move.h"
#include "piece.h"
#include "square.h"


// functions

// move_is_ok()

bool move_is_ok( int move )
{
	int from;
	int to;
	
	if( move < 0 || move >= 65536 )
	{
		ASSERT(FALSE);
		return false;
	}

	if( move == MoveNone )
	{
		
		return false;
	}

	if( move == MoveNull )
	{
		ASSERT(FALSE);
		return false;
	}

	from = MOVE_FROM(move);
	to   = MOVE_TO(move);

	if(!SQUARE_IS_OK(from))
	{
		ASSERT(FALSE);
		return false;
	}
	if(!SQUARE_IS_OK(to))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}


// move_order()

int move_order( int move )
{

	ASSERT(move_is_ok(move));

	return 0;
}

// move_is_capture()

bool move_is_capture( int move, board_t *board )
{

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	return board->square[MOVE_TO(move)] != Empty;
}


// move_is_tactical()

bool move_is_tactical( int move, board_t *board )
{

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	return board->square[MOVE_TO(move)] != Empty;
}

// move_capture()

int move_capture( int move, board_t *board )
{

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);


	return board->square[MOVE_TO(move)];
}

// move_to_string()

bool move_to_string( int move, char string [], int size )
{
	int from, to;
	int file, rank;
	ASSERT(move == MoveNull || move_is_ok(move));
	ASSERT(string != NULL);

	if(move == MoveNone)
	{
		strcpy(string,NullMoveString);
		return true;
	}

	from = MOVE_FROM(move);
	to   = MOVE_TO(move);

	file = SQUARE_FILE(from);
	rank = SQUARE_RANK(from);

	string[0] = 'a' + file - FileA;
	string[1] = '0' + 9 - (rank - Rank0);

	file = SQUARE_FILE(to);
	rank = SQUARE_RANK(to);

	string[2] = 'a' + file - FileA;
	string[3] = '0' + 9 - (rank - Rank0);

	string[4] = '\0';

	return true;
}

// move_from_string()

int move_from_string( const char string [], board_t *board )
{

	char tmp_string[3];
	int from, to;
	int move;
	int file, rank;

	ASSERT(string != NULL);
	ASSERT(board != NULL);
	ASSERT(strlen(string) >= 4);

	// from
    tmp_string[0] = (string[0]);
    tmp_string[1] = (string[1]);
    tmp_string[2] = '\0';

	file   =   tmp_string[0] - 'a' + FileA;
    rank   =   -tmp_string[1] + '0' + Rank0 + 9;

    from = SQUARE_MAKE(file, rank);
    if( from == SquareNone )
        return MoveNone;

    // to
    tmp_string[0] = (string[2]);
    tmp_string[1] = (string[3]);
    tmp_string[2] = '\0';

	file = tmp_string[0] - 'a'  + FileA;
    rank = -tmp_string[1] + '0' + Rank0 + 9;

    to = SQUARE_MAKE(file, rank);
    if( to == SquareNone )
        return MoveNone;

    move = MOVE_MAKE(from, to);

	return move;
}

// end of move.cpp
