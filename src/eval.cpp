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
#include "movetable.h"


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

const int KnightMobOpen[10] = { -50, -10, 0, 10, 30, 40, 50, 52, 54, 56};
const int KnightMobEnd[10] =  { -50, -10, 0, 10, 30, 40, 50, 52, 54, 56};
const int CannonControlOpen[22] = { 0, 10, 15, 18, 20, 22, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25 ,25, 25, 25, 25};
const int CannonAttackOpen[6] = { 0, 10, 20, 23, 26, 29};
const int CannonMobOpen[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
const int PawnMobOpen[5] = {0, 10, 20, 30, 0};
const int PawnMobEnd[5] = {0, 10, 20, 30, 0};
const int RookMobOpen[21] = {-50, -30, -20, -10, 10, 20, 30, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35};
const int RookMobEnd[21] = {-50, -30, -20, -10, 6, 12, 18, 20, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
const int File3BlockPawn = 25;
const int CityBlockKnight = 80;
const int NOProtectedKnight = 20;
const int KingRookAttack = 150;
const int CannonEmptyHead[13] = { 0, 0, 0, 35, 40, 45, 55, 65, 75, 80, 80, 80 };
const int Cannon2PieceHead[13] = {0, 0, 0, 38, 32, 26, 20, 20, 20, 20, 20, 20};

static const int KingAttackWeight[32] =
    {
    0, 0, 128, 192, 224, 240, 248, 252, 254, 255, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
    };

static const int SSpaceWeight[32] =
    {
    0, 2, 5, 8, 12, 15, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20
    };

static int MobUnit[ColourNb][PieceNb];
static int AttackAndProtected[TypeNb][TypeNb];
static int PinValue[TypeNb][TypeNb];
static int BlockKnight[TypeNb];
static int  CannonBatteryPieceValue[TypeNb];
static int KingAttackUnit[PieceNb];
static int ThreatValue[TypeNb][TypeNb];


#define THROUGH(piece) ((piece)==Empty)
#define ABS(x) ((x)<0?-(x):(x))
#define MAX(x, y) ( (x)>(y)?(x):(y))
#define MIN(x,y)  ( (x)>(y)?(y):(x))


static void eval_piece( board_t *board, const material_info_t *mat_info, const pawn_info_t *pawn_info,
	int *opening, int *endgame, int ControlArea[2][256]);
static void eval_pattern(board_t *board,int *opening, int *endgame, int ControlArea[2][256]);

static void eval_pattern(board_t *board,int *opening, int *endgame, int ControlArea[2][256]);
static void eval_king( const board_t *board, const material_info_t *mat_info, int *opening, int *endgame );
static void eval_passer( const board_t *board, int *opening, int *endgame, int ControlArea[2][256]);
static void eval_threat(const board_t *board, int *opening, int *endgame, int ControlArea[2][256]);

static bool attackbypawn(board_t *board, int to, int colour);
static int file_distance(int from, int to);
static int rank_distance(int from, int to);
static int pawn_distance(int from, int to);


// eval_init()

void eval_init()
{
	int i, j;
	int piece;

	// mobility table
	for ( i = 0; i < TypeNb; i++ )
	{
		for(j = 0; j < TypeNb; j++)
		{
			AttackAndProtected[i][j] = 0;
			PinValue[i][j] = 0;			
		}

		BlockKnight[i] = 0;
		CannonBatteryPieceValue[i] = 0;
	}

	//
	AttackAndProtected[Rook][Rook] =  0;
	AttackAndProtected[Rook][Knight] = 20;
	AttackAndProtected[Rook][Cannon] = 20;
	AttackAndProtected[Rook][Pawn] = 10;
	AttackAndProtected[Rook][Advisor] = 0;
	AttackAndProtected[Rook][Bishop] = 0;
	AttackAndProtected[Rook][King] = 0;

	AttackAndProtected[Knight][Rook] = 20;
	AttackAndProtected[Knight][Knight] = 15;
	AttackAndProtected[Knight][Cannon] = 5;
	AttackAndProtected[Knight][Pawn] = 10;
	AttackAndProtected[Knight][Advisor] = 0;
	AttackAndProtected[Knight][Bishop] = 0;
	AttackAndProtected[Knight][King] = 0;

	AttackAndProtected[Cannon][Rook] = 10;
	AttackAndProtected[Cannon][Knight] = 30;
	AttackAndProtected[Cannon][Cannon] = 20;
	AttackAndProtected[Cannon][Pawn] = 0;
	AttackAndProtected[Cannon][Advisor] = 0;
	AttackAndProtected[Cannon][Bishop] = 10;
	AttackAndProtected[Cannon][King] = 0;

	AttackAndProtected[Pawn][Rook] = 5;
	AttackAndProtected[Pawn][Knight] = 30;
	AttackAndProtected[Pawn][Cannon] = 5;
	AttackAndProtected[Pawn][Pawn] = 10;
	AttackAndProtected[Pawn][Advisor] = 0;
	AttackAndProtected[Pawn][Bishop] = 0;
	AttackAndProtected[Pawn][King] = 0;

	AttackAndProtected[Advisor][Rook] = 5;
	AttackAndProtected[Advisor][Knight] = 5;
	AttackAndProtected[Advisor][Cannon] = 25;
	AttackAndProtected[Advisor][Pawn] = 20;
	AttackAndProtected[Advisor][Advisor] = 35;
	AttackAndProtected[Advisor][Bishop] = 0;
	AttackAndProtected[Advisor][King] = 0;

	AttackAndProtected[Bishop][Rook] = 5;
	AttackAndProtected[Bishop][Knight] = 5;
	AttackAndProtected[Bishop][Cannon] = 8;
	AttackAndProtected[Bishop][Pawn] = 20;
	AttackAndProtected[Bishop][Advisor] = 0;
	AttackAndProtected[Bishop][Bishop] = 35;
	AttackAndProtected[Bishop][King] = 0;

	AttackAndProtected[King][Rook] = 0;
	AttackAndProtected[King][Knight] = 0;
	AttackAndProtected[King][Cannon] = 0;
	AttackAndProtected[King][Pawn] = 0;
	AttackAndProtected[King][Advisor] = 0;
	AttackAndProtected[King][Bishop] = 0;
	AttackAndProtected[King][King] = 0;

	//threat
	ThreatValue[Rook][Rook] =  0;
	ThreatValue[Rook][Knight] = 40;
	ThreatValue[Rook][Cannon] = 39;
	ThreatValue[Rook][Pawn] = 15;
	ThreatValue[Rook][Advisor] = 13;
	ThreatValue[Rook][Bishop] = 14;
	ThreatValue[Rook][King] = 0;

	ThreatValue[Knight][Rook] = 60;
	ThreatValue[Knight][Knight] = 15;
	ThreatValue[Knight][Cannon] = 31;
	ThreatValue[Knight][Pawn] = 4;
	ThreatValue[Knight][Advisor] = 6;
	ThreatValue[Knight][Bishop] = 6;
	ThreatValue[Knight][King] = 0;

	ThreatValue[Cannon][Rook] = 60;
	ThreatValue[Cannon][Knight] = 36;
	ThreatValue[Cannon][Cannon] = 13;
	ThreatValue[Cannon][Pawn] = 4;
	ThreatValue[Cannon][Advisor] = 11;
	ThreatValue[Cannon][Bishop] = 14;
	ThreatValue[Cannon][King] = 0;

	ThreatValue[Pawn][Rook] = 60;
	ThreatValue[Pawn][Knight] = 50;
	ThreatValue[Pawn][Cannon] = 50;
	ThreatValue[Pawn][Pawn] = 0;
	ThreatValue[Pawn][Advisor] = 0;
	ThreatValue[Pawn][Bishop] = 0;
	ThreatValue[Pawn][King] = 0;

	ThreatValue[Advisor][Rook] = 100;
	ThreatValue[Advisor][Knight] = 50;
	ThreatValue[Advisor][Cannon] = 60;
	ThreatValue[Advisor][Pawn] = 40;
	ThreatValue[Advisor][Advisor] = 0;
	ThreatValue[Advisor][Bishop] = 0;
	ThreatValue[Advisor][King] = 0;

	ThreatValue[Bishop][Rook] = 100;
	ThreatValue[Bishop][Knight] = 50;
	ThreatValue[Bishop][Cannon] = 60;
	ThreatValue[Bishop][Pawn] = 40;
	ThreatValue[Bishop][Advisor] = 0;
	ThreatValue[Bishop][Bishop] = 0;
	ThreatValue[Bishop][King] = 0;

	ThreatValue[King][Rook] = 0;
	ThreatValue[King][Knight] = 0;
	ThreatValue[King][Cannon] = 0;
	ThreatValue[King][Pawn] = 0;
	ThreatValue[King][Advisor] = 0;
	ThreatValue[King][Bishop] = 0;
	ThreatValue[King][King] = 0;



    // Pin
	PinValue[Rook][Rook] =  40;
	PinValue[Rook][Knight] = 80;
	PinValue[Rook][Cannon] = 60;
	PinValue[Rook][Pawn] = 0;
	PinValue[Rook][Advisor] = 0;
	PinValue[Rook][Bishop] = 0;
	PinValue[Rook][King] = 0;

	PinValue[Knight][Rook] = 40;
	PinValue[Knight][Knight] = 30;
	PinValue[Knight][Cannon] = 20;
	PinValue[Knight][Pawn] = 0;
	PinValue[Knight][Advisor] = 0;
	PinValue[Knight][Bishop] = 0;
	PinValue[Knight][King] = 0;

	PinValue[Cannon][Rook] = 50;
	PinValue[Cannon][Knight] = 60;
	PinValue[Cannon][Cannon] = 10;
	PinValue[Cannon][Pawn] = 0;
	PinValue[Cannon][Advisor] = 0;
	PinValue[Cannon][Bishop] = 0;
	PinValue[Cannon][King] = 0;

	PinValue[Pawn][Rook] = 0;
	PinValue[Pawn][Knight] = 0;
	PinValue[Pawn][Cannon] = 0;
	PinValue[Pawn][Pawn] = 0;
	PinValue[Pawn][Advisor] = 0;
	PinValue[Pawn][Bishop] = 0;
	PinValue[Pawn][King] = 0;

	PinValue[Advisor][Rook] = 0;
	PinValue[Advisor][Knight] = 0;
	PinValue[Advisor][Cannon] = 0;
	PinValue[Advisor][Pawn] = 0;
	PinValue[Advisor][Advisor] = 0;
	PinValue[Advisor][Bishop] = 0;
	PinValue[Advisor][King] = 0;

	PinValue[Bishop][Rook] = 0;
	PinValue[Bishop][Knight] = 0;
	PinValue[Bishop][Cannon] = 0;
	PinValue[Bishop][Pawn] = 0;
	PinValue[Bishop][Advisor] = 0;
	PinValue[Bishop][Bishop] = 0;
	PinValue[Bishop][King] = 0;

	PinValue[King][Rook] = 0;
	PinValue[King][Knight] = 0;
	PinValue[King][Cannon] = 0;
	PinValue[King][Pawn] = 2;
	PinValue[King][Advisor] = 2;
	PinValue[King][Bishop] = 2;
	PinValue[King][King] = 0; 

	// 
	BlockKnight[Rook] = 30;
	BlockKnight[Knight] = 10;
	BlockKnight[Cannon] = 10;
	BlockKnight[Pawn] = 20;
	BlockKnight[Advisor] = 0;
	BlockKnight[Bishop] = 0;
	BlockKnight[King] = 0;

	//
	CannonBatteryPieceValue[Rook] = 60;
	CannonBatteryPieceValue[Knight] = 50;
	CannonBatteryPieceValue[Cannon] = 40;
	CannonBatteryPieceValue[Pawn] = 3;
	CannonBatteryPieceValue[Advisor] = 3;
	CannonBatteryPieceValue[Bishop] = 26;
	CannonBatteryPieceValue[King] = 3;

	for ( piece = 0; piece < PieceNb; piece++ )
	{
		KingAttackUnit[piece] = 0;
	}

	KingAttackUnit[RedPawn] = 2;
	KingAttackUnit[RedKnight] = 2;
	KingAttackUnit[RedCannon] = 2;
	KingAttackUnit[RedRook] = 4;

	KingAttackUnit[BlackPawn] = 2;
	KingAttackUnit[BlackKnight] = 2;
	KingAttackUnit[BlackCannon] = 2;
	KingAttackUnit[BlackRook] = 4;

}

// eval()

int eval( board_t *board, int alpha, int beta, int ThreadId )
{	
	int opening, endgame;
	material_info_t mat_info[1];
	pawn_info_t pawn_info[1];
	int phase;
	int eval;
	int lazy_eval;
	int ControlArea[2][256] = {0};

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

	eval_piece(board, mat_info, pawn_info, &opening, &endgame, ControlArea);

	eval_pattern(board, &opening, &endgame, ControlArea);

	eval_king( board, mat_info,  &opening, &endgame );

	eval_passer(board,  &opening, &endgame, ControlArea);

	eval_threat(board,  &opening, &endgame, ControlArea);

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

static void eval_piece( board_t *board, const material_info_t *mat_info, const pawn_info_t *pawn_info,
	int *opening, int *endgame, int ControlArea[2][256])
{

	int colour;
	int op[ColourNb], eg[ColourNb];
	int me, opp;
	int from, to;
	int piece;
	int mob, con, att;
	int king_file, king_rank;
	int new_mob;
	int king;
	int piece_nb, attackvalue;
	int index_begin, index_end;
	int i, index;
	int row;
	int col;
	int filebit;
	int rankbit;
	bool equalking;
	bool cannonthreat;
    int attprotect[ColourNb];
	

	ASSERT(board != NULL);
	ASSERT(mat_info != NULL);
	ASSERT(pawn_info != NULL);
	ASSERT(opening != NULL);
	ASSERT(endgame != NULL);

	// init

	for ( colour = 0; colour < ColourNb; colour++ )
	{
		op[colour] = 0;
		eg[colour] = 0;
		attprotect[colour] = 0;
	}

	// eval

	for ( colour = 0; colour < ColourNb; colour++ )
	{

		me = colour;
		opp = COLOUR_OPP(me);

		piece_nb = 0;
		attackvalue = 0;

		king = KING_POS(board, opp);
		king_file = SQUARE_FILE(king)-3;
		king_rank = SQUARE_RANK(king)-3;

		// piece loop

		index_begin = INDEX_START(me);
		index_end   = index_begin + 15;// no king
		i = index_begin;

		for (from = board->piece[i]; from = board->piece[i],i < index_end; i++)
		{			
			if(from == SquareNone) continue;

		
			piece = board->square[from];

			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			filebit = board->file[col];
			rankbit = board->rank[row];

			switch( INDEX_TO_PIECE(piece) )
			{
			case RedKnight:
			case BlackKnight:

				// mobility
				mob = 0;
				index=0;
				while(KnightMoves[from][index])
				{
					if( board->square[HorseLegs[from][index]] == PieceNone)
					{
						to = KnightMoves[from][index];


						if( !INDEX_SAME_COLOUR( board->square[from],  board->square[to]))
						{							
							mob += 1;
						}

						attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

						ControlArea[me][to]++;

					}

					++index;
				} 

				if(mob < 0) mob = 0;

				op[me] += KnightMobOpen[mob];
				eg[me] += KnightMobEnd[mob];

				// outpost
				if(mob > 0)
				{
                    if(!attackbypawn(board, from, opp))
					   op[me] += KnightOutpost[me][SQUARE_TO_90(from)];
				}


				//threat
				if(me == Red)
				{
					if(SQUARE_RANK(from) <= Rank4)
					{	
						piece_nb++;
					    attackvalue += 2;
					}
				}
				else
				{
					if(SQUARE_RANK(from) >= Rank5)
					{	
						piece_nb++;
					    attackvalue += 2;
					}
				}

				break;
			case RedCannon:
			case BlackCannon:

				mob = 0;
				con = 0;
				att = 0;
				equalking = false;
				cannonthreat = false;

				index=0;
				while(CannonFileCapMoves[row][filebit][index])
				{
					to = CannonFileCapMoves[row][filebit][index] + col;

					
					{
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
						{
							mob += 1;
							con += 1;
							att += 1;
							ControlArea[me][to]++;
						}


						attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

						if(SQUARE_COLOUR(to) == opp)
							cannonthreat = true;

						if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
							equalking = true;
					}

					++index;
				}

				index=0;
				while(CannonRankCapMoves[col][rankbit][index])
				{
					to = CannonRankCapMoves[col][rankbit][index] + (row << 4);

					
					{
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[to])  )
						{
							mob += 1;
							con += 1;
							att += 1;
							ControlArea[me][to]++;
						}

						attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

						if(SQUARE_COLOUR(to) == opp)
							cannonthreat = true;

						if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
							equalking = true;
					}

					++index;
				}

				index=0;
				while(CannonFileControlMoves[row][filebit][index])
				{
					to=CannonFileControlMoves[row][filebit][index] + col;
					con += 1;
					ControlArea[me][to]++;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;
					++index;
				}

				index=0;
				while(CannonRankControlMoves[col][rankbit][index])
				{
					to=CannonRankControlMoves[col][rankbit][index] + (row << 4);
					con += 1;
					ControlArea[me][to]++;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;

					++index;
				}

				index=0;
				while(FileNoneCapMoves[row][filebit][index])
				{
					to = FileNoneCapMoves[row][filebit][index] + col;
					mob += 1;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;
					++index;
				}

				index=0;
				while(RankNoneCapMoves[col][rankbit][index])
				{
					to = RankNoneCapMoves[col][rankbit][index] + (row << 4);
					mob += 1;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;
					++index;
				}

				op[me] += CannonMobOpen[mob] ;
				op[me] += CannonControlOpen[con];
				op[me] += CannonAttackOpen[att];

				if(!attackbypawn(board, from, opp))
					op[me] += CannonOutPost[me][SQUARE_TO_90(from)];			
	

				if(EQUAL_FILE(from, king) || EQUAL_RANK(from, king) || equalking)
				{
					piece_nb++;
					attackvalue += 2;
				}

				break;

			case RedPawn:

				mob = 0;

				index=0;
				while(PawnMoves[Red][from][index])
				{
					to = PawnMoves[Red][from][index];
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) )
					{
						mob += 1;
					}
					ControlArea[me][to]++;

					attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

					++index;
				}


				op[me] += PawnMobOpen[mob] ;
				eg[me] += PawnMobEnd[mob] ;



				new_mob = 10 - ((ABS(king_file - col) + ABS(king_rank - row)));

				op[me] += new_mob;
				eg[me] += new_mob;

				
                op[me] += PawnOutpost[me][SQUARE_TO_90(from)];


				if(SQUARE_RANK(from) <= Rank4)
				{	
					piece_nb++;
					attackvalue += 5 - (Rank4 - Rank0);
				}


				break;
			case BlackPawn:

				// mobility
				mob = 0;

				index=0;
				while(PawnMoves[Black][from][index])
				{
					to = PawnMoves[Red][from][index];
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mob += 1;
					}

					ControlArea[me][to]++;

					attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

					++index;
				}


				op[me] += PawnMobOpen[mob] ;
				eg[me] += PawnMobEnd[mob] ;


				new_mob = 10 - ((ABS(king_file - col) + ABS(king_rank - row)));

				op[me] += new_mob;
				eg[me] += new_mob;

				op[me] += PawnOutpost[me][SQUARE_TO_90(from)];


				if(SQUARE_RANK(from) >= Rank5)
				{	
					piece_nb++;

					attackvalue += 5 - (Rank9 - Rank5);
				}				

				break;
			case RedBishop:
			case BlackBishop:

				// mobility

				mob = 0;

				index=0;
				while(BishopMoves[from][index])
				{
					if(board->square[ElephantEyes[from][index]] == PieceNone)
					{
						to=BishopMoves[from][index];
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
						{
							mob += 1;
						}

						ControlArea[me][to]++;

						attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];
					}					

					++index;
				}


				op[me] += mob;
				eg[me] += mob;  

				break;
			case RedAdvisor:
			case BlackAdvisor:

				mob = 0;

				index=0;
				while(AdvisorMoves[from][index])
				{
					to = AdvisorMoves[from][index];
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mob += 1;
					}

					ControlArea[me][to]++;

					attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

					++index;
				}

				op[me] += mob;
				eg[me] += mob;  

				break;
			case RedRook:
			case BlackRook:

				mob = 0;

				equalking = false;

				// mobility

				index=0;
				while(FileCapMoves[row][filebit][index])
				{
					to = FileCapMoves[row][filebit][index] + col;
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mob += 1;
					}

					ControlArea[me][to]++;

					attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
                          equalking = true;

					++index;
				}

				index=0;
				while(RankCapMoves[col][rankbit][index])
				{
					to=RankCapMoves[col][rankbit][index] + (row << 4);
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mob += 1;
					}

					ControlArea[me][to]++;

					attprotect[me] += AttackAndProtected[INDEX_TO_TYPE(piece)][INDEX_TO_TYPE(board->square[to])];

					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;

					++index;
				}

				index=0;
				while(FileNoneCapMoves[row][filebit][index])
				{
					to=FileNoneCapMoves[row][filebit][index] + col;
					mob += 1;
					ControlArea[me][to]++;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;
					++index;
				}

				index=0;
				while(RankNoneCapMoves[col][rankbit][index])
				{
					to=RankNoneCapMoves[col][rankbit][index] + (row << 4);
					mob += 1;
					ControlArea[me][to]++;
					if(EQUAL_FILE(to, king) || EQUAL_RANK(to, king))
						equalking = true;

					++index;
				}



				op[me] += RookMobOpen[mob];
				//eg[me] += RookMobEnd[mob];

				// open file
				new_mob = 10 - ((ABS(king_file - col) + ABS(king_rank - row)));

				op[me] += new_mob;
				eg[me] += new_mob;


				if(me == Red)
				{
					if(equalking || EQUAL_FILE(from, king) || EQUAL_RANK(from, king))
					{
						piece_nb++;
					    attackvalue += 4;
					}
				}
				else
				{
					if(equalking|| EQUAL_FILE(from, king) || EQUAL_RANK(from, king))
					{
						piece_nb++;
					    attackvalue += 4;
					}
				}

				break;
			}
		}
		op[me] += SSpaceWeight[piece_nb] * attackvalue;
	}

	// update

	*opening += ((op[Red] - op[Black]) * PieceActivityWeightOpening) / 256 + (attprotect[Red] - attprotect[Black] )/2;
	*endgame += ((eg[Red] - eg[Black]) * PieceActivityWeightEndgame) / 256 + (attprotect[Red] - attprotect[Black] )/2;
}

