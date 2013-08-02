#include "bridge.h"

unsigned short checksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;

    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }

    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);       /* add hi 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}

int phys_arp(char* buf, int len){
	int i;
	struct arp_hdr* arp;
	struct tun_dev* tund;

	tund = get_tun();
	arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));
	
	// If packet was arp set arp target to tun physical address
	for (i = 0; i < 6; i++)
		arp->target_mac[i] = ((uint8_t *)&tund->mac.ifr_hwaddr.sa_data)[i];
	memcpy(arp->target_ip, (uint8_t*)&tund->ip_addr, 4);	
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
		arp->sender_mac[i] = ((uint8_t *)&physd->mac.ifr_hwaddr.sa_data)[i];
	memcpy(arp->sender_ip, (uint8_t*)&physd->ip_addr, 4);	

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