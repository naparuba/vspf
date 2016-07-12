#include "packet.h"

#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>

#include <libnet.h>



int which_protocol(ipq_packet_msg_t *m){
  struct iphdr *iph = (struct iphdr *)m->payload;
  
  if(iph->protocol == ICMP_PROTOCOL_ID)return IS_ICMP;
  if(iph->protocol == TCP_PROTOCOL_ID)return IS_TCP;
  if(iph->protocol == UDP_PROTOCOL_ID)return IS_UDP;

  return IS_UNKNOW;
}

//prend un int[4] sur 256 et renvoi la reprensentation u32
unsigned long int ip_t2dec(int *ipv4){
  return ( (*ipv4 << 24) + ((*(ipv4+1)) << 16) + ((*(ipv4+2)) << 8) + (*(ipv4+3)) );
}

//prend un u32 et renvoi le tableau int[4] des 256
int * dec2ip_t(unsigned long int ip){
  int * p = (int*)malloc(4*sizeof(int));
  p[0] = ((ip >> 24) & 0xFF);
  p[1] = ((ip >> 16) & 0xFF);
  p[2] = ((ip >> 8) & 0xFF);
  p[3] = (ip & 0xFF);
  return p;
}

