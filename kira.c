#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "kira.h"
#include "print.h"

#define BUFSIZE 2048

kira_p kira_init(){
  kira_p kira = (kira_p)malloc(sizeof(kira_t));
  
  
  kira->guardians_to_sin_fd = -1;
  kira->sin_to_guardians_fd = -1;
  return kira;
}

void kira_exit(kira_p kira){
  if (kira->ipq_h)
    ipq_destroy_handle(kira->ipq_h);
  if(kira->sin_to_guardians_fd)
    close(kira->sin_to_guardians_fd);
  if(kira->guardians_to_sin_fd)
    close(kira->guardians_to_sin_fd);
  
  free(kira);
}

int kira_open_sin_to_guardians(kira_p kira){
  //myprintfDebugSin("On fait un appel a un open d'un pipe nomme ds kira\n");

  //  kira->sin_to_guardians_fd = open(FIFO_SIN_TO_GUARDIANS, O_WRONLY|O_NONBLOCK| O_NDELAY);
  return kira->sin_to_guardians_fd;
}

int kira_open_guardians_to_sin(kira_p kira){
  //myprintfDebugSin("On fait un appel a un open d'un pipe nomme ds kira\n");
  
  /*  kira->guardians_to_sin_fd = open(FIFO_GUARDIANS_TO_SIN, O_RDONLY| O_NONBLOCK| O_NDELAY);
  if(kira->guardians_to_sin_fd < 0){
    myprintf("Erreur ds ouverture de %s\n",FIFO_GUARDIANS_TO_SIN);
    exit(1);
    }*/
  return kira->guardians_to_sin_fd;
}

void kira_declare_guardians_to_sin(kira_p kira,fd_set * read_fds){
  FD_SET(kira->guardians_to_sin_fd, read_fds);
}

int kira_read_guardians(kira_p kira,char * buf){
  myprintfDebugSin("SIN:******* On read du guardians ls fd = %d\n",kira->guardians_to_sin_fd);
  
  int status = read(kira->guardians_to_sin_fd,buf,BUFSIZE);
  if (status < 0) {
    myprintfDebugSin("Erreur de lecture sur le pipe %s\n",FIFO_GUARDIANS_TO_SIN);
  }
  return status;
}

void kira_close_guardians_to_sin(kira_p kira){
  // printf("ON A FAIT UN APPEL A CLOSE UN PIPE\n");
  //close(kira->guardians_to_sin_fd);
}


int kira_ipq_read(kira_p kira,unsigned char *buf){
  int status = ipq_read(kira->ipq_h, buf, BUFSIZE, 0);
  if (status < 0) {
    ipq_perror("passer");
  }
  return status;
}

ipq_packet_msg_t *kira_ipq_get_packet(unsigned char *buf){
  return ipq_get_packet(buf);
}


int kira_write_to_guardians(kira_p kira,char * buf, int size){

  /*if(kira->sin_to_guardians_fd < 0)
    kira->sin_to_guardians_fd = open(FIFO_SIN_TO_GUARDIANS, O_WRONLY|O_NONBLOCK| O_NDELAY);*/
  
  int r = write(kira->sin_to_guardians_fd, buf, size);
  if(r < 0)
    myprintfDebugSin("N'a pas pu ecrire sur %d car %d\n",kira->sin_to_guardians_fd,r);
  else
    myprintfDebugSin("SIN:On a ecrit: %d sur %d\n",r,kira->sin_to_guardians_fd);
  
  //on ne ferme pas le pipe
  /*close(kira->sin_to_guardians_fd);
    kira->sin_to_guardians_fd = open(FIFO_SIN_TO_GUARDIANS, O_WRONLY|O_NONBLOCK| O_NDELAY);*/
  return r;
}


int kira_send_smth_to_guardians(kira_p kira, u16 type, u16 length, u8* payload){
  message_t mes;
  u8* packet;
  mes = creer_message(type,length,(u8*)(payload));
  packet = creer_packet(mes);
  //2*sizeof(u16) correspond a la preske taille du message
  //voir message.h pour s'en rendre compte
  return kira_write_to_guardians(kira, packet, length + 2*sizeof(u16) );
}
