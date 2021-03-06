#ifndef _Z_BRIDGE_
#define _Z_BRIDGE_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "thread.h"
#include "log.h"
#include "netdev.h"
#include "packet.h"
#include "gum.h"
#include "proto/ip.h"

void init_bridge(struct phys_dev* pfd, struct tun_dev* tfd);

struct phys_dev* get_phys();
struct tun_dev* get_tun();

int phys_arp(char* buf, int len);
int phys_ip(char* buf, int len);

int tun_arp(char* buf, int len);
int tun_ip(char* buf, int len);

#endif
