#ifndef MOVETABLE_H
#define MOVETABLE_H

// moves
extern int FileNoneCapMoves[10][1024][12];
extern int RankNoneCapMoves[9][512][12];
extern int FileCapMoves[10][1024][4];
extern int RankCapMoves[9][512][4];

extern int CannonFileCapMoves[10][1024][4];
extern int CannonRankCapMoves[9][512][4];
extern int SuperCannonFileCapMoves[10][1024][4];
extern int SuperCannonRankCapMoves[9][512][4];
extern int CannonFileControlMoves[10][1024][12];
extern int CannonRankControlMoves[9][512][12];

extern int  KingMoves[256][6];
extern int  AdvisorMoves[256][6];
extern int  BishopMoves[256][6];
extern int  ElephantEyes[256][6];
extern int  KnightMoves[256][10];
extern int  HorseLegs[256][10];
extern int  PawnMoves[2][256][4];

// cap sq
extern int FileCapMax[10][1024];
extern int FileCapMin[10][1024];
extern int FileNonCapMax[10][1024];
extern int FileNonCapMin[10][1024];

extern int RankCapMax[9][512];
extern int RankCapMin[9][512];
extern int RankNonCapMax[9][512];
extern int RankNonCapMin[9][512];

extern int CannonFileCapMax[10][1024];
extern int CannonFileCapMin[10][1024];
extern int CannonRankCapMax[9][512];
extern int CannonRankCapMin[9][512];
extern int SupperCannonFileCapMax[10][1024];
extern int SupperCannonFileCapMin[10][1024];
extern int SupperCannonRankCapMax[9][512];
extern int SupperCannonRankCapMin[9][512];


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

// Delta
extern int       NLegalDt[512];
extern int       KLegalDt[512];
extern int       ALegalDt[512];
extern int       BLegalDt[512];

extern void init_movetable();

#endif