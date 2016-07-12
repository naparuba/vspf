#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

#include "caract.h"
#include "packet.h"


//fonction qui rempli un caract de puis un ipq
caract_t fill_caract(ipq_packet_msg_t *m){
  caract_t res;
  
  struct iphdr *iph = (struct iphdr *)m->payload;
  u32 i = iph->ihl*4;
  res.proto = which_protocol(m);
  
  union transp_layer tl;
  if(res.proto == IS_ICMP){
    struct libnet_icmpv4_hdr * icmph = (struct libnet_icmpv4_hdr *)((u32)iph + i);
    tl=fill_icmp(icmph);
  }
  
  if(res.proto == IS_TCP){
    struct libnet_tcp_hdr * tcph = (struct libnet_tcp_hdr *)((u32)iph + i);
    tl=fill_tcp(tcph);
  }
  
  if(res.proto == IS_UDP){
    struct libnet_udp_hdr * udph = (struct libnet_udp_hdr *)((u32)iph + i);
    tl=fill_udp(udph);
  }
  if(res.proto == IS_UNKNOW){
    printf("Protocole unknow\n");
  }
  
  //maintenant on rempli vraiment les choses
  res.id = m->packet_id;
  
  strncpy((char*)res.indev,(char*)m->indev_name,8);
  res.indev[7]='\0';  
  
  
  strncpy((char*)res.outdev,(char*)m->outdev_name,8);
  res.outdev[7]='\0';
  
  strncpy((char*)res.commande,(char*)m->commande,8);
  res.commande[7]='\0';
  
  res.addr_src=ntohl(iph->saddr);
  res.addr_dst=ntohl(iph->daddr);
  
  // on rempli le dernier
  //pb pour unknow?
  res.trans_lay=tl;
  
  return res;
}


caract_next_layer_t fill_icmp(struct libnet_icmpv4_hdr * icmph){
  caract_next_layer_t res;
  icmp_caract_t icmp;
  icmp.type = icmph->icmp_type;
  res.icmp_caract=icmp;
  return res;
}

caract_next_layer_t fill_tcp(struct libnet_tcp_hdr * tcph){
  caract_next_layer_t res;
  tcp_caract_t tcp;
  tcp.port_src = ntohs(tcph->th_sport);
  tcp.port_dst = ntohs(tcph->th_dport);
  //RMQ: pk pas de ntohs? -> car en brut protocol, donc nan
  tcp.flags = tcph->th_flags;
  res.tcp_caract = tcp;
  return res;
}

caract_next_layer_t fill_udp(struct libnet_udp_hdr * udph){
  caract_next_layer_t res;
  udp_caract_t udp;
  udp.port_src = ntohs(udph->uh_sport);
  udp.port_dst = ntohs(udph->uh_dport);
  res.udp_caract = udp;
  return res;
}
