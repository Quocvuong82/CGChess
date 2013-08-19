#ifndef DATA_H
#define DATA_H

// includes

//extern int       PinNb[10][10][1024];
//extern int       RankMoveNb[9][512];
//extern int       FileMoveNb[10][1024];

extern int       NLegalDt[512];//ÂíÍÈ²½³¤
extern int       KLegalDt[512];
extern int       ALegalDt[512];
extern int       BLegalDt[512];

//extern int       RankNonCapMax[9][512];
//extern int       RankNonCapMin[9][512];
//extern int       FileNonCapMax[10][1024];
//extern int       FileNonCapMin[10][1024];

extern int       RookRankCapMax[9][512];
extern int       RookRankCapMin[9][512];
extern int       RookFileCapMax[10][1024];
extern int       RookFileCapMin[10][1024];

//extern int       CannonRankCapMax[9][512];
//extern int       CannonRankCapMin[9][512];
//extern int       CannonFileCapMax[10][1024];
//extern int       CannonFileCapMin[10][1024];

extern int       KingMoves[256][8];
extern int       AdvisorMoves[256][8];
extern int       BishopMoves[256][8];
extern int       ElephantEyes[256][4];
extern int       KnightMoves[256][12];
extern int       HorseLegs[256][8];
//extern int       FileNonCapMoves[10][1024][12];
//extern int       RankNonCapMoves[9][512][12];
//extern int       RookFileCapMoves[10][1024][4];
//extern int       RookRankCapMoves[9][512][4];
//extern int       CannonFileCapMoves[10][1024][4];
//extern int       CannonRankCapMoves[9][512][4];
extern int       PawnMoves[2][256][4];
extern int       PawnChecks[2][256][4];

extern void         init_data();

#endif