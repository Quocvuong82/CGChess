#include "attack.h"
#include "board.h"
#include "colour.h"
#include "piece.h"
#include "movetable.h"
#include "move.h"
#include "types.h"
#include "move_do.h"

#define MIN(x,y)  ( (x)>(y)?(y):(x))
#define ABS(x) ((x)<0?-(x):(x))
#define FILE_DISTANCE( from, to) (ABS( SQUARE_FILE(from) - SQUARE_FILE(to) ))
#define RANK_DISTANCE( from, to) (ABS( SQUARE_RANK(from) - SQUARE_RANK(to) ))
#define SQUARE_DISTANCE(from, to) ( MIN(FILE_DISTANCE( from, to), RANK_DISTANCE( from, to)) )


bool in_check(board_t *board,  int colour);

bool attack_is_ok( const attack_t *attack )
{
	int i;
    int sq, inc;

    if( attack == NULL )
        return false;

    // checks

    if( attack->dn < 0)
        return false;

    for ( i = 0; i < attack->dn; i++ )
        {
        sq = attack->ds[i];

        if( !SQUARE_IS_OK(sq) )
            return false;

        inc = attack->di[i];

		if(!SQUARE_IS_OK(sq + inc))
			 return false;

        if( inc == IncNone)
            return false;
        }

    if( attack->ds[attack->dn] != SquareNone )
        return false;

    if( attack->di[attack->dn] != IncNone )
        return false;

	return true;
}

// colour side attack square to
bool is_attacked( board_t *board, int src, int to, int colour )
{
#if 0
    int i;
	int index_start, index_end;
	int from;
	int row, col;
	int rowbit, colbit;
	undo_t undo[1];
	bool attack;
	
	ASSERT(board != NULL);

	attack = false;
	move_pre_do(board, src, to, undo);

	index_start = INDEX_START(colour);
	index_end   = INDEX_END(colour);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	for( i = index_start; i <= index_end; ++i)
	{
		from = board->piece[i];
		if(from != SquareNone)
		{
            switch(INDEX_TO_PIECE16(i))
			{
			case RedRook16:
			case BlackRook16:
				if(RookFileAttack[from][to][colbit]) {attack = true; goto end;}
				if(RookRankAttack[from][to][rowbit]) {attack = true; goto end;}
				break;
			case RedCannon16:
			case BlackCannon16:
				if(CannonFileAttack[from][to][colbit]) {attack = true; goto end;}
                if(CannonRankAttack[from][to][rowbit]) {attack = true; goto end;}
				break;
			case RedKnight16:
			case BlackKnight16:
				if(KnightAttack[from][to] && board->square[KnightLegs[from][to]] == PieceNone) {attack = true; goto end;}
				break;
			case RedAdvisor16:
			case BlackAdvisor16:
				if(AdvisorAttack[from][to]) {attack = true; goto end;}
				break;
			case RedBishop16:
			case BlackBishop16:
				if(BishopAttack[from][to] && board->square[BishopEyes[from][to]] == PieceNone) {attack = true; goto end;}
				break;
			case RedPawn16:
				if(PawnAttack[Red][from][to]) {attack = true; goto end;}
				break;
			case BlackPawn16:
				if(PawnAttack[Black][from][to]) {attack = true; goto end;}
				break;
			case RedKing16:
			case BlackKing16:
				if(KingAttack[from][to]) {attack = true; goto end;}
				// king face to king face
				if(INDEX_IS_KING(board->square[to]) && RookFileAttack[from][to][colbit]) {attack = true; goto end;}
				break;
			default:
				break;
			}
		}
	}
end:
    move_pre_undo(board, src, to, undo);
	return attack;
#endif

	return false;

}

// 
bool is_pinned( board_t *board, int move, int colour )
{

	int from, to;
	undo_t undo[1];

	ASSERT(board != NULL);
	ASSERT(COLOUR_IS_OK(colour));

	from = MOVE_FROM(move);
	to =   MOVE_TO(move);

	move_pre_do(board, from, to, undo);
	if (in_check(board, colour))//notice colour,colour side is incheck
	{
		move_pre_undo(board, from, to, undo);
		return true;
	}

	move_pre_undo(board, from, to, undo);
	return false;
}

