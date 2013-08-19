#include "attack.h"
#include "board.h"
#include "colour.h"
#include "piece.h"
#include "movetable.h"
#include "move.h"
#include "types.h"
#include "move_do.h"

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

	index_start = INDEX32_START(colour);
	index_end   = INDEX32_END(colour);

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

	start = INDEX32_START(opp);
	end   = INDEX32_END(opp);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

	for( i = start; i <= end; ++i)
	{
		from = board->piece[i];
		if(from != SquareNone)
		{
			switch(INDEX_TO_PIECE16(i))
			{
			case RedRook16:
			case BlackRook16:
				if(RookFileAttack[from][to][colbit])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				else if(RookRankAttack[from][to][rowbit])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case RedCannon16:
			case BlackCannon16:
				if(CannonFileAttack[from][to][colbit])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				else if(CannonRankAttack[from][to][rowbit])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case RedKnight16:
			case BlackKnight16:
				if(KnightAttack[from][to] && board->square[KnightLegs[from][to]] == PieceNone)
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case RedAdvisor16:
			case BlackAdvisor16:
				break;
			case RedBishop16:
			case BlackBishop16:
				break;
			case RedPawn16:
				if(PawnAttack[Red][from][to])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case BlackPawn16:
				if(PawnAttack[Black][from][to])
				{
					attack->ds[attack->dn] = from;
					attack->di[attack->dn] = to - from;
					attack->dn++;
				}
				break;
			case RedKing16:
			case BlackKing16:
				
				// king face to king face				
				ASSERT(RookFileAttack[from][to][colbit] == 0);
				
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
	return false;
}

bool in_check(board_t *board,  int colour)
{
	int me, opp;
	int from, to;
	int start, end;
	int row, col;
	int rowbit, colbit;
	int i,knight_i;

	ASSERT(board != NULL);

	me = colour;
	opp = COLOUR_OPP(me);

	to = KING_POS(board, me);

	start = INDEX32_START(opp);
	end   = INDEX32_END(opp);

	row = SQUARE_RANK(to) - 3;
	col = SQUARE_FILE(to) - 3;
	rowbit = board->rank[row];
	colbit = board->file[col];

#if 1
	// rook
	from = FileCapMax[row][col][colbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Rook7 )
		return true;

	from = FileCapMin[row][col][colbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Rook7 )
		return true;

	from = RankCapMax[col][row][rowbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Rook7 )
		return true;

	from = RankCapMin[col][row][rowbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Rook7 )
		return true;

	// Cannon
	from = CannonFileCapMax[row][col][colbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Cannon7 )
		return true;
	from = CannonFileCapMin[row][col][colbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Cannon7 )
		return true;

	from = CannonRankCapMax[col][row][rowbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Cannon7 )
		return true;
	from = CannonRankCapMin[col][row][rowbit];
	if(SQUARE_IS_OK(from) && INDEX_TO_COLOUR(board->square[from]) == opp && INDEX_TO_PIECE7(board->square[from]) == Cannon7 )
		return true;

	// Knight
	if(COLOUR_IS_BLACK(me))
	{
		knight_i=PIECE16_TO_INDEX(RedKnight16);		
	}
	else
	{
		knight_i=PIECE16_TO_INDEX(BlackKnight16);
	}
   	for(i = knight_i; i < knight_i + 2; ++i)
	{
		if((from = board->piece[i]) != SquareNone)
		{
			if(board->square[from + NLegalDt[to - from + 256]] == SquareNone) return true;
		}
	}

	// Pawn
	if(COLOUR_IS_BLACK(me))
	{
		from = FileCapMax[row][col][colbit];
		if(SQUARE_IS_OK(from) && INDEX_TO_PIECE16(board->square[from]) == RedKing16) return true;
		if(INDEX_TO_PIECE16(board->square[to - 1]) == RedPawn16 ||
			INDEX_TO_PIECE16(board->square[to + 1]) == RedPawn16 ||
			INDEX_TO_PIECE16(board->square[to + 16]) == RedPawn16)
			return true;
	}
	else
	{
		from = FileCapMin[row][col][colbit];
		if(SQUARE_IS_OK(from) && INDEX_TO_PIECE16(board->square[from]) == BlackKing16) return true;
		if(INDEX_TO_PIECE16(board->square[to - 1]) == BlackPawn16||
			INDEX_TO_PIECE16(board->square[to + 1]) == BlackPawn16 ||
			INDEX_TO_PIECE16(board->square[to - 16]) == BlackPawn16)
			return true;
	}

    return false;
#endif
#if 0
	for( i = start; i <= end; ++i)
	{
		from = board->piece[i];
		if(from != SquareNone)
		{
			switch(INDEX_TO_PIECE16(i))
			{
			case RedRook16:
			case BlackRook16:
				if(RookFileAttack[from][to][colbit])
				{
                   return true;
				}
				else if(RookRankAttack[from][to][rowbit])
				{
                   return true;
				}
				break;
			case RedCannon16:
			case BlackCannon16:
				if(CannonFileAttack[from][to][colbit])
				{
                   return true;
				}
				else if(CannonRankAttack[from][to][rowbit])
				{
                   return true;
				}
				break;
			case RedKnight16:
			case BlackKnight16:
				if(KnightAttack[from][to] && board->square[KnightLegs[from][to]] == PieceNone)
				{
                    return true;
				}
				break;
			case RedAdvisor16:
			case BlackAdvisor16:
				break;
			case RedBishop16:
			case BlackBishop16:
				break;
			case RedPawn16:
				if(PawnAttack[Red][from][to])
				{
                   return true;
				}
				break;
			case BlackPawn16:
				if(PawnAttack[Black][from][to])
				{
                   return true;
				}
				break;
			case RedKing16:
			case BlackKing16:
				
				// king face to king face
				if(INDEX_IS_KING(board->square[to]) && RookFileAttack[from][to][colbit])
				{
					return true;
				}
				break;
			default:
				break;
			}//switch

		}
	}
#endif
	return false;

}

