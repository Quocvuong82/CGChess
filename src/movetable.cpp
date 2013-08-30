#include "movetable.h"
#include "colour.h"
#include "square.h"
#include <assert.h>
#include <stdio.h>

 const int    RedPawnInc[3 + 1]={ -16, -1, 1, 0 };
 const int    BlackPawnInc[3 + 1]={ 16, -1, 1, 0 };
 const int    KnightInc[8 + 1]={ -33, -31, -18, -14, +14, +18, +31, +33, 0 };
 const int    BishopInc[4 + 1]={ -34, -30, +30, +34, 0 };
 const int    RookInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    CannonInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    AdvisorInc[4 + 1]={ -17, -15, +15, +17, 0 };
 const int    KingInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    KnightLeg[8 + 1]={-16, -16, -1, +1, -1,  1,  16, 16 , 0};

// moves
extern int FileNoneCapMoves[10][1024][12] = {0};
extern int RankNoneCapMoves[9][512][12] = {0};
extern int FileCapMoves[10][1024][4] = {0};
extern int RankCapMoves[9][512][4] = {0};

extern int CannonFileCapMoves[10][1024][4] = {0};
extern int CannonRankCapMoves[9][512][4] = {0};
extern int SuperCannonFileCapMoves[10][1024][4] = {0};
extern int SuperCannonRankCapMoves[9][512][4] = {0};
extern int CannonFileControlMoves[10][1024][12] = {0};
extern int CannonRankControlMoves[9][512][12] = {0};

extern int  KingMoves[256][6] = {0};
extern int  AdvisorMoves[256][6] = {0};
extern int  BishopMoves[256][6] = {0};
extern int  ElephantEyes[256][6] = {0};
extern int  KnightMoves[256][10] = {0};
extern int  HorseLegs[256][10] = {0};
extern int  PawnMoves[2][256][4] = {0};

// cap sq
extern int FileCapMax[10][1024] = {0};
extern int FileCapMin[10][1024] = {0};
extern int FileNonCapMax[10][1024] = {0};
extern int FileNonCapMin[10][1024] = {0};

extern int RankCapMax[9][512] = {0};
extern int RankCapMin[9][512] = {0};
extern int RankNonCapMax[9][512] = {0};
extern int RankNonCapMin[9][512] = {0};

extern int CannonFileCapMax[10][1024] = {0};
extern int CannonFileCapMin[10][1024] = {0};
extern int CannonRankCapMax[9][512] = {0};
extern int CannonRankCapMin[9][512] = {0};
extern int SupperCannonFileCapMax[10][1024] = {0};
extern int SupperCannonFileCapMin[10][1024] = {0};
extern int SupperCannonRankCapMax[9][512] = {0};
extern int SupperCannonRankCapMin[9][512] = {0};

// attack

// Move Nb
extern int PinNb[10][10][1024] = {0};
extern int RankCapMoveNb[9][512] = {0};
extern int FileCapMoveNb[10][1024] = {0};
extern int RankNoneCapMoveNb[9][512] = {0};
extern int FileNoneCapMoveNb[10][1024] = {0};

extern int CannonFileCapMoveNb[10][1024] = {0};
extern int CannonRankCapMoveNb[9][512] = {0};
extern int CannonFileControlNb[10][1024] = {0};
extern int CannonRankControlNb[9][512] = {0};

// Delta
extern int       NLegalDt[512] = {0};
extern int       KLegalDt[512] = {0};
extern int       ALegalDt[512] = {0};
extern int       BLegalDt[512] = {0};

