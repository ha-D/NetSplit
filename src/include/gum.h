#ifndef _Z_GUM_
#define _Z_GUM_

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "proto/dhcp.h"
//#include "bridge.h"

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

void gum_request_dhcp();
#endif