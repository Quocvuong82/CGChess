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
bool draw_krkr( const board_t *board);

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
	 else if(mat_info->recog == MAT_KRKR)
	 {
        return draw_krkr(board);
	 }

	
	return false;
}

bool draw_krkr( const board_t *board)
{
	int rrook, brook;
	int rking, bking;

	if(board->piece[PIECE_TO_INDEX(RedRook)] != PieceNone)
		rrook = board->piece[PIECE_TO_INDEX(RedRook)];

	if(board->piece[PIECE_TO_INDEX(RedRook) + 1] != PieceNone)
		rrook = board->piece[PIECE_TO_INDEX(RedRook) + 1];

	if(board->piece[PIECE_TO_INDEX(BlackRook)] != PieceNone)
		brook = board->piece[PIECE_TO_INDEX(RedRook)];

	if(board->piece[PIECE_TO_INDEX(BlackRook) + 1] != PieceNone)
		brook = board->piece[PIECE_TO_INDEX(RedRook) + 1];

	rking = KING_POS(board, Red);
	bking = KING_POS(board, Black);

	if(EQUAL_FILE(rrook, rking) && EQUAL_RANK(brook, bking))
		return true;

    if(SQUARE_FILE(rking) != 0x7 && SQUARE_FILE(bking) != 0x7)
	{
		if(SQUARE_RANK(rking) < 0xa && SQUARE_RANK(bking) > 0x5)
			return true;

	} 

	return false;
}

// end of recog.cpp
