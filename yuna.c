#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "print.h"
#include "yuna.h"
#include "messages.h"

#define BUFSIZE 2048

yuna_p yuna_init(){
  yuna_p yuna = (yuna_p)malloc(sizeof(yuna_t));
  yuna->stats = (yuna_stats_p)malloc(sizeof(yuna_stats_t));
  //on ouvre les pipes en Write deja
  //yuna->guardians_to_sin_fd = open(FIFO_GUARDIANS_TO_SIN, O_WRONLY|O_NONBLOCK| O_NDELAY);

  yuna->guardians_to_ihm_fd = open(FIFO_GUARDIANS_TO_IHM, O_WRONLY|O_NONBLOCK| O_NDELAY);
  
  return yuna;
}

void yuna_exit(yuna_p yuna){
  if(yuna->guardians_to_ihm_fd)
    close(yuna->guardians_to_ihm_fd);
  if(yuna->ihm_to_guardians_fd)
    close(yuna->ihm_to_guardians_fd);
  free(yuna);
}


void yuna_open_sin_to_guardians(yuna_p yuna){
  //on regarde le sin
  /*
  yuna->sin_to_guardians_fd = open(FIFO_SIN_TO_GUARDIANS, O_RDONLY| O_NONBLOCK| O_NDELAY);
  if(yuna->sin_to_guardians_fd < 0){
    myprintfDebugGuardians("Erreur ds ouverture de %s\n",FIFO_SIN_TO_GUARDIANS);
    exit(1);
    }*/
}

void yuna_declare_sin_to_guardians(yuna_p yuna,fd_set * read_fds){
  //on le declare
  myprintfDebugGuardians("guard: on declare son to guardians = %d\n",yuna->sin_to_guardians_fd);
  FD_SET(yuna->sin_to_guardians_fd, read_fds);
}


void yuna_open_ihm_to_guardians(yuna_p yuna){
  //Un par interface avec l'ihm
  //on re-ouvre le pipe nomme guardians_to_ihm
  yuna->ihm_to_guardians_fd = open(FIFO_IHM_TO_GUARDIANS, O_RDONLY| O_NONBLOCK| O_NDELAY);
  if(yuna->ihm_to_guardians_fd < 0){
    myprintfDebugGuardians("Erreur ds ouverture de %s\n",FIFO_IHM_TO_GUARDIANS);
    exit(1);
  }
}

void yuna_declare_ihm_to_guardians(yuna_p yuna,fd_set * read_fds){
  //on de clare la communication avec l ihm
  FD_SET(yuna->ihm_to_guardians_fd, read_fds);
}

int yuna_read_ihm(yuna_p yuna, char * buf){
  //atttention ici!
  //besoin d'une double verif si besoin
  return read(yuna->ihm_to_guardians_fd,buf,BUFSIZE);
}

int yuna_write_to_sin(yuna_p yuna,char * buf, int size){
  /*
  if(yuna->guardians_to_sin_fd < 0)
    yuna->guardians_to_sin_fd = open(FIFO_GUARDIANS_TO_SIN, O_WRONLY|O_NONBLOCK| O_NDELAY);
  */
  
  int r;
  r = write(yuna->guardians_to_sin_fd, buf, size);
  if(r < 0)
    myprintfDebugGuardians("N'a pas pu ecrire sur %d car %d\n",yuna->guardians_to_sin_fd,r);
  else
    myprintfDebugGuardians("YUNA TO SIN:On a ecrit: %d sur %d\n",r,yuna->guardians_to_sin_fd);
  
  /*close(yuna->guardians_to_sin_fd);
  yuna->guardians_to_sin_fd = open(FIFO_GUARDIANS_TO_SIN, O_WRONLY|O_NONBLOCK| O_NDELAY);
  */
  return r;
}

int yuna_write_to_ihm(yuna_p yuna,char * buf, int size){
  if(yuna->guardians_to_ihm_fd < 0)
    yuna->guardians_to_ihm_fd = open(FIFO_GUARDIANS_TO_IHM, O_WRONLY|O_NONBLOCK| O_NDELAY);
  
  int r = write(yuna->guardians_to_ihm_fd, buf, size);
  if(r < 0)
    myprintfDebugGuardians("N'a pas pu ecrire sur %s car %d\n",FIFO_GUARDIANS_TO_IHM,r);
  else
    myprintfDebugGuardians("YUNA TO IHMOn a ecrit: %d sur %s\n",r,FIFO_GUARDIANS_TO_IHM);
  close(yuna->guardians_to_ihm_fd);
  yuna->guardians_to_ihm_fd = open(FIFO_GUARDIANS_TO_IHM, O_WRONLY|O_NONBLOCK| O_NDELAY);
  return r;
}


void yuna_close_ihm_to_guardians(yuna_p yuna){
  close(yuna->ihm_to_guardians_fd);
}


int yuna_read_sin(yuna_p yuna,char * buf){

  myprintfDebugGuardians("YUNA:***** On a un read de sin par Guardians fd = %d\n",yuna->sin_to_guardians_fd);

  int status = read(yuna->sin_to_guardians_fd, buf, BUFSIZE);
  if (status < 0) {
    myprintfDebugGuardians("Erreur de lecture sur le pipe %s\n",FIFO_SIN_TO_GUARDIANS);
  }
  return status;
}

int yuna_send_smth_to_ihm(yuna_p yuna, u16 type, u16 length, u8* payload){
  message_t mes;
  u8* packet;
  mes = creer_message(type,length,(u8*)(payload));
  packet = creer_packet(mes);
  //2*sizeof(u16) correspond a la preske taille du message
  //voir message.h pour s'en rendre compte
  return yuna_write_to_ihm(yuna, packet, length + 2*sizeof(u16) );
}

int yuna_send_smth_to_sin(yuna_p yuna, u16 type, u16 length, u8* payload){
  message_t mes;
  u8* packet;
  mes = creer_message(type,length,(u8*)(payload));
  packet = creer_packet(mes);
  //2*sizeof(u16) correspond a la preske taille du message
  //voir message.h pour s'en rendre compte
  return yuna_write_to_sin(yuna, packet, length + 2*sizeof(u16) );
}
