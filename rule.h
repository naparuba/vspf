#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

#include "common.h"
#include "caract.h"

#ifndef RULE_H
#define RULE_H


//defini les verdicts
#define TO_ACCEPT 1
#define TO_DROP 2
#define MATCHED 3
#define NOT_MATCHED 4


typedef struct icmp_rule{
  u8 is_type_used;//si on utilise le champ
  u8 type;
} icmp_rule_t;

typedef struct udp_rule{
  u8 is_port_src_used;
  u8 is_port_dst_used;
  u16 port_src_beg;
  u16 port_src_end;
  u16 port_dst_beg;
  u16 port_dst_end;
} udp_rule_t;

typedef struct tcp_rule{
  u8 is_port_src_used;
  u8 is_port_dst_used;
  u16 port_src_beg;
  u16 port_src_end;
  u16 port_dst_beg;
  u16 port_dst_end;
  u8 is_flags_used;
  u8 flags;
} tcp_rule_t;

typedef union rule_next_layer{
  icmp_rule_t icmp_rule;
  udp_rule_t udp_rule;
  tcp_rule_t tcp_rule;
} rule_next_layer_t;

typedef struct rule{
  u16 verdict;
  
  u32 id;
  
  u8 is_indev_used;
  u8 is_outdev_used;
  u8 is_commande_used;
  u8 is_addr_src_used;
  u8 is_addr_dst_used;
  u8 is_proto_used;
  
  u8 indev[8];
  u8 outdev[8];
  u8 commande[8];
  u32 addr_src_beg;
  u32 addr_src_end;
  u32 addr_dst_beg;
  u32 addr_dst_end;
  u16 proto;
  rule_next_layer_t next_layer;
} rule_t;

/*********** Creation ******************/
//Fonctions de base de rule
//fonction qui cree une rule vide
rule_t create_void_rule();
rule_t copy_rule(rule_t r);
rule_t fill_verdict_rule(rule_t r, u16 v);
rule_t fill_indev_rule(rule_t r,u8 * indev);
rule_t fill_outdev_rule(rule_t r,u8 * outdev);
rule_t fill_commande_rule(rule_t r,u8 * commande);
rule_t fill_addr_src_rule(rule_t r,u32 addr_src_beg,u32 addr_src_end);
rule_t fill_addr_dst_rule(rule_t r,u32 addr_dst_beg,u32 addr_dst_end);
//remplir la derniere partie, rempli un icmp/tcp/udp dans un
//transp_layer puis rempli le proto et is_proto_used
rule_t add_icmp_rule(rule_t r,icmp_rule_t icmpr);
rule_t add_udp_rule(rule_t r,udp_rule_t udpr);
rule_t add_tcp_rule(rule_t r,tcp_rule_t tcpr);

//creer des regles vides
icmp_rule_t create_void_icmp_rule();
udp_rule_t create_void_udp_rule();
tcp_rule_t create_void_tcp_rule();

//necessite un icmp_rule vide qui sera reecrase, peut mieux faire...
icmp_rule_t fill_icmp_type_rule(icmp_rule_t r,u8 type);

udp_rule_t fill_udp_port_src_rule(udp_rule_t r,u32 port_src_beg,u32 port_src_end);
udp_rule_t fill_udp_port_dst_rule(udp_rule_t r,u32 port_dst_beg,u32 port_dst_end);

tcp_rule_t fill_tcp_port_src_rule(tcp_rule_t r,u32 port_src_beg,u32 port_src_end);
tcp_rule_t fill_tcp_port_dst_rule(tcp_rule_t r,u32 port_dst_beg,u32 port_dst_end);
tcp_rule_t fill_tcp_flags_rules(tcp_rule_t r,u8 flags);


/********************* Verification ****************/
//verifie si un caract est coherent avec une regle
u16 is_matched(caract_t c,rule_t r);

u16 is_next_layer_matched(u16 proto,caract_next_layer_t tlc,rule_next_layer_t tlr);
u16 is_icmp_matched(icmp_caract_t ic,icmp_rule_t ir);
u16 is_udp_matched(udp_caract_t uc,udp_rule_t ur);
u16 is_tcp_matched(tcp_caract_t tc,tcp_rule_t tr);


#endif
