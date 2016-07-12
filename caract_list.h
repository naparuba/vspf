#include "common.h"
#include "rule.h"
#include "messages.h"
#include "caract.h"

#ifndef WAITING_CARACT_QUEUE_H
#define WAITING_CARACT_QUEUE_H

// Définition des structure policy et policy_p

typedef struct Queue_caract
{
  caract_t c;
  struct Queue_caract* prec;
}caract_queue,* caract_list_p;




// Prototypes des fonctions implémentée dans le fichier policy.c

caract_list_p caract_list_create(void);

caract_list_p caract_list_add(caract_list_p p,caract_t c);

caract_list_p caract_list_remove(caract_list_p p);

caract_t caract_list_get(caract_list_p p);

caract_list_p caract_list_next(caract_list_p p);

int caract_list_is_void(caract_list_p p);

void caract_list_print(caract_list_p p);

u32 caract_list_size(caract_list_p p);

void caract_list_flush(caract_list_p p);

//u16 is_matched_policy(caract_t c,queue_p p);
void caract_list_accept_all(int fd, caract_list_p p);
void caract_list_drop_all(int fd, caract_list_p p);

caract_list_p caract_list_apply(int fd, caract_list_p p, choix_t c);
caract_t caract_list_get_from_id(caract_list_p p, unsigned long id);
#endif // CARACT_QUEUE_H
