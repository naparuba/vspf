#include <stdlib.h>
#include "caract.h"

#ifndef TIDUS_H
#define TIDUS_H

typedef struct{
  int nbCaractGenerated;
  size_t sizeModule;
} tidus_stats_t,*tidus_stats_p;


typedef struct {

  tidus_stats_p stats;
} tidus_t, * tidus_p;

tidus_p tidus_init();
void tidus_exit(tidus_p tidus);

caract_t tidus_create_caract_from_ipq(tidus_p tidus, ipq_packet_msg_t *m);

#endif
