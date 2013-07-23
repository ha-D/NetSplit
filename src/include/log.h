#ifndef _Z_LOG_
#define _Z_LOG_

#include "color.h"
#include <stdio.h>

#define LOGD(message) CHANGECOL(COL_BLUE) printf("log: "); CHANGECOL(COL_DEF) printf message;
#define LOG(title, message) CHANGECOL(COL_BLUE) printf("%s: ", title); CHANGECOL(COL_DEF) printf message;

#define DEBUGD(message) CHANGECOL(COL_GREEN) printf("debug: "); CHANGECOL(COL_DEF) printf message ;
#define DEBUG(title, message) CHANGECOL(COL_GREEN) printf("%s: ", title); CHANGECOL(COL_DEF) printf message;


#endif