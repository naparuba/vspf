#include "caract.h"
#include "tidus.h"

tidus_p tidus_init(){
  tidus_p tidus = (tidus_p)malloc(sizeof(tidus_t));
  tidus->stats = (tidus_stats_p)malloc(sizeof(tidus_stats_t));
  return tidus;
}

void tidus_exit(tidus_p tidus){
  free(tidus);
}

caract_t tidus_create_caract_from_ipq(tidus_p tidus, ipq_packet_msg_t *m){
  return fill_caract(m);
}