static void init_pin();
static void init_dt();
static void init_moves();
static void init_bits();

 void init_movetable()
 {
	 init_bits();
	 init_pin();
	 init_dt();
	 init_moves();
 }

 static void init_pin()
 {
	 int i, j, nb, x, k;
	 // PinNb
	 for(i = 0; i < 1024; ++i)
	 {
		 for(j = 0; j < 10; ++j)
		 {
			 for(k =j + 2; k < 10; ++k)
			 {
				 nb=0;
				 for(x=j + 1; x < k; ++x)
				 {
					 if(i & (1 << x)) nb++;
				 }

				 PinNb[j][k][i]=nb;
				 PinNb[k][j][i]=nb;
			 }
		 }
	 }
	 
 }

 static void init_dt()
{
	int i;
	for(i=0; i < 512; ++i)
	{
		KLegalDt[i]=0;
		ALegalDt[i]=0;
		BLegalDt[i]=0;
		NLegalDt[i]=0;
	}

	i=0;
	while(KingInc[i])
	{
		KLegalDt[KingInc[i] + 256]=KingInc[i];
		++i;
	}

	i=0;
	while(AdvisorInc[i])
	{
		ALegalDt[AdvisorInc[i] + 256]=AdvisorInc[i];
		++i;
	}

	i=0;
	while(BishopInc[i])
	{
		BLegalDt[BishopInc[i] + 256]=BishopInc[i];
		++i;
	}

	i=0;
	while(KnightInc[i])
	{
		NLegalDt[KnightInc[i] + 256] = KnightLeg[i];
		++i;
	}
}

 static void init_bits()
 {
	 int file, rank, filebit, rankbit, index, k, c, s;


	 for( file = 0; file < 9; ++file)
	 {
		 for( rankbit = 0; rankbit < 512; ++rankbit)
		 {
			 index = 0;

			 // none cap
			 //max
			 for( k = file + 1; k < 9; ++k )
			 {
				 if( rankbit & (1 << k))
				 {
					 break;
				 }

				 RankNonCapMax[file][rankbit] = A0 + k;
				 RankNoneCapMoves[file][rankbit][index++] =  A0 + k;
				 RankNoneCapMoveNb[file][rankbit]++;
			 }

			 //min
			 for( k = file - 1; k >= 0; --k)
			 {
				 if(rankbit & (1 << k))
				 {
					 break;
				 }
				 RankNonCapMin[file][rankbit] = A0 + k;
				 RankNoneCapMoves[file][rankbit][index++] =  A0 + k;
				 RankNoneCapMoveNb[file][rankbit]++;
			 }

			 RankNoneCapMoves[file][rankbit][index++] =  0;

			 // cap
			 index = 0;
			 // max
			 for( k = file + 1; k < 9; ++k )
			 {
				 if( rankbit & (1 << k))
				 {
					 RankCapMoves[file][rankbit][index++] =  A0 + k;
					 RankCapMax[file][rankbit] = A0 + k;
					 RankCapMoveNb[file][rankbit]++;
					 break;
				 }
			 }

			 // min
			 for( k = file - 1; k >= 0; --k)
			 {
				 if(rankbit & (1 << k))
				 {
					 RankCapMoves[file][rankbit][index++] =  A0 + k;
					 RankCapMin[file][rankbit] = A0 + k;
					 RankCapMoveNb[file][rankbit]++;
					 break;
				 }
			 }
			 RankCapMoves[file][rankbit][index++] = 0;

			 ///////////////Cannon////
			 index = 0;
			 c     = 0;
			 s     = 0;
			 // max
			 for(k = file + 1; k < 9; ++k)
			 {
				 if( rankbit & (1 << k))
				 {
					 break;//battery
				 }
			 }
			 k++;
			 for ( ; k < 9; ++k)
			 {
				 if( rankbit & (1 << k))
				 {
					 CannonRankCapMoves[file][rankbit][index++] = A0 + k;
					 CannonRankCapMax[file][rankbit] = A0 + k;
					 CannonRankCapMoveNb[file][rankbit]++;
					 break;
				 }

				 CannonRankControlMoves[file][rankbit][c++] = A0 + k;
				 CannonRankControlNb[file][rankbit]++;
			 }

			 // supper cannon
			 k++;
			 for ( ; k < 9; ++k)
			 {
				 if( rankbit & (1 << k))
				 {
					 SuperCannonRankCapMoves[file][rankbit][s++] = A0 + k;
					 SupperCannonRankCapMax[file][rankbit] = A0 + k;
					 break;
				 }
			 }

			 // min
			 for (k = file - 1; k >= 0; --k)
			 {
				 if( rankbit & (1 << k))
				 {
					 break;//battery
				 }
			 }
			 k--;
			 for(; k >= 0; --k)
			 {
				 if( rankbit & (1 << k))
				 {
					 CannonRankCapMoves[file][rankbit][index++] = A0 + k;
					 CannonRankCapMin[file][rankbit] = A0 + k;
					 CannonRankCapMoveNb[file][rankbit]++;
					 break;
				 }
				 CannonRankControlMoves[file][rankbit][c++] = A0 + k;
				 CannonRankControlNb[file][rankbit]++;
			 }
			 // supper cannon
			 k--;
			 for( ; k >= 0; --k)
			 {
				 if( rankbit & (1 << k))
				 {
					 SuperCannonRankCapMoves[file][rankbit][s++] = A0 + k;
					 SupperCannonRankCapMin[file][rankbit] = A0 + k;
					 break;
				 }
			 }

			 CannonRankCapMoves[file][rankbit][index++] = 0;
			 CannonRankControlMoves[file][rankbit][c++] = 0;
			 SuperCannonRankCapMoves[file][rankbit][s++]= 0;

		 }//rankbit
	 }//file




	 for(rank = 0; rank < 10; ++rank)
	 {
		 for(filebit = 0; filebit < 1024; ++filebit)
		 {

			 // max
			 index = 0;
			 for(k = rank + 1; k < 10; ++k)
			 {
				 if(filebit & (1 << k))
				 {
					 break;
				 }
				 FileNoneCapMoves[rank][filebit][index++] = (k << 4) + A0;
				 FileNonCapMax[rank][filebit] = (k << 4) + A0;
				 FileNoneCapMoveNb[rank][filebit]++;
			 }
			 // min
			 for(k = rank - 1; k >=0; --k)
			 {
				 if(filebit & (1 << k))
				 {
					 break;
				 }
				 FileNoneCapMoves[rank][filebit][index++] = (k << 4) + A0;
				 FileNonCapMin[rank][filebit] = (k << 4) + A0;
				 FileNoneCapMoveNb[rank][filebit]++;
			 }
			 FileNoneCapMoves[rank][filebit][index++] = 0;

			 // cap
			 // max
			 index = 0;
			 for(k = rank + 1; k < 10; ++k)
			 {
				 if(filebit & (1 << k))
				 {
					 FileCapMoves[rank][filebit][index++] = (k << 4) + A0;
					 FileCapMax[rank][filebit] = (k << 4) + A0;
					 FileCapMoveNb[rank][filebit]++;
					 break;
				 }
			 }
			 //min
			 for(k = rank - 1; k >=0; --k)
			 {
				 if(filebit & (1 << k))
				 {
					 FileCapMoves[rank][filebit][index++] = (k << 4) + A0;
					 FileCapMin[rank][filebit] = (k << 4) + A0;
					 FileCapMoveNb[rank][filebit]++;
					 break;
				 }
			 }
			 FileCapMoves[rank][filebit][index++] = 0;

			 ///////cannon/////
			 index = 0;
			 c     = 0;
			 s     = 0;

			 // max
			 for( k = rank + 1; k < 10; ++k)
			 {
				 if(filebit & (1 << k))
				 {
					 //battery
					 break;
				 }
			 }
			 k++;
			 for( ; k < 10; ++k)
			 {
				 if(filebit & (1 << k))
				 {
					 CannonFileCapMoves[rank][filebit][index++] = (k << 4) + A0;
					 CannonFileCapMax[rank][filebit] = (k << 4) + A0;
					 CannonFileCapMoveNb[rank][filebit]++;
					 break;
				 }
				 CannonFileControlMoves[rank][filebit][c++];
				 CannonFileControlNb[rank][filebit]++;
			 }
			 k++;
			 // suppercannon
			 for( ; k < 10; ++k)
			 {
				 if(filebit & (1 << k))
				 {
					 SuperCannonFileCapMoves[rank][filebit][s++] = (k << 4) + A0;
					 SupperCannonFileCapMax[rank][filebit] = (k << 4) + A0;
					 break;
				 }
			 }

			 // min
			 for(k = rank - 1; k >= 0; --k)
			 {
				 if(filebit & (1 << k))
				 {
					 //battery
					 break;
				 }
			 }
			 k--;
			 for( ; k >= 0; --k)
			 {
				 if(filebit & (1 << k))
				 {
					 CannonFileCapMoves[rank][filebit][index++] = (k << 4) + A0;
					 CannonFileCapMin[rank][filebit] = (k << 4) + A0;
					 CannonFileCapMoveNb[rank][filebit]++;
					 break;
				 }
				 CannonFileControlMoves[rank][filebit][c++];
				 CannonFileControlNb[rank][filebit]++;
			 }
			 k--;
			 for( ; k >= 0; --k)
			 {
				 if(filebit & (1 << k))
				 {
					 SuperCannonFileCapMoves[rank][filebit][s++] = (k << 4) + A0;
					 SupperCannonFileCapMin[rank][filebit] = (k << 4) + A0;
					 break;
				 }
			 }
			 CannonFileCapMoves[rank][filebit][index++] = 0;
			 CannonFileControlMoves[rank][filebit][c++] = 0;
			 SuperCannonFileCapMoves[rank][filebit][s++] = 0;

		 }//filebit
	 }//rank

 }

 static void init_moves()
 {
	int from, to, i, j, index;

	for(from=0; from < 256; from++)
	{
		
		if(IN_CITY(from))
		{
			// king
			index=0;
			for(i=0; i < 4; i++)
			{
				to=from + KingInc[i];
				if(IN_CITY(to))
				{
					KingMoves[from][index]=to;
					index++;
				}
			}
			KingMoves[from][index]=0;

			//advisor
			index=0;
			for(i=0; i < 4; i++)
			{
				to=from + AdvisorInc[i];
				if(IN_CITY(to))
				{
					AdvisorMoves[from][index]=to;
					index++;
				}
			}

			AdvisorMoves[from][index]=0;
		}

		if(SquareTo90[from] != -1)
		{
			//Bishop
			index=0;
			for(i=0; i < 4; i++)
			{
				to=from + BishopInc[i];
				if(SquareTo90[to] != -1 && !((from ^ to) & 0x80))
				{
					BishopMoves[from][index]=to;
					ElephantEyes[from][index]=(from + to) >> 1;
					index++;
				}
			}
			BishopMoves[from][index]=0;

			// Knight
			index=0;
			for(i=0; i < 8; i++)
			{
				to=from + KnightInc[i];
				if(SquareTo90[to] != -1)
				{
					KnightMoves[from][index]=to;
					HorseLegs[from][index]=from + NLegalDt[to - from + 256];
					index++;
				}
			}
			KnightMoves[from][index]=0;

			//pawn
			for(i=0; i <= 1; i++)
			{
				index=0;
				to=i ? from + 16 : from - 16;
				if(SquareTo90[to] != -1)
				{
					PawnMoves[i][from][index]=to;
					index++;
				}

				if(i ? (from & 0x80) : !(from & 0x80))
				{
					for(j= -1; j <= 1; j+=2)
					{
						to=from + j;
						if(SquareTo90[to] != -1)
						{
							PawnMoves[i][from][index]=to;
							index++;
						}
					}
				}

				PawnMoves[i][from][index]=0;
			}
	
		}
	}

 }

