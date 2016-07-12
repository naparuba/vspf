#include "common.h"
#include "rule.h"
#include "messages.h"

#include <libipq/libipq.h>

#ifndef IPQ_LIST_H
#define IPQ_LIST_H


typedef struct ipq_list
{
  ipq_packet_msg_t *m;
  struct ipq_list* prec;
}ipq_list_t,* ipq_list_p;




// Prototypes des fonctions implémentée dans le fichier policy.c

ipq_list_p ipq_list_create(void);

ipq_list_p ipq_list_add(ipq_list_p p,ipq_packet_msg_t *m);

ipq_list_p ipq_list_remove(ipq_list_p p);

ipq_packet_msg_t * ipq_list_get(ipq_list_p p);

ipq_list_p ipq_list_next(ipq_list_p p);

int ipq_list_is_void(ipq_list_p p);

void ipq_list_print(ipq_list_p p);

u32 ipq_list_size(ipq_list_p p);

void ipq_list_flush(ipq_list_p p);

void ipq_list_accept_all(struct ipq_handle *h, ipq_list_p p);
void ipq_list_drop_all(struct ipq_handle *h, ipq_list_p p);

void ipq_list_apply(struct ipq_handle *h, ipq_list_p p, choix_t c);

#endif // IPQ_LIST_H