static void eval_pattern(board_t *board,int *opening, int *endgame, int ControlArea[2][256])
{
	int colour;
	int from, to, mid, mid1;
	int me, opp;
	int i;
	int op[ColourNb], eg[ColourNb];
	int index_begin, index_end;
	int row, col, rowbit, colbit;
	int king, king_row, king_col;
	int piece;
	int index;

    op[0] = 0;
	op[1] = 0;
	eg[0] = 0;
	eg[1] = 0;


	for ( colour = 0; colour < ColourNb; colour++ )
	{
		me = colour;
		opp = COLOUR_OPP(me);

		king = KING_POS(board,opp);
		king_row = SQUARE_RANK(king) - 3;
		king_col = SQUARE_FILE(king) - 3;

		index_begin = INDEX_START(me);
		index_end   = index_begin + 16;
		i = index_begin;

		for (from = board->piece[i]; from = board->piece[i],i < index_end; i++)
		{			
			if(from == SquareNone) continue;

			row = SQUARE_RANK(from) - 3;
			col = SQUARE_FILE(from) - 3;
			rowbit = board->rank[row];
			colbit = board->file[col];

			switch( INDEX_TO_PIECE(i) )
			{
			case RedRook:
			case BlackRook:
				{
					//牵制

					// 1
					to = CannonFileCapMin[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = FileCapMin[row][colbit] + col;
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
							}
							else if(INDEX_IS_ROOK(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
							}
							else if(INDEX_IS_KNIGHT(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
							}
						}
					}

					// 2
					to = CannonFileCapMax[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = FileCapMax[row][colbit] + col;
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
							}
							else if(INDEX_IS_ROOK(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
							}
							else if(INDEX_IS_KNIGHT(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
							}
						}
					}

					// 3
					to = CannonRankCapMax[col][rowbit] + (row <<4);
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = RankCapMax[col][rowbit] + (row <<4);
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
							}
							else if(INDEX_IS_ROOK(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
							}
							else if(INDEX_IS_KNIGHT(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
							}
						}
					}

					// 4
					to = CannonRankCapMin[col][rowbit] + (row <<4);
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						
						mid = RankCapMin[col][rowbit] + (row <<4);
						ASSERT(SQUARE_IS_OK(mid));
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[mid])];
							}
							else if(INDEX_IS_ROOK(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/2;
							}
							else if(INDEX_IS_KNIGHT(board->square[to]) && ControlArea[opp][to] == 0)
							{
								op[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
								eg[me] += PinValue[Rook][INDEX_TO_TYPE(board->square[to])]/4;
							}
							
						}
						
					}

				}
				break;
			case RedKnight:
			case BlackKnight:
				{
					index=0;
					while(KnightMoves[from][index])
					{
						mid = HorseLegs[from][index];
						if( board->square[mid] != PieceNone)
						{
							to = KnightMoves[from][index];

							if(!INDEX_SAME_COLOUR( board->square[from],  board->square[mid]))
							{
								 op[me] -= BlockKnight[INDEX_TO_TYPE(board->square[mid])];
								 eg[me] -= BlockKnight[INDEX_TO_TYPE(board->square[mid])];
							}
							
							if(!INDEX_SAME_COLOUR( board->square[from],  board->square[to]))
							{
                                if(INDEX_IS_KING(board->square[to]))
								{
								   op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								   eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								}
								else if(INDEX_IS_ROOK(board->square[to]))
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
									eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else if(INDEX_IS_CANNON(board->square[to]))
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
									eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else if(INDEX_IS_KNIGHT(board->square[to]))
								{
									if(NLegalDt[from - to + 256] && INDEX_SAME_COLOUR( board->square[from],  board->square[mid]))
									{
										op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
									    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
									}
								}
								else
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
									eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
							}
						}

						++index;
					}

					// 
                    if(me == Red)
					{
                        //3FilePawn
						if(from == C7 || from == G7)
						{
							to = from - 16;
							if(INDEX_TO_PIECE(board->square[to]) == RedPawn)
							{
                                   op[me] -= File3BlockPawn;
								   eg[me] -= File3BlockPawn;
							}
						}

						// INCITY 
						if(from == E1)
						{
							if(INDEX_IS_ADVISOR(board->square[D9]) && INDEX_IS_ADVISOR(board->square[F9]) && INDEX_IS_KING(board->square[E9]))
							{
                               to = CannonFileCapMin[row][colbit] + col;
							   if(INDEX_TO_PIECE(board->square[to]) == BlackCannon)
							   {
								   op[me] -= CityBlockKnight;
								   eg[me] -= CityBlockKnight;
							   }

							   to = FileCapMin[row][colbit] + col;
							   if(INDEX_TO_PIECE(board->square[to]) == BlackRook || INDEX_TO_PIECE(board->square[to]) == BlackKing)
							   {
								   op[me] -= CityBlockKnight;
								   eg[me] -= CityBlockKnight;
							   }
							}
						}
					}
					else
					{
                        //3FilePawn
						if(from == C2 || from == G2)
						{
							to = from + 16;
							if(INDEX_TO_PIECE(board->square[to]) == BlackPawn)
							{
                                   op[me] -= File3BlockPawn;
								   eg[me] -= File3BlockPawn;
							}
						}

						// INCITY 
						if(from == E8)
						{
							if(INDEX_IS_ADVISOR(board->square[D0]) && INDEX_IS_ADVISOR(board->square[F0]) && INDEX_IS_KING(board->square[E1]))
							{
                               to = CannonFileCapMax[row][colbit] + col;
							   if(INDEX_TO_PIECE(board->square[to]) == RedCannon )
							   {
								   op[me] -= CityBlockKnight;
								   eg[me] -= CityBlockKnight;
							   }
                               to = FileCapMax[row][colbit] + col;
							   if(INDEX_TO_PIECE(board->square[to]) == RedRook || INDEX_TO_PIECE(board->square[to]) == RedKing)
							   {
								   op[me] -= CityBlockKnight;
								   eg[me] -= CityBlockKnight;
							   }
							}
						}
					}


					if(ControlArea[me][from] == 0)
					{
						op[me] -= NOProtectedKnight;
						eg[me] -= NOProtectedKnight;
					}

				}
				break;
			case RedCannon:
			case BlackCannon:
				{
					//牵制
					// 1
					to = SupperCannonFileCapMin[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = FileCapMin[row][colbit] + col;
						mid1= CannonFileCapMin[row][colbit] + col;
						
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];

								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];


							}
							else
							{
								if(ControlArea[me][to] >= ControlArea[opp][to])
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								}
							}

						}
                        
						if(INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							op[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
							eg[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
						}

						
					}

					// 2
					to = SupperCannonFileCapMax[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = FileCapMax[row][colbit] + col;
						mid1= CannonFileCapMax[row][colbit] + col;
						
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];

								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
							}
							else
							{
								if(ControlArea[me][to] >=ControlArea[opp][to])
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								}
							}
						}

						if(INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							op[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
							eg[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
						}
					}

					// 3
					to = SupperCannonRankCapMax[col][rowbit] + (row <<4);
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = RankCapMax[col][rowbit] + (row <<4);
						mid1=CannonRankCapMax[col][rowbit] + (row <<4);
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];

								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
							}
							else
							{
								if(ControlArea[me][to] >=ControlArea[opp][to])
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								}
							}
						}

						if(INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							op[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
							eg[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
						}
					}

					// 4
					to = SupperCannonRankCapMin[col][rowbit] + (row <<4);
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						mid = RankCapMin[col][rowbit] + (row <<4);
						mid1= CannonRankCapMin[col][rowbit] + (row <<4);
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];

								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid1])];
							}
							else
							{
								if(ControlArea[me][to] >=ControlArea[opp][to])
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
								}
								else
								{
									op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								    eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])]/4;
								}
							}
						}

						if(INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							op[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
							eg[me] += CannonBatteryPieceValue[INDEX_TO_TYPE(board->square[mid])];
						}
					}

					//中路威胁
                    
					//空头炮
					if(EQUAL_FILE(from, king))
					{
						// 空头炮
						if(PinNb[row][king_row][colbit] == 0)
						{
							if( ControlArea[me][to] >=  ControlArea[opp][to])
							{
								op[me] += CannonEmptyHead[file_distance(from, king)];
							    eg[me] += CannonEmptyHead[file_distance(from, king)];
							}
						}

						// 2
						if(PinNb[row][king_row][colbit] == 2)
						{
							if( ControlArea[me][to] >=  ControlArea[opp][to])
							{
								op[me] += Cannon2PieceHead[file_distance(from, king)];
							    eg[me] += Cannon2PieceHead[file_distance(from, king)];
							}
						}
					}
					else if(EQUAL_RANK(from, king))
					{
						if(PinNb[col][king_col][rowbit] == 0 && ControlArea[me][to] >=  ControlArea[opp][to])
						{
							op[me] += CannonEmptyHead[rank_distance(from, king)];
							eg[me] += CannonEmptyHead[rank_distance(from, king)];
						}

						if(PinNb[col][king_col][rowbit] == 2&& ControlArea[me][to] >=  ControlArea[opp][to])
						{
							op[me] += Cannon2PieceHead[rank_distance(from, king)];
							eg[me] += Cannon2PieceHead[rank_distance(from, king)];
						}
					}

				}
				break;
			case RedPawn:
				{
					if(from == C5 )
					{
						if(INDEX_TO_PIECE(board->square[C2]) == BlackKnight)
						{
                              op[me] +=  File3BlockPawn;
							  eg[me] +=  File3BlockPawn;
						}
						
					}
					else if(from == G5)
					{
						if(INDEX_TO_PIECE(board->square[G2]) == BlackKnight)
						{
							op[me] +=  File3BlockPawn;
							eg[me] +=  File3BlockPawn;
						}
					}
				}
				break;
			case BlackPawn:
				{
					if(from == C4 )
					{
						if(INDEX_TO_PIECE(board->square[C7]) == BlackKnight)
						{
                              op[me] +=  File3BlockPawn;
							  eg[me] +=  File3BlockPawn;
						}
						
					}
					else if(from == G4)
					{
						if(INDEX_TO_PIECE(board->square[G7]) == BlackKnight)
						{
							op[me] +=  File3BlockPawn;
							eg[me] +=  File3BlockPawn;
						}
					}
				}
				break;
			case RedAdvisor:
			case BlackAdvisor:

				break;
			case RedBishop:
			case BlackBishop:

				break;
			case RedKing:
			case BlackKing:

					// 1
					to = CannonFileCapMin[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]) && IN_CITY(to))
					{
						mid = FileCapMin[row][colbit] + col;
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
							}
						}
						else
						{
							if(INDEX_IS_ROOK(board->square[mid]))
							{
								if(ControlArea[me][to] > ControlArea[opp][to])
								{
									op[me] += KingRookAttack;
									eg[me] += KingRookAttack;
								}
								else
								{
									op[me] += KingRookAttack/2;
									eg[me] += KingRookAttack/2;
								}
							}

						}
					}

					// 2
					to = CannonFileCapMax[row][colbit] + col;
					if(SQUARE_IS_OK(to) && !INDEX_SAME_COLOUR(board->square[from], board->square[to]) && IN_CITY(to))
					{
						mid = FileCapMax[row][colbit] + col;
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[mid]))
						{
							if(INDEX_IS_KING(board->square[to]))
							{
								op[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
								eg[me] += PinValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[mid])];
							}
						}
						else
						{
							if(INDEX_IS_ROOK(board->square[mid]))
							{
								if(ControlArea[me][to] > ControlArea[opp][to])
								{
									op[me] += KingRookAttack;
									eg[me] += KingRookAttack;
								}
								else
								{
									op[me] += KingRookAttack/2;
									eg[me] += KingRookAttack/2;
								}
							}

						}
					}
				break;
			}
		}
	}

	*opening += ((op[Red] - op[Black]) * 256) / 256;
	*endgame += ((eg[Red] - eg[Black]) * 256) / 256;
}

