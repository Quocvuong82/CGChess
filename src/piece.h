#ifndef PIECE_H
#define PIECE_H

const int PieceNone = 0;

const int Piece7Nb = 8;

const int  Rook7   =1;
const int  Knight7 =2;
const int  Cannon7 =3;
const int  Pawn7   =4;
const int  Advisor7=5;
const int  Bishop7 =6;
const int  King7   =7;

const int Piece16Nb = 16;

const int RedRook16   = 1;
const int RedKnight16 = 2;
const int RedCannon16 = 3;
const int RedPawn16   = 4;
const int RedAdvisor16= 5;
const int RedBishop16 = 6;
const int RedKing16   = 7;

const int  BlackRook16   = 9;
const int  BlackKnight16 =10;
const int  BlackCannon16 =11;
const int  BlackPawn16   =12;
const int  BlackAdvisor16=13;
const int  BlackBishop16 =14;
const int  BlackKing16   =15;

const int Index32Nb   = 33;

const int IndexRedRook0   = 1;
const int IndexRedRook1   = 2;
const int IndexRedKnight0 = 3;
const int IndexRedKnight1 = 4;
const int IndexRedCannon0 = 5;
const int IndexRedCannon1 = 6;
const int IndexRedPawn0   = 7;
const int IndexRedPawn1   = 8;
const int IndexRedPawn2   = 9;
const int IndexRedPawn3   = 10;
const int IndexRedPawn4   = 11;
const int IndexRedAdvisor0= 12;
const int IndexRedAdvisor1= 13;
const int IndexRedBishop0 = 14;
const int IndexRedBishop1 = 15;
const int IndexRedKing    = 16;

const int IndexBlackRook0   = 17;
const int IndexBlackRook1   = 18;
const int IndexBlackKnight0 = 19;
const int IndexBlackKnight1 = 20;
const int IndexBlackCannon0 = 21;
const int IndexBlackCannon1 = 22;
const int IndexBlackPawn0   = 23;
const int IndexBlackPawn1   = 24;
const int IndexBlackPawn2   = 25;
const int IndexBlackPawn3   = 26;
const int IndexBlackPawn4   = 27;
const int IndexBlackAdvisor0= 28;
const int IndexBlackAdvisor1= 29;
const int IndexBlackBishop0 = 30;
const int IndexBlackBishop1 = 31;
const int IndexBlackKing    = 32;

extern const int Index32ToPiece16[Index32Nb];
extern const int Index32ToColour[Index32Nb];
extern const int Piece16ToIndex32[Piece16Nb];
extern const char Index32ToChar[Index32Nb + 1];
extern const int Index32ToPiece7[Index32Nb];
extern const int Index32Order[Index32Nb];
extern const int Piece16ToColour[Piece16Nb];

#define PIECE16_IS_OK(piece)                 (piece && (piece ^ 8) && !((piece | 15) ^ 15))
#define INDEX_IS_OK(index)                   (1 <= (index) && (index) <= 32)

#define INDEX_TO_COLOUR(index)          (Index32ToColour[(index)])
#define INDEX_TO_PIECE7(index)          (Index32ToPiece7[(index)])
#define INDEX_TO_PIECE16(index)         (Index32ToPiece16[(index)])
#define INDEX_ORDER(index)              (Index32Order[(index)])
#define INDEX_COLOUR_IS(index, colour)  ( Index32ToColour[(index)] == (colour))
#define PIECE16_TO_INDEX(piece)         (Piece16ToIndex32[(piece)])
#define INDEX_SAME_COLOUR(ia, ib)       ( Index32ToColour[(ia)] == Index32ToColour[(ib)])
#define PIECE16_TO_COLOUR(piece)        (Piece16ToColour[piece])

#define INDEX_IS_PAWN(index)        ( Index32ToPiece7[(index)] == Pawn7 )
#define INDEX_IS_CANNON(index)      ( Index32ToPiece7[(index)] == Cannon7 )
#define INDEX_IS_KNIGHT(index)      ( Index32ToPiece7[(index)] == Knight7 )
#define INDEX_IS_ROOK(index)        ( Index32ToPiece7[(index)] == Rook7 )
#define INDEX_IS_BISHOP(index)      ( Index32ToPiece7[(index)] == Bishop7 )
#define INDEX_IS_ADVISOR(index)     ( Index32ToPiece7[(index)] == Advisor7 )
#define INDEX_IS_KING(index)        ( Index32ToPiece7[(index)] == King7 )


#endif