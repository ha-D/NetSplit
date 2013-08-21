#include "proto/ip.h"

unsigned short checksum_ip(unsigned short *addr, int len){
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;

    while (nleft > 1){
      sum += *w++;
      nleft -= 2;
    }

    if (nleft == 1){
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);    
    sum += (sum >> 16);          
    answer = ~sum;            
    return (answer);
}

char* create_ip(struct iphdr** ip, struct ethhdr** eth, int* len){
	char* buf;
	struct iphdr* buf_ip;
	struct ethhdr* buf_eth;

	if(*len < sizeof(struct ethhdr) + sizeof(struct iphdr)){
		*len = sizeof(struct ethhdr) + sizeof(struct iphdr);
	}

	buf = create_eth(&buf_eth, len);
	buf_eth->h_proto = htons(ETH_P_IP);

	buf_ip = (struct iphdr*)(buf + sizeof(struct iphdr));

	buf_ip->ihl = 5;
	buf_ip->tot_len = htons(*len);
	buf_ip->ttl = 255;
	buf_ip->version = 4;

	if(ip != 0)
		(*ip) = buf_ip;
	if(eth != 0)
		(*eth) = buf_eth;

	return buf;
}