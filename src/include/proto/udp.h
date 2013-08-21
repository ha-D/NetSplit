#ifndef _Z_PROTO_UDP_
#define _Z_PROTO_UDP_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include <string.h>

#include "proto/ip.h"


char* create_udp(struct udphdr** udp, int* len);
#endif