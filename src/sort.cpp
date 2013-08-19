// sort.cpp

// includes

#include "attack.h"
#include "board.h"
#include "colour.h"
#include "list.h"
#include "move.h"
#include "move_check.h"
#include "move_evasion.h"
#include "move_gen.h"
#include "move_legal.h"
#include "piece.h"
#include "search.h"
#include "see.h"
#include "sort.h"
#include "util.h"
#include "value.h"

// macros

#define MIN(a,b) (a < b ? a : b)

// constants

static const int HistorySize = 16 * 90;
static const int HistoryMax = 16384;
static const int TransScore = +32766;
static const int GoodScore = +4000;
static const int KillerScore = KillerNb;
static const int HistoryScore = -24000;
static const int BadScore = -28000;

static const int CODE_SIZE = 256;

// macros

#define HISTORY_INC(depth) ((depth)*(depth))

// types

enum gen_t
{
	GEN_ERROR,
	GEN_LEGAL_EVASION,
	GEN_TRANS,
	GEN_GOOD_CAPTURE,
	GEN_BAD_CAPTURE,
	GEN_KILLER,
	GEN_QUIET,
	GEN_EVASION_QS,
	GEN_CAPTURE_QS,
	GEN_CHECK_QS,
	GEN_END
};

enum test_t
{
	TEST_ERROR,
	TEST_NONE,
	TEST_LEGAL,
	TEST_TRANS_KILLER,
	TEST_GOOD_CAPTURE,
	TEST_BAD_CAPTURE,
	TEST_KILLER,
	TEST_QUIET,
	TEST_CAPTURE_QS,
	TEST_CHECK_QS
};

// variables

static int PosLegalEvasion;
static int PosSEE;
static int PosEvasionQS;
static int PosCheckQS;
static int PosCaptureQS;
static int Code[CODE_SIZE];
static uint32 Killer[MaxThreads][HeightMax][KillerNb];
static uint32 History[MaxThreads][HistorySize];
static uint32 HistHit[MaxThreads][HistorySize];
static uint32 HistTot[MaxThreads][HistorySize];

// prototypes

static void note_captures( list_t *list, board_t *board );
static void note_quiet_moves( list_t *list, board_t *board, int ThreadId );
static void note_moves_simple( list_t *list, board_t *board );
static void note_mvv_lva( list_t *list, board_t *board );
static int move_value( int move, board_t *board, int height, int trans_killer, int ThreadId );
static int capture_value( int move, board_t *board );
static int quiet_move_value( int move, board_t *board, int ThreadId );
static int move_value_simple( int move, board_t *board );
static int history_prob( int move, board_t *board, int ThreadId );
static void store_killer( int move, int height, int ThreadId );
static int killer_number( int move, int height, int ThreadId );
static bool move_is_killer( int move, int killer [] );
static bool capture_is_good( int move, board_t *board );
static int mvv_lva( int move, board_t *board );
static uint16 history_index( int move, board_t *board );

// functions

// sort_init()

void sort_init( int ThreadId )
{

	int i, height;
	int pos;

	// killer

	for ( height = 0; height < HeightMax; height++ )
	{
		for ( i = 0; i < KillerNb; i++ )
			Killer[ThreadId][height][i] = MoveNone;
	}

	// history

	for ( i = 0; i < HistorySize; i++ )
		History[ThreadId][i] = 0;

	for ( i = 0; i < HistorySize; i++ )
	{
		HistHit[ThreadId][i] = 1;
		HistTot[ThreadId][i] = 1;
	}

	// Code[]

	for ( pos = 0; pos < CODE_SIZE; pos++ )
		Code[pos] = GEN_ERROR;

	pos = 0;

	// main search

	PosLegalEvasion = pos;
	Code[pos++] = GEN_LEGAL_EVASION;
	Code[pos++] = GEN_END;

	PosSEE = pos;
	Code[pos++] = GEN_TRANS;
	Code[pos++] = GEN_GOOD_CAPTURE;
	Code[pos++] = GEN_KILLER;
	Code[pos++] = GEN_QUIET;
	Code[pos++] = GEN_BAD_CAPTURE;
	Code[pos++] = GEN_END;

	// quiescence search

	PosEvasionQS = pos;
	Code[pos++] = GEN_EVASION_QS;
	Code[pos++] = GEN_END;

	PosCheckQS = pos;
	Code[pos++] = GEN_CAPTURE_QS;
	Code[pos++] = GEN_CHECK_QS;
	Code[pos++] = GEN_END;

	PosCaptureQS = pos;
	Code[pos++] = GEN_CAPTURE_QS;
	Code[pos++] = GEN_END;

	ASSERT(pos < CODE_SIZE);
}