static void eval_king( const board_t *board, const material_info_t *mat_info, int *opening, int *endgame )
{

	int colour;
	int op[ColourNb], eg[ColourNb];
	int me, opp;
	int from;
	int king;
	int piece;
	int attack_tot;
	int piece_nb;
	int king_file, king_rank, file;
	int index_begin;
	int index_end;
	int i;

	ASSERT(board != NULL);
	ASSERT(mat_info != NULL);
	ASSERT(opening != NULL);
	ASSERT(endgame != NULL);

	// init

	for ( colour = 0; colour < ColourNb; colour++ )
	{
		op[colour] = 0;
		eg[colour] = 0;
	}

	// king attacks

	if( UseKingAttack )
	{
		for ( colour = 0; colour < ColourNb; colour++ )
		{

			me = colour;
			opp = COLOUR_OPP(me);

			king = KING_POS(board, me);
			king_file = SQUARE_FILE(king);
			king_rank = SQUARE_RANK(king);

			// piece attacks

			attack_tot = 0;
			piece_nb = 0;

			index_begin = INDEX_START(me);
			index_end   = index_begin + 11;
			i = index_begin;

			for (from = board->piece[i]; from = board->piece[i],i < index_end; i++)
			{			
				if(from == SquareNone) continue;
				if( piece_attack_king(board, i, from, king) )
				{
					piece_nb++;
					attack_tot += KingAttackUnit[INDEX_TO_PIECE(i)];
				}
			}

			// scoring

			ASSERT(piece_nb >= 0 && piece_nb < 16);

			op[colour] -= (attack_tot * KingAttackOpening * KingAttackWeight[piece_nb]) / 256;

		}
	}

	// update

	*opening += ((op[Red] - op[Black]) * KingSafetyWeightOpening) / 256;
	*endgame += ((eg[Red] - eg[Black]) * KingSafetyWeightEndgame) / 256;
}

