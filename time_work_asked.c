#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include "time_work_asked.h"

time_work_asked_t time_work_asked_create(u16 time,u16 type,u16 data_length,u8* payload){
  time_work_asked_t t;
  t.time = time;
  t.type = type;
  t.data_length = data_length;
  t.payload = (u8*)malloc(data_length);
  //mon mem cpy la payload, donc on devra free ca apres
  //memcpy(void *dst, const void *src, size_t len);
  memcpy(t.payload,payload,data_length);

  return t;
}

void time_work_asked_remove(time_work_asked_t t){
  free(t.payload);
}
