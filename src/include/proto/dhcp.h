#ifndef _Z_DHCP_
#define _Z_DHCP_

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>


#include "log.h"
#include "netdev.h"
#include "proto/udp.h"

#define DHCP_MIN_SIZE 300
struct dhcphdr{
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t client_addr;
	uint32_t your_addr;
	uint32_t server_addr;
	uint32_t gateway_addr;
	uint8_t  client_haddr[6];
	uint8_t  padding[10];
	uint32_t magic_cookie;
}__attribute__ ((packed));

struct dhcp_args{
	int option_offset; // Changes
	uint8_t mac[6];
	uint8_t has_req_ip;
	uint32_t req_ip;
	uint32_t xid;
	uint32_t server_addr;
}__attribute__ ((packed));

/* Options */

#define DHCP_PARAM_SUBNETMASK 1
#define DHCP_PARAM_TIMEOFFSET 2
#define DHCP_PARAM_ROUTER 3
#define DHCP_PARAM_DNS 6
#define DHCP_PARAM_HOST 12
#define DHCP_PARAM_DOMAIN 15
#define DHCP_PARAM_BROADCAST 28


struct dhcpoption{
#define DHCP_OPTION_TYPE 53
	uint8_t opt_num;
	uint8_t size;
	uint8_t padding[4];
}__attribute__ ((packed));

struct dhcpoption_53{	// DHCP TYPE 
	uint8_t opt_num;
	uint8_t size;
#define DHCP_TYPE_DISCOVER 1
#define DHCP_TYPE_OFFER    2
#define DHCP_TYPE_REQUEST  3
#define DHCP_TYPE_ACK 	   5
	uint8_t dhcp;
}__attribute__ ((packed));;

struct dhcpoption_50{  // REQ IP ADDRESS
	uint8_t opt_num;
	uint8_t size;
	uint32_t req_ip;
}__attribute__ ((packed));

struct dhcpoption_54{  // Server Identifier
	uint8_t opt_num;
	uint8_t size;
	uint32_t server;
}__attribute__ ((packed));

struct dhcpoption_255{  // End
	uint8_t opt_num;
	uint8_t size;
}__attribute__ ((packed));;

void dhcp_add_option(char** buf, struct dhcpoption* option, int* offset, int* len);
int dhcp_read_option(char* buf, struct dhcpoption* option, int* offset, int len);

char* create_dhcp_discover(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len, struct dhcp_args* args);
char* create_dhcp_request(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len, struct dhcp_args* args);
#endif