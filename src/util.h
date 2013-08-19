#ifndef UTIL_H
#define UTIL_H

// includes
#include <cstdio>
#include "types.h"


struct my_timer_t
{
	double start_real;
	double start_cpu;
	double elapsed_real;
	double elapsed_cpu;
	bool running;
};
// functions

extern void util_init();
extern void my_random_init();
extern int my_random( int n );
extern sint64 my_atoll( const char string [] );
extern int my_round( double x );
extern void *my_malloc( int size );
extern void my_free( void *address );
extern void my_fatal( const char format [], ... );
extern bool my_file_read_line( FILE *file, char string [], int size );
extern bool my_string_empty( const char string [] );
extern bool my_string_equal( const char string_1 [], const char string_2 [] );
extern char *my_strdup( const char string [] );
extern void my_string_clear( const char ** variable );
extern void my_string_set( const char ** variable, const char string [] );
extern void my_timer_reset( my_timer_t *timer );
extern void my_timer_start( my_timer_t *timer );
extern void my_timer_stop( my_timer_t *timer );
extern double my_timer_elapsed_real( const my_timer_t *timer );
extern double my_timer_elapsed_cpu( const my_timer_t *timer );
extern double my_timer_cpu_usage( const my_timer_t *timer );
extern void show_version();

#endif // !defined UTIL_H

// end of util.h