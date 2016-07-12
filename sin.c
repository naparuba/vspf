#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "messages.h"
#include "print.h"
#include "sin.h"
#include "ipq_list.h"

#include "guardians.h"

#define BUFSIZE 2048

static sin_t *sin_p;//utilise pour le quit

static void sigquit(int signum){
  myprintfDebugSin("SIGINT, SIGTERM or SIGQUIT catched, Bye Bye\n");
  exit(0);
}

int main(int argc, char **argv){
  
  int fifo_father_to_son_fd;
  int fifo_son_to_father_fd;
  
  int fd_son_to_father[2];
  int fd_father_to_son[2];
  
  if(pipe(fd_father_to_son) != 0){
    /* fd devient descripteur de fichier */
    fprintf(stderr,"Problemes dans l'ouverture de Pipe \n");
    exit(1);
  }
  
  if(pipe(fd_son_to_father) != 0){
    /* fd devient descripteur de fichier */
    fprintf(stderr,"Problemes dans l'ouverture de Pipe \n");
    exit(1);
  }
  
  // Lancement d'un fils
  if(fork() == 0){ 
    /* Processus Fils 1 */
    fifo_father_to_son_fd = fd_father_to_son[0];
    fifo_son_to_father_fd = fd_son_to_father[1];
    
    close(fd_father_to_son[1]);
    close(fd_son_to_father[0]);
    
    return guardians_thread(fifo_son_to_father_fd,fifo_father_to_son_fd);
  }else{//on est ds le pere  
    //attention, interversion ici
    fifo_father_to_son_fd = fd_father_to_son[1];
    fifo_son_to_father_fd = fd_son_to_father[0];
    
    close(fd_father_to_son[0]);
    close(fd_son_to_father[1]);
    
    return sin_thread(fifo_son_to_father_fd,fifo_father_to_son_fd);
  }
}

int sin_thread(int fifo_son_to_father_fd, int fifo_father_to_son_fd){
  sin_t sin;      
  /* set up signal handlers to exit nicely when needed */
  sin_p = &sin;
  signal(SIGINT, sigquit);
  signal(SIGTERM, sigquit);
  signal(SIGQUIT, sigquit);
  
  sin_init(&sin);
  
  //moche
  (sin.kira)->guardians_to_sin_fd = fifo_son_to_father_fd;
  (sin.kira)->sin_to_guardians_fd = fifo_father_to_son_fd;
  
  myprintfDebugSin("SIN:guardians->sin = %d\nSIN:sin->guardians = %d\n",
	 (sin.kira)->guardians_to_sin_fd,
	 (sin.kira)->sin_to_guardians_fd);
  
  myprintfDebugSin("Waiting for queued packets ...\n");
  
  do{
    fd_set read_fds;
    
    struct timeval t_before_select;
    struct timezone tz;
    
    gettimeofday(&t_before_select, &tz);
    
    int r;
    /* watch ipq fd and l'autre fd */
    if ( (r = sin_wait_message(&sin, &read_fds)) > 0)
      sin_handle_message(&sin, &read_fds , &t_before_select);
    else{
      if(r==0){
	myprintfDebugSin("On a un timeout\n");
	//Pas besoin de faire le diff avec t_before, on gere les action que demande auron
	//puis on lui fait regenerer son timeval interne
	//on replace les elements de auron->working dans work_to_do si besoin
	//on met a jour working car il s est passe auron->timeout.tv_sec 
	//secondes depuis le dernier tour
	belial_update_work_from_to_do(sin.belial, ((sin.belial)->timeout).tv_sec);
	sin_do_time_work(&sin);
	belial_generate_timeout(sin.belial);
      }else{
	printf("On a une erreur dans le select\n");
      }
    }
  }while (1);
  
  sin_exit(&sin, EXIT_SUCCESS);
  return 0;
}

void sin_exit(sin_t *sin, int exit_code) {
  kira_exit(sin->kira);
  rochel_exit(sin->rochel);
  itachi_exit(sin->itachi);
  belial_exit(sin->belial);
  
  myprintfDebugSin("Sorti par sin exit\n");
  exit(exit_code);
}

int sin_init(sin_t *sin) {
  int status;
  
  sin->kira = kira_init();
  kira_p kira = sin->kira;
  
  sin->itachi = itachi_init();
  //itachi_p itachi = sin->itachi;
  
  sin->rochel = rochel_init();
  //rochel_p rochel = sin->rochel;

  sin->belial = belial_init();
  
  kira->ipq_h = ipq_create_handle(0, PF_INET);
  if (!kira->ipq_h) {
    ipq_perror("passer");
    sin_exit(sin, EXIT_FAILURE);
  }
  
  status = ipq_set_mode(kira->ipq_h, IPQ_COPY_PACKET, BUFSIZE);
  if (status < 0) {
    ipq_perror("passer");
    sin_exit(sin, EXIT_FAILURE);
  } 
  
  //on ouvre pas le pipe, on le fait deja plus loin
    
  //on ouvre la communication vers Guardians
  //kira_open_sin_to_guardians(kira);
  
  myprintfDebugSin("Fin d initialisation\n");
  return 0;
}