static void eval_passer( const board_t *board, int *opening, int *endgame, int ControlArea[2][256])
{

	int colour;
	int op[ColourNb], eg[ColourNb];
	int att, def;
	int file, rank;
	int sq;
	int min, max;
	int delta;
	int index_start, index_end;
	int i;
	int Bonus[RankNb] = { 256, 256, 256, 256, 192, 128, 64, 32, 16, 8, 4, 2, 1, 0};


	ASSERT(board != NULL);
	ASSERT(pawn_info != NULL);
	ASSERT(opening != NULL);
	ASSERT(endgame != NULL);

	// init

	for ( colour = 0; colour < ColourNb; colour++ )
	{
		op[colour] = 0;
		eg[colour] = 0;
	}

	// passed pawns

	for ( colour = 0; colour < ColourNb; colour++ )
	{

		att = colour;
		def = COLOUR_OPP(att);

		if(att == Red)
		{
			index_start = PIECE_TO_INDEX(RedPawn);
			index_end    = index_start + 5;
		}
		else
		{
			index_start = PIECE_TO_INDEX(BlackPawn);
			index_end    = index_start + 5;
		}

		for( i = index_start; i < index_end; ++i)
		{
			sq = board->piece[i];
			if(sq == SquareNone) continue;
			if(SQUARE_COLOUR(sq) != def) continue;

			rank = SQUARE_RANK(sq) - 3;
			file = SQUARE_FILE(sq) - 3;

			if(att == Red)
			{
				op[att] += PassedOpeningMin + ((PassedOpeningMax - PassedOpeningMin) * Bonus[rank] + 128) / 256;
			}
			else
			{
				op[att] += PassedOpeningMin + ((PassedOpeningMax - PassedOpeningMin) * Bonus[9-rank] + 128) / 256;
			}

			min = PassedEndgameMin;
			max = PassedEndgameMax;

			delta = max - min;

			if(att == Red)
			{
                if(board->number[BlackRook] + board->number[BlackKnight] + board->number[BlackCannon] == 0)
				{
                    delta += FreePasser;
				}
				else if(board->number[BlackRook] == 0 && ControlArea[att][sq] >= ControlArea[def][sq])
				{
                    delta += FreePasser/2;
				}
				else if(ControlArea[att][sq] >= ControlArea[def][sq])
				{
                     delta += FreePasser/4;
				}

				if(board->number[BlackAdvisor] == 0)
				{
                     delta += FreePasser/4;
				}
			}
			else
			{
                if(board->number[RedRook] + board->number[RedKnight] + board->number[RedCannon] == 0)
				{
                    delta += FreePasser;
				}
				else if(board->number[RedRook] == 0 && ControlArea[att][sq] >= ControlArea[def][sq])
				{
                    delta += FreePasser/2;
				}
				else if(ControlArea[att][sq] >= ControlArea[def][sq])
				{
                     delta += FreePasser/4;
				}

				if(board->number[RedAdvisor] == 0)
				{
					delta += FreePasser/4;
				}
			}

			// king-distance bonus
			delta += pawn_distance(sq, KING_POS(board, def))* AttackerDistance;

			// endgame scoring

			eg[att] += min;

			if( delta > 0 )
			{				
				if(att == Red)
				{
					eg[att] += 0 + ((delta - 0) * Bonus[rank] + 128) / 256;
				}
				else
				{
					eg[att] += 0 + ((delta - 0) * Bonus[9-rank] + 128) / 256;
				}
			}
		}
	}

	*opening += ((op[Red] - op[Black]) * PassedPawnWeightOpening) / 256;
	*endgame += ((eg[Red] - eg[Black]) * PassedPawnWeightEndgame) / 256;
}

