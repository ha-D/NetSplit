#include "netdev.h"


#define TAP_PERSIST 0


struct phys_dev* phys_init(){
    struct phys_dev* physd;
    struct ifreq ifr;
    struct ifreq ifrmac;
    struct in_addr ip_addr;
    struct gumpck* gum;
    // struct ifreq  ifopts;
    char* dev = "wlan0";
    // char* m;

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

    memset(&ifrmac, 0, sizeof(struct ifreq));
    strncpy(ifrmac.ifr_name, dev, IFNAMSIZ-1);
    if (ioctl(physd->sockfd, SIOCGIFHWADDR, &ifrmac) < 0){
        perror("phys_init SIOCGIFHWADDR");
        exit(1);
    }

    memcpy(physd->mac, ifrmac.ifr_hwaddr.sa_data, 6);
    
     // m = (char*)physd->mac.ifr_hwaddr.sa_data;
    // printf("Physical Device MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n", m[0], m[1], m[2], m[3], m[4], m[5]);

    // DEBUG("phys_dev", ("binded to physical device %s\n", dev));

    //inet_aton("192.168.1.68", &ip_addr);
    inet_aton("213.233.171.16", &ip_addr);
    physd->ip_addr = ip_addr.s_addr;

    // Add the default phys as a gum
    gum = (struct gumpck*)malloc(sizeof(struct gumpck));
    gum->ip = ip_addr.s_addr;
    memcpy(gum->mac, physd->mac, 6);
    gum_add(gum);

    return physd;
}


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

void send_to_phys(struct phys_dev* physd, char* buf, int len){
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

void send_to_tun(struct tun_dev* tund, char* buf, int len){
    len = write(tund->sockfd, buf, len);
    if(len < 0){
        perror("phys_thread send");
        exit(1);
    }
}