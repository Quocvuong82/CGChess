// material.cpp

// includes

#include <cstring>

#include "board.h"
#include "colour.h"
#include "hash.h"
#include "init.h"
#include "material.h"
#include "option.h"
#include "piece.h"
#include "protocol.h"
#include "square.h"
#include "util.h"
#include "search.h"
#include "value.h"

// constants

static const bool UseTable = false;
static const uint32 TableSize = 256;
static const int PawnPhase = 0;
static const int KnightPhase = 2;
static const int CannonPhase = 2;
static const int BishopPhase = 1;
static const int AdvisorPhase= 1;
static const int RookPhase = 4;
static const int TotalPhase = PawnPhase * 10 + KnightPhase * 4 + CannonPhase * 4 + BishopPhase * 4 + AdvisorPhase * 4 + RookPhase * 4;
//4*5=20

// types

typedef material_info_t entry_t;

struct material_t
{
	entry_t *table;
	uint32 size;
	uint32 mask;
	uint32 used;
	sint64 read_nb;
	sint64 read_hit;
	sint64 write_nb;
	sint64 write_collision;
};

// variables

static material_t Material[MaxThreads][1];

// prototypes

static void material_comp_info( material_info_t *info, const board_t *board );

// functions

// material_init()

void material_init()
{

	int ThreadId;

	// material table

	for ( ThreadId = 0; ThreadId < NumberThreads; ThreadId++ )
	{
		Material[ThreadId]->size = 0;
		Material[ThreadId]->mask = 0;
		Material[ThreadId]->table = NULL;
	}
}

// material_alloc()

void material_alloc()
{

	int ThreadId;

	ASSERT(sizeof(entry_t) == 16);

	if( UseTable )
	{
		for ( ThreadId = 0; ThreadId < NumberThreads; ThreadId++ )
		{
			Material[ThreadId]->size = TableSize;
			Material[ThreadId]->mask = TableSize - 1;
			Material[ThreadId]->table = (entry_t *)my_malloc(Material[ThreadId]->size * sizeof(entry_t));

			material_clear(ThreadId);
		}
	}
}

// material_clear()

void material_clear( int ThreadId )
{
	if( Material[ThreadId]->table != NULL )
	{
		memset(Material[ThreadId]->table, 0, Material[ThreadId]->size * sizeof(entry_t));
	}

	Material[ThreadId]->used = 0;
	Material[ThreadId]->read_nb = 0;
	Material[ThreadId]->read_hit = 0;
	Material[ThreadId]->write_nb = 0;
	Material[ThreadId]->write_collision = 0;
}

// material_get_info()

void material_get_info( material_info_t *info, const board_t *board, int ThreadId )
{

	uint64 key;
	entry_t *entry;

	ASSERT(info != NULL);
	ASSERT(board != NULL);

	// probe

	if( UseTable )
	{

		Material[ThreadId]->read_nb++;

		key = board->material_key;
		entry = &Material[ThreadId]->table[KEY_INDEX(key) & Material[ThreadId]->mask];

		if( entry->lock == KEY_LOCK(key) )
		{

			// found

			Material[ThreadId]->read_hit++;

			*info = *entry;

			return;
		}
	}

	// calculation

	material_comp_info(info, board);

	// store

	if( UseTable )
	{

		Material[ThreadId]->write_nb++;

		if( entry->lock == 0 )
		{
			Material[ThreadId]->used++;
		}
		else
		{
			Material[ThreadId]->write_collision++;
		}

		*entry = *info;
		entry->lock = KEY_LOCK(key);
	}
}

// material_comp_info()