void eval_threat(const board_t *board, int *opening, int *endgame, int ControlArea[2][256])
{
	int me, opp;
	int from, to;
	int row, col;
	int filebit, rankbit;
	int i, index;
	int start, end;
	int colour;
	int op[ColourNb], eg[ColourNb];

	ASSERT(list != NULL);
	ASSERT(board != NULL);

	for ( colour = 0; colour < ColourNb; colour++ )
	{
		op[colour] = 0;
		eg[colour] = 0;
	}

	for(colour = 0; colour < ColourNb; ++colour)
	{

		me = colour;
		opp = COLOUR_OPP(me);

		start = INDEX_START(me);
		end   = INDEX_END(me);

		for( i= start; i <= end; ++i)
		{
			if(board->piece[i] == SquareNone) continue;

			from = board->piece[i];
			switch(INDEX_TO_PIECE(i))
			{
			case RedRook:
			case BlackRook:
				row = SQUARE_RANK(from) - 3;
				col = SQUARE_FILE(from) - 3;
				filebit =  board->file[col];
				rankbit =  board->rank[row];
				index=0;
				while(FileCapMoves[row][filebit][index])
				{
					to = FileCapMoves[row][filebit][index] + col;
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}

					++index;				
				}

				index=0;			
				while(RankCapMoves[col][rankbit][index])
				{	
					to = RankCapMoves[col][rankbit][index] + (row<<4);
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}
					++index;				
				}

				break;

			case RedCannon:
			case BlackCannon:
				row = SQUARE_RANK(from) - 3;
				col = SQUARE_FILE(from) - 3;
				filebit = board->file[col];
				rankbit = board->rank[row];

				index=0;
				while( CannonFileCapMoves[row][filebit][index])
				{
					to = CannonFileCapMoves[row][filebit][index] + col;
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}

					++index;				
				}

				index=0;			
				while(CannonRankCapMoves[col][rankbit][index])
				{
					to = CannonRankCapMoves[col][rankbit][index] + (row<<4);
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]))
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}

					++index;
				}

				break;

			case RedKnight:
			case BlackKnight:

				index=0;
				to = KnightMoves[from][index];
				while(to != SquareNone)
				{
					if(board->square[HorseLegs[from][index]] == PieceNone)
					{
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) &&  board->square[to]  != PieceNone)
						{
							op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
							eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						}
					}
					to = KnightMoves[from][++index];

				}
				break;

			case RedPawn:
			case BlackPawn:
				index=0;
				to = PawnMoves[me][from][index];
				while(to != SquareNone)
				{				
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}
					to = PawnMoves[me][from][++index];

				}
				break;

			case RedAdvisor:
			case BlackAdvisor:
				index=0;
				to = AdvisorMoves[from][index];
				while(to != SquareNone)
				{				
					if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
					{
						op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
					}
					to = AdvisorMoves[from][++index];

				}
				break;

			case RedBishop:
			case BlackBishop:
				index=0;
				to=BishopMoves[from][index];
				while(to != SquareNone)
				{
					if(board->square[ElephantEyes[from][index]] == PieceNone)
					{					
						if(!INDEX_SAME_COLOUR(board->square[from], board->square[to]) && board->square[to]  != PieceNone)
						{
							op[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
							eg[me] += ThreatValue[INDEX_TO_TYPE(board->square[from])][INDEX_TO_TYPE(board->square[to])];
						}
					}
					to=BishopMoves[from][++index];

				}
				break;

			case RedKing:
			case BlackKing:
				break;
			default:
				ASSERT(false);
				break;
			}
		}
	}

	*opening += ((op[Red] - op[Black]) * 256) / 256;
	*endgame += ((eg[Red] - eg[Black]) * 256) / 256;
}

