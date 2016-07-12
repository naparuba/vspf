#include "common.h"

#ifndef CHOIX_H
#define CHOIX_H

typedef struct choix{
  ul32 ipq_id;
  u16 verdict;
} choix_t;

choix_t creer_choix(ul32 ipq_id,u16 verdict);
choix_t copy_choix(choix_t c);

#endif
