#include <libnet.h>
#include <libipq/libipq.h>

#ifndef PACKET_H
#define PACKET_H 


/*
typedef struct ipq_packet_msg {
	unsigned long packet_id;	 ID of queued packet 
	unsigned long mark;		 Netfilter mark value 
	long timestamp_sec;		 Packet arrival time (seconds) 
	long timestamp_usec;		 Packet arrvial time (+useconds) 
	unsigned int hook;		 Netfilter hook we rode in on 
	char indev_name[IFNAMSIZ];	 Name of incoming interface 
	char outdev_name[IFNAMSIZ];	 Name of outgoing interface 
	unsigned short hw_protocol;	 Hardware protocol (network order) 
	unsigned short hw_type;		 Hardware type 
	unsigned char hw_addrlen;	 Hardware address length 
	unsigned char hw_addr[8];	 Hardware address 
	size_t data_len;		 Length of packet data 
  unsigned char commande[8]; ajout
	unsigned char payload[0];	 Optional packet data 
} ipq_packet_msg_t;
*/


/*
struct iphdr {
	__u8	ihl:4,
		version:4;
	__u8	tos;
	__u16	tot_len;
	__u16	id;
	__u16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__u16	check;
	__u32	saddr;
	__u32	daddr;
};
*/

/*
struct icmpheader {
 unsigned char icmp_type; 0=echo reply 8=echo request 3=dest unreach
 unsigned char icmp_code; //utile ke en cas d erreur
 unsigned short int icmp_cksum;
  The following data structures are ICMP type specific 
 unsigned short int icmp_id;//pas utile
 unsigned short int icmp_seq;//pas utile
};  total icmp header length: 8 bytes (=64 bits) 

*/

/*
struct udpheader {
 unsigned short int uh_sport; //source port
 unsigned short int uh_dport; //desct port
 unsigned short int uh_len; //len du header + payload
 unsigned short int uh_check; 
};  total udp header length: 8 bytes (=64 bits) 
*/

/*
struct tcpheader {
 unsigned short int th_sport;
 unsigned short int th_dport;
 unsigned int th_seq;
 unsigned int th_ack;
 unsigned char th_x2:4, th_off:4; //inutile
 unsigned char th_flags; //tres utile
 unsigned short int th_win;
 unsigned short int th_sum;
 unsigned short int th_urp; //priorite
};  total tcp header length: 20 bytes (=160 bits) 
*/

//ds le header IP
#define TCP_PROTOCOL_ID 6
#define ICMP_PROTOCOL_ID 1
#define UDP_PROTOCOL_ID 17


//pour nous
#define IS_UNKNOW 0
#define IS_ICMP 1
#define IS_TCP 2
#define IS_UDP 3

//pour le tcp
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define TH_ECNECHO	0x40	/* ECN Echo */
#define TH_CWR		0x80	/* ECN Cwnd Reduced */

int which_protocol(ipq_packet_msg_t *m);
unsigned long int ip_t2dec(int *ipv4);
int * dec2ip_t(unsigned long int ip);

#endif
