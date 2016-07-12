#include "tidus.h"
#include "yuna.h"
#include "kimari.h"
#include "auron.h"

#ifndef WAKKA_H
#define WAKKA_H



typedef struct{
  tidus_stats_p tidus_stats;
  kimari_stats_p kimari_stats;
  yuna_stats_p yuna_stats;
  auron_stats_p auron_stats;
} wakka_t,*wakka_p;



wakka_p wakka_init(tidus_stats_p, yuna_stats_p, kimari_stats_p, auron_stats_p);







#endif
