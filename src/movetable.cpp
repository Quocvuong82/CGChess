#include "stdio.h"
#include "movetable.h"
#include "square.h"
#include "colour.h"

 const int    RedPawnInc[3 + 1]={ -16, -1, 1, 0 };
 const int    BlackPawnInc[3 + 1]={ 16, -1, 1, 0 };
 const int    KnightInc[8 + 1]={ -33, -31, -18, -14, +14, +18, +31, +33, 0 };
 const int    BishopInc[4 + 1]={ -34, -30, +30, +34, 0 };
 const int    RookInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    CannonInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    AdvisorInc[4 + 1]={ -17, -15, +15, +17, 0 };
 const int    KingInc[4 + 1]={ -16, -1, +1, +16, 0 };
 const int    KnightLeg[8 + 1]={16, 16, 1, -1, 1, -1, -16, -16 , 0};

 // delta
 int       NLegalDt[512] = {0};
 int       KLegalDt[512] = {0};
 int       ALegalDt[512] = {0};
 int       BLegalDt[512] = {0};

// move
 int FileNoneCapMoves[10][9][1024][12] = {0};//10 row 9 col filebit 1024*
 int RankNoneCapMoves[9][10][512][12] = {0}; //9 col 10 row rankbit 512 *
 int FileCapMoves[10][9][1024][4] = {0};//*
 int RankCapMoves[9][10][512][4] = {0}; //*

 int CannonFileCapMoves[10][9][1024][4] = {0};//*
 int CannonRankCapMoves[9][10][512][4] = {0};//*
 int SuperCannonFileCapMoves[10][9][1024][4] = {0};//*
 int SuperCannonRankCapMoves[9][10][512][4] = {0};//*
 int CannonFileControlMoves[10][9][1024][12] = {0};//*
 int CannonRankControlMoves[9][10][512][12] = {0};//*

// cap sq
 int FileCapMax[10][9][1024] = {0};
 int FileCapMin[10][9][1024] = {0};
 int FileNonCapMax[10][9][1024] = {0};
 int FileNonCapMin[10][9][1024] = {0};

 int RankCapMax[9][10][512] = {0};//*
 int RankCapMin[9][10][512] = {0};//*
 int RankNonCapMax[9][10][512] = {0};//*
 int RankNonCapMin[9][10][512] = {0};//*

 int CannonFileCapMax[10][9][1024] = {0};
 int CannonFileCapMin[10][9][1024] = {0};
 int CannonRankCapMax[9][10][512] = {0};//*
 int CannonRankCapMin[9][10][512] = {0};//
 int SupperCannonFileCapMax[10][9][1024] = {0};
 int SupperCannonFileCapMin[10][9][1024] = {0};
 int SupperCannonRankCapMax[9][10][512] = {0};//*
 int SupperCannonRankCapMin[9][10][512] = {0};//*

// Move Nb
 int PinNb[10][10][1024] = {0};
 int RankCapMoveNb[9][512] = {0};
 int FileCapMoveNb[10][1024] = {0};
 int RankNoneCapMoveNb[9][512] = {0};
 int FileNoneCapMoveNb[10][1024] = {0};

 int CannonFileCapMoveNb[10][1024] = {0};
 int CannonRankCapMoveNb[9][512] = {0};
 int CannonFileControlNb[10][1024] = {0};
 int CannonRankControlNb[9][512] = {0};

// Attack
 // rook
 int RookFileAttack[256][256][1024] = {0};
 int RookRankAttack[256][256][512] = {0};
 // knight
 int KnightAttack[256][256] = {0};
 int KnightLegs[256][256] = {0};
 // cannon
 int CannonFileAttack[256][256][1024] = {0};
 int CannonRankAttack[256][256][512] = {0};
 // pawn
 int PawnAttack[2][256][256] = {0};
 // bishop
 int BishopAttack[256][256] = {0};
 int BishopEyes[255][255] = {0};
 //advisor
 int AdvisorAttack[256][256] = {0};
 // king 
 int KingAttack[256][256] = {0};

 // Moves
 int       KingMoves[256][8] = {0};
 int       AdvisorMoves[256][8] = {0};
 int       BishopMoves[256][8] = {0};
 int       ElephantEyes[256][4] = {0};
 int       KnightMoves[256][12] = {0};
 int       HorseLegs[256][8] = {0};
 int       PawnMoves[2][256][4] = {0};
 int       PawnChecks[2][256][4] = {0};

 static void init_pin();
 static void init_dt();
 static void init_moves();
 static void init_bits();
 static void init_attacks();

 void init_movetable()
 {
	 init_bits();
	 init_pin();
	 init_dt();
	 init_moves();
	 init_attacks();
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
	 x = 0;
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
		NLegalDt[256 - KnightInc[i]]=KnightLeg[i];
		++i;
	}

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

  static void init_bits()
  {
	      int file, rank, filebit, rankbit, index, k, c, s;

	 for (rank = 0; rank < 10; ++rank)
	 {
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

					 RankNonCapMax[file][rank][rankbit] = A0 + k + (rank << 4);
					 RankNoneCapMoves[file][rank][rankbit][index++] =  A0 + k + (rank << 4);
                     RankNoneCapMoveNb[file][rank]++;
				 }

				 //min
				 for( k = file - 1; k >= 0; --k)
				 {
					 if(rankbit & (1 << k))
					 {
						 break;
					 }
					 RankNonCapMin[file][rank][rankbit] = A0 + k + (rank << 4);
					 RankNoneCapMoves[file][rank][rankbit][index++] =  A0 + k + (rank << 4);
					 RankNoneCapMoveNb[file][rankbit]++;
				 }

                 RankNoneCapMoves[file][rank][rankbit][index++] =  0;

                 // cap
				 index = 0;
				 // max
				 for( k = file + 1; k < 9; ++k )
				 {
					 if( rankbit & (1 << k))
					 {
						 RankCapMoves[file][rank][rankbit][index++] =  A0 + k + (rank << 4);
                         RankCapMax[file][rank][rankbit] = A0 + k + (rank << 4);
						 RankCapMoveNb[file][rankbit]++;
						 break;
					 }
				 }

				 // min
				 for( k = file - 1; k >= 0; --k)
				 {
					 if(rankbit & (1 << k))
					 {
						 RankCapMoves[file][rank][rankbit][index++] =  A0 + k + (rank << 4);
						 RankCapMin[file][rank][rankbit] = A0 + k + (rank << 4);
						 RankCapMoveNb[file][rankbit]++;
						 break;
					 }
				 }
				 RankCapMoves[file][rank][rankbit][index++] = 0;

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
						 CannonRankCapMoves[file][rank][rankbit][index++] = A0 + k + (rank << 4);
						 CannonRankCapMax[file][rank][rankbit] = A0 + k + (rank << 4);
						 CannonRankCapMoveNb[file][rankbit]++;
						 break;
					 }

                     CannonRankControlMoves[file][rank][rankbit][c++] = A0 + k + (rank << 4);
					 CannonRankControlNb[file][rankbit]++;
				 }

				 // supper cannon
				 k++;
				 for ( ; k < 9; ++k)
				 {
					 if( rankbit & (1 << k))
					 {
						 SuperCannonRankCapMoves[file][rank][rankbit][s++] = A0 + k + (rank << 4);
						 SupperCannonRankCapMax[file][rank][rankbit] = A0 + k + (rank << 4);
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
						   CannonRankCapMoves[file][rank][rankbit][index++] = A0 + k + (rank << 4);
						   CannonRankCapMin[file][rank][rankbit] = A0 + k + (rank << 4);
						   CannonRankCapMoveNb[file][rankbit]++;
						   break;
					   }
					   CannonRankControlMoves[file][rank][rankbit][c++] = A0 + k + (rank << 4);
					   CannonRankControlNb[file][rankbit]++;
				 }
				 // supper cannon
				 k--;
				 for( ; k >= 0; --k)
				 {
                      if( rankbit & (1 << k))
					  {
						 SuperCannonRankCapMoves[file][rank][rankbit][s++] = A0 + k + (rank << 4);
						 SupperCannonRankCapMin[file][rank][rankbit] = A0 + k + (rank << 4);
						 break;
					  }
				 }
				
                  CannonRankCapMoves[file][rank][rankbit][index++] = 0;
                  CannonRankControlMoves[file][rank][rankbit][c++] = 0;
				  SuperCannonRankCapMoves[file][rank][rankbit][s++]= 0;

			 }//rankbit
		 }//file
	 }//rank


     for(file = 0; file < 9; ++file)
	 {
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
					 FileNoneCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
					 FileNonCapMax[rank][file][filebit] = (k << 4) + A0 + file;
					 FileNoneCapMoveNb[rank][filebit]++;
				 }
				 // min
                 for(k = rank - 1; k >=0; --k)
				 {
					 if(filebit & (1 << k))
					 {
						 break;
					 }
                     FileNoneCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
					 FileNonCapMin[rank][file][filebit] = (k << 4) + A0 + file;
					 FileNoneCapMoveNb[rank][filebit]++;
				 }
                 FileNoneCapMoves[rank][file][filebit][index++] = 0;

                 // cap
				 // max
				 index = 0;
				 for(k = rank + 1; k < 10; ++k)
				 {
                      if(filebit & (1 << k))
					  {
						  FileCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
						  FileCapMax[rank][file][filebit] = (k << 4) + A0 + file;
						  FileCapMoveNb[rank][filebit]++;
						  break;
					  }
				 }
                 //min
				 for(k = rank - 1; k >=0; --k)
				 {
					 if(filebit & (1 << k))
					 {
						 FileCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
						 FileCapMin[rank][file][filebit] = (k << 4) + A0 + file;
						 FileCapMoveNb[rank][filebit]++;
						 break;
					 }
				 }
				 FileCapMoves[rank][file][filebit][index++] = 0;

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
						 CannonFileCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
						 CannonFileCapMax[rank][file][filebit] = (k << 4) + A0 + file;
						 CannonFileCapMoveNb[rank][filebit]++;
						 break;
					 }
					 CannonFileControlMoves[rank][file][filebit][c++];
					 CannonFileControlNb[rank][filebit]++;
				 }
				 k++;
				 // suppercannon
				 for( ; k < 10; ++k)
				 {
                      if(filebit & (1 << k))
					  {
						  SuperCannonFileCapMoves[rank][file][filebit][s++] = (k << 4) + A0 + file;
						  SupperCannonFileCapMax[rank][file][filebit] = (k << 4) + A0 + file;
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
						   CannonFileCapMoves[rank][file][filebit][index++] = (k << 4) + A0 + file;
						   CannonFileCapMin[rank][file][filebit] = (k << 4) + A0 + file;
						   CannonFileCapMoveNb[rank][filebit]++;
						   break;
					   }
                        CannonFileControlMoves[rank][file][filebit][c++];
						CannonFileControlNb[rank][filebit]++;
				 }
				 k--;
				 for( ; k >= 0; --k)
				 {
					  if(filebit & (1 << k))
					  {
						  SuperCannonFileCapMoves[rank][file][filebit][s++] = (k << 4) + A0 + file;
						  SupperCannonFileCapMin[rank][file][filebit] = (k << 4) + A0 + file;
						  break;
					  }
				 }
				 CannonFileCapMoves[rank][file][filebit][index++] = 0;
				 CannonFileControlMoves[rank][file][filebit][c++] = 0;
                 SuperCannonFileCapMoves[rank][file][filebit][s++] = 0;

			 }//filebit
		 }//rank
	 }//file
  }

  static void init_attacks()
  {
	  int filebit,rankbit,file_from, rank_from, file_to, rank_to, colour;
      int from, to;
	  for(from = 0; from < 256; ++from)
	  {
		  for(to = 0; to < 256; ++to)
		  {
              if(SQUARE_TO_90(from) >= 0 && SQUARE_TO_90(to) >= 0)
			  {
				  file_from = SQUARE_FILE(from)-3;
				  file_to   = SQUARE_FILE(to)-3;
				  rank_from = SQUARE_RANK(from)-3;
				  rank_to   = SQUARE_RANK(to)-3;				  

				  // rook
				  if(file_from == file_to)
				  {
                     for(filebit = 0; filebit < 1024; ++filebit)
					 {
                         if(PinNb[rank_from][rank_to][filebit] == 0)
						 {
							 RookFileAttack[from][to][filebit] = 1;
						 }
					 }
				  }
				  else if(rank_from == rank_to)
				  {
                     for(rankbit = 0; rankbit < 512; ++rankbit)
					 {
                         if(PinNb[file_from][file_to][rankbit] == 0)
						 {
							 RookRankAttack[from][to][rankbit] = 1;
						 }
					 }
				  }

				  //Knight
                  if(NLegalDt[to - from + 256] != 0)//the legs delta not 0, mean it is presudo move
				  {
                      KnightAttack[from][to] = 1;
                      KnightLegs[from][to] = from + NLegalDt[to - from + 256];
				  }

				  //Cannon
				  if(file_from == file_to)
				  {
					  for(filebit = 0; filebit < 1024; ++filebit)
					  {
						  if(PinNb[rank_from][rank_to][filebit] == 1)
						  {
							  CannonFileAttack[from][to][filebit] = 1;// cap and control square
						  }
					  }
				  }
				  else if(rank_from == rank_to)
				  {
					  for(rankbit = 0; rankbit < 512; ++rankbit)
					  {
						  if(PinNb[file_from][file_to][rankbit] == 1)
						  {
							  CannonRankAttack[from][to][rankbit] = 1; // cap and control square
						  }
					  }
				  }

				  // Pawn
				  for(colour = 0; colour < 2; ++colour)
				  {				  
					  if(colour == Red)
					  {
						  if(from - to == 16)
						  {
							  PawnAttack[colour][from][to] = 1;
						  }

						  if(!(from & 0x80))
						  {
							  if(to - from == 1 || to - from == -1)
							  {
								  PawnAttack[colour][from][to] = 1;
							  }
						  }
					  }
					  else
					  {
						  if(to - from == 16)
						  {
							  PawnAttack[colour][from][to] = 1;
						  }
						  if(from & 0x80)
						  {
							  if(to - from == 1 || to - from == -1)
							  {
								  PawnAttack[colour][from][to] = 1;
							  }
						  }
					  }
				  }// Pawn

				  // Bishop
				  if(BISHOP_IN_CITY(from) && BISHOP_IN_CITY(to) && (SQUARE_SIDE(from) == SQUARE_SIDE(to)))
				  {
					  if(BLegalDt[to - from + 256] != 0)
					  {
                          BishopAttack[from][to] = 1;
                          BishopEyes[from][to]   = (to + from) / 2;
					  }
				  }

				  // Advisor
				  if(ADVISOR_IN_CITY(from) && ADVISOR_IN_CITY(to) && (SQUARE_SIDE(from) == SQUARE_SIDE(to)))
				  {
                      if(ALegalDt[to - from + 256] != 0)
					  {
						  AdvisorAttack[from][to] = 1;
					  }
				  } 

				  // King
                  if(IN_CITY(from) && IN_CITY(to))
				  {
					  if(to - from == 16 || to - from == -16 || to - from == 1 || to - from == -1)
					  {
                          KingAttack[from][to] = 1;
					  }
				  }


			  }// SQUARE_TO_90
		  }//to
	  }//from

  }
