#include<stdio.h>
#include<stdio.h>
#include "attack.h"
#include "trans.h"
#include "hash.h"
#include "init.h"
#include "move_do.h"
#include "option.h"
#include "piece.h"
#include "protocol.h"
#include "random.h"
#include "search.h"
#include "square.h"
#include "util.h"
#include "value.h"
#include "data.h"
#include "pst.h"
#include "fen.h"
#include "list.h"
#include "move_gen.h"
#include "move.h"
#include "movetable.h"
#include "book.h"
int main()
{

	init_movetable();
	value_init();
	random_init();

	util_init();
	my_random_init();
	show_version();
	option_init();

	trans_init(Trans);
	read_ini_file("CGChess.cfg");
	trans_parameter();
	book_init();
	book_parameter();
	//bitbase_parameter();
	threads_init();

	init();

	return 0;
}