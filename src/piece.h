#ifndef PICEC_H
#define PICEC_H

const int   PieceNb =16;
const int   TypeNb  = 8;

const int   PieceNone=0;

const int   Rook    =1;
const int   Knight  =2;
const int   Cannon  =3;
const int   Pawn    =4;
const int   Advisor =5;
const int   Bishop  =6;
const int   King    =7;

const int  RedRook   =1;
const int  RedKnight =2;
const int  RedCannon =3;
const int  RedPawn   =4;
const int  RedAdvisor=5;
const int  RedBishop =6;
const int  RedKing   =7;

const int  BlackRook   =9;
const int  BlackKnight =10;
const int  BlackCannon =11;
const int  BlackPawn   =12;
const int  BlackAdvisor=13;
const int  BlackBishop =14;
const int  BlackKing   =15;

const int  IndexNb = 33;

#define PIECE_IS_OK(piece)          ( 1<= (piece) && (piece) <= 15 && (piece) != 8 )
#define INDEX_IS_OK(index)          ( 1 <= (index) && (index) <= 32)
#define INDEX_TO_PIECE(index)       (IndexToPiece[index])
#define INDEX_TO_COLOUR(index)      (IndexToColour[index])
#define INDEX_SAME_COLOUR(ia, ib)   (IndexToColour[ia] == IndexToColour[ib])
#define PIECE_TO_INDEX(piece)       (PieceToIndex[piece])
#define INDEX_ORDER(index)          (IndexOrder[index])
#define INDEX_TO_CHAR(index)        (IndexToChar[(index)])
#define PIECE_TO_COLOUR(piece)      (PieceToColour[piece])
#define INDEX_TO_TYPE(index)        (IndexToType[index])

#define INDEX_IS_PAWN(piece)        ( (INDEX_TO_PIECE(piece) == RedPawn) || (INDEX_TO_PIECE(piece) == BlackPawn) )
#define INDEX_IS_CANNON(piece)      ((INDEX_TO_PIECE(piece) == RedCannon) || (INDEX_TO_PIECE(piece) == BlackCannon) )
#define INDEX_IS_KNIGHT(piece)      ((INDEX_TO_PIECE(piece) == RedKnight) || (INDEX_TO_PIECE(piece) == BlackKnight) )
#define INDEX_IS_ROOK(piece)        ((INDEX_TO_PIECE(piece) == RedRook) || (INDEX_TO_PIECE(piece) == BlackRook) )
#define INDEX_IS_BISHOP(piece)      ((INDEX_TO_PIECE(piece) == RedBishop) || (INDEX_TO_PIECE(piece) == BlackBishop) )
#define INDEX_IS_ADVISOR(piece)     ((INDEX_TO_PIECE(piece) == RedAdvisor) || (INDEX_TO_PIECE(piece) == BlackAdvisor) )
#define INDEX_IS_KING(piece)        ((INDEX_TO_PIECE(piece) == RedKing)||(INDEX_TO_PIECE(piece) == BlackKing))

extern const int IndexToPiece[IndexNb];
extern const int IndexToColour[IndexNb]; 
extern const int IndexOrder[IndexNb];
extern const int PieceToIndex[PieceNb];
extern const char IndexToChar[IndexNb + 1];
extern const int PieceToColour[PieceNb];
extern const int IndexToType[IndexNb];


#endif