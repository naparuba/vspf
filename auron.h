#include <stdlib.h>
#include <stdio.h>

#include "time_work_asked_list.h"

#ifndef AURON_H
#define AURON_H

#define TYPE_TIME_REMOVE_CARACT 1
#define TYPE_TIME_REMOVE_RULE 2


typedef struct{
  int nbOpGenerated;
  size_t sizeModule;
} auron_stats_t,*auron_stats_p;


typedef struct{
  time_work_asked_list_p waiting_work;
  time_work_asked_list_p work_to_do;

  struct timeval timeout;  

  auron_stats_p stats;
}auron_t,*auron_p;


auron_p auron_init();
void auron_exit(auron_p auron);
void auron_add_time_work(auron_p auron,u16 time,u16 type,u16 data_length,u8* payload);
void auron_generate_timeout(auron_p auron);
void auron_update_work_from_to_do(auron_p auron,u32 time);
void auron_do_time_work(auron_p auron);

#endif
