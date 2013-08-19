// value.cpp

// includes

#include "piece.h"
#include "search.h"
#include "types.h"
#include "value.h"
#include "board.h"

// variables

int ValueIndex[Index32Nb];//33

// functions

// value_init()

void value_init()
{
	int index;

	// ValuePiece[]

	for ( index = 0; index < Index32Nb; index++ )
		ValueIndex[index] = 0;
    
	for ( index = 0; index < Index32Nb; index++)
	{
		switch(INDEX_TO_PIECE16(index))
		{
		case PieceNone:
			ValueIndex[index] = 0; break;
		case RedRook16:
		case BlackRook16:
			ValueIndex[index] = ValueRook; break;
		case RedKnight16:
		case BlackKnight16:
			ValueIndex[index] = ValueKnight; break;
		case RedCannon16:
		case BlackCannon16:
			ValueIndex[index] = ValueCannon; break;
		case RedBishop16:
		case BlackBishop16:
            ValueIndex[index] = ValueBishop; break;
		case RedAdvisor16:
		case BlackAdvisor16:
			ValueIndex[index] = ValuekAdvisor; break;
		case RedKing16:
		case BlackKing16:
            ValueIndex[index] = ValueKing; break;
		case RedPawn16:
		case BlackPawn16:
            ValueIndex[index] = ValuePawn; break;
		default:
			ASSERT(FALSE); break;
		}
	}

}

// value_is_ok()

bool value_is_ok( int value )
{
	if( value < -ValueInf || value > +ValueInf )
		return false;

	return true;
}

// range_is_ok()

bool range_is_ok( int min, int max )
{
	if( !value_is_ok(min) )
		return false;

	if( !value_is_ok(max) )
		return false;

	if( min >= max )
		return false; // alpha-beta-like ranges cannot be null

	return true;
}

// value_is_mate()

bool value_is_mate( int value )
{

	ASSERT(value_is_ok(value));

	if( value < -ValueEvalInf || value > +ValueEvalInf )
		return true;

	return false;
}

// value_to_trans()

int value_to_trans( int value, int height )
{

	ASSERT(value_is_ok(value));
	ASSERT( 0 <= height && height < 256);

	if( value < -ValueWin )
	{
		if( value < -ValueEvalInf )
			value -= height;

		else if( value > -ValueEvalInf )
			value -= ValuePly *height;
	}
	else if( value > +ValueWin )
	{
		if( value > ValueEvalInf )
			value += height;

		else if( value < +ValueWin )
			value += ValuePly *height;
	}

	ASSERT(value_is_ok(value));

	return value;
}

// value_from_trans()

int value_from_trans( int value, int height )
{

	ASSERT(value_is_ok(value));
	ASSERT( 0 <= height && height < 256);

	if( value < -ValueWin )
	{
		if( value < -ValueEvalInf )
			value += height;

		else if( value > -ValueEvalInf )
			value += ValuePly *height;
	}
	else if( value > +ValueWin )
	{
		if( value > ValueEvalInf )
			value -= height;

		else if( value < ValueEvalInf )
			value -= ValuePly *height;
	}

	ASSERT(value_is_ok(value));

	return value;
}

// value_to_mate()

int value_to_mate( int value )
{

	int dist;

	ASSERT(value_is_ok(value));

	if( value < -ValueEvalInf )
	{

		dist = (ValueMate + value) / 2;
		ASSERT(dist > 0);

		return -dist;
	}
	else if( value > +ValueEvalInf )
	{

		dist = (ValueMate - value + 1) / 2;
		ASSERT(dist > 0);

		return +dist;
	}

	return 0;
}

// end of value.cpp
