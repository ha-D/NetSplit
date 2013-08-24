#ifndef _Z_LOG_
#define _Z_LOG_

#include "color.h"
#include <stdio.h>

#define DEBUG_ON 0

#define LOGD(message) CHANGECOL(COL_BLUE) printf("log: "); CHANGECOL(COL_DEF) printf message;
#define LOG(title, message) CHANGECOL(COL_BLUE) printf("%s: ", title); CHANGECOL(COL_DEF) printf message;

#define DEBUGD(message) CHANGECOL(COL_GREEN) printf("debug: "); CHANGECOL(COL_DEF) printf message ;
#define DEBUG(title, message) CHANGECOL(COL_GREEN) printf("%s: ", title); CHANGECOL(COL_DEF) printf message;

char c;
#define PROMPT(prompt, message) if(prompt){CHANGECOL(COL_RED) printf message; CHANGECOL(COL_DEF) scanf("%c", &c);}

#define IFPROMPT(prompt, message, val) if(prompt){CHANGECOL(COL_RED) printf message; CHANGECOL(COL_DEF) do{scanf("%c", &c);}while(c!='y' && c!='n');} if(prompt && c == val)

#endif