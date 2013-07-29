#include "bridge.h"

#define BUFLEN 4096

struct phys_dev* physd;
struct tun_dev* tund;

void send_to_tun(char* buf, int read_len){
	int i;
	char* m;
	struct ethhdr* eth= (struct ethhdr*)buf;

	m = (char*)tund->mac.ifr_hwaddr.sa_data;		
	for (i = 0; i < 6; i++)
		eth->h_dest[i] = m[i];

	// Set source to physical device to know if it has been forwarded from here in read tun
	m = (char*)physd->mac.ifr_hwaddr.sa_data;		
	for (i = 0; i < 6; i++)
		eth->h_source[i] = m[i];

	// If pcaket was arp set arp target to tun physical address
	if(eth->h_proto == htons(ETH_P_ARP)){
		struct arp_hdr* arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));
		for (i = 0; i < 6; i++)
			arp->target_mac[i] = ((uint8_t *)&tund->mac.ifr_hwaddr.sa_data)[i];
	}

	read_len = write(tund->sockfd, buf, read_len);
	if(read_len < 0){
		perror("phys_thread send");
		exit(1);
	}
}


int check_phys_packet(struct ethhdr* eth){
	uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	// If ethernet source was physical device, drop packet (Huh? when does this happen?)
	if(memcmp(eth->h_source, physd->mac.ifr_hwaddr.sa_data , 6) == 0){
		return 0;
	}

	// If ethernet destination was not physical device address and was not broadcast, drop packet
	if(memcmp(eth->h_dest, physd->mac.ifr_hwaddr.sa_data , 6) != 0 &&
		memcmp(eth->h_dest, broadcast, 6) != 0)
		return 0;

	return 1;
}

int is_ip(char* buf){
	/* !IMPORTANT Applications seem to recieve IP packets on physical device by default
		probably gonna have to solve this when IP Spoofing comes into action           */
	struct ethhdr* eth;
	eth = (struct ethhdr*)buf;
	return eth->h_proto == htons(ETH_P_IP);
}

void listen_phys(){
	char buf[BUFLEN];
	int read_len;

	DEBUG("bridge", ("phys to tun bridge started\n"));
	while(1){
		read_len = recvfrom(physd->sockfd, buf, BUFLEN-1, 0, NULL, NULL);

		if(read_len < 0){
			perror("phys_thread recv");
			exit(1);
		}

		if(check_phys_packet((struct ethhdr *)(buf))){
			DEBUG("bridge", ("received %d bytes on phys\n", read_len));

			// Drop Ip packets
			if(!is_ip(buf))
				send_to_tun(buf, read_len);
		}		
	}	
}

int check_tun_packet(struct ethhdr* eth){
	// uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	// struct iphdr* ip;

	/* If ethernet source was physical device drop packet (Don't think this is needed) */

	// if(memcmp(eth->h_source, physd->mac.ifr_hwaddr.sa_data , 6) == 0){
	// 	return 0;
	// }

	/* Only allow ICMP packets */

	// ip = (struct iphdr*)((char*)eth + sizeof(struct ethhdr));
	// if(eth->h_proto == htons(ETH_P_IP) && ip->protocol != IPPROTO_ICMP){
	// 	// printf("NOT ICMP: %d  %d\n", ip->protocol, IPPROTO_ICMP);
	// 	return 0;
	// }

	return 1;
}

void send_to_phys(char* buf, int read_len){
	int i;
	struct sockaddr_ll socket_address;
	char* m;

	socket_address.sll_ifindex = physd->if_id.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;
	struct ethhdr* eth= (struct ethhdr*)buf;

	// Take gateway mac address from ethernet packet
	for (i = 0; i < 6; i++)
		socket_address.sll_addr[i] = eth->h_dest[i];

	// Set ethernet source address to physical device address
	m = (char*)physd->mac.ifr_hwaddr.sa_data;		
	for (i = 0; i < 6; i++)
		eth->h_source[i] = m[i];

	// If packet was arp set sender mac in packet to physical device
	if(eth->h_proto == htons(ETH_P_ARP)){
		struct arp_hdr* arp = (struct arp_hdr*)(buf + sizeof(struct ethhdr));
		for (i = 0; i < 6; i++)
			arp->sender_mac[i] = ((uint8_t *)&physd->mac.ifr_hwaddr.sa_data)[i];
	}

	if (sendto(physd->sockfd, buf, read_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0){
		perror("tun_thread send");
		exit(1);
	}
}

void listen_tun(){
	char buf[BUFLEN];
	int read_len;

	DEBUG("bridge", ("tun to phys bridge started\n"));

	while(1){
		read_len = read(tund->sockfd, buf, BUFLEN-1);
		if(read_len < 0){
			perror("tun_thread recv");
			exit(1);
		}

		if(check_tun_packet((struct ethhdr*)buf) == 0){
			continue;
		}

		DEBUG("bridge", ("received %d bytes on tun\n", read_len));

		send_to_phys(buf, read_len);
	}
}

void init_bridge(struct phys_dev* pfd, struct tun_dev* tfd){
	int x;
	physd = pfd;
	tund = tfd;
	printf("\nPress a key to start interface bridging...\n");
	scanf("%d", &x);

	thread_init(listen_phys);
	thread_init(listen_tun);
}

