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

#include "tuntap.h"

char dev[256];
int main(int argc, char const *argv[]){
   int tunfd;
   
   tunfd = tun_init();

   printf("%d\n", tunfd);
   pause();
   return 0;
}	