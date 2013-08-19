#include "attack.h"
#include "board.h"
#include "colour.h"
#include "eval.h"
#include "init.h"
#include "material.h"
#include "move.h"
#include "option.h"
#include "piece.h"
#include "see.h"
#include "util.h"
#include "value.h"
#include "pawn.h"
#include "square.h"


const int KnightOutpost[2][90] =
	{
	 0,	 0,  0,  0,  0,  0,  0,  0,  0,
     0,	10, 50,  0,  0,  0, 50, 10,  0,
	10,	 0, 40, 40, 40, 40, 40,  0, 10, 
     0,	40, 30, 40, 30, 40, 30, 40,  0,
     0,	20, 20, 30, 35, 30, 20, 20,  0,
     0,	20, 30, 30, 35, 30, 30, 20,  0,
     0,	 5, 10, 15, 30, 15, 10,  5,  0,
     15, 0, 25, 15,  0, 15, 25,  0, 15, 
     0,	 0,  0,  0,  0,  0,  0,  0,  0, 
     0,	 0,  0,  0,  0,  0,  0,  0,  0, 

	 0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	 0,	 0,  0,  0,  0,  0,  0,  0,  0, 
     15, 0, 25, 15,  0, 15, 25,  0, 15,
     0,	 5, 10, 15, 30, 15, 10,  5,  0,
     0,	20, 30, 30, 35, 30, 30, 20,  0,
     0,	20, 20, 30, 35, 30, 20, 20,  0, 
     0,	40, 30, 40, 30, 40, 30, 40,  0,
    10,	 0, 40, 40, 40, 40, 40,  0, 10, 
     0,	10, 40,  0,  0,  0, 40, 10,  0, 
     0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	};

const int PawnOutpost[2][90] =
{
	//------------------------------------------
	1,  2,  3,  5,  5,  5,  3,  2,  1,
	5,	10, 15, 20, 35, 20, 15, 10, 5,
	5,	10, 15, 20, 20, 20, 15, 10, 5,
	5,	10, 15, 15, 15, 15, 15, 10, 5,
	5,	5,  10, 10, 15, 10, 10,  5, 5,
	//-------------------------------//
	0,	 0, 10, 0,  10,  0, 10,  0, 0,
	0,	 0,-10,  0,  5,  0,-10,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,

	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0,  0,  0,  0,  0,  0,  0, 0,
	0,	 0, -5,  0,  5,  0, -5,  0, 0,
	3,	 0, 10, 0,  10,  0, 10,  0, 3,
	//-------------------------------//
	5,	5,  10, 10, 15, 10, 10,  5, 5,
	5,	10, 15, 15, 15, 15, 15, 10, 5,
	5,	10, 15, 20, 20, 20, 15, 10, 5,
	5,	10, 15, 20, 35, 20, 15, 10, 5,
	1,  2,  3,  5,  5,  5,  3,  2,  1,
};

const int CannonOutPost[2][90] = 
{
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	10, 10,  0, 30,  0, 10, 10,  0,
	0,	 0, -5,  0, 40,  0, -5,  0,  0,
   -10,	 0,  0,  0,  0,  0,  0,  0, -10,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
   10,	10, 10, 10, 40, 10, 10,  10, 10,
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 

	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
	10,	10, 10, 10, 40, 10, 10,  10, 10,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
   -10,	 0,  0,  0,  0,  0,  0,  0, -10,
	0,	 0, -5,  0, 40,  0, -5,  0,  0,
	0,	10, 10,  0, 40,  0, 10, 10,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0,
	0,	 0,  0,  0,  0,  0,  0,  0,  0, 
};

// eval_init()

void eval_init()
{
}

// eval()

int eval( board_t *board, int alpha, int beta, int ThreadId )
{	
	int opening, endgame;
	material_info_t mat_info[1];
	int phase;
	int eval;
	int lazy_eval;

	// init	

	opening = 0;
	endgame = 0;

	// material

	material_get_info(mat_info, board, ThreadId);

	opening += mat_info->opening;
	endgame += mat_info->endgame;


	// pst

	opening += board->opening;
	endgame += board->endgame;

	if (UseTempo)
	{
		if( COLOUR_IS_RED(board->turn) )
		{
			opening += TempoOpening;
			endgame += TempoEndgame;
		}
		else
		{
			opening -= TempoOpening;
			endgame -= TempoEndgame;
		}
	}

	phase = mat_info->phase;
	eval = ((opening * (256 - phase)) + (endgame * phase)) / 256;
	if( COLOUR_IS_BLACK(board->turn) )
		eval = -eval;

	return eval;

}








