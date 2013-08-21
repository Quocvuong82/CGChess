#ifndef COLOUR_H
#define COLOUR_H

const int ColourNone = -1;
const int Red        = 0;
const int Black      = 1;
const int ColourNb   = 2;

#define COLOUR_IS_OK(colour)  (((colour)&~1)==0)
#define COLOUR_IS_RED(colour)   ((colour)==Red)
#define COLOUR_IS_BLACK(colour) ((colour)!=Red)
#define COLOUR_OPP(colour)      ((colour)^(Red^Black))

#endif