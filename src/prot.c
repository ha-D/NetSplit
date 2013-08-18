#include "bridge.h"

unsigned short checksum(unsigned short *addr, int len)
{
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

int phys_arp(char* buf, int len){
	int i;
	struct arp_hdr* arp;
	struct ethhdr* eth;
	struct tun_dev* tund;

	tund = get_tun();
	eth = (struct ethhdr*)(buf);
	arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));
	
	if(arp->opcode == htons(ARP_REQUEST)){
		struct gumpck gum;
		if(gum_lookup(arp->target_ip, &gum) == 0){
			// Send arp reply
			char arpbuf[sizeof(struct ethhdr) + sizeof(struct arp_hdr) + 2];
			struct ethhdr* replyeth = (struct ethhdr*)arpbuf;
			struct arp_hdr* replyarp = (struct arp_hdr*)(arpbuf + sizeof(struct ethhdr));

			memcpy(arpbuf, buf, len);
			memcpy(replyeth->h_source, eth->h_dest, 6);
			memcpy(replyeth->h_dest, eth->h_source, 6);
			memcpy(replyarp->sender_mac, arp->target_mac, 6);
			memcpy(replyarp->target_mac, arp->sender_mac, 6);
			replyarp->target_ip = arp->sender_ip;
			replyarp->sender_ip = arp->target_ip;
			replyarp->opcode = htons(ARP_REPLY);

			send_to_phys(arpbuf, len);
		}
		// arp request shouldn't be forwarded to tun
		return 0;
	}else if(arp->opcode == htons(ARP_REPLY)){
		// Set arp target to tun physical address
		for (i = 0; i < 6; i++)
			arp->target_mac[i] = tund->mac[i];
		arp->target_ip = tund->ip_addr;	
	}
	return 1;
}

int phys_ip(char* buf, int len){
	struct iphdr* ip;
	struct tun_dev* tund;

	tund = get_tun();
	ip = (struct iphdr*)(buf + sizeof(struct ethhdr));

	// If packet was ip set ip destination to tun address
	ip->daddr = tund->ip_addr;

	ip->check = 0;
	ip->check = checksum((unsigned short*)ip, ip->ihl * 4);
	return 1;
}

int tun_arp(char* buf, int len){
	int i;
	struct arp_hdr* arp;
	struct phys_dev* physd;

	physd = get_phys();
	arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));

	// If packet was arp set sender mac in packet to physical device
	for (i = 0; i < 6; i++)
		arp->sender_mac[i] = physd->mac[i];
	arp->sender_ip = physd->ip_addr;	

	return 1;
}

int tun_ip(char* buf, int len){
	struct iphdr* ip;
	struct phys_dev* physd;

	physd = get_phys();
	ip = (struct iphdr*)(buf + sizeof(struct ethhdr));
	
	// If packet was ip, set source ip to phys dev ip
	ip->saddr = physd->ip_addr;

	ip->check = 0;
	ip->check = checksum((unsigned short*)ip, ip->ihl * 4);
	return 1;
}