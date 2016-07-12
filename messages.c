#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>


#include "common.h"
#include "messages.h"

#define BUFSIZE 2048

message_t creer_message(u16 type, u16 len, u8* payload){
  message_t r;
  r.type=type;
  r.len=len;
  r.payload=payload;
  
  return r;
}

u8* creer_packet(message_t r){
  u8* buf = (u8*)malloc(BUFSIZE); 
  int size=0;
  
  memcpy(buf,&(r.type),sizeof(r.type));
  size+=sizeof(r.type);
  memcpy(buf+size,&(r.len),sizeof(r.len));
  size+=sizeof(r.len);
  memcpy(buf+size,r.payload,r.len);
  size+=r.len;
   
  return buf;
}

message_t get_message(u8* m){
  message_t r;

  r.type = ((message_t*)m)->type;
  r.len = ((message_t*)m)->len;
  r.payload = malloc(r.len);
  memcpy(r.payload,m+sizeof(r.type)+sizeof(r.len),r.len);
  
  return r;  
}

choix_t creer_choix(ul32 ipq_id,u16 verdict){
  choix_t r;
  r.ipq_id = ipq_id;
  r.verdict = verdict;

  return r;
}

choix_t copy_choix(choix_t c){
  choix_t r;
  r.ipq_id = c.ipq_id;
  r.verdict = c.verdict;

  return r;
}

message_t creer_message_choix(choix_t c){
  message_t m;
  u8 * payload = (u8*)malloc(sizeof(choix_t));
  memcpy(payload,&c,sizeof(choix_t));
  m.type = TYPE_CHOIX;
  m.len = sizeof(choix_t);
  m.payload = payload;
  
  return m;
}
