#include <stdlib.h>
#include <stdio.h>

#include "time_work_asked_list.h"

#ifndef BELIAL_H
#define BELIAL_H

#define TYPE_TIME_REMOVE_IPQ 1

//temps en secondes pour ces actions
#define TIME_REMOVE_IPQ 60

typedef struct{
  int nbOpGenerated;
  size_t sizeModule;
} belial_stats_t,*belial_stats_p;


typedef struct{
  time_work_asked_list_p waiting_work;
  time_work_asked_list_p work_to_do;

  struct timeval timeout;  

  belial_stats_p stats;
}belial_t,*belial_p;


belial_p belial_init();
void belial_exit(belial_p belial);
void belial_add_time_work(belial_p belial,u16 time,u16 type,u16 data_length,u8* payload);
void belial_generate_timeout(belial_p belial);
void belial_update_work_from_to_do(belial_p belial,u32 time);
void belial_do_time_work(belial_p belial);

#endif
