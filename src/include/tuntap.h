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

int tun_alloc(char *dev, int flags);
int tun_init();
#endif