#include "proto/dhcp.h"

void dhcp_add_option(char** buf, struct dhcpoption* option, int* offset, int* len){
	char* new_buf;
	if(*len < *offset + option->size + 2){
		new_buf = (char*)malloc(*offset + option->size + 2);
		memcpy(new_buf, *buf, *len);
		*len = *offset + option->size + 2;
		free(*buf);

		*buf = new_buf;
	}

	memcpy(*buf + *offset, option, option->size + 2);
	*offset = *offset + option->size + 2;
}

int dhcp_read_option(char* buf, struct dhcpoption* option, int* offset, int len){
	if(*offset >= len)
		return -1;
	
	option->opt_num = buf[*offset++];
	option->size    = buf[*offset++];
	memcpy((char*)option + 2, buf + *offset, option->size);
	*offset += option->size;

	return 0;
}

char* create_dhcp(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len){
	char* buf;	
	struct ethhdr* buf_eth;
	struct iphdr* buf_ip;
	struct udphdr* buf_udp;
	struct dhcphdr* buf_dhcp;
	

	if(*len < sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + DHCP_MIN_SIZE)
		*len  = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + DHCP_MIN_SIZE;
	buf = create_udp(&buf_udp, &buf_ip, &buf_eth, len);
	buf_dhcp = (struct dhcphdr*)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr));

	buf_dhcp->op = 0x01;
	buf_dhcp->htype = 0x01;
	buf_dhcp->hlen = 0x06;
	buf_dhcp->hops = 0x00;

	//buf_dhcp->xid = rand();
	buf_dhcp->secs = 0x00;
	buf_dhcp->flags = 0x00;

	buf_dhcp->client_addr  = 0x0;
	buf_dhcp->your_addr    = 0x0;
	buf_dhcp->server_addr  = 0x0;
	buf_dhcp->gateway_addr = 0x0;



	buf_dhcp->magic_cookie = 0x63825383;


	/* Lower Protocols */

	memset(buf_eth->h_dest, 0xff, 6);


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

char* create_dhcp_discover(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len, struct dhcp_args* args){
	char* buf;
	struct dhcphdr* buf_dhcp;
	struct ethhdr* buf_eth;
	struct dhcpoption_53 type_option;

	buf = create_dhcp(&buf_dhcp, udp, ip, &buf_eth, len);

	buf_dhcp->xid = rand();
	memcpy(buf_dhcp->client_haddr, args->mac, 6);
	memcpy(buf_eth->h_source, args->mac, 6);	

	type_option.opt_num = 53;
	type_option.size = 1;
	type_option.dhcp = DHCP_TYPE_DISCOVER;
	dhcp_add_option(&buf, (struct dhcpoption*)&type_option, &args->option_offset, len);	

	if(dhcp != 0)
		*dhcp = buf_dhcp;
	if(eth != 0)
		*eth = buf_eth;

	return buf;
}

char* create_dhcp_request(struct dhcphdr** dhcp, struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len, struct dhcp_args* args){
	char* buf;
	struct dhcphdr* buf_dhcp;
	struct ethhdr* buf_eth;
	struct dhcpoption_53 type_option;

	buf = create_dhcp(&buf_dhcp, udp, ip, &buf_eth, len);

	buf_dhcp->xid = args->xid;
	buf_dhcp->server_addr = args->server_addr;
	memcpy(buf_dhcp->client_haddr, args->mac, 6);
	memcpy(buf_eth->h_source, args->mac, 6);	

	type_option.opt_num = 53;
	type_option.size = 1;
	type_option.dhcp = DHCP_TYPE_REQUEST;
	dhcp_add_option(&buf, (struct dhcpoption*)&type_option, &args->option_offset, len);	

	if(dhcp != 0)
		*dhcp = buf_dhcp;
	if(eth != 0)
		*eth = buf_eth;

	return buf;
}