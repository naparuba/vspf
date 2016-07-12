#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "time_work_asked.h"

#ifndef TIME_WORK_ASKED_LIST_H
#define TIME_WORK_ASKED_LIST_H

// Définition des structure policy et policy_p

typedef struct time_work_asked_list{
  time_work_asked_t t;
  struct time_work_asked_list* prec;
}time_work_asked_list_t,* time_work_asked_list_p;


time_work_asked_list_p time_work_asked_list_create(void);

time_work_asked_list_p time_work_asked_list_add(time_work_asked_list_p p,time_work_asked_t t);

time_work_asked_list_p time_work_asked_list_remove(time_work_asked_list_p p);

time_work_asked_t time_work_asked_list_get(time_work_asked_list_p p);

time_work_asked_list_p time_work_asked_list_next(time_work_asked_list_p p);

int time_work_asked_list_is_void(time_work_asked_list_p p);

void time_work_asked_list_print(time_work_asked_list_p p);

u32 time_work_asked_list_size(time_work_asked_list_p p);

void time_work_asked_list_flush(time_work_asked_list_p p);

time_work_asked_list_p time_work_asked_list_waiting_to_to_do(time_work_asked_list_p wl,
							     time_work_asked_list_p tdl,
							     u32 time);

time_work_asked_list_p time_work_asked_list_waiting_update(time_work_asked_list_p wl,u32 time);

u32 time_work_asked_list_get_min_time(time_work_asked_list_p p);

#endif
