// move_do.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "hash.h"
#include "move.h"
#include "move_do.h"
#include "piece.h"
#include "pst.h"
#include "random.h"
#include "types.h"
#include "value.h"


// prototypes

static void square_clear( board_t *board, int square, int piece, bool update );
static void square_set( board_t *board, int square, int piece, bool update );
static void square_move( board_t *board, int from, int to, int piece, bool update );

// functions


// move_do()

void move_do( board_t *board, int move, undo_t *undo )
{

	int me, opp;
	int from, to;
	int piece, capture;	
	int sq;
	int row, col;

	ASSERT(board_is_ok(board));
	ASSERT(move_is_ok(move));
	ASSERT(undo != NULL);

	ASSERT(board_is_legal(board));

	// initialise undo

	undo->capture = false;

	undo->turn = board->turn;
	undo->ply_nb = board->ply_nb;

	undo->cap_sq = board->cap_sq;
	undo->moving_piece = board->moving_piece;

	undo->opening = board->opening;
	undo->endgame = board->endgame;

	undo->key = board->key;
	undo->material_key = board->material_key;

	// init

	me = board->turn;
	opp = COLOUR_OPP(me);

	from = MOVE_FROM(move);
	to = MOVE_TO(move);

	piece = board->square[from];
	board->moving_piece = piece;

	// update key stack

	ASSERT(board->sp < StackSize);
	board->stack[board->sp++] = board->key;

	// update turn

	board->turn = opp;
	board->key ^= RANDOM_90(RandomTurn);


	// update move number (captures are handled later)

	board->ply_nb++;

	// update last square

	board->cap_sq = SquareNone;

	// remove the captured piece
	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	board->file[col]^=1 << row;
	board->rank[row]^=1 << col;

	sq = to;
	if( (capture = board->square[sq]) != Empty )
	{

		ASSERT(INDEX_TO_COLOUR(capture) == opp);
		ASSERT(!INDEX_IS_KING(capture));

		undo->capture = true;
		undo->capture_square = sq;
		undo->capture_piece = capture;

		square_clear(board, sq, capture, true);

		board->ply_nb = 0; // conversion
		board->cap_sq = to;
	}
	else
	{
		row=SQUARE_RANK(to) - 3;
		col=SQUARE_FILE(to) - 3;
		board->file[col]^=1 << row;
		board->rank[row]^=1 << col;
	}

	// normal move

	ASSERT( 0 <= piece && piece < 33);
	square_move(board, from, to, piece, true);

    // debug

    ASSERT(board_is_ok(board));
}

// move_undo()

void move_undo( board_t *board, int move, const undo_t *undo )
{

	int me;
	int from, to;
	int piece;
	int row, col;

	ASSERT(board != NULL);
	ASSERT(move_is_ok(move));
	ASSERT(undo != NULL);

	// init

	me = undo->turn;

	from = MOVE_FROM(move);
	to = MOVE_TO(move);

	piece = board->square[to];
	ASSERT(INDEX_TO_COLOUR(piece) == me);

	// normal move
	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	board->file[col]^=1 << row;
	board->rank[row]^=1 << col;

	square_move(board, to, from, piece, false);

	// put the captured piece back

	if( undo->capture )
	{
		square_set(board, undo->capture_square, undo->capture_piece, false);
	}
	else
	{
		row = SQUARE_RANK(to) - 3;
		col = SQUARE_FILE(to) - 3;
		board->file[col]^=1 << row;
		board->rank[row]^=1 << col;
	}

	// update board info

	board->turn = undo->turn;
	board->ply_nb = undo->ply_nb;

	board->cap_sq = undo->cap_sq;
	board->moving_piece = undo->moving_piece;

	board->opening = undo->opening;
	board->endgame = undo->endgame;

	board->key = undo->key;
	board->material_key = undo->material_key;

	// update key stack

	ASSERT(board->sp > 0);
	board->sp--;

	// debug

	ASSERT(board_is_ok(board));
	ASSERT(board_is_legal(board));
}

// move_do_null()

void move_do_null( board_t *board, undo_t *undo )
{

	ASSERT(board != NULL);
	ASSERT(undo != NULL);

	ASSERT(board_is_legal(board));
	ASSERT(!board_is_check(board));

	// initialise undo

	undo->turn = board->turn;
	undo->ply_nb = board->ply_nb;
	undo->cap_sq = board->cap_sq;
	undo->moving_piece = board->moving_piece;
	undo->key = board->key;

	// update key stack

	ASSERT(board->sp < StackSize);
	board->stack[board->sp++] = board->key;

	// update turn

	board->turn = COLOUR_OPP(board->turn);
	board->key ^= RANDOM_90(RandomTurn);

	// update move number

	board->ply_nb = 0;

	// update last square

	board->cap_sq = SquareNone;
	board->moving_piece = PieceNone;

	// debug

	ASSERT(board_is_ok(board));
}

// move_undo_null()

void move_undo_null( board_t *board, const undo_t *undo )
{

	ASSERT(board != NULL);
	ASSERT(undo != NULL);

	ASSERT(board_is_legal(board));
	ASSERT(!board_is_check(board));

	// update board info

	board->turn = undo->turn;
	board->ply_nb = undo->ply_nb;
	board->cap_sq = undo->cap_sq;
	board->moving_piece = undo->moving_piece;
	board->key = undo->key;

	// update key stack

	ASSERT(board->sp > 0);
	board->sp--;

	// debug

	ASSERT(board_is_ok(board));
}

// square_clear()

static void square_clear( board_t *board, int square, int piece, bool update )
{

	int piece_16, colour;
	int size;
	int sq_90;
	uint64 hash_xor;

	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(square));
	ASSERT(INDEX_IS_OK(piece));
	ASSERT(update == true || update == false);

	// init

	piece_16 = INDEX_TO_PIECE(piece);
	colour = INDEX_TO_COLOUR(piece);

	// square

	ASSERT(board->square[square] == piece);
	board->square[square] = Empty;

	// piece list

	// init

	size = board->piece_size[colour];
	ASSERT(size >= 1);

	size--;

	board->piece[piece] = SquareNone;
	board->piece_size[colour] = size;

	// material

	ASSERT(board->piece_nb > 0);
	board->piece_nb--;

	ASSERT(board->number[piece_16] > 0);
	board->number[piece_16]--;

	board->piece_material[colour] -= VALUE_INDEX(piece);

	// update

	if( update )
	{

		// init

		sq_90 = SQUARE_TO_90(square);

		// PST

		board->opening -= PST(piece_16, sq_90, Opening);
		board->endgame -= PST(piece_16, sq_90, Endgame);

		// hash key

		hash_xor = RANDOM_90(RandomPiece + piece_16* 90 + sq_90);

		board->key ^= hash_xor;

		// material key

		board->material_key ^= RANDOM_90(piece_16 * 16 + board->number[piece_16]);//see board->number[piece_16]--;

	}
}

// square_set()

static void square_set( board_t *board, int square, int piece, bool update )
{

	int piece_16, colour;
	int size;
	int sq_90;
	uint64 hash_xor;

	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(square));
	ASSERT(INDEX_IS_OK(piece));
	ASSERT(update == true || update == false);

	// init

	piece_16 = INDEX_TO_PIECE(piece);
	colour = INDEX_TO_COLOUR(piece);

	// square

	ASSERT(board->square[square] == Empty);
	board->square[square] = piece;

	// piece list
	// init

	size = board->piece_size[colour];
	ASSERT(size >= 0);

	// size
	size++;

	board->piece_size[colour] = size;

	board->piece[piece] = square;


	// material

	ASSERT(board->piece_nb < 32);
	board->piece_nb++;


	ASSERT(board->number[piece_16] < 9);
	board->number[piece_16]++;

	board->piece_material[colour] += VALUE_INDEX(piece);

	// update

	if( update )
	{
		// init
		sq_90 = SQUARE_TO_90(square);

		// PST

		board->opening += PST(piece_16, sq_90, Opening);
		board->endgame += PST(piece_16, sq_90, Endgame);

		// hash key

		hash_xor = RANDOM_90(RandomPiece + (piece_16) * 64 + sq_90);

		board->key ^= hash_xor;

		// material key

		board->material_key ^= RANDOM_90(piece_16 * 16 + (board->number[piece_16] - 1));//see board->number[piece_16]--;
	}
}

