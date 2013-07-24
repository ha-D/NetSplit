#include "netdev.h"

int phys_init(){
	int sockfd;
	struct ifreq ifr;
	char* dev = "wlan0";

	sockfd = socket(PF_PACKET, SOCK_RAW, IPPROTO_RAW);
	if(sockfd < 0){
		perror("socket");
		exit(1);
	}

	DEBUG("phys_dev", ("socket created\n"));

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, dev);
	// snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0");
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	DEBUG("phys_dev", ("binded to physical device %s\n", dev));

	return sockfd;
}



