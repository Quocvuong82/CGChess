// protocol.h

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define ENGINE "GCChess"
#define VERSION "1.0.0 w32"
#define AUTHOR "grefen"

// includes

#include "util.h"

// variables

// functions

extern void init();
extern void event();

extern void get( char string [], int size );
extern void send( const char format [], ... );

#endif // !defined PROTOCOL_H

// end of protocol.h
