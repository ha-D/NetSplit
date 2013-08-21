#include "proto/udp.h"

char* create_udp(struct udphdr** udp, struct iphdr** ip, struct ethhdr** eth, int* len){
	char* buf;
	struct udphdr* buf_udp;
	struct iphdr* buf_ip;
	struct ethhdr* buf_eth;

	if(*len < sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr))
		*len = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

	buf = create_ip(&buf_ip, &buf_eth, len);
	buf_ip->protocol = IPPROTO_UDP;

	buf_udp = (struct udphdr*)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr));

	if(udp != 0)
		*udp = buf_udp;
	if(ip != 0)
		*ip = buf_ip;
	if(eth != 0)
		*eth = buf_eth;

	return buf;
}