bool attackbypawn(board_t *board, int to, int colour)
{
   if(colour == Red)// attack by red pawn
   {
	   if(SQUARE_IS_OK(to + 16) && INDEX_IS_PAWN(board->square[to + 16]) && INDEX_TO_COLOUR(board->square[to + 16]) == Red)
		   return true;
	   if(SQUARE_COLOUR(to) == Black)// passed
	   {
		   if(SQUARE_IS_OK(to + 1) && INDEX_IS_PAWN(board->square[to + 1]) && INDEX_TO_COLOUR(board->square[to + 1]) == Red)
			   return true;
		   if(SQUARE_IS_OK(to - 1) && INDEX_IS_PAWN(board->square[to - 1]) && INDEX_TO_COLOUR(board->square[to - 1]) == Red)
			   return true;
	   }
   }
   else
   {
	   if(SQUARE_IS_OK(to - 16) && INDEX_IS_PAWN(board->square[to - 16]) && INDEX_TO_COLOUR(board->square[to - 16]) == Black)
		   return true;
	   if(SQUARE_COLOUR(to) == Red)// passed
	   {
		   if(SQUARE_IS_OK(to + 1) && INDEX_IS_PAWN(board->square[to + 1]) && INDEX_TO_COLOUR(board->square[to + 1]) == Black)
			   return true;
		   if(SQUARE_IS_OK(to - 1) && INDEX_IS_PAWN(board->square[to - 1]) && INDEX_TO_COLOUR(board->square[to - 1]) == Black)
			   return true;
	   }
   }
   return false;
}

int file_distance(int from, int to)
{
	return ABS((SQUARE_FILE(from) - SQUARE_FILE(to)));
};

int rank_distance(int from, int to)
{
	return ABS((SQUARE_RANK(from) - SQUARE_RANK(to)));
};

int pawn_distance(int from, int to)
{
	return  file_distance(from, to) + rank_distance(from, to);
}









