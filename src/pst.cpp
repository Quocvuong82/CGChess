// pst.cpp

// includes

#include "piece.h"
#include "pst.h"
#include "types.h"

// macros

#define P(piece,square_90,stage) (Pst[(piece)][(square_90)][(stage)])

// variables

int Pst[16][90][StageNb];


// prototypes

static int square_make( int file, int rank );
static int square_file( int square );
static int square_rank( int square );
static int square_opp( int square );

static void init_pst();

// pst_init()

void pst_init()
{
	init_pst();
}

// square_make()

static int square_make( int file, int rank )
{

	ASSERT(file >= 0 && file < 8);
	ASSERT(rank >= 0 && rank < 8);

	return rank * 9 + file;
}

// square_file()

static int square_file( int square )
{

	ASSERT(square >= 0 && square < 90);

	return square % 9;
}

// square_rank()

static int square_rank( int square )
{

	ASSERT(square >= 0 && square < 90);

	return square / 9;
}

// square_opp()

static int square_opp( int square )
{

	ASSERT(square >= 0 && square < 90);

	return (9 - (square/9))*9 + square%9;
}

const int RookPostion[90] = 
{
	1,	 1,  1,  3,  4,  3,  1,  1,  1,
	1,	 2,  1,  3,  5,  3,  1,  2,  1,
	1,	 1,  1,  2,  4,  2,  1,  1,  1, 
	1,	 3,  3,  3,  3,  3,  3,  3,  1,
	1,	 2,  1,  2,  3,  2,  1,  2,  1,
	1,	 3,  3,  3,  3,  3,  3,  3,  1,
	0,	 2,  2,  2,  3,  2,  2,  2,  0,
	0,	 1,  1,  2,  2,  2,  1,  1,  0,
	0,	 1,  1,  2, -4,  2,  1,  1,  0,
	0,	 1,  1,  2, -4,  2,  1,  1,  0,
};

const int KnightPostion[90] = 
{
	1,	 1,  1,  2, -4,  2,  1,  1,  1,
	1,	 2,  5,  2, -2,  2,  5,  2,  1,
	2,	 2,  3,  4,  4,  4,  3,  2,  2, 
	1,	 4,  3,  4,  3,  4,  3,  4,  1,
	0,	 1,  0,  2,  3,  2,  0,  1,  0,
	0,	 1,  3,  2,  3,  2,  3,  1,  0,
    0,	 0,  1,  1,  3,  1,  1,  0,  0,
    1,	 0,  2,  1,  0,  1,  2,  0,  1,
	0,	 0,  0,  0, -4,  0,  0,  0,  0,
	0,	 0,  0,  0, -4,  0,  0,  0,  0,
};

const int CannonPostion[90] =
{
	3,	 2,  0, -3, -5, -3,  0,  2,  3,
	2,	 0, -2, -2, -4, -2, -2,  2,  2,
	2,	 2,  2,  1,  2,  1,  2,  2,  2, 
	2,	 2,  2,  1,  4,  1,  2,  2,  2,
	1,	 1, -1,  0,  4,  0, -1,  1,  1,
	0,	 2,  2,  0,  4,  0,  2,  2,  0,
	0,	 1,  2,  0,  3,  0,  2,  1,  0,
	3,	 3,  2,  3,  4,  3,  2,  3,  3,
	2,	 2,  2,  2,  4,  2,  2,  2,  2,
	2,	 2,  2,  3,  3,  3,  2,  2,  2,
};

const int PawnPostion[90] = 
{
	0,	 0,  2,  2,  0,  2,  2,  0,  0,
	0,	 3,  6,  7,  8,  7,  6,  3,  0,
	1,	 3,  6,  6,  6,  6,  6,  3,  1, 
	2,	 3,  5,  5,  5,  5,  5,  3,  2,
	0,	 2,  3,  4,  5,  4,  3,  2,  0,

    -1,	 0,  1,  0,  2,  0,  1,  0, -1,
	-2,	 0, -3,  0,  2,  0, -3,  0, -2,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
};

const int BishopPostion[90] = 
{
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
    0,	 0,  0,  0,  2,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  1,  0,  0,  0,  1,  0,  0,
};

const int AdvisorPostion[90] = 
{
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  2,  0,  0,  0,  0,
	0,	 0,  0,  1,  0,  1,  0,  0,  0,
};

const int KingPostion[90] = 
{
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0, -4, -4, -4,  0,  0,  0,
	0,	 0,  0,  1,  1,  1,  0,  0,  0,
	0,	 0,  0,  3,  4,  3,  0,  0,  0,
};

static void init_pst()
{
	int piece, sq, stage;

	// init
	for ( piece = 0; piece < 16; piece++ )
	{
		for ( sq = 0; sq < 90; sq++ )
		{
			for ( stage = 0; stage < StageNb; stage++ )
			{
				P(piece, sq, stage) = 0;
			}
		}
	}

	//Rook
	piece = RedRook16;
	for ( sq = 0; sq < 90; sq++ )
	{
	    P(piece, sq, Opening) = (RookPostion[sq]) * 10;
		P(piece, sq, Endgame) = (RookPostion[sq]);
	}

	// Knight
	piece = RedKnight16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (KnightPostion[sq]) * 10;
		P(piece, sq, Endgame) = (KnightPostion[sq]);
	}

	//// Cannon
	piece = RedCannon16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (CannonPostion[sq]) * 10;
		P(piece, sq, Endgame) = (CannonPostion[sq]);
	}

	//Pawn
	piece = RedPawn16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (PawnPostion[sq]) * 6;
		P(piece, sq, Endgame) = (PawnPostion[sq]);
	}

		// Bishop
	piece = RedBishop16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (BishopPostion[sq]) * 1;
		P(piece, sq, Endgame) = (BishopPostion[sq]);
	}

	// Advisor
	piece = RedAdvisor16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (AdvisorPostion[sq]) * 1;
		P(piece, sq, Endgame) = (AdvisorPostion[sq]);
	}

	// King
	piece = RedKing16;
	for ( sq = 0; sq < 90; sq++ )
	{
		P(piece, sq, Opening) = (KingPostion[sq]) * 20;
		P(piece, sq, Endgame) = (KingPostion[sq]);
	}


	// Black
	for (piece = 9; piece <= 15; piece++)
	{
		for ( sq = 0; sq < 90; sq++ )
		{

			P(piece, sq, Opening) = -P(piece-8, square_opp(sq), Opening);
			P(piece, sq, Endgame) = -P(piece-8, square_opp(sq), Endgame);

		}
		
	}
}


// end of pst.cpp
