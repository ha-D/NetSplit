#ifndef _Z_BRIDGE_
#define _Z_BRIDGE_

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread.h"
#include "log.h"
#include "unistd.h"
#include "errno.h"

void init_bridge(int physfd, int tunfd);

#endif