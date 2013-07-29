#include "packet.h"

void print_bytes(char* buff, int len){
    uint8_t* buf = (uint8_t*)buff;
	int i;
	printf("Bytes:\n");
	for(i = 0; i < len; i ++){
		printf("%02x ", buf[i]);
	}
	printf("\n------------------------------------------\n");
}

void print_tcp(char* buf, int print_rest){
    struct tcphdr* tcp;
    tcp = (struct tcphdr*)buf;

    printf("TCP: \n");
    printf("\tSource Port:      %d\n", ntohs(tcp->source));
    printf("\tDestination Port: %d\n", ntohs(tcp->dest));
    printf("\tSequence:         %d\n", ntohl(tcp->seq));
    printf("\tAck Sequence:     %d\n", ntohl(tcp->ack_seq));
    printf("\tWindow Size:      %d\n", ntohs(tcp->window));
    printf("\tFlags:              \n\t\t");
    if(tcp->syn)
        printf("SYN ");
    if(tcp->ack)
        printf("ACK ");
    if(tcp->fin)
        printf("FIN ");
    if(tcp->rst)
        printf("RST ");
    if(tcp->psh)
        printf("PSH ");
    if(tcp->res1)
        printf("RESL ");
    printf("\n");
}

void print_ip(char* buf, int print_payload){
    struct iphdr* ip;
    struct in_addr src, dst;

    ip = (struct iphdr*)buf;
    src.s_addr = ip->saddr;
    dst.s_addr = ip->daddr;

    printf("------------------------------------------\n");
    printf("IP: \n");
    printf("\tSource Addr:      %s\n", inet_ntoa(src));
    printf("\tDestination Addr: %s\n", inet_ntoa(dst));
    printf("\tHeader Length:    %d\n", ip->ihl);
    printf("\tTotal Length:     %d\n", ntohs(ip->tot_len));
    printf("\tProtocol:         %d\n", ip->protocol);

    if(print_payload){
        printf("\n");
        if(ip->protocol == IPPROTO_TCP){
            print_tcp(buf + sizeof(struct iphdr), print_payload);
        }
    }

    printf("------------------------------------------\n");
}

void print_arp(char* buf, int print_payload){
    struct arp_hdr* arp;
    struct in_addr src, dst;
    unsigned char* m;

    arp = (struct arp_hdr*)buf;
    src.s_addr = (in_addr_t)arp->sender_ip;
    dst.s_addr = (in_addr_t)arp->target_ip;

    printf("------------------------------------------\n");
    printf("ARP: \n");
    printf("\tSender  IP:      %s\n", inet_ntoa(src));
    m = arp->sender_mac;
    printf("\tSender Mac:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    printf("\tTarget  IP:      %s\n", inet_ntoa(dst));
    m = arp->target_mac;
    printf("\tTarget Mac:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    if(arp->opcode == 0){
		printf("\tOP:              Reply\n");	
    }else{
    	printf("\tOP:              Request\n");	
    }
    printf("------------------------------------------\n");
}

void print_eth(char* buf, int print_payload){
    struct ethhdr* eth;
	unsigned char* m;
    eth = (struct ethhdr*)buf;
    // printf("------------------------------------------\n");
    printf("Eth: \n");
    m = eth->h_source;
    printf("\tSource Addr:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    m = eth->h_dest;
    printf("\tDestination Addr: %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    if(ntohs(eth->h_proto) == ETH_P_IP)
    	printf("\tProtocol:         IP\n");
    else if(ntohs(eth->h_proto) == ETH_P_ARP)
    	printf("\tProtocol:         ARP\n");
    else
    	printf("\tProtocol:         %d\n", eth->h_proto);

    if(print_payload){
        printf("\n");
        if(eth->h_proto == htons(ETH_P_IP)){
            print_ip(buf + sizeof(struct ethhdr), print_payload);
        }else if(eth->h_proto == htons(ETH_P_ARP)){
        	print_arp(buf + sizeof(struct ethhdr), print_payload);
        }
    }

    printf("------------------------------------------\n");
}
