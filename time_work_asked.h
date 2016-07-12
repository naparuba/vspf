#include "common.h"

#ifndef TIME_WORK_ASKED_H
#define TIME_WORK_ASKED_H

typedef struct{
  u16 time;
  u16 type;
  u16 data_length;
  u8* payload;
} time_work_asked_t,*time_work_asked_p;

time_work_asked_t time_work_asked_create(u16 time, u16 type, u16 data_length, u8* payload);
void time_work_asked_remove(time_work_asked_t t);


#endif
