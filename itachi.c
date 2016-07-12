#include <stdlib.h>
#include "itachi.h"

itachi_p itachi_init(){
  itachi_p itachi = (itachi_p)malloc(sizeof(itachi_t));

  //on initialise la liste des paquets en file d attente
  itachi->queued_packets = ipq_list_create();
  
  return itachi;
}

void itachi_exit(itachi_p itachi){
  free(itachi);
}



ipq_packet_msg_t* itachi_add_ipq(itachi_p itachi, ipq_packet_msg_t * m){
  ipq_packet_msg_t * m_cpy;
  size_t m_len = sizeof(ipq_packet_msg_t) + m->data_len;
  m_cpy = malloc(m_len);
  //on copie m ds m_cpy...
  memcpy(m_cpy,m,m_len);
  //et ici on empile la copie car l autre on ne sais pas ce 
  //qu'il devient...
  itachi->queued_packets=ipq_list_add(itachi->queued_packets,m_cpy);
  return m_cpy;
}
