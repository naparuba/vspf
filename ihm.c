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
#include "rule.h"
#include "print.h"
#include "caract.h"
#include "caract_list.h"
#include "ihm.h"

#define BUFSIZE 2048

static ihm_t *ihm_p; /* global variable used in signal handlers only */
static void sigquit(int signum);

static void sigquit(int signum){
  printf("SIGINT, SIGTERM or SIGQUIT catched, Bye Bye\n");
  exit(0);
}

void ihm_init(ihm_t * ihm){
  ihm->ihm_to_core_fd = open(FIFO_IHM_TO_CORE, O_WRONLY | O_NONBLOCK);
  assert(ihm->ihm_to_core_fd >= 0);
  
  struct rule r_icmp = create_void_rule();
  //creer une regle icmp
  r_icmp=fill_outdev_rule(r_icmp,(u8*)"eth0");
  r_icmp=fill_commande_rule(r_icmp,(u8*)"ping");//juste 192.168.0.3
  //r_icmp=fill_addr_src_rule(r_icmp,3232235523,3232235523);//juste 147.210.18.60 Tanit
  struct icmp_rule i = create_void_icmp_rule();
  i=fill_icmp_type_rule(i,8);//type des unreach a priori
  r_icmp=add_icmp_rule(r_icmp,i);
  r_icmp=fill_verdict_rule(r_icmp,TO_DROP);
  
  
  message_t m;
  u8* packet;
  m=creer_message(TYPE_RULE,sizeof(struct rule),&r_icmp);
  packet=creer_packet(m);
  
  //heu le -1 il vient d'ou?
  printf("Envois de Rule taille: %d\n",sizeof(struct rule)+2*sizeof(u16));
  write(ihm->ihm_to_core_fd, packet, sizeof(struct rule)+2*sizeof(u16));
  
  //pareil on envoi un choix
  choix_t c = creer_choix(1234,TO_ACCEPT);
  m=creer_message_choix(c);
  packet=creer_packet(m);
  printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
  write(ihm->ihm_to_core_fd, packet,sizeof(struct choix)+2*sizeof(u16));
  
}


int main(int argc, char ** argv){
  ihm_t ihm;
  ihm_p = &ihm;
  
  signal(SIGINT, sigquit);
  signal(SIGTERM, sigquit);
  signal(SIGQUIT, sigquit);

  ihm_init(&ihm);
  
  printf("Waiting for messages\n");
  
  //int ihm_to_core_fd;
  //int core_to_ihm_fd;
    
  //core_to_ihm_fd = open(FIFO_CORE_TO_IHM, O_RDONLY);
  //assert(core_to_ihm_fd >= 0);
  
  
  
  int nbytes;
  //status = mkfifo(FIFO, 666);
  //assert(status != -1 || errno == EEXIST);
  //fd = open(FIFO, O_RDONLY);
  //assert(fd >= 0);
  
  
  char buf[BUFSIZE];  

  
  //debug
  int nb_packet = 0;
  caract_list_p waiting_caract=caract_list_create();

  while(1){
    message_t *m;
    caract_t *r;
    
    ihm_p->core_to_ihm_fd = open(FIFO_CORE_TO_IHM, O_RDONLY);
    assert(ihm_p->core_to_ihm_fd >= 0);

    nbytes = read(ihm_p->core_to_ihm_fd, buf, BUFSIZE);
    
    if(nbytes < 0){
      printf("Problemes dans la lecture de %s car %i\n",FIFO_CORE_TO_IHM,nbytes);
    }else{
      tmp = (nbytes<BUFSIZE?nbytes:BUFSIZE); 
      //on met un 0 ds le dernier caract
      //    buf[tmp] = '\0';
      printf("On a receptionne un message de taille %d\n",tmp);   
      m=(message_t*)buf;
      printf("Get Type:%d Len:%d\n",m->type,m->len);
      //m->payload[m->len-1]='\0';
      //printf("Payload:%s\n",m->payload);
      r = (struct caract*)(&(m->payload));
      ///printf("Addr r:%p m:%p\n", r,m);
      
      caract_t r2 = *r;
      print_caract(r2);
      
      waiting_caract = empiler_caract_queue(waiting_caract,r2);
      nb_packet++;
      if(nb_packet >= 10){
	printf("On vide les vannes!\n");
	accept_them_all_caract(ihm_p->ihm_to_core_fd,waiting_caract);
      }
      
      
      /*
      printf("On renvoi TO_ACCEPT au packet:\n");
      message_t m_choix;
      u8* packet_choix;
      //pareil on envoi un choix
      choix_t c = creer_choix(r2.id,TO_ACCEPT);
      m_choix=creer_message_choix(c);
      packet_choix=creer_packet(m_choix);
      printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
      print_choix(c);
      write(ihm_to_core_fd, packet_choix,sizeof(struct choix)+2*sizeof(u16));
      */
      
      //printf("Policy a jour:\n");
      close(ihm_p->core_to_ihm_fd);
    }
  }
    
  close(ihm_p->core_to_ihm_fd);
  close(ihm_p->ihm_to_core_fd);
  
  return 0;
}
