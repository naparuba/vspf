#include "ipq_list.h"

#ifndef ITACHI_H
#define ITACHI_H

typedef struct {
  ipq_list_p queued_packets;
} itachi_t, *itachi_p;

itachi_p itachi_init();
void itachi_exit(itachi_p);
ipq_packet_msg_t* itachi_add_ipq(itachi_p itachi, ipq_packet_msg_t * m);

#endif
