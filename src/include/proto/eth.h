#ifndef _Z_PROTO_ETH_
#define _Z_PROTO_ETH_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include <string.h>


char* create_eth(struct ethhdr** eth, int* len);

char* create_eth_ip(struct ethhdr** eth, int *len);
#endif