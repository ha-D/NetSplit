#include "bridge.h"

#define BUFLEN 4096
// #define TUNPROM 1

struct phys_dev* physd;
struct tun_dev* tund;

void send_to_tun(char* buf, int len){
	len = write(tund->sockfd, buf, len);
	if(len < 0){
		perror("phys_thread send");
		exit(1);
	}
}

int is_ip(char* buf){
	/* !IMPORTANT Applications seem to recieve IP packets on physical device by default
		probably gonna have to solve this when IP Spoofing comes into action           */
	/* Solved by not giving ip to physical device */
	struct ethhdr* eth;
	eth = (struct ethhdr*)buf;
	return eth->h_proto == htons(ETH_P_IP);
}

void phys_to_tun(char* buf, int read_len){
	int i, send;
	char* m;
	struct ethhdr* eth;
	int PHYSPROM;

	eth = (struct ethhdr*)buf;

	// if(eth->h_proto == htons(ETH_P_ARP))
	// 	PHYSPROM = 1;
	// else
	PHYSPROM = 0;
	
	IFPROMPT(PHYSPROM, ("phys packet recieved, print? y/n\n"), 'y'){
		print_eth(buf, 1);
	}

	// Drop Ip packets
	// if(!is_ip(buf))
	//    return;
	m = (char*)tund->mac;		
	for (i = 0; i < 6; i++)
		eth->h_dest[i] = m[i];

	// Set source to physical device to know if it has been forwarded from here in read tun
	m = (char*)physd->mac;		
	for (i = 0; i < 6; i++)
		eth->h_source[i] = m[i];

	send = 1;
	switch(ntohs(eth->h_proto)){
		case ETH_P_ARP:
			send = phys_arp(buf, read_len);
			break;
		case ETH_P_IP:
			send = phys_ip(buf, read_len);
			break;
	}

	IFPROMPT(PHYSPROM, ("print altered phys packet? y/n\n"), 'y'){
		print_eth(buf, 1);
	}

	IFPROMPT(PHYSPROM, ("send to tun? y/n\n"), 'n'){
	}else{
		if(send)
			send_to_tun(buf, read_len);
	}
}

int check_phys_packet(struct ethhdr* eth){
	uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	// If ethernet source was physical device, drop packet (Huh? when does this happen?)
	if(memcmp(eth->h_source, physd->mac, 6) == 0){
		return 0;
	}

	// If ethernet destination was not physical device address and was not broadcast, drop packet
	if(memcmp(eth->h_dest, physd->mac, 6) != 0 &&
		memcmp(eth->h_dest, broadcast, 6) != 0)
		return 0;

	return 1;
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
			phys_to_tun(buf, read_len);
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

void send_to_phys(char* buf, int len){
	struct sockaddr_ll socket_address;
	struct ethhdr* eth;
	int i;

	eth= (struct ethhdr*)buf;
	socket_address.sll_ifindex = physd->if_id.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;


	// Take gateway mac address from ethernet packet
	for (i = 0; i < 6; i++)	
		socket_address.sll_addr[i] = eth->h_dest[i];

	if (sendto(physd->sockfd, buf, len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0){
		perror("tun_thread send");
		exit(1);
	}
}

void tun_to_phys(char* buf, int read_len){
	int i, send;
	// struct sockaddr_ll socket_address;
	char* m;
	struct ethhdr* eth;
	int TUNPROM;
	eth = (struct ethhdr*)buf;

	// if(eth->h_proto == htons(ETH_P_IP))
	// 	TUNPROM = 1;
	// else
		TUNPROM = 0;

	IFPROMPT(TUNPROM, ("tun packet recieved, print? y/n\n"), 'y'){
		print_eth(buf, 1);
	}

	// Set ethernet source address to physical device address
	m = (char*)physd->mac;		
	for (i = 0; i < 6; i++)
		eth->h_source[i] = m[i];


	send = 1;
	switch(ntohs(eth->h_proto)){
		case ETH_P_ARP:
			send = tun_arp(buf, read_len);
			break;
		case ETH_P_IP:
			send = tun_ip(buf, read_len);
			break;
	}

	IFPROMPT(TUNPROM, ("print altered tun packet? y/n\n"), 'y'){
		print_eth(buf, 1);
	}

	IFPROMPT(TUNPROM, ("send to phys? y/n\n"), 'n'){
	}else{
		if(send)
			send_to_phys(buf, read_len);
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

		tun_to_phys(buf, read_len);
	}
}

struct phys_dev* get_phys(){
	return physd;
}

struct tun_dev* get_tun(){
	return tund;
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

