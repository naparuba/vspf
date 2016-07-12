#include <stdlib.h>

#ifndef YUNA_H
#define YUNA_H

typedef struct{
  int nbPacketRead;
  int nbPacketWrite;
  size_t sizeModule;
} yuna_stats_t,*yuna_stats_p;


typedef struct {
  int ihm_to_guardians_fd;
  int guardians_to_ihm_fd;
  
  int sin_to_guardians_fd;
  int guardians_to_sin_fd;

  yuna_stats_p stats;
} yuna_t, * yuna_p;

yuna_p yuna_init();
void yuna_exit();


void yuna_open_sin_to_guardians(yuna_p yuna);
void yuna_declare_sin_to_guardians(yuna_p, fd_set * read_fds);

void yuna_open_ihm_to_guardians(yuna_p yuna);
void yuna_declare_ihm_to_guardians(yuna_p,fd_set * read_fds);

int yuna_read_ihm(yuna_p yuna, char * buf);
int yuna_write_to_sin(yuna_p yuna,char * buf, int size);

int yuna_write_to_ihm(yuna_p yuna,char * buf, int size);

void yuna_close_ihm_to_guardians(yuna_p yuna);
int yuna_read_sin(yuna_p,char *buf);

int yuna_send_smth_to_ihm(yuna_p yuna, u16 type, u16 length, u8* payload);
int yuna_send_smth_to_sin(yuna_p yuna, u16 type, u16 length, u8* payload);

#endif
