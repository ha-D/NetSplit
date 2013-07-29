#ifndef _Z_PACKET_
#define _Z_PACKET_
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_tun.h>


struct arp_hdr {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];
  uint8_t target_mac[6];
  uint8_t target_ip[4];
};

void print_bytes(char* buf, int len);
void print_tcp(char* buf, int print_rest);
void print_ip(char* buf, int print_payload);
void print_arp(char* buf, int print_payload);
void print_eth(char* buf, int print_payload);

#endif