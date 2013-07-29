#ifndef _Z_BRIDGE_
#define _Z_BRIDGE_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "thread.h"
#include "log.h"
#include "netdev.h"
#include "packet.h"

void init_bridge(struct phys_dev* pfd, struct tun_dev* tfd);

#endif
