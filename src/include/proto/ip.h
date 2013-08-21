#ifndef _Z_PROTO_IP_
#define _Z_PROTO_IP_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include <string.h>

#include "proto/eth.h"


unsigned short checksum_ip(unsigned short *addr, int len);
char* create_ip(struct iphdr** ip, int* len);

#endif