// square_move()

static void square_move( board_t *board, int from, int to, int piece, bool update )
{

	int colour;
	int from_90, to_90;
	int piece_16;
	int piece_index;
	uint64 hash_xor;

	ASSERT(board != NULL);
	ASSERT(SQUARE_IS_OK(from));
	ASSERT(SQUARE_IS_OK(to));
	ASSERT(INDEX_IS_OK(piece));
	ASSERT(update == true || update == false);

	// init
	colour = INDEX_TO_COLOUR(piece);

	// from
	ASSERT(board->square[from] == piece);
	board->square[from] = Empty;

	// to
	ASSERT(board->square[to] == Empty);
	board->square[to] = piece;

	// piece list
	ASSERT(board->piece[piece] == from);
	board->piece[piece] = to;


	// update

	if( update )
	{
		// init

		from_90 = SQUARE_TO_90(from);
		to_90 = SQUARE_TO_90(to);
		piece_16 = INDEX_TO_PIECE(piece);

		// PST

		board->opening += PST(piece_16, to_90, Opening) - PST(piece_16, from_90, Opening);
		board->endgame += PST(piece_16, to_90, Endgame) - PST(piece_16, from_90, Endgame);

		// hash key

		piece_index = RandomPiece + (piece_16) * 90;

		hash_xor = RANDOM_90(piece_index + to_90) ^ RANDOM_90(piece_index + from_90);

		board->key ^= hash_xor;

	}
}

void move_pre_do(board_t *board, int from, int to ,undo_t *undo)
{
	int me, opp;
	int row, col;
	int sq;
	int capture;
	int piece;

	me = board->turn;
	opp = COLOUR_OPP(me);

	undo->capture = false;

	piece = board->square[from];

	// remove the captured piece
	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	board->file[col]^=1 << row;
	board->rank[row]^=1 << col;

	sq = to;
	if( (capture = board->square[sq]) != Empty )
	{

		ASSERT(INDEX_TO_COLOUR(capture) == opp);
		ASSERT(!INDEX_IS_KING(capture));

		undo->capture = true;
		undo->capture_square = sq;
		undo->capture_piece = capture;

		board->piece[capture] = SquareNone;
	}
	else
	{
		row=SQUARE_RANK(to) - 3;
		col=SQUARE_FILE(to) - 3;
		board->file[col]^=1 << row;
		board->rank[row]^=1 << col;
	}

    board->square[to] = piece;
	board->square[from] = PieceNone;
	board->piece[piece] = to;
}

void move_pre_undo(board_t *board, int from, int to ,undo_t *undo)
{
	int row, col;
	int piece;

	row = SQUARE_RANK(from) - 3;
	col = SQUARE_FILE(from) - 3;
	board->file[col]^=1 << row;
	board->rank[row]^=1 << col;

	piece = board->square[to];

	if(undo->capture)
	{
		board->piece[undo->capture_piece] = to;
		board->square[to]   = undo->capture_piece;
	}
	else
	{
		row = SQUARE_RANK(to) - 3;
	    col = SQUARE_FILE(to) - 3;
	    board->file[col]^=1 << row;
	    board->rank[row]^=1 << col;

		board->square[to] = PieceNone;
	}

	board->square[from] = piece;	
	board->piece[piece] = from;
}

// end of move_do.cpp


