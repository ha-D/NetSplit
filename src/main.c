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

char dev[256];
int main(int argc, char const *argv[]){
	int physfd;
	int tunfd;

	tunfd = tun_init();
	physfd = phys_init();

	init_bridge(physfd, tunfd);
	
	pause();
	return 0;
}	