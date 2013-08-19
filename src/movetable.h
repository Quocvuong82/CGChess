#ifndef MOVETABLE_H
#define MOVETABLE_H

// move
extern int FileNoneCapMoves[10][9][1024][12];//10 row 9 col filebit 1024
extern int RankNoneCapMoves[9][10][512][12]; //9 col 10 row rankbit 512
extern int FileCapMoves[10][9][1024][4];
extern int RankCapMoves[9][10][512][4];

extern int CannonFileCapMoves[10][9][1024][4];
extern int CannonRankCapMoves[9][10][512][4];
extern int SuperCannonFileCapMoves[10][9][1024][4];
extern int SuperCannonRankCapMoves[9][10][512][4];
extern int CannonFileControlMoves[10][9][1024][12];
extern int CannonRankControlMoves[9][10][512][12];

// cap sq
extern int FileCapMax[10][9][1024];
extern int FileCapMin[10][9][1024];
extern int FileNonCapMax[10][9][1024];
extern int FileNonCapMin[10][9][1024];

extern int RankCapMax[9][10][512];
extern int RankCapMin[9][10][512];
extern int RankNonCapMax[9][10][512];
extern int RankNonCapMin[9][10][512];

extern int CannonFileCapMax[10][9][1024];
extern int CannonFileCapMin[10][9][1024];
extern int CannonRankCapMax[9][10][512];
extern int CannonRankCapMin[9][10][512];
extern int SupperCannonFileCapMax[10][9][1024];
extern int SupperCannonFileCapMin[10][9][1024];
extern int SupperCannonRankCapMax[9][10][512];
extern int SupperCannonRankCapMin[9][10][512];

//-------
// Move Nb
extern int PinNb[10][10][1024];
extern int RankCapMoveNb[9][512];
extern int FileCapMoveNb[10][1024];
extern int RankNoneCapMoveNb[9][512];
extern int FileNoneCapMoveNb[10][1024];

extern int CannonFileCapMoveNb[10][1024];
extern int CannonRankCapMoveNb[9][512];
extern int CannonFileControlNb[10][1024];
extern int CannonRankControlNb[9][512];

// Attack
// rook
extern int RookFileAttack[256][256][1024];
extern int RookRankAttack[256][256][512];
// knight
extern int KnightAttack[256][256];
extern int KnightLegs[256][256];
// cannon
extern int CannonFileAttack[256][256][1024];
extern int CannonRankAttack[256][256][512];
// pawn
extern int PawnAttack[2][256][256];
// bishop
extern int BishopAttack[256][256];
extern int BishopEyes[255][255];
//advisor
extern int AdvisorAttack[256][256];
// king 
extern int KingAttack[256][256];

// Moves
extern int       KingMoves[256][8];
extern int       AdvisorMoves[256][8];
extern int       BishopMoves[256][8];
extern int       ElephantEyes[256][4];
extern int       KnightMoves[256][12];
extern int       HorseLegs[256][8];
extern int       PawnMoves[2][256][4];
extern int       PawnChecks[2][256][4];

// Delta
extern int       NLegalDt[512];
extern int       KLegalDt[512];
extern int       ALegalDt[512];
extern int       BLegalDt[512];

extern void init_movetable();

#endif