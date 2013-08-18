#include "netdev.h"


#define TAP_PERSIST 0


/* Function Taken From http://backreference.org/2010/03/26/tuntap-interface-tutorial/ */
int tun_alloc(char *dev, int flags) {
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if( (fd = open(clonedev, O_RDWR)) < 0 ) {
        return fd;
    }
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;   
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    err = ioctl(fd, TUNSETIFF, (void *) &ifr);
    if( err < 0 ) {
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

char dev[256];

struct tun_dev* tun_init(){
    struct tun_dev* tund;
    struct in_addr ip_addr;
    struct ifreq ifrmac;
    // char* m;

    tund = (struct tun_dev*)malloc(sizeof(struct tun_dev));
    strcpy(dev, "mytun");

    tund->sockfd = tun_alloc(dev, IFF_TAP | IFF_NO_PI);
    DEBUG("tuntap", ("allocating new tap device with name %s\n", dev));
    DEBUG("tuntap", ("tap persist set to %d\n", TAP_PERSIST));

    if(ioctl(tund->sockfd, TUNSETPERSIST, TAP_PERSIST) < 0){
        perror("enabling TUNSETPERSIST");
        exit(1);
    }

    if(tund->sockfd < 0){
        fprintf(stderr, "Failed allocating new TAP device\n");
        exit(1);
    }

    memset(&ifrmac, 0, sizeof(struct ifreq));
    strncpy(ifrmac.ifr_name, dev, IFNAMSIZ-1);
    if (ioctl(tund->sockfd, SIOCGIFHWADDR, &ifrmac) < 0){
        perror("tun_init SIOCGIFHWADDR");
        exit(1);
    }

    memcpy(tund->mac, ifrmac.ifr_hwaddr.sa_data, 6);


    // m = (char*)ifrmac.ifr_hwaddr.sa_data;
    // printf("TUN Device MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);

    inet_aton("192.168.1.10", &ip_addr);
    tund->ip_addr = ip_addr.s_addr;

    printf("TAP device allocated: %s\n", dev);

    return tund;
}
