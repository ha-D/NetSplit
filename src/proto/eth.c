#include "proto/eth.h"

char* create_eth(struct ethhdr** eth, int* len){
	char* buf;

	if(*len < sizeof(struct ethhdr))
		*len = sizeof(struct ethhdr);

	buf = (char*) malloc(*len);
	memset(buf, 0, *len);

	if(eth != 0)
		*eth = (struct ethhdr*)buf;
	return buf;
}

char* create_eth_ip(struct ethhdr** eth, int* len){
	char* buf;
	struct ethhdr* buf_eth;

	buf = create_eth(&buf_eth, len);

	buf_eth->h_proto = htons(ETH_P_IP);

	if(eth != 0)
		(*eth) = buf_eth;
	
	return buf;
}