int sin_wait_message(sin_t *sin, fd_set *read_fds) {
  int tmp;
  FD_ZERO(read_fds);
  //on regarde les ipq
  FD_SET(sin->kira->ipq_h->fd, read_fds);
  kira_p kira = sin->kira;
  
  //Un par interface avec guardians
  //on re-ouvre le pipe nomme sin_to_guardians
  //kira_open_guardians_to_sin(kira);
  
  //on regarde ce qui vient de guardians
  kira_declare_guardians_to_sin(kira,read_fds);
  
  //on prend le max +1 des deux fd
  tmp=MAX(kira->ipq_h->fd,kira->guardians_to_sin_fd)+1;

  struct timeval t = sin->belial->timeout;
  myprintfDebugSin("On pose un time out de %lu\n",t.tv_sec);
  
  return select(tmp, read_fds, NULL, NULL,&t);
}

void sin_handle_message(sin_t *sin, fd_set *read_fds,struct timeval* t_before_select) {
  struct timeval t_after_select;
  struct timezone tz;  
  int diff;  
  gettimeofday(&t_after_select, &tz);
  diff=(t_after_select.tv_sec - (*t_before_select).tv_sec);
  //on change le timeout de t en enlevant le diff
  // comme ca c'est comme si l'action ne s'etait pas passee
  //normalement on a un temps "positif" sinon on a un gros gros 
  (sin->belial->timeout).tv_sec -= diff;
  
  //on replace les elements de auron->working dans work_to_do si besoin
  belial_update_work_from_to_do(sin->belial,diff);
  

  //si on s'apercoit qu'on va avoir les deux actions a faire alors bah on la fait et 
  //on regenere le timeout
  if((sin->belial->timeout).tv_sec < 0){
    (sin->belial->timeout).tv_sec = 0;
    sin_do_time_work(sin);
    belial_generate_timeout(sin->belial);
  }
  
  if (FD_ISSET(sin->kira->ipq_h->fd, read_fds)) {
    sin_handle_ipq(sin);
  }
  
  if (FD_ISSET(sin->kira->guardians_to_sin_fd, read_fds)) {
    sin_handle_guardians(sin);
  }
}

void sin_handle_guardians(sin_t *sin) {
  int nbytes;
  char buf[BUFSIZE];
  int size;
  message_t *m;
  kira_p kira = sin->kira;
  
  nbytes = kira_read_guardians(kira,buf);
  if(nbytes < 0){
    myprintfDebugSin("Problemes dans la lecture de %s\n",FIFO_GUARDIANS_TO_SIN);
  }else{
    size = MIN(nbytes,BUFSIZE);//?nbytes:BUFSIZE); 
    myprintfDebugSin("On a receptionne un message de taille %d\n",size);   
    
    if(size != 0){
      m=(message_t*)buf;
      myprintfDebugSin("Get Type:%d Len:%d\n",m->type,m->len);
      switch(m->type){
	choix_t *c,c2,c3;
      case TYPE_RULE:
	myprintfDebugSin("On a recu une regle, pour l'instant on ne prends pas ca en compte\n");
	break;
      case TYPE_CHOIX:
	c = (struct choix*)(&(m->payload));
        c2 = *c;
	c3 = copy_choix(c2);
	
	print_choix(c3);
	ipq_list_apply(kira->ipq_h,sin->itachi->queued_packets,c3);
	break;
      default:
	myprintfDebugSin("Type non reconnu\n");
	break;
      }//fin du switch
    }else{//si on a zero
      myprintfDebugSin("On a recu un message de taille nule, on le drop..\n");
    }
  }
  //on close le fd de guardians_to_sin
  //enleve pour les pipes
  /*
    kira_close_guardians_to_sin(kira);
  */
  return;
}

void sin_handle_ipq(sin_t *sin) {
  unsigned char buf[BUFSIZE];
  int status;
  kira_p kira = sin->kira;
  itachi_p itachi = sin->itachi;
  belial_p belial = sin->belial;

  status = kira_ipq_read(kira,buf);
  if (status < 0) {
    sin_exit(sin, EXIT_FAILURE);
  }
  
  switch (ipq_message_type(buf)) {
  case NLMSG_ERROR:
    fprintf(stderr, "Received error message %d\n", ipq_get_msgerr(buf));
    fprintf(stderr, "Erreur:%s\n",ipq_errstr());
    break;
    
  case IPQM_PACKET: {
    ipq_packet_msg_t *m = kira_ipq_get_packet(buf);
    myprintfDebugSin("\n***************Paquet intercepte:****************\n");
    
    //on empile et on va prevenir l'guardians
    //ATTENTION: surement recopier memcpy les iqp packet!!
    ipq_packet_msg_t * m_cpy = itachi_add_ipq(itachi,m);
    size_t payload_len = sizeof(ipq_packet_msg_t) + m->data_len;
    kira_send_smth_to_guardians(kira,TYPE_IPQ,payload_len,(u8*)m_cpy);
    
    //on ajoute le paquet ipq pour supression dans 1min=60s
    ul32 packet_id = m_cpy->packet_id;
    myprintfDebugSin("PacketId = %lu\n",packet_id);
    belial_add_time_work(belial,
			 TIME_REMOVE_IPQ ,
			 TYPE_TIME_REMOVE_IPQ,
			 sizeof(ul32),
			 (u8*)(&packet_id));
    belial_generate_timeout(belial);
    
    break;
  }//fin IPQ_PACKET
    
  default:
    fprintf(stderr, "Unknown message type!\n");
    break;
  }
}

void sin_do_time_work(sin_t *sin){
  belial_p belial = sin->belial;
  int nbAction = time_work_asked_list_size(belial->work_to_do);

  if(nbAction){
    myprintfDebugSin("Il y a %d actions a faire, donc on les vides\n",nbAction);
    belial_do_time_work(sin->belial);
    time_work_asked_list_flush(belial->work_to_do);
    belial->work_to_do = time_work_asked_list_create();
  }  
}