// all check king move
void attack_set( attack_t *attack, const board_t *board )
{
	int me, opp;
	int from, to;
	int start, end;
	int row, col;
	int rowbit, colbit;
	int i;

	ASSERT(attack != NULL);
	ASSERT(board != NULL);

	attack->dn = 0;

	me = board->turn;
	opp = COLOUR_OPP(me);

	to = KING_POS(board, me);

	start = INDEX_START(opp);
	end   = INDEX_END(opp);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	for( i = start; i <= end; ++i)
	{
		from = board->piece[i];
		if(from != SquareNone)
		{
			switch(INDEX_TO_PIECE(i))
			{
			case RedRook:
			case BlackRook:
				if(EQUAL_FILE(from, to))
				{
					if(FileCapMax[row][colbit] + col == from || FileCapMin[row][colbit] + col == from)
					{
						attack->ds[attack->dn] = from;
						attack->di[attack->dn] = to - from;
						attack->dn++;
					}
				}
				else if(EQUAL_RANK(from, to))
				{
					if(RankCapMax[col][rowbit] + (row<<4) == from || RankCapMin[col][rowbit] + (row<<4) == from)
					{
						attack->ds[attack->dn] = from;
						attack->di[attack->dn] = to - from;
						attack->dn++;
					}
				}

				break;
			case RedCannon:
			case BlackCannon:

				if(EQUAL_FILE(from, to))
				{
					if(CannonFileCapMax[row][colbit] + col == from || CannonFileCapMin[row][colbit] + col == from)
					{
						attack->ds[attack->dn] = from;
						attack->di[attack->dn] = to - from;
						attack->dn++;
					}
				}
				else if(EQUAL_RANK(from, to))
				{
					if(CannonRankCapMax[col][rowbit] + (row<<4) == from || CannonRankCapMin[col][rowbit] + (row<<4) == from)
					{
						attack->ds[attack->dn] = from;
						attack->di[attack->dn] = to - from;
						attack->dn++;
					}
				}
				break;
			case RedKnight:
			case BlackKnight:

				if(board->square[from + NLegalDt[to - from + 256]] == SquareNone)
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}

				break;
			case RedAdvisor:
			case BlackAdvisor:
				break;
			case RedBishop:
			case BlackBishop:
				break;
			case RedPawn:

				if(INDEX_TO_PIECE(board->square[to - 1]) == RedPawn ||
					INDEX_TO_PIECE(board->square[to + 1]) == RedPawn ||
					INDEX_TO_PIECE(board->square[to + 16]) == RedPawn)
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case BlackPawn:

				if(INDEX_TO_PIECE(board->square[to - 1]) == BlackPawn||
					INDEX_TO_PIECE(board->square[to + 1]) == BlackPawn ||
					INDEX_TO_PIECE(board->square[to - 16]) == BlackPawn)
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case RedKing:
			case BlackKing:
				
				// king face to king face	
				
				
				break;
			default:
				break;
			}//switch

		}
	}

	attack->ds[attack->dn] = SquareNone;
	attack->di[attack->dn] = 0;

	// debug

	ASSERT(attack_is_ok(attack));
}

