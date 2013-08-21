#include "proto/dhcp.h"

void dhcp_add_option(struct dhcphdr** dhcp, struct dhcpoption* option, int* len){
	char* buf;

	buf = (char*)malloc(*len + option->size + 2);

	memcpy(buf, *dhcp, *len);
	memcpy(buf, option, option->size + 2);

	free(*dhcp);

	*dhcp = (struct dhcphdr*)buf;
	*len = *len + option->size + 2;
}

char* create_dhcp_discover(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len, struct dhcp_args* args){
	char* buf;
	struct ethhdr* buf_eth;
	struct iphdr* buf_ip;
	struct udphdr* buf_udp;
	struct dhcphdr* buf_dhcp;
	struct dhcpoption_53 option_53;
	struct dhcpoption* option_param;
	struct dhcpoption_255 option_255;
	

	if(*len < sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct dhcphdr))
		*len  = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct dhcphdr);
	buf = create_udp(&buf_udp, &buf_ip, &buf_eth, len);


	buf_dhcp->op = 0x01;
	buf_dhcp->htype = 0x01;
	buf_dhcp->hlen = 0x06;
	buf_dhcp->hops = 0x00;

	buf_dhcp->xid = rand();
	buf_dhcp->secs = 0x00;
	buf_dhcp->flags = 0x00;

	buf_dhcp->client_addr  = 0x0;
	buf_dhcp->your_addr    = 0x0;
	buf_dhcp->server_addr  = 0x0;
	buf_dhcp->gateway_addr = 0x0;

	memcpy(buf_dhcp->client_haddr, args->mac, 6);

	buf_dhcp->magic_cookie = 0x63825383;

	/* Options */
	option_53.opt_num = 53;
	option_53.size = 1;
	option_53.dhcp = 3;
	dhcp_add_option(&buf_dhcp, (struct dhcpoption*)&option_53, len);

	// Requested Ip
	if(args->has_req_ip){
		struct dhcpoption_50 option_50;
		option_50.opt_num = 50;
		option_50.size = 4;
		option_50.req_ip = args->req_ip;
		dhcp_add_option(&buf_dhcp, (struct dhcpoption*)&option_50, len);
	}

	/* Params */
	option_param = (struct dhcpoption*) malloc(sizeof(struct dhcpoption) + 7);
	option_param->opt_num = 55;
	option_param->size = 7;
	*((char*)option_param + 1) = (char)DHCP_PARAM_SUBNETMASK;
	*((char*)option_param + 2) = (char)DHCP_PARAM_BROADCAST;
	*((char*)option_param + 3) = (char)DHCP_PARAM_TIMEOFFSET;
	*((char*)option_param + 4) = (char)DHCP_PARAM_ROUTER;
	*((char*)option_param + 5) = (char)DHCP_PARAM_DOMAIN;
	*((char*)option_param + 6) = (char)DHCP_PARAM_DNS;
	*((char*)option_param + 7) = (char)DHCP_PARAM_HOST;
	dhcp_add_option(&buf_dhcp, option_param, len);
	free(option_param);

	option_255.opt_num = 255;
	option_255.size = 0;
	dhcp_add_option(&buf_dhcp, (struct dhcpoption*)&option_255, len);

	/* End Options */

	/* Lower Protocols */

	memset(buf_eth->h_dest, 0xff, 6);
	memcpy(buf_eth->h_source, args->mac, 6);

	buf_ip->saddr = 0;
	buf_ip->daddr = 0xffffffff;

	buf_udp->source = htons(68);
	buf_udp->dest = htons(67);


	/* Args */
	if(dhcp != 0)
		*dhcp = buf_dhcp;
	if(udp != 0)
		*udp = buf_udp;
	if(ip != 0)
		*ip = buf_ip;
	if(eth != 0)
		*eth = buf_eth;
	
	return buf;
}