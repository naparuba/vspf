#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "print.h"
#include "packet.h"
#include "caract.h"
#include "common.h"

//va sur stdout
void myprintf(char* message,...)
{
  va_list args;
  va_start(args,message);
  if(DEBUG == 1)
    vprintf(message,args);
  va_end(args);
}

//print sur le fichier FIFO_DEBUG_SIN
void myprintfDebugSin(char* message,...)
{
  
  FILE *file = fopen(FIFO_DEBUG_SIN,"w");
  if (file == NULL) {
    fprintf(stderr,"Erreur dans l'ouverture du fichier %s",FIFO_DEBUG_SIN);
    return;
  }
  
  va_list args;
  va_start(args,message);
  if(DEBUG == 1)
    vfprintf(file,message,args);
  va_end(args);
  
  fclose(file);
}

//print sur le fichier FIFO_WARN_SIN
void myprintfWarnSin(char* message,...)
{
  va_list args;
  va_start(args,message);
  vprintf(message,args);
  va_end(args);
}

//print sur le fichier FIFO_CRIT_SIN
void myprintfCritSin(char* message,...)
{
  va_list args;
  va_start(args,message);
  vprintf(message,args);
  va_end(args);
}

//print sur le fichier FIFO_DEBUG_GUARDIANS
void myprintfDebugGuardians(char* message,...)
{
  FILE *file = fopen(FIFO_DEBUG_GUARDIANS,"w");
  if (file == NULL) {
    fprintf(stderr,"Erreur dans l'ouverture du fichier %s",FIFO_DEBUG_GUARDIANS);
    return;
  }

  va_list args;
  va_start(args,message);
  if(DEBUG == 1)
    vfprintf(file,message,args);
  va_end(args);

  fclose(file);
}

//print sur le fichier FIFO_WARN_GUARDIANS
void myprintfWarnGuardians(char* message,...)
{
  va_list args;
  va_start(args,message);
  vprintf(message,args);
  va_end(args);
}

//print sur le fichier FIFO_CRIT_GUARDIANS
void myprintfCritGuardians(char* message,...)
{
  va_list args;
  va_start(args,message);
  vprintf(message,args);
  va_end(args);
}


/* prints the content of a message */
void print_ipq(ipq_packet_msg_t *message) {
  myprintf("Packet id : %lu\n", message->packet_id);
  myprintf("Netfilter mark value : %lu\n", message->mark);
  myprintf("Packet arrival time (seconds) : %li\n", message->timestamp_sec);
  myprintf("Packet arrival time (+useconds) : %li\n", message->timestamp_usec);
  myprintf("Netfilter hook we rode in : %u\n", message->hook);
  myprintf("Name of incoming interface : %s\n", message->indev_name);
  myprintf("Name of outgoing interface : %s\n", message->outdev_name);
  myprintf("Hardware protocol : %u\n", message->hw_protocol);
  myprintf("Hardware type : %u\n", message->hw_type);
  myprintf("Hardware address length : %c\n", message->hw_addrlen);
  myprintf("Hardware address : %s\n", message->hw_addr);
  myprintf("Length of packet data : %i\n", message->data_len);
  myprintf("Commande : %s\n", message->commande);
}

//print a tcp header
void print_tcp(struct libnet_tcp_hdr * tcph){
  myprintf("Packet TCP\n");
  
  u_short sport = ntohs(tcph->th_sport);
  u_short dport = ntohs(tcph->th_dport);
  myprintf("\tOrigFlags:%u",tcph->th_flags);
  unsigned char flags;
  //from tcpdump/print-tcp.c
  if ((flags = tcph->th_flags) & (TH_SYN|TH_FIN|TH_RST|TH_PUSH|
				  TH_ECNECHO|TH_CWR)) {
    if (flags & TH_SYN)
      myprintf("S");
    if (flags & TH_FIN)
      myprintf("F");
    if (flags & TH_RST)
	myprintf("R");
    if (flags & TH_PUSH)
      myprintf("P");
    if (flags & TH_CWR)
      myprintf("W");	/* congestion _W_indow reduced (ECN) */
    if (flags & TH_ECNECHO)
      myprintf("E");	/* ecn _E_cho sent (ECN) */
  } else
    myprintf(".");
  myprintf("\n");
  //char *buf = (char*)(tcph) + 4*(tcph->th_off);
  
  myprintf("\tPort src=%i   dst=%i\nOffset : %u\n", sport, dport, tcph->th_off);
}

//print a udp_header
void print_udp(struct libnet_udp_hdr * udph){
  myprintf("Packet UDP\n");
  
  u_short sport = ntohs(udph->uh_sport);
  u_short dport = ntohs(udph->uh_dport);
  myprintf("\tPort src=%i   dst=%i\n", sport, dport);
}

