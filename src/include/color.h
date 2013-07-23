#ifndef _Z_COL_
#define _Z_COL_

#define COL_RED  31
#define COL_GREEN 32
#define COL_BLUE  34
#define COL_DEF 39

#define CHANGECOL(col) printf("\033[%dm", col);
#endif