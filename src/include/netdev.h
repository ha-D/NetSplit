#ifndef _Z_TAPTUN_
#define _Z_TAPTUN_

#include <string.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <stdlib.h>
#include <errno.h>



#include "log.h"
#include "gum.h"

struct phys_dev{
	int sockfd;
	struct ifreq if_id;
	uint8_t mac[6];
	uint32_t ip_addr;
};

struct tun_dev{
	int sockfd;
	uint8_t  mac[6];
	uint32_t ip_addr;
};

struct phys_dev{
	int sockfd;
	struct ifreq if_id;
	struct ifreq mac;
};

struct tun_dev{
	int sockfd;
	struct ifreq mac;
};

int tun_alloc(char *dev, int flags);
struct tun_dev* tun_init();
struct phys_dev* phys_init();


struct phys_dev* get_phys();
struct tun_dev* get_tun();

void send_to_phys(struct phys_dev* physd, char* buf, int len);
void send_to_tun(struct tun_dev* tund, char* buf, int len);

#endif