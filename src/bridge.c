#include "bridge.h"

#define BUFLEN 4096

int physfd, tunfd;

void listen_phys(){
	char buf[BUFLEN];
	int read_len;

	DEBUG("bridge", ("phys to tun bridge started\n"));
	while(1){
		read_len = read(physfd, buf, BUFLEN-1);
		if(read_len < 0){
			perror("phys_thread recv");
			exit(1);
		}

		read_len = write(tunfd, buf, read_len);
		if(read_len < 0){
			perror("phys_thread send");
			exit(1);
		}
	}	
}

void listen_tun(){
	char buf[BUFLEN];
	int read_len;

	DEBUG("bridge", ("tun to phys bridge started\n"));
	while(1){
		read_len = read(tunfd, buf, BUFLEN-1);
		if(read_len < 0){
			perror("tun_thread recv");
			exit(1);
		}

		read_len = write(physfd, buf, read_len);
		if(read_len < 0){
			printf("%d %d\n", errno, EAGAIN);
			perror("tun_thread send");
			exit(1);
			}
	}
}

void init_bridge(int pfd, int tfd){
	physfd = pfd;
	tunfd = tfd;

	thread_init(listen_phys);
	thread_init(listen_tun);
}