// sort_init()

void sort_init( sort_t *sort, board_t *board, const attack_t *attack, int depth, int height, int trans_killer,
	int ThreadId )
{

	int i;

	ASSERT(sort != NULL);
	ASSERT(board != NULL);
	ASSERT(attack != NULL);
	ASSERT(depth_is_ok(depth));
	ASSERT(height_is_ok(height));
	ASSERT(trans_killer == MoveNone || move_is_ok(trans_killer));

	sort->board = board;
	sort->attack = attack;

	sort->depth = depth;
	sort->height = height;
	sort->capture_nb = 0;

	sort->trans_killer = trans_killer;

	for ( i = 0; i < KillerNb; i++ )
		sort->killer[i] = Killer[ThreadId][height][i];

	if( ATTACK_IN_CHECK(sort->attack) )
	{

		gen_legal_evasions(sort->list, sort->board, sort->attack);
		note_moves(sort->list, sort->board, sort->height, sort->trans_killer, ThreadId);
		list_sort(sort->list);

		sort->gen = PosLegalEvasion + 1;
		sort->test = TEST_NONE;
	}
	else
	{ // not in check

		LIST_CLEAR(sort->list);
		sort->gen = PosSEE;
	}

	sort->pos = 0;
}

// sort_next()

int sort_next( sort_t *sort, int ThreadId )
{

	int move;
	int gen;
	int i;

	ASSERT(sort != NULL);

	while( true )
	{
		while( sort->pos < LIST_SIZE(sort->list) )
		{

			// next move

			move = LIST_MOVE(sort->list, sort->pos);
			sort->value = 16384; // default score
			sort->valuePV = 16384;
			sort->pos++;

			ASSERT(move != MoveNone);

			// test

			if( false ) { }
			else if( sort->test == TEST_NONE ) {

				// no-op

			}
			else if( sort->test == TEST_TRANS_KILLER )
			{
				if( !move_is_pseudo(move, sort->board) )
					continue;

				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else if( sort->test == TEST_GOOD_CAPTURE )
			{

				ASSERT(move_is_tactical(move, sort->board));

				if( move == sort->trans_killer )
					continue;

				if( !capture_is_good(move, sort->board) )
				{
					LIST_ADD(sort->bad, move);
					continue;
				}

				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else if( sort->test == TEST_BAD_CAPTURE )
			{

				ASSERT(move_is_tactical(move, sort->board));
				ASSERT(!capture_is_good(move, sort->board));

				ASSERT(move != sort->trans_killer);

				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else if( sort->test == TEST_KILLER )
			{
				if( move == sort->trans_killer )
					continue;

				if( !quiet_is_pseudo(move, sort->board) )
					continue;

				if( !pseudo_is_legal(move, sort->board) )
					continue;

				if( killer_number(move, sort->height, ThreadId) >= KillerNoPruning )
					sort->value = history_prob(move, sort->board, ThreadId);

				ASSERT(!move_is_tactical(move, sort->board));
			}
			else if( sort->test == TEST_QUIET )
			{

				ASSERT(!move_is_tactical(move, sort->board));

				if( move == sort->trans_killer )
					continue;

				if( move_is_killer(move, sort->killer) )
					continue;

				if( !pseudo_is_legal(move, sort->board) )
					continue;

				sort->value = history_prob(move, sort->board, ThreadId);
			}
			else
			{

				ASSERT(false);

				return MoveNone;
			}

			ASSERT(pseudo_is_legal(move, sort->board));

			return move;
		}

		// next stage

		gen = Code[sort->gen++];

		if( false ) { }
		else if( gen == GEN_TRANS )
		{

			LIST_CLEAR(sort->list);

			if( sort->trans_killer != MoveNone )
				LIST_ADD(sort->list, sort->trans_killer);

			sort->test = TEST_TRANS_KILLER;
		}
		else if( gen == GEN_GOOD_CAPTURE )
		{

			gen_captures(sort->list, sort->board);
			sort->capture_nb = LIST_SIZE(sort->list);
			note_mvv_lva(sort->list, sort->board);
			list_sort(sort->list);

			LIST_CLEAR(sort->bad);

			sort->test = TEST_GOOD_CAPTURE;
		}
		else if( gen == GEN_BAD_CAPTURE )
		{

			list_copy(sort->list, sort->bad);

			sort->test = TEST_BAD_CAPTURE;
		}
		else if( gen == GEN_KILLER )
		{

			LIST_CLEAR(sort->list);

			for ( i = 0; i < KillerNb; i++ )
			{
				if( sort->killer[i] != MoveNone )
					LIST_ADD(sort->list, sort->killer[i]);
			}

			sort->test = TEST_KILLER;
		}
		else if( gen == GEN_QUIET )
		{

			gen_quiet_moves(sort->list, sort->board);
			note_quiet_moves(sort->list, sort->board, ThreadId);
			list_sort(sort->list);

			sort->test = TEST_QUIET;
		}
		else
		{

			ASSERT(gen == GEN_END);

			return MoveNone;
		}

		sort->pos = 0;
	}
}

// sort_init_qs()

void sort_init_qs( sort_t *sort, board_t *board, const attack_t *attack, bool check )
{

	ASSERT(sort != NULL);
	ASSERT(board != NULL);
	ASSERT(attack != NULL);
	ASSERT(check == true || check == false);

	sort->board = board;
	sort->attack = attack;

	if( ATTACK_IN_CHECK(sort->attack) )
	{
		sort->gen = PosEvasionQS;
	}
	else if( check )
	{
		sort->gen = PosCheckQS;
	}
	else
	{
		sort->gen = PosCaptureQS;
	}

	LIST_CLEAR(sort->list);
	sort->pos = 0;
}

// sort_next_qs()

int sort_next_qs( sort_t *sort )
{

	int move;
	int gen;

	ASSERT(sort != NULL);

	while( true )
	{
		while( sort->pos < LIST_SIZE(sort->list) )
		{

			// next move

			move = LIST_MOVE(sort->list, sort->pos);
			sort->pos++;

			ASSERT(move != MoveNone);
			ASSERT(move_is_ok(move));

			// test

			if( false ) { }
			else if( sort->test == TEST_LEGAL )
			{
				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else if( sort->test == TEST_CAPTURE_QS )
			{

				ASSERT(move_is_tactical(move, sort->board));

				if( !capture_is_good(move, sort->board) )
					continue;

				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else if( sort->test == TEST_CHECK_QS )
			{
				ASSERT(!move_is_tactical(move, sort->board));
				ASSERT(move_is_check(move, sort->board));

				if( see_move(move, sort->board) < 0 )
					continue;

				if( !pseudo_is_legal(move, sort->board) )
					continue;
			}
			else
			{

				ASSERT(false);

				return MoveNone;
			}

			ASSERT(pseudo_is_legal(move, sort->board));

			return move;
		}

		// next stage

		gen = Code[sort->gen++];

		if( false ) { }
		else if( gen == GEN_EVASION_QS )
		{

			gen_pseudo_evasions(sort->list, sort->board, sort->attack);
			note_moves_simple(sort->list, sort->board);
			list_sort(sort->list);

			sort->test = TEST_LEGAL;
		}
		else if( gen == GEN_CAPTURE_QS )
		{

			gen_captures(sort->list, sort->board);
			note_mvv_lva(sort->list, sort->board);
			list_sort(sort->list);

			sort->test = TEST_CAPTURE_QS;
		}
		else if( gen == GEN_CHECK_QS )
		{

			gen_quiet_checks(sort->list, sort->board);

			sort->test = TEST_CHECK_QS;
		}
		else
		{

			ASSERT(gen == GEN_END);

			return MoveNone;
		}

		sort->pos = 0;
	}
}

// good_move()

void good_move( int move, board_t *board, int depth, int height, int ThreadId )
{

	uint16 index;
	int i;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);
	ASSERT(depth_is_ok(depth));
	ASSERT(height_is_ok(height));

	if( move_is_tactical(move, board) )
		return;

	// killer

	store_killer(move, height, ThreadId);

	// history

	index = history_index(move, board);

	History[ThreadId][index] += HISTORY_INC(depth);

	if( History[ThreadId][index] >= HistoryMax )
	{
		for ( i = 0; i < HistorySize; i++ )
		{
			History[ThreadId][i] = (History[ThreadId][i] + 1) / 2;
		}
	}
}

// history_good()

void history_good( int move, board_t *board, int ThreadId )
{

	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	if( move_is_tactical(move, board) )
		return;

	// history

	index = history_index(move, board);

	HistHit[ThreadId][index]++;
	HistTot[ThreadId][index]++;

	if( HistTot[ThreadId][index] >= HistoryMax )
	{
		HistHit[ThreadId][index] = (HistHit[ThreadId][index] + 1) / 2;
		HistTot[ThreadId][index] = (HistTot[ThreadId][index] + 1) / 2;
	}

	ASSERT(HistHit[ThreadId][index] <= HistTot[ThreadId][index]);
	ASSERT(HistTot[ThreadId][index] < HistoryMax);
}

// history_bad()

void history_bad( int move, board_t *board, int ThreadId )
{

	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	if( move_is_tactical(move, board) )
		return;

	// history

	index = history_index(move, board);

	HistTot[ThreadId][index]++;

	if( HistTot[ThreadId][index] >= HistoryMax )
	{
		HistHit[ThreadId][index] = (HistHit[ThreadId][index] + 1) / 2;
		HistTot[ThreadId][index] = (HistTot[ThreadId][index] + 1) / 2;
	}

	ASSERT(HistHit[ThreadId][index] <= HistTot[ThreadId][index]);
	ASSERT(HistTot[ThreadId][index] < HistoryMax);
}

void history_reset( int move, board_t *board, int ThreadId )
{

	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	// history

	index = history_index(move, board);

	HistHit[ThreadId][index] = 1; //HistHit[ThreadId][index]/3 + 1;
	HistTot[ThreadId][index] = 1; //HistHit[ThreadId][index]/2 + 1;

	ASSERT(HistHit[ThreadId][index] <= HistTot[ThreadId][index]);
	ASSERT(HistTot[ThreadId][index] < HistoryMax);
}

// note_moves()

void note_moves( list_t *list, board_t *board, int height, int trans_killer, int ThreadId )
{

	int size;
	int i, move;

	ASSERT(list_is_ok(list));
	ASSERT(board != NULL);
	ASSERT(height_is_ok(height));
	ASSERT(trans_killer == MoveNone || move_is_ok(trans_killer));

	size = LIST_SIZE(list);

	if( size >= 2 )
	{
		for ( i = 0; i < size; i++ )
		{
			move = LIST_MOVE(list, i);
			list->value[i] = move_value(move, board, height, trans_killer, ThreadId);
		}
	}
}

// note_captures()

static void note_captures( list_t *list, board_t *board )
{

	int size;
	int i, move;

	ASSERT(list_is_ok(list));
	ASSERT(board != NULL);

	size = LIST_SIZE(list);

	if( size >= 2 )
	{
		for ( i = 0; i < size; i++ )
		{
			move = LIST_MOVE(list, i);
			list->value[i] = capture_value(move, board);
		}
	}
}

// note_quiet_moves()

static void note_quiet_moves( list_t *list, board_t *board, int ThreadId )
{

	int size;
	int i, move;

	ASSERT(list_is_ok(list));
	ASSERT(board != NULL);

	size = LIST_SIZE(list);

	if( size >= 2 )
	{
		for ( i = 0; i < size; i++ )
		{
			move = LIST_MOVE(list, i);
			list->value[i] = quiet_move_value(move, board, ThreadId);
		}
	}
}

// note_moves_simple()

static void note_moves_simple( list_t *list, board_t *board )
{

	int size;
	int i, move;

	ASSERT(list_is_ok(list));
	ASSERT(board != NULL);

	size = LIST_SIZE(list);

	if( size >= 2 )
	{
		for ( i = 0; i < size; i++ )
		{
			move = LIST_MOVE(list, i);
			list->value[i] = move_value_simple(move, board);
		}
	}
}

// note_mvv_lva()

static void note_mvv_lva( list_t *list, board_t *board )
{

	int size;
	int i, move;

	ASSERT(list_is_ok(list));
	ASSERT(board != NULL);

	size = LIST_SIZE(list);

	if( size >= 2 )
	{
		for ( i = 0; i < size; i++ )
		{
			move = LIST_MOVE(list, i);
			list->value[i] = mvv_lva(move, board);
		}
	}
}

// move_value()

static int move_value( int move, board_t *board, int height, int trans_killer, int ThreadId )
{

	int value, killer_nb;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);
	ASSERT(height_is_ok(height));
	ASSERT(trans_killer == MoveNone || move_is_ok(trans_killer));

	if( false ) { }
	else if( move == trans_killer )
	{ // transposition table killer
		value = TransScore;
	}
	else if( move_is_tactical(move, board) )
	{ // capture or promote
		value = capture_value(move, board);
	}
	else if( (killer_nb = killer_number(move, height, ThreadId)) >= 0 )
	{ // killer
		value = KillerScore - killer_nb;
	}
	else
	{ // quiet move
		value = quiet_move_value(move, board, ThreadId);
	}

	return value;
}

// capture_value()

static int capture_value( int move, board_t *board )
{

	int value;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(move_is_tactical(move, board));

	value = mvv_lva(move, board);

	if( capture_is_good(move, board) )
	{
		value += GoodScore;
	}
	else
	{
		value += BadScore;
	}

	ASSERT(value >= -30000 && value <= +30000);

	return value;
}

// quiet_move_value()

static int quiet_move_value( int move, board_t *board, int ThreadId )
{

	int value;
	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!move_is_tactical(move, board));

	index = history_index(move, board);

	value = HistoryScore + History[ThreadId][index];
	ASSERT(value >= HistoryScore && value <= KillerScore - 4);

	return value;
}

// move_value_simple()

static int move_value_simple( int move, board_t *board )
{

	int value;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	value = HistoryScore;

	if( move_is_tactical(move, board) )
		value = mvv_lva(move, board);

	return value;
}

// history_prob()

static int history_prob( int move, board_t *board, int ThreadId )
{

	int value;
	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!move_is_tactical(move, board));

	index = history_index(move, board);

	ASSERT(HistHit[ThreadId][index] <= HistTot[ThreadId][index]);
	ASSERT(HistTot[ThreadId][index] < HistoryMax);

	value = (HistHit[ThreadId][index] * 16384) / HistTot[ThreadId][index];

	ASSERT(value >= 0 && value <= 16384);

	return value;
}

// store_killer()

static void store_killer( int move, int height, int ThreadId )
{
	int i, j;

	for ( i = 0; i < KillerNb && move != Killer[ThreadId][height][i]; i++ );

	for ( j = MIN(KillerNb - 1, i); j > 0; j-- )
	{
		Killer[ThreadId][height][j] = Killer[ThreadId][height][j - 1];
	}
	Killer[ThreadId][height][0] = move;
}

// killer_number()

static int killer_number( int move, int height, int ThreadId )
{
	for ( int i = 0; i < KillerNb; i++ )
	{
		if( move == Killer[ThreadId][height][i] )
			return i;
	}
	return -1;
}

// move_is_killer()

static bool move_is_killer( int move, int killer [] )
{
	for ( int i = 0; i < KillerNb; i++ )
		if( move == killer[i] )
			return true;
	return false;
}

// capture_is_good()

static bool capture_is_good( int move, board_t *board )
{

	int piece, capture;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(move_is_tactical(move, board));


	// captures 

	capture = board->square[MOVE_TO(move)];

	if( capture != Empty )
	{
		// capture

		ASSERT(move_is_capture(move, board));

		piece = board->square[MOVE_FROM(move)];

		if( VALUE_INDEX(capture) >= VALUE_INDEX(piece) )
			return true;
	}

	return see_move(move, board) >= 0;
}

// mvv_lva()

static int mvv_lva( int move, board_t *board )
{

	int piece, capture;
	int value;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(move_is_tactical(move, board));


	capture = board->square[MOVE_TO(move)];
	ASSERT( capture!= Empty);

	piece = board->square[MOVE_FROM(move)];

	value = INDEX_ORDER(capture) * 6 - INDEX_ORDER(piece) + 5;
	ASSERT(value >= 0 && value < 30);

	ASSERT(value >= -4 && value < +30);

	return value;
}

// history_index()

static uint16 history_index( int move, board_t *board )
{

	uint16 index;

	ASSERT(move_is_ok(move));
	ASSERT(board != NULL);

	ASSERT(!move_is_tactical(move, board));

	index = INDEX_TO_PIECE16(board->square[MOVE_FROM(move)]) * 90 + SQUARE_TO_90(MOVE_TO(move));

	ASSERT(index >= 0 && index < HistorySize);

	return index;
}

// end of sort.cpp
