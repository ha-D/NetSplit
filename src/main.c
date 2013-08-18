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

	// Init gums before devices (atleast as long as adding gums in phys)
	init_gums();

	tund = tun_init();
	physd = phys_init();

	init_bridge(physd, tund);

	pause();
	return 0;
}	