// used in eval king safe
bool piece_attack_king( const board_t *board, int piece, int from, int king )
{
	int row, col, rowbit, colbit;
	int king_row, king_col, king_rowbit, king_colbit;
	int to;
	int index;

	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	king_row = SQUARE_RANK(king) - 3;
	king_col = SQUARE_FILE(king) - 3;
	king_rowbit = board->rank[king_row];
	king_colbit = board->file[king_col];
	
	switch(INDEX_TO_PIECE(piece))
	{
	case RedRook:
	case BlackRook:

		// 同行同列
		if(EQUAL_FILE(from, king) && PinNb[row][king_row][colbit] == 1) return true;
		if(EQUAL_RANK(from, king) && PinNb[col][king_col][rowbit] == 1 ) return true;

		// to位置与king同行同列
		index=0;
		while(FileCapMoves[row][colbit][index])
		{
			to = FileCapMoves[row][colbit][index] + col;
			if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
			{
				if(PinNb[row][king_row][colbit] <= 1) return true;
			}

			++index;				
		}

		index=0;			
		while(RankCapMoves[col][rowbit][index])
		{	
			to = RankCapMoves[col][rowbit][index] + (row<<4);
			if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
			{
				if(PinNb[col][king_col][rowbit] <= 1) return true;
			}
			++index;				
		}

		index=0;           
		while(FileNoneCapMoves[row][colbit][index])
		{
			to =  FileNoneCapMoves[row][colbit][index] + col;

			if(PinNb[row][king_row][colbit] <= 1) return true;

			++index;				
		}

		index=0;
		while(RankNoneCapMoves[col][rowbit][index])
		{
			to = RankNoneCapMoves[col][rowbit][index] + (row<<4);
			
			if(PinNb[col][king_col][rowbit] <= 1) return true;

			++index;
		}

		break;
	case RedKnight:
	case BlackKnight:

		//马脚位置在king附近
		index=0;
		to = KnightMoves[from][index];
		while(to != SquareNone)
		{
			//if(board->square[HorseLegs[from][index]] == PieceNone)
			{
                 if(FILE_DISTANCE(from, to) <= 2 || RANK_DISTANCE(from, to) <= 2)
					 return true;
			}
			to = KnightMoves[from][++index];

		}
		break;
	case RedCannon:
	case BlackCannon:

			index=0;			
			while( CannonFileCapMoves[row][colbit][index])
			{
				to = CannonFileCapMoves[row][colbit][index] + col;
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if(FILE_DISTANCE(from, to) <= 2 || RANK_DISTANCE(from, to) <= 2)
						return true;
				}

				++index;				
			}

			index=0;			
			while(CannonRankCapMoves[col][rowbit][index])
			{
				to = CannonRankCapMoves[col][rowbit][index] + (row<<4);
				if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
				{
					if(FILE_DISTANCE(from, to) <= 2 || RANK_DISTANCE(from, to) <= 2)
						return true;
				}
				
				++index;
			}



			//index=0;			
			//while(FileNoneCapMoves[row][colbit][index])
			//{	
			//	to = FileNoneCapMoves[row][colbit][index] + col;
			//	if(FILE_DISTANCE(from, to) <= 1 || RANK_DISTANCE(from, to) <= 1)
			//		return true;
   //             ++index;				
			//}

			//index=0;			
			//while(RankNoneCapMoves[col][rowbit][index])
			//{
			//	to = RankNoneCapMoves[col][rowbit][index]  + (row<<4);
			//	if(FILE_DISTANCE(from, to) <= 1 || RANK_DISTANCE(from, to) <= 1)
			//		return true;
   //             ++index;				
			//}
		break;
	case RedPawn:
	case BlackPawn:
		
		if(IN_CITY(from)) return true;

		if( ( FILE_DISTANCE(from, king) + RANK_DISTANCE(from, king) ) <= 3)
			return true;		

		break;
	}
	
	return false;
}

bool in_check(board_t *board,  int colour)
{
	int me, opp;
	int from, to;
	int row, col;
	int rowbit, colbit;
	int i,knight_i, rook_i, cannon_i;

	ASSERT(board != NULL);

	me = colour;
	opp = COLOUR_OPP(me);

	to = KING_POS(board, me);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];


	// rook
	if(COLOUR_IS_BLACK(me))
	{
		rook_i=PIECE_TO_INDEX(RedRook);		
	}
	else
	{
		rook_i=PIECE_TO_INDEX(BlackRook);
	}
    for(i = rook_i; i < rook_i + 2; ++i)
	{
       if((from = board->piece[i]) != SquareNone)
	   {
		   if(EQUAL_FILE(from, to))
		   {
               if(FileCapMax[row][colbit] + col == from || FileCapMin[row][colbit] + col == from)
				   return true;
		   }
		   else if(EQUAL_RANK(from, to))
		   {
               if(RankCapMax[col][rowbit] + (row<<4) == from || RankCapMin[col][rowbit] + (row<<4) == from)
				   return true;
		   }
	   }
	}


	// Cannon
	if(COLOUR_IS_BLACK(me))
	{
		cannon_i=PIECE_TO_INDEX(RedCannon);		
	}
	else
	{
		cannon_i=PIECE_TO_INDEX(BlackCannon);
	}
    for(i = cannon_i; i < cannon_i + 2; ++i)
	{
       if((from = board->piece[i]) != SquareNone)
	   {
		   if(EQUAL_FILE(from, to))
		   {
               if(CannonFileCapMax[row][colbit] + col == from || CannonFileCapMin[row][colbit] + col == from)
				   return true;
		   }
		   else if(EQUAL_RANK(from, to))
		   {
               if(CannonRankCapMax[col][rowbit] + (row<<4) == from || CannonRankCapMin[col][rowbit] + (row<<4) == from)
				   return true;
		   }
	   }
	}

	// Knight
	if(COLOUR_IS_BLACK(me))
	{
		knight_i=PIECE_TO_INDEX(RedKnight);		
	}
	else
	{
		knight_i=PIECE_TO_INDEX(BlackKnight);
	}
   	for(i = knight_i; i < knight_i + 2; ++i)
	{
		if((from = board->piece[i]) != SquareNone)
		{
			if(board->square[from + NLegalDt[to - from + 256]] == SquareNone) return true;
		}
	}

	// Pawn and king
	if(COLOUR_IS_BLACK(me))
	{
		from = FileCapMax[row][colbit] + col;
		if(SQUARE_IS_OK(from) && INDEX_TO_PIECE(board->square[from]) == RedKing) return true;
		if(INDEX_TO_PIECE(board->square[to - 1]) == RedPawn ||
			INDEX_TO_PIECE(board->square[to + 1]) == RedPawn ||
			INDEX_TO_PIECE(board->square[to + 16]) == RedPawn)
			return true;
	}
	else
	{
		from = FileCapMin[row][colbit] + col;
		if(SQUARE_IS_OK(from) && INDEX_TO_PIECE(board->square[from]) == BlackKing) return true;
		if(INDEX_TO_PIECE(board->square[to - 1]) == BlackPawn||
			INDEX_TO_PIECE(board->square[to + 1]) == BlackPawn ||
			INDEX_TO_PIECE(board->square[to - 16]) == BlackPawn)
			return true;
	}

    return false;
}

bool is_king_to_king(board_t *board, int move)
{
	int from, to;
	int row, col;
	int rowbit, colbit;
	int piece;
	int me, opp;
	int king_sq_me, king_sq_opp;
	undo_t undo[1];

    from = MOVE_FROM(move);
	to   = MOVE_TO(move);

	piece = board->square[from];

	me = INDEX_TO_COLOUR(piece);
	opp= COLOUR_OPP(me);

	move_pre_do(board, from, to, undo);
	
	king_sq_me = KING_POS(board, me);
	king_sq_opp= KING_POS(board, opp);

	if(EQUAL_FILE(king_sq_me, king_sq_opp))
	{
		col    = SQUARE_FILE(king_sq_me) - 3;
		row    = SQUARE_RANK(king_sq_me) - 3;
		rowbit = board->rank[row];
		colbit = board->file[col];

		if(  (FileCapMax[row][colbit] + col == king_sq_opp) || (FileCapMin[row][colbit] + col == king_sq_opp))
		{
			//face to face

			move_pre_undo(board, from, to, undo);

			return true;
		}
	}

	move_pre_undo(board, from, to, undo);

	return false;
}



