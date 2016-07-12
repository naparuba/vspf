#include "common.h"
#include "rule.h"
#include "packet.h"
#include "print.h"

static u32 uin_rule = 0;

//Fonctions de base de rule
//fonction qui cree une rule vide
rule_t create_void_rule(){
  rule_t res;
  
  res.id = uin_rule++;
  res.is_indev_used = FALSE;
  res.is_outdev_used = FALSE;
  res.is_commande_used = FALSE;
  res.is_addr_src_used = FALSE;
  res.is_addr_dst_used = FALSE;
  res.is_proto_used = FALSE;
  res.verdict = NOT_MATCHED;

  return res;
}

rule_t copy_rule(rule_t r){
  rule_t res = r;
  return res;
}

//parmi TO_ACCEPT et TO_DROP
rule_t fill_verdict_rule(rule_t r,u16 v){
  r.verdict = v;
  return r;
}

//attention, taille de 8
rule_t fill_indev_rule(rule_t r,u8 * indev){
  if(indev){
    strncpy((char *)r.indev,(char*)indev,8);
    r.indev[7] = '\0';
    r.is_indev_used = TRUE;
  }
  return r;
}

rule_t fill_outdev_rule(rule_t r,u8 * outdev){
  if(outdev){
    strncpy((char*)r.outdev,(char*)outdev,8);
    r.outdev[7] = '\0';
    r.is_outdev_used = TRUE;
  }
  return r;
}

rule_t fill_commande_rule(rule_t r,u8 * commande){
  if(commande){
    strncpy((char*)r.commande,(char*)commande,8);
    r.commande[7] = '\0';
    r.is_commande_used = TRUE;
  }
  return r;
}

rule_t fill_addr_src_rule(rule_t r,u32 addr_src_beg,u32 addr_src_end){
  r.addr_src_beg = addr_src_beg;
  r.addr_src_end = addr_src_end;
  r.is_addr_src_used = TRUE;

  return r;
}

rule_t fill_addr_dst_rule(rule_t r,u32 addr_dst_beg,u32 addr_dst_end){
  r.addr_dst_beg = addr_dst_beg;
  r.addr_dst_end = addr_dst_end;
  r.is_addr_dst_used = TRUE;

  return r;
}
//remplir la derniere partie, rempli un icmp/tcp/udp dans un
//transp_layer puis rempli le proto et is_proto_used
rule_t add_icmp_rule(rule_t r,icmp_rule_t icmpr){
  union rule_next_layer tl;
  tl.icmp_rule=icmpr;
  r.next_layer=tl;
  r.proto = IS_ICMP;
  r.is_proto_used = TRUE;
  return r;
}

rule_t add_udp_rule(rule_t r,udp_rule_t udpr){
  union rule_next_layer tl;
  tl.udp_rule=udpr;
  r.next_layer=tl;
  r.proto = IS_UDP;
  r.is_proto_used = TRUE;
  return r;
}

rule_t add_tcp_rule(rule_t r,tcp_rule_t tcpr){
  union rule_next_layer tl;
  tl.tcp_rule=tcpr;
  r.next_layer=tl;
  r.proto = IS_TCP;
  r.is_proto_used = TRUE;
  return r;
}

//creer un icmp_rule vide
icmp_rule_t create_void_icmp_rule(){
  icmp_rule_t r;
  r.is_type_used = FALSE;
  return r;
}

//necessite un icmp_rule vide qui sera reecrase, peut mieux faire...
icmp_rule_t fill_icmp_type_rule(icmp_rule_t r,u8 type){
  r.type = type;
  r.is_type_used = TRUE;
  return r;
}

//creer un udp_rule vide
udp_rule_t create_void_udp_rule(){
  udp_rule_t r;
  r.is_port_src_used = FALSE;
  r.is_port_dst_used = FALSE;
  return r;
}

udp_rule_t fill_udp_port_src_rule(udp_rule_t r,u32 port_src_beg,u32 port_src_end){
  r.port_src_beg = port_src_beg;
  r.port_src_end = port_src_end;
  r.is_port_src_used = TRUE;

  return r;
}

udp_rule_t fill_udp_port_dst_rule(udp_rule_t r,u32 port_dst_beg,u32 port_dst_end){
  r.port_dst_beg = port_dst_beg;
  r.port_dst_end = port_dst_end;
  r.is_port_dst_used = TRUE;

  return r;
}

//creer un tcp_rule vide
tcp_rule_t create_void_tcp_rule(){
  tcp_rule_t r;
  r.is_port_src_used = FALSE;
  r.is_port_dst_used = FALSE;
  r.is_flags_used = FALSE;
  return r;
}

tcp_rule_t fill_tcp_port_src_rule(tcp_rule_t r,u32 port_src_beg,u32 port_src_end){
  r.port_src_beg = port_src_beg;
  r.port_src_end = port_src_end;
  r.is_port_src_used = TRUE;

  return r;
}

tcp_rule_t fill_tcp_port_dst_rule(tcp_rule_t r,u32 port_dst_beg,u32 port_dst_end){
  r.port_dst_beg = port_dst_beg;
  r.port_dst_end = port_dst_end;
  r.is_port_dst_used = TRUE;

  return r;
}

tcp_rule_t fill_tcp_flags_rules(tcp_rule_t r,u8 flags){
  r.flags = flags;
  r.is_flags_used = TRUE;

  return r;
}



/****************** Verification ****************/

// NOT_MATCHED par defaut
// si matche tout, alors rendre le verdict de la regle
//soit TO_DROP
//soit TO_ACCEPT
u16 is_matched(caract_t c,rule_t r){
  //return res des qu'on a une non verif

  printf("Matching:\n");
  
  //on verifie suivant les regles  
  //indev
  if(r.is_indev_used == TRUE){
    if(!strncmp((char*)r.indev,(char*)c.indev,8))//ca match donc on continue
      printf("\tOn match indev: %s = %s\n",r.indev,c.indev);
    else{// ca match pas
      printf("\tOn a pas matche indev: %s != %s\n",r.indev,c.indev);
      return NOT_MATCHED;
    }
  }
  //outdev
  if(r.is_outdev_used == TRUE){
    if(!strncmp((char*)r.outdev,(char*)c.outdev,8)){//ca match donc on continue
      printf("\tOn match oudev: %s = %s\n",r.outdev,c.outdev);
    }else{// ca match pas
      printf("\tOn a pas matche outdev: %s != %s\n",r.outdev,c.outdev);
      return NOT_MATCHED;
    }
  }

  //commande
  if(r.is_commande_used == TRUE){
    if(!strncmp((char*)r.commande,(char*)c.commande,8)){//ca match donc on continue
      printf("\tOn match commande: %s = %s\n",r.commande,c.commande);
    }else{// ca match pas
      printf("\tOn a pas matche commande: %s != %s\n",r.commande,c.commande);
      return NOT_MATCHED;
    }
  }

  //addr_src
  if(r.is_addr_src_used == TRUE){
    int * ip;
    //on verifie ke l adresse se trouve entre les deux
    if(c.addr_src >= r.addr_src_beg && c.addr_src <= r.addr_src_end){//ca match on continu
      printf("\ton a matche les ips src: CAddrSrc: ");
      ip=(int*)dec2ip_t(c.addr_src);
      print_ip_t(ip);
      free(ip);
      printf("RAddrSrc_beg: ");
      ip=(int*)dec2ip_t(r.addr_src_beg);
      print_ip_t(ip);
      free(ip);
      printf("RAddrSrc_end: ");
      ip=(int*)dec2ip_t(r.addr_src_end);
      print_ip_t(ip);
      free(ip);
      printf("\n");
    }else{//ca match pas
      printf("\tOn a pas matche les ips src: CAddrSrc: ");
      ip=(int*)dec2ip_t(c.addr_src);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrSrc_beg: ");
      ip=(int*)dec2ip_t(r.addr_src_beg);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrSrc_end: ");
      ip=(int*)dec2ip_t(r.addr_src_end);
      print_ip_t(ip);
      free(ip);
      printf("\n");
      return NOT_MATCHED;
    }
  }//fin addr_src
  //addr_dst
  if(r.is_addr_dst_used == TRUE){
    int * ip;
    //on verifie ke l adresse se trouve entre les deux
    if(c.addr_dst >= r.addr_dst_beg && c.addr_dst <= r.addr_dst_end){//ca match on continu
      printf("\tOn a matche les ips dst: CAddrDst: ");
      ip=(int*)dec2ip_t(c.addr_dst);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrDst_beg: ");
      ip=(int*)dec2ip_t(r.addr_dst_beg);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrDst_end: ");
      ip=(int*)dec2ip_t(r.addr_dst_end);
      print_ip_t(ip);
      free(ip);
      printf("\n");
    }else{//ca match pas
      printf("\tOn a pas matche les ips dst: CAddrDst: ");
      ip=(int*)dec2ip_t(c.addr_dst);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrDst_beg: ");
      ip=(int*)dec2ip_t(r.addr_dst_beg);
      print_ip_t(ip);
      free(ip);
      printf("\n\tRAddrDst_end: ");
      ip=(int*)dec2ip_t(r.addr_dst_end);
      print_ip_t(ip);
      free(ip);
      printf("\n");
      return NOT_MATCHED;
    }
  }
  //proto
  if(r.is_proto_used == TRUE){
    if(r.proto == c.proto){//ca match mais attention a matche aussi en dessous
      printf("\tLes proto ont matche: %d = %d\n",r.proto,c.proto);
      if(is_next_layer_matched(r.proto,c.trans_lay,r.next_layer) == MATCHED)
	printf("\tOn a aussi matche le protocol %d\n",r.proto);
      else{
	printf("\tOn a pas matche le protocol %d\n",r.proto);
	return NOT_MATCHED;
      }
    }//fin du proto
    else{
      printf("\tLes proto n'ont pas matches: %d != %d\n",r.proto,c.proto);
      return NOT_MATCHED;
    }
  }  

  return r.verdict;
}

//fille NOT_MATCHED MATCHED suivant les sous parties
u16 is_next_layer_matched(u16 proto,caract_next_layer_t tlc,rule_next_layer_t tlr){
  switch(proto){
  case IS_ICMP:
    return is_icmp_matched(tlc.icmp_caract,tlr.icmp_rule);
    break;//code mort, enfin j espere...
  case IS_UDP:
    return is_udp_matched(tlc.udp_caract,tlr.udp_rule);
    break;
  case IS_TCP:
    return is_tcp_matched(tlc.tcp_caract,tlr.tcp_rule);
    break;
  default:
    printf("\tProtocole Mitmatched\n");
    return NOT_MATCHED;
  }
}

//les fonction pour les verif sur les bas nivo
//dc tjs continuer jusqu a ce k on trouve un not matched
//sinon renvoyer matched
u16 is_icmp_matched(icmp_caract_t ic,icmp_rule_t ir){
  if(ir.is_type_used == TRUE){
    if(ir.type == ic.type)//on a matche
      printf("\tOn a matche les type ICMP: %d = %d\n",ic.type,ir.type);
    else{//on balance NOT_MATCHED hehe
      printf("\tOn a pas matche ICMP type: %d != %d\n",ic.type,ir.type);
      return NOT_MATCHED;
    }
  }
  //TODO, heu si vide on fait koi pour la regle?
  return MATCHED;     
}

u16 is_udp_matched(udp_caract_t uc,udp_rule_t ur){
  if(ur.is_port_src_used == TRUE){//on verifie ke c est ds l intervale
    if(uc.port_src >= ur.port_src_beg && uc.port_src <= ur.port_src_end)//on matched
      printf("\tOn a matche le port UdpSrc: %d est dans [%d,%d]\n",uc.port_src,ur.port_src_beg,ur.port_src_end);
    else{//on balance NOT_MATCHED
      printf("\tOn a pas matche UdpSrc: %d n'est pas dans [%d,%d]\n",uc.port_src,ur.port_src_beg,ur.port_src_end);
      return NOT_MATCHED;
    }
  }  

  if(ur.is_port_dst_used == TRUE){//on verifie ke c est ds l intervale
    if(uc.port_dst >= ur.port_dst_beg && uc.port_dst <= ur.port_dst_end)//on matched
      printf("\tOn a matche le port UdpDst: %d est dans [%d,%d]\n",uc.port_dst,ur.port_dst_beg,ur.port_dst_end);
    else{//on balance NOT_MATCHED
      printf("\tOn a pas matche UdpDst: %d n'est pas dans [%d,%d]\n",uc.port_dst,ur.port_dst_beg,ur.port_dst_end);
      return NOT_MATCHED;
    }  
  }
  //todo verif qu au moins une regle existe sinon y a foutage de gueule
  return MATCHED;
}

u16 is_tcp_matched(tcp_caract_t tc,tcp_rule_t tr){
  if(tr.is_port_src_used == TRUE){//on verifie ke c est ds l intervale
    if(tc.port_src >= tr.port_src_beg && tc.port_src <= tr.port_src_end)//on matched
      printf("\tOn a matche le port TcpSrc: %d est dans [%d,%d]\n",tc.port_src,tr.port_src_beg,tr.port_src_end);
    else{//on balance NOT_MATCHED
      printf("\tOn a pas matche TcpSrc: %d n'est pas dans [%d,%d]\n",tc.port_src,tr.port_src_beg,tr.port_src_end);
      return NOT_MATCHED;
    }
  }

  if(tr.is_port_dst_used == TRUE){//on verifie ke c est ds l intervale
    if(tc.port_dst >= tr.port_dst_beg && tc.port_dst <= tr.port_dst_end)//on matched
      printf("\tOn a matche le port TcpDst: %d est dans [%d,%d]\n",tc.port_dst,tr.port_dst_beg,tr.port_dst_end);
    else{//on balance NOT_MATCHED
      printf("\tOn a pas matche TcpDst: %d n'est pas dans [%d,%d]\n",tc.port_dst,tr.port_dst_beg,tr.port_dst_end);
      return NOT_MATCHED;
    }  
  }

  //les flags:
  //TODO: faire un plus joli filtre pour les flags, la c'est moche..
  //et l'affichage des flags...
  if(tr.is_flags_used == TRUE){
    if(tc.flags == tr.flags)//on matched, youhou
      printf("\tOn a matche les flags: %d = %d\n",tc.flags,tr.flags);
    else{// ah not matched
      printf("\tOn n'a pas matche les flags: %d != %d\n",tc.flags,tr.flags);
      return NOT_MATCHED;
    }
  }

  return MATCHED;
}
