#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

#include "common.h"
#include "caract.h"
#include "packet.h"
#include "rule.h"
#include "messages.h"

#ifndef PRINT_H
#define PRINT_H

//mon printf a moi
void myprintf(char* message,...);

void myprintfDebugSin(char* message,...);
void myprintfDebugGuardians(char* message,...);

/* prints the content of a message */
void print_ipq(ipq_packet_msg_t *message);

//print a tcp header
void print_tcp(struct libnet_tcp_hdr * tcph);

//print a udp_header
void print_udp(struct libnet_udp_hdr * udph);

//print a icmp header
void print_icmp(struct libnet_icmpv4_hdr * icmph);

/* prints the data field of the message */
void print_ip(ipq_packet_msg_t *m);

//
void print_ip_t(int * ip);

//print a caract entierement
void print_caract(struct caract c);

//print a rule entierement
void print_rule(struct rule r);

//print un choix
void print_choix(choix_t c);

#endif
