#ifndef _Z_GUM_
#define _Z_GUM_

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "proto/dhcp.h"
#include "netdev.h"
//#include "bridge.h"

struct dhcphdr;

struct gumpck{
	uint32_t ip; /* Ip in network byte order */
	uint8_t mac[6];
	struct gumpck* next;
};

struct gumpck* gum_list();
int gum_lookup(uint32_t ip, struct gumpck* gum);
void gum_add(struct gumpck* gum);
void gum_remove(struct gumpck* gum);
void init_gums();
struct gumpck* get_assigned_gum(uint32_t hash);

/* DHCP Stuff */

struct dhcp_trans{
#define GUM_DHCP_STATE_DISCOVER 1
#define GUM_DHCP_STATE_REQUEST	2
#define GUM_DHCP_STATE_READY 	3
	uint8_t state;
	uint32_t xid;
	struct phys_dev* physd;
	struct dhcpoption* params_option;
	struct dhcp_trans* next;
};

void init_gum_dhcp();
void gum_dhcp_discover(struct phys_dev* physd);
void gum_dhcp_request(struct dhcp_trans* trans, struct dhcphdr* offer);
void gum_handle_dhcp();
#endif