#include "bridge.h"


/*
 * Hash packet based on ip adresses and port numbers (for gum allocation)
*/ 
uint32_t hash_ip_packet(struct iphdr* ip){
	uint32_t ip_part =  (ip->saddr & 0x0ffffff) + (ip->daddr & 0x0ffffff);
	if(ip->protocol == IPPROTO_TCP){
		struct tcphdr* tcp = (struct tcphdr*)((char*)ip + sizeof(struct iphdr));
		return ip_part + tcp->dest + tcp->source;
	}else if(ip->protocol == IPPROTO_UDP){
		struct udphdr* udp = (struct udphdr*)((char*)ip + sizeof(struct iphdr));
		return ip_part + udp->dest + udp->source;
	}else{
		return ip_part;
	}
}

int phys_arp(char* buf, int len){
	int i;
	struct arp_hdr* arp;
	//struct ethhdr* eth;
	struct tun_dev* tund;
	struct phys_dev* physd;

	tund = get_tun();
	physd = get_phys();
	//eth = (struct ethhdr*)(buf);
	arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));
	
	if(arp->opcode == htons(ARP_REQUEST)){
		struct gumpck gum;
		if(gum_lookup(arp->target_ip, &gum) == 0){
			// Send arp reply
			char arpbuf[sizeof(struct ethhdr) + sizeof(struct arp_hdr) + 2];
			struct ethhdr* replyeth = (struct ethhdr*)arpbuf;
			struct arp_hdr* replyarp = (struct arp_hdr*)(arpbuf + sizeof(struct ethhdr));

			memcpy(arpbuf, buf, len);
			memcpy(replyeth->h_source, physd->mac, 6);
			memcpy(replyeth->h_dest, arp->sender_mac, 6);
			memcpy(replyarp->sender_mac, physd->mac, 6);
			memcpy(replyarp->target_mac, arp->sender_mac, 6);
			replyarp->target_ip = arp->sender_ip;
			replyarp->sender_ip = arp->target_ip;
			replyarp->opcode = htons(ARP_REPLY);

			send_to_phys(physd, arpbuf, len);
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
	ip->check = checksum_ip((unsigned short*)ip, ip->ihl * 4);
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
	//struct phys_dev* physd;
	struct gumpck* gum;
	uint32_t hash;

	//physd = get_phys();
	ip = (struct iphdr*)(buf + sizeof(struct ethhdr));
	
	hash = hash_ip_packet(ip);
	gum = get_assigned_gum(hash);

	// If packet was ip, set source ip to phys dev ip
	//ip->saddr = physd->ip_addr;

	ip->saddr = gum->ip;

	ip->check = 0;
	ip->check = checksum_ip((unsigned short*)ip, ip->ihl * 4);
	return 1;
}