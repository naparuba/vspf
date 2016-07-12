#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

#include "common.h"

#ifndef CARACT_H
#define CARACT_H

typedef struct icmp_caract{
  u8 type;
} icmp_caract_t;

typedef struct udp_caract{
  u16 port_src;
  u16 port_dst;
} udp_caract_t;

typedef struct tcp_caract{
  u16 port_src;
  u16 port_dst;
  u8 flags;
} tcp_caract_t;

typedef union transp_layer{
  icmp_caract_t icmp_caract;
  udp_caract_t udp_caract;
  tcp_caract_t tcp_caract;
} caract_next_layer_t;

typedef struct caract{
  ul32 id;
  u8 indev[8];
  u8 outdev[8];
  u8 commande[8];
  u32 addr_src;
  u32 addr_dst;
  u16 proto;
  caract_next_layer_t trans_lay;
} caract_t;


//fonction qui rempli un caract depuis un ipq
caract_t fill_caract(ipq_packet_msg_t *m);

caract_next_layer_t fill_icmp(struct libnet_icmpv4_hdr * icmph);

caract_next_layer_t fill_tcp(struct libnet_tcp_hdr * tcph);

caract_next_layer_t fill_udp(struct libnet_udp_hdr * udph);


#endif
