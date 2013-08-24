#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "netdev.h"
#include "bridge.h"
#include "gum.h"


char dev[256];
int main(int argc, char const *argv[]){
	struct phys_dev* physd;
	struct tun_dev* tund;
	// Init gums before devices (for gum adding and map initializing)
	init_gums();

	tund = tun_init();
	physd = phys_init();

	init_bridge(physd, tund);
	
	printf("Enter a key to send dhcp request\n");
	sleep(3);
	gum_dhcp_discover(physd);
	printf("SENT\n");
	pause();
	return 0;
}	