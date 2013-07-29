#include "netdev.h"

struct phys_dev* phys_init(){
	struct phys_dev* physd;
	struct ifreq ifr;
	// struct ifreq  ifopts;
	char* dev = "wlan0";
	char* m;

	physd = (struct phys_dev*)malloc(sizeof(struct phys_dev));
	physd->sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(physd->sockfd < 0){
		perror("socket");
		exit(1);
	}

	DEBUG("phys_dev", ("socket created\n"));

	// // // Set to promicsous mode
	// strncpy(ifopts.ifr_name, dev, IFNAMSIZ-1);
	// ioctl(physd->sockfd, SIOCGIFFLAGS, &ifopts);
	// ifopts.ifr_flags &= ~IFF_PROMISC;
	// ioctl(physd->sockfd, SIOCSIFFLAGS, &ifopts);

	// Bind to device
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, dev);
	// snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0");
	if (setsockopt(physd->sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
		perror("phys_init setsockopt");
		exit(1);
	}

	// Get the index of the interface to send on
	memset(&physd->if_id, 0, sizeof(struct ifreq));
	strncpy(physd->if_id.ifr_name, dev, IFNAMSIZ-1);
	if (ioctl(physd->sockfd, SIOCGIFINDEX, &physd->if_id) < 0){
    	perror("phys_init SIOCGIFINDEX");
    	exit(1);
    }

	memset(&physd->mac, 0, sizeof(struct ifreq));
	strncpy(physd->mac.ifr_name, dev, IFNAMSIZ-1);
	if (ioctl(physd->sockfd, SIOCGIFHWADDR, &physd->mac) < 0){
	    perror("phys_init SIOCGIFHWADDR");
		exit(1);
	}
	
	 // m = (char*)physd->mac.ifr_hwaddr.sa_data;
	// printf("Physical Device MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);

	// DEBUG("phys_dev", ("binded to physical device %s\n", dev));

	return physd;
}