//print a icmp header
void print_icmp(struct libnet_icmpv4_hdr * icmph){
  myprintf("Packet ICMP\n");
  myprintf("\tType:%u\n",icmph->icmp_type);
}

/* prints the data field of the message */
void print_ip(ipq_packet_msg_t *m){
  struct iphdr *iph = (struct iphdr *)m->payload;
  u_int i = iph->ihl*4;

  myprintf("\tIHL: %u\n", iph->ihl);
  myprintf("\tAddr scr=%u   dst=%u\n", ntohl(iph->saddr), ntohl(iph->daddr));

  int proto = which_protocol(m);
  
  if(proto == IS_ICMP){
    struct libnet_icmpv4_hdr * icmph = (struct libnet_icmpv4_hdr *)((u_int32_t)iph + i);
    print_icmp(icmph);
  }
  
  if(proto == IS_TCP){
    struct libnet_tcp_hdr * tcph = (struct libnet_tcp_hdr *)((u_int32_t)iph + i);
    print_tcp(tcph);
  }
  
  if(proto == IS_UDP){
    struct libnet_udp_hdr * udph = (struct libnet_udp_hdr *)((u_int32_t)iph + i);
    print_udp(udph);
  }
  if(proto == IS_UNKNOW){
    myprintf("\tProtocole unknow\n");
  }
}

//print une ip represente sous un int * de 4
//attention a bien avoir un tableau de 4 au moins..
void print_ip_t(int * ip){
  myprintf("%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
}



/* prints the data field of the caract*/
void print_caract(struct caract c){
  int * ip = NULL;
  myprintf("CARACT:\n");
  myprintf("\tId:%lu\n",c.id);
  myprintf("\tInDev:%s\n",c.indev);
  myprintf("\tOutDev:%s\n",c.outdev);
  myprintf("\tCommande:%s\n",c.commande);

  ip=(int*)dec2ip_t(c.addr_src);
  myprintf("\tAddrSrc:%u = ",c.addr_src);
  print_ip_t(ip);
  free(ip);
  ip=(int*)dec2ip_t(c.addr_dst);
  myprintf("\n\tAddrDst:%u = ",c.addr_dst);  
  print_ip_t(ip);
  free(ip);  
  myprintf("\n");
  
  struct icmp_caract *tl_icmp;
  struct tcp_caract *tl_tcp;
  struct udp_caract *tl_udp;
  
  switch(c.proto){
  case IS_ICMP:
    myprintf("\tProtocole: ICMP\n");
    tl_icmp = (icmp_caract_t*)&(c.trans_lay);
    myprintf("\tIcmpType:%u",tl_icmp->type);
    if(tl_icmp->type == 0) myprintf(" Echo Reply\n");
    if(tl_icmp->type == 8) myprintf(" Echo Request\n");
    if(tl_icmp->type == 3) myprintf(" Dest Unreach\n");
    break;
  case IS_TCP:
    myprintf("\tProtocole: TCP\n");
    tl_tcp = (tcp_caract_t*)&(c.trans_lay);
    myprintf("\tPortSrc:%u\n",tl_tcp->port_src);
    myprintf("\tPortDst:%u\n",tl_tcp->port_dst);
    myprintf("\tFlags:%u = ",tl_tcp->flags);
    unsigned char flags;
    //from tcpdump/print-tcp.c
    if ((flags = tl_tcp->flags) & (TH_SYN|TH_FIN|TH_RST|TH_PUSH|
				  TH_ECNECHO|TH_CWR)) {
      if (flags & TH_SYN)
	myprintf("S");
      if (flags & TH_FIN)
	myprintf("F");
      if (flags & TH_RST)
	myprintf("R");
      if (flags & TH_PUSH)
	myprintf("P");
      if (flags & TH_CWR)
	myprintf("W");	/* congestion _W_indow reduced (ECN) */
      if (flags & TH_ECNECHO)
	myprintf("E");	/* ecn _E_cho sent (ECN) */
    } else
      myprintf(".");
    myprintf("\n");
    break;
  case IS_UDP:
    myprintf("\tProtocole UDP\n");
    tl_udp = (udp_caract_t*)&(c.trans_lay);
    myprintf("\tPortSrc:%u\n",tl_udp->port_src);
    myprintf("\tPortDst:%u\n",tl_udp->port_dst);
    break;	   
  default:
    myprintf("\tProtocole Unknow\n");
  }
}


/* prints the data field of the rule*/
void print_rule(struct rule r){
  int * ip = NULL;
  myprintf("RULE:\n");
  
  myprintf("Number: %d\n",r.id);

  if(r.is_indev_used == TRUE)
    myprintf("\tInDev:%s\n",r.indev);
  if(r.is_outdev_used == TRUE)
    myprintf("\tOutDev:%s\n",r.outdev);
  if(r.is_commande_used == TRUE)
    myprintf("\tCommande:%s\n",r.commande);
  if(r.is_addr_src_used == TRUE){
    ip=(int*)dec2ip_t(r.addr_src_beg);
    myprintf("\tAddrSrcBeg:%u = ",r.addr_src_beg);
    print_ip_t(ip);
    free(ip);
    ip=(int*)dec2ip_t(r.addr_src_end);
    myprintf("\n\tAddrSrcEnd:%u = ",r.addr_src_end);  
    print_ip_t(ip);
    free(ip);  
    myprintf("\n");
  }

  if(r.is_addr_dst_used == TRUE){
    ip=(int*)dec2ip_t(r.addr_dst_beg);
    myprintf("\tAddrdstBeg:%u = ",r.addr_dst_beg);
    print_ip_t(ip);
    free(ip);
    ip=(int*)dec2ip_t(r.addr_dst_end);
    myprintf("\n\tAddrDstEnd:%u = ",r.addr_dst_end);  
    print_ip_t(ip);
    free(ip); 
    myprintf("\n");
  }

  if(r.is_proto_used == TRUE){
    struct icmp_rule *tl_icmp;
    struct tcp_rule *tl_tcp;
    struct udp_rule *tl_udp;
    
    switch(r.proto){
    case IS_ICMP:
      myprintf("\tProtocole: ICMP\n");
      tl_icmp = (icmp_rule_t*)&(r.next_layer);
      if(tl_icmp->is_type_used == TRUE){
	myprintf("\tIcmpType:%u",tl_icmp->type);
	if(tl_icmp->type == 0) myprintf(" Echo Reply\n");
	if(tl_icmp->type == 8) myprintf(" Echo Request\n");
	if(tl_icmp->type == 3) myprintf(" Dest Unreach\n");
      }
      break;
    case IS_TCP:
      myprintf("\tProtocole: TCP\n");
      tl_tcp = (tcp_rule_t*)&(r.next_layer);
      if(tl_tcp->is_port_src_used == TRUE){
	myprintf("\tPortSrcBeg:%u\n",tl_tcp->port_src_beg);
	myprintf("\tPortSrcEnd:%u\n",tl_tcp->port_src_end);
      }
      if(tl_tcp->is_port_dst_used == TRUE){
	myprintf("\tPortDstBeg:%u\n",tl_tcp->port_dst_beg);
	myprintf("\tPortDstEnd:%u\n",tl_tcp->port_dst_end);
      }
      if(tl_tcp->is_flags_used == TRUE){
	myprintf("\tFlags:%u = ",tl_tcp->flags);
	unsigned char flags;
	//from tcpdump/print-tcp.c
	if ((flags = tl_tcp->flags) & (TH_SYN|TH_FIN|TH_RST|TH_PUSH|
				       TH_ECNECHO|TH_CWR)) {
	  if (flags & TH_SYN)
	    myprintf("S");
	  if (flags & TH_FIN)
	    myprintf("F");
	  if (flags & TH_RST)
	    myprintf("R");
	  if (flags & TH_PUSH)
	    myprintf("P");
	  if (flags & TH_CWR)
	    myprintf("W");	/* congestion _W_indow reduced (ECN) */
	  if (flags & TH_ECNECHO)
	    myprintf("E");	/* ecn _E_cho sent (ECN) */
	} else
	  myprintf(".");
	myprintf("\n");
      }//fin flags
      break;
    case IS_UDP:
      myprintf("\tProtocole UDP\n");
      tl_udp = (udp_rule_t*)&(r.next_layer);
      if(tl_udp->is_port_src_used == TRUE){
	myprintf("\tPortSrcBeg:%u\n",tl_udp->port_src_beg);
	myprintf("\tPortSrcEnd:%u\n",tl_udp->port_src_end);
      }
      if(tl_udp->is_port_dst_used == TRUE){
	myprintf("\tPortDstBeg:%u\n",tl_udp->port_dst_beg);
	myprintf("\tPortDstEnd:%u\n",tl_udp->port_dst_end);
      }      
      break;	   
    default:
      myprintf("Protocole Unknow\n");
    }
  }
}


void print_choix(choix_t c){
  myprintf("Choix:\n");
  switch(c.verdict){
  case TO_DROP:
    myprintf("Verdict DROP pour %lu\n",c.ipq_id);
    break;
  case TO_ACCEPT:
    myprintf("Verdict ACCEPT pour %lu\n",c.ipq_id);
    break;
  default:
    myprintf("Verdict unknow pour %lu\n",c.ipq_id);
    break;
  }
}