static void material_comp_info( material_info_t *info, const board_t *board )
{
	// 根据双方子的数量判断和棋
	// multipliers， 判断双方子力能否杀死king

	int tp;
	int np;
	int tm;
	int colour;
	int mul[ColourNb];
	int phase;
	int opening, endgame;

	int rpawn, rknight, rcannon, rrook, rbishop, radvisor;
	int bpawn, bknight, bcannon, brook, bbishop, badvisor;
	int rtotal, btotal;
	int tpawn;

	int ropenf;
	int bopenf;
	int rendf;
	int bendf;
	int rm,bm;
	int rattack, battack;
	int rpromote, bpromote;


	ASSERT(info != NULL);
	ASSERT(board != NULL);

	rpawn = board->number[RedPawn];
	rknight=board->number[RedKnight];
	rcannon=board->number[RedCannon];
	rbishop=board->number[RedBishop];
	radvisor=board->number[RedAdvisor];
	rrook  = board->number[RedRook];

	bpawn = board->number[BlackPawn];
	bknight=board->number[BlackKnight];
	bcannon=board->number[BlackCannon];
	bbishop=board->number[BlackBishop];
	badvisor=board->number[BlackAdvisor];
	brook  = board->number[BlackRook];

	phase = TotalPhase;

	rtotal = rpawn + rknight + rcannon + rrook + rbishop + radvisor;
	btotal = bpawn + bknight + bcannon + brook + bbishop + badvisor;

	rm = rknight + rcannon;
	bm = bknight + bcannon;

	rattack = rm + rpawn + rrook;
	battack = bm + bpawn + brook;

	rpromote = rbishop + radvisor;
	bpromote = bbishop + badvisor;

	//和棋判断

	mul[Red] = 16;
    mul[Black] = 16;

	//红方
	if(false){}
	else if(rattack == 0)
	{
		mul[Red] = 0;
	}
	else if(rattack == 1)
	{
		if(rpawn == 1)
		{
           if(battack == 0)
		   {
			   if(bpromote == 0)
			   {
				   mul[Red] = 16;
			   }
			   else if(bpromote == 1)
			   {
				   if(badvisor == 1)
				   {
					    mul[Red] = 16;
				   }
				   else
				   {
					    mul[Red] = 0;
				   }
			   }
			   else
			   {
				    mul[Red] = 0;
			   }
		   }
		   else
		   {
			   mul[Red] = 0;
		   }
		}
		else if(rcannon == 1)
		{
			if(battack == 0)
			{
				if(bpromote == 0)
				{
					if(radvisor > 0)
					{
                        mul[Red] = 16;
					}
					else
					{
						mul[Red] = 0;
					}
				}
				else if(bpromote == 1)
				{
					if(radvisor > 0)
					{
						mul[Red] = 16;
					}
					else
					{
						mul[Red] = 0;
					}
				}
				else if(bpromote == 2)
				{
                   if(badvisor > 1 || bbishop > 1)
				   {
					   mul[Red] = 1;
				   }
				   else
				   {
                      mul[Red] = 16;
				   }
				}
				else
				{
					 mul[Red] = 1;
				}
			}
			else if(battack >= 1)
			{
               mul[Red] = 1;
			}
		}
		else if(rknight == 1)
		{
			if(battack == 0)
			{
				if(bpromote == 0)
				{
					mul[Red] = 16;
				}
				if(bpromote == 1)
				{
					if(badvisor == 1)
					{
						mul[Red] = 16;
					}
					else
					{
						mul[Red] = 8;
					}
				}
				else
				{
                    mul[Red] = 1;
				}
			}
			else if(battack >=1) 
			{
				mul[Red] = 1;
			}
		
		}
		else if(rrook == 1)
		{
			mul[Red] = 16;
		}
	}
	else if(rattack == 2)
	{
		if(rpawn == 2)
		{
			if(battack == 0)
			{
				if(bpromote <= 2)
				{
					mul[Red] = 16;
				}
				else
				{
					mul[Red] = 1;
				}
			}
			else if(battack == 1)
			{
				if(bpromote <= 2)
				{
					mul[Red] = 8;
				}
				else
				{
					mul[Red] = 1;
				}
			}
			else
			{
				if(bpromote <= 2)
				{
					mul[Red] = 4;
				}
				else
				{
					mul[Red] = 1;
				}
			}
		}
		else if(rpawn == 1)
		{
			if(rknight == 1)
			{
               if(badvisor > 1)
			   {
				   mul[Red] = 1;
			   }
			}
			else
			{
				mul[Red] = 16;
			}
		}
		else
		{
			mul[Red] = 16;
		}
	}
	else if(rattack >= 3)
	{
		mul[Red] = 16;
	}

	//黑方
	if(false){}
	else if(battack == 0)
	{
		mul[Black] = 0;
	}
	else if(battack == 1)
	{
		if(bpawn == 1)
		{
           if(rattack == 0)
		   {
			   if(rpromote == 0)
			   {
				   mul[Black] = 16;
			   }
			   else if(rpromote == 1)
			   {
				   if(radvisor == 1)
				   {
					    mul[Black] = 16;
				   }
				   else
				   {
					    mul[Black] = 0;
				   }
			   }
			   else
			   {
				    mul[Black] = 0;
			   }
		   }
		   else
		   {
			   mul[Black] = 0;
		   }
		}
		else if(bcannon == 1)
		{
			if(rattack == 0)
			{
				if(rpromote == 0)
				{
					if(badvisor > 0)
					{
                        mul[Black] = 16;
					}
					else
					{
						mul[Black] = 0;
					}
				}
				else if(rpromote == 1)
				{
					if(badvisor > 0)
					{
						mul[Black] = 16;
					}
					else
					{
						mul[Black] = 0;
					}
				}
				else if(rpromote == 2)
				{
                   if(radvisor > 1 ||rbishop > 1)
				   {
					   mul[Black] = 1;
				   }
				   else
				   {
                      mul[Black] = 16;
				   }
				}
				else
				{
					 mul[Black] = 1;
				}
			}
			else if(rattack >= 1)
			{
               mul[Black] = 1;
			}
		}
		else if(bknight == 1)
		{
			if(rattack == 0)
			{
				if(rpromote == 0)
				{
					mul[Black] = 16;
				}
				if(rpromote == 1)
				{
					if(radvisor == 1)
					{
						mul[Black] = 16;
					}
					else
					{
						mul[Black] = 8;
					}
				}
				else
				{
                    mul[Black] = 1;
				}
			}
			else if(battack >=1) 
			{
				mul[Black] = 1;
			}
		
		}
		else if(brook == 1)
		{
			mul[Black] = 16;
		}
	}
	else if(battack == 2)
	{
		if(bpawn == 2)
		{
			if(rattack == 0)
			{
				if(rpromote <= 2)
				{
					mul[Black] = 16;
				}
				else
				{
					mul[Black] = 1;
				}
			}
			else if(rattack == 1)
			{
				if(rpromote <= 2)
				{
					mul[Black] = 8;
				}
				else
				{
					mul[Black] = 1;
				}
			}
			else
			{
				if(rpromote <= 2)
				{
					mul[Black] = 4;
				}
				else
				{
					mul[Black] = 1;
				}
			}
		}
		else if(bpawn == 1)
		{
			if(bknight == 1)
			{
				if(radvisor > 1)
				{
					mul[Black] = 1;
				}
			}
			else
			{
				mul[Black] = 16;
			}
		}
		else
		{
			mul[Black] = 16;
		}
	}
	else if(battack >= 3)
	{
		mul[Black] = 16;
	}

	// 计算分值
	tpawn = rpawn + bpawn;

	phase -= rpawn * PawnPhase;
	phase -= rknight * KnightPhase;
	phase -= rcannon * CannonPhase;
	phase -= rbishop * BishopPhase;
	phase -= radvisor * AdvisorPhase;
	phase -= rrook * RookPhase;

	phase -= bpawn * PawnPhase;
	phase -= bknight * KnightPhase;
	phase -= bcannon * CannonPhase;
	phase -= bbishop * BishopPhase;
	phase -= radvisor * AdvisorPhase;
	phase -= rrook * RookPhase;

	if( phase < 0 )
		phase = 0;

	ASSERT(phase >= 0 && phase <= TotalPhase);
	phase = (phase * 256 + (TotalPhase / 2)) / TotalPhase;

	ASSERT(phase >= 0 && phase <= 256);

	opening = 0;
	endgame = 0;

	// material
	ropenf = rknight * KnightOpening + rcannon * CannonOpening + rbishop * BishopOpening + rrook * RookOpening;
	opening += ropenf;
	opening += rpawn * PawnOpening;

	bopenf = bknight * KnightOpening + bcannon * CannonOpening + bbishop * BishopOpening + brook * RookOpening;
	opening -= bopenf;
	opening -= bpawn * PawnOpening;

	rendf = rknight * KnightEndgame  + rcannon * CannonEndgame+ rbishop * BishopEndgame + rrook * RookEndgame;
	endgame += rpawn * PawnEndgame;
	endgame += rendf;

	bendf = bknight * KnightEndgame + bcannon * CannonEndgame + bbishop * BishopEndgame + brook * RookEndgame;
	endgame -= bpawn * PawnEndgame;
	endgame -= bendf;

	// Trade Bonus

	if( ropenf > bopenf && bpawn > rpawn )
	{
		opening += OpeningExchangePenalty;
		endgame += OpeningExchangePenalty;
	}
	else if( bopenf > ropenf && rpawn > bpawn )
	{
		opening -= OpeningExchangePenalty;
		endgame -= OpeningExchangePenalty;
	}


	opening = (opening * MaterialWeightOpening) / 256;
	endgame = (endgame * MaterialWeightEndgame) / 256;

	ASSERT(opening <= ValueEvalInf && opening >= -ValueEvalInf);
	ASSERT(endgame <= ValueEvalInf && endgame >= -ValueEvalInf);

	for ( colour = 0; colour < ColourNb; colour++ )
		info->mul[colour] = mul[colour];

	info->phase = phase;		
	info->opening = opening;
	info->endgame = endgame;
}
