// recog.cpp

// includes

#include "board.h"
#include "colour.h"
#include "material.h"
#include "piece.h"
#include "recog.h"
#include "util.h"


// prototypes



// functions

// recog_draw()

bool recog_draw( const board_t *board, int ThreadId )
{
	
	material_info_t mat_info[1];

    ASSERT(board != NULL);

    if( board->number[RedRook] + board->number[RedKnight] + board->number[RedCannon] > 1 )
        return false;
	if(board->number[BlackRook] + board->number[BlackKnight] + board->number[BlackCannon] > 1)
		return false;

	if(board->number[RedPawn] > 2 || board->number[BlackPawn] > 2)
		return false;

    // material
     material_get_info(mat_info, board, ThreadId);

	 if(mat_info->recog == MAT_KK)
		 return true;
	 else if(mat_info->recog == MAT_KABKAB)
		 return true;
	 else if(mat_info->recog == MAT_KPKAB || mat_info->recog == MAT_KABKP)
		 return true;
	 else if(mat_info->recog == MAT_KABKR || mat_info->recog == MAT_KRKAB)
		 return true;
	 else if(mat_info->recog == MAT_KNKN)
		 return true;
	 else if(mat_info->recog == MAT_KCKC)
		 return true;
	 else if(mat_info->recog == MAT_KPKP)
		 return true;

	
	return false;
}

// end of recog.cpp
