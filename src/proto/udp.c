#include "proto/udp.h"

char* create_udp(struct udphdr** udp, int* len){
	char* buf;
	struct udphdr* buf_udp;
	struct iphdr* buf_ip;

	if(*len < sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr))
		*len = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

	buf = create_ip(&buf_ip, len);
	buf_ip->protocol = IPPROTO_UDP;

	buf_udp = (struct udphdr*)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr));

	if(udp != 0)
		*udp = buf_udp;

	return buf;
}