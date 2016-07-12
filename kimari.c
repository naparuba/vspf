#include "kimari.h"

kimari_p kimari_init(){
  kimari_p kimari = (kimari_p)malloc(sizeof(kimari_t));
  kimari->stats = (kimari_stats_p)malloc(sizeof(kimari_stats_t));

  //on initialise la liste des regles
  kimari->policy = rule_list_create();  
  policy_init(kimari);
  
  return kimari;
}

void kimari_exit(kimari_p kimari){
  rule_list_flush(kimari->policy);
  free(kimari);
}

void policy_init(kimari_p kimari){
  struct rule r_icmp = create_void_rule();
  //creer une regle icmp
  r_icmp=fill_outdev_rule(r_icmp,(u8*)"eth0");
  //    r_icmp=fill_commande_rule(r_icmp,(u8*)"swapper");//juste 192.168.0.3
  struct icmp_rule i = create_void_icmp_rule();
  i=fill_icmp_type_rule(i,8);//type des unreach a priori
  r_icmp=add_icmp_rule(r_icmp,i);
  r_icmp=fill_verdict_rule(r_icmp,TO_ACCEPT);
  
  struct rule r_udp = create_void_rule();
  //creer une regle udp
  r_udp=fill_indev_rule(r_udp,(u8*)"eth0");
  struct udp_rule udp_r = create_void_udp_rule();
  udp_r = fill_udp_port_dst_rule(udp_r,4672,4672);//plage de port en src
  r_udp = add_udp_rule(r_udp,udp_r);
  r_udp = fill_verdict_rule(r_udp,TO_DROP);
  kimari->policy = rule_list_add(kimari->policy,r_udp);      
  
  struct rule r_tcp = create_void_rule();
  //creer une regle tcp
  r_tcp=fill_outdev_rule(r_tcp,(u8*)"eth0");
  r_tcp=fill_addr_src_rule(r_tcp,3232235523,3232235523);//juste 147.210.18.60 Tanit
  struct tcp_rule tcp_r = create_void_tcp_rule();
  tcp_r=fill_tcp_port_src_rule(tcp_r,22,25);//plage de port en src
  r_tcp=add_tcp_rule(r_tcp,tcp_r);
  r_tcp=fill_verdict_rule(r_tcp,TO_ACCEPT);
  
  //on add dans les list
  kimari->policy = rule_list_add(kimari->policy,r_icmp);
  kimari->policy = rule_list_add(kimari->policy,r_tcp);

}

void kimari_add_rule(kimari_p kimari, rule_t r){
  kimari->policy = rule_list_add(kimari->policy,r);
}

void kimari_delete_rule(kimari_p kimari, u32 id){
  kimari->policy = rule_list_remove_from_id(kimari->policy,id);
}



unsigned short kimari_is_matched_caract(kimari_p kimari,caract_t c){
  return rule_list_is_matched(c,kimari->policy);
}
