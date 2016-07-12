#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "messages.h"
#include "packet.h"
#include "print.h"
#include "caract.h"
#include "rule.h"
#include "rule_list.h"
#include "tidus.h"
#include "kimari.h"
#include "yuna.h"
#include "auron.h"
#include "guardians.h"


#define BUFSIZE 2048



static guardians_t *guardians_p;
static void sigquit(int signum);

static void sigquit(int signum){
  myprintfDebugGuardians("SIGINT, SIGTERM or SIGQUIT catched, Bye Bye\n");
  guardians_exit(guardians_p, EXIT_SUCCESS);
  exit(0);
}

int give_up_root(void) 
{
  /* give up root and get the real uid*/
  uid_t uid;
  int err;
  
  err=seteuid(1000);
  uid=getuid();
  
  myprintfDebugGuardians("UID: %d\nErr:%d\n",(int)uid,err);  
  
  return (err);
}


int guardians_thread(int fifo_son_to_father_fd, int fifo_father_to_son_fd){
  guardians_t guardians;
  
  myprintfDebugGuardians("GUARD: Sin -> guardians %d\nGUARD: guardians -> sin %d\n",
	 fifo_father_to_son_fd,
	 fifo_son_to_father_fd);
  
  give_up_root();
  
  /* set up signal handlers to exit nicely when needed */
  guardians_p = &guardians;
  signal(SIGINT, sigquit);
  signal(SIGTERM, sigquit);
  signal(SIGQUIT, sigquit);
  
  guardians_init(&guardians);
  
  //ultra moche
  (guardians.yuna)->guardians_to_sin_fd = fifo_son_to_father_fd;
  (guardians.yuna)->sin_to_guardians_fd = fifo_father_to_son_fd;
  
  myprintfDebugGuardians("Waiting for questionning ...\n");
  
  do{
    fd_set read_fds;
    int r;
    
    //debug
    int i=32;
    myprintfDebugGuardians("On pose une demande dans 5sec\n");
    //on fait tjs un generate timeout quand on ajoute un time work de 45s
    
    if((guardians.auron->timeout).tv_sec > 2){
      //auron_add_time_work(guardians.auron,25,1,sizeof(int),(u8*)(&i));
    }
    auron_generate_timeout(guardians.auron);
    myprintfDebugGuardians("On a un timeout dans auron de %d\n",(guardians.auron->timeout).tv_sec);

    struct timeval t_before_select;
    struct timezone tz;
    
    gettimeofday(&t_before_select, &tz);
    
    /* watch sin fd and ihm fd */
    if( (r=guardians_wait_message(&guardians, &read_fds)) > 0)
      guardians_handle_message(&guardians, &read_fds, &t_before_select);
    else{
      if(r==0){
	myprintfDebugGuardians("C'est un vrai temps\n");
	//Pas besoin de faire le diff avec t_before, on gere les action que demande auron
	//puis on lui fait regenerer son timeval interne
	//on replace les elements de auron->working dans work_to_do si besoin
	//on met a jour working car il s est passe auron->timeout.tv_sec 
	//secondes depuis le dernier tour
	printf("On update dans main de %d s\n",(guardians.auron->timeout).tv_sec);
	printf("On demdane un size de waiting_work\n");
	u32 size = time_work_asked_list_size((guardians.auron)->waiting_work);
	printf("On apelle le work from to do\n");
	auron_update_work_from_to_do(guardians.auron, ((guardians.auron)->timeout).tv_sec);
	guardians_do_time_work(&guardians);
	auron_generate_timeout(guardians.auron);
      }else{
	printf("On a une erreur dans le select\n");
      }
    }
  }while (1);
  
  guardians_exit(&guardians, EXIT_SUCCESS);
  return 0;
}


void guardians_exit(guardians_t *guardians, int exit_code) {
  yuna_exit(guardians->yuna);
  auron_exit(guardians->auron);
  kimari_exit(guardians->kimari);
  tidus_exit(guardians->tidus);

  exit(exit_code);
}


int guardians_init(guardians_t *guardians) {  
  guardians->tidus = tidus_init();
  guardians->yuna = yuna_init();
  guardians->kimari = kimari_init();

  guardians->auron = auron_init();

  guardians->wakka = wakka_init(guardians->tidus->stats,
				guardians->yuna->stats,
				guardians->kimari->stats,
				guardians->auron->stats);
  
  return 0;
}


int guardians_wait_message(guardians_t *guardians, fd_set *read_fds) {
  int tmp;
  FD_ZERO(read_fds);
  yuna_p yuna = guardians->yuna;
  
  yuna_open_sin_to_guardians(yuna);
  yuna_declare_sin_to_guardians(yuna,read_fds);
  
  yuna_open_ihm_to_guardians(yuna);
  yuna_declare_ihm_to_guardians(yuna,read_fds);
  
  //on prend le max+1 des deux fd
  tmp=MAX(yuna->sin_to_guardians_fd,yuna->ihm_to_guardians_fd)+1;
  
  struct timeval t = guardians->auron->timeout;
  /*
    t.tv_sec = 5;
    t.tv_usec = 0;
  */
  
  myprintfDebugGuardians("On pose un time out de %lu\n",t.tv_sec);
  
  return select(tmp, read_fds, NULL, NULL, &t);
}


void guardians_handle_message(guardians_t *guardians, 
			      fd_set *read_fds,
			      struct timeval* t_before_select) {
  int isTime = 1;
  struct timeval t_after_select;
  struct timezone tz;  
  int diff;  
  gettimeofday(&t_after_select, &tz);
  diff=(t_after_select.tv_sec - (*t_before_select).tv_sec);
  //on change le timeout de t en enlevant le diff
  // comme ca c'est comme si l'action ne s'etait pas passee
  //normalement on a un temps "positif" sinon on a un gros gros 
  //pb kelke part
  //printf("Le timeout etait de %i\n",(guardians->auron->timeout).tv_sec);
  //printf("On applique un diff de %i\n",diff);
  (guardians->auron->timeout).tv_sec -= diff;
  
  //on replace les elements de auron->working dans work_to_do si besoin
  auron_update_work_from_to_do(guardians->auron,diff);
  

  //si on s'apercoit qu'on va avoir les deux actions a faire alors bah on la fait et 
  //on regenere le timeout
  if((guardians->auron->timeout).tv_sec < 0){
    myprintfDebugGuardians("On pose a zero\n");
    (guardians->auron->timeout).tv_sec = 0;
    guardians_do_time_work(guardians);
    auron_generate_timeout(guardians->auron);
  }
  
  myprintfDebugGuardians("Maintenant le timeout est de %i\n",(guardians->auron->timeout).tv_sec);
  
  if (FD_ISSET(guardians->yuna->sin_to_guardians_fd, read_fds)) {
    isTime = 0;
    guardians_handle_sin(guardians);
  }
  
  if (FD_ISSET(guardians->yuna->ihm_to_guardians_fd, read_fds)) {
    isTime = 0;
    guardians_handle_ihm(guardians);
  }
  if(isTime == 1)
    myprintfDebugGuardians("C'est un temps \n");
}


void guardians_handle_ihm(guardians_t *guardians) {
  int nbytes;
  char buf[BUFSIZE];
  int tmp;
  message_t *m;
  yuna_p yuna = guardians->yuna;
  kimari_p kimari = guardians->kimari;
  
  nbytes = yuna_read_ihm(yuna,buf);
  if(nbytes < 0){
    myprintfDebugGuardians("Problemes dans la lecture de %s\n",FIFO_IHM_TO_GUARDIANS);
  }else{
    tmp = MIN(nbytes,BUFSIZE);
    myprintfDebugGuardians("On a receptionne un message de taille %d\n",tmp);   
    
    if(tmp != 0){
      m = (message_t*)buf;
      myprintfDebugGuardians("Get Type:%d Len:%d\n",m->type,m->len);
      
      if(m->type == TYPE_RULE){
	rule_t *r;
	r = (struct rule*)(&(m->payload));
	rule_t r2 = *r;
      	rule_t r3 = copy_rule(r2);
	
	kimari_add_rule(kimari,r3);
	myprintfDebugGuardians("Nouvelle regle recue de ihm:\n");
	print_rule(r3);
      }
      
      if(m->type == TYPE_CHOIX){
	myprintfDebugGuardians("On recoi un choix, on le transmet au sin\n");
	//a faire une verif sur ce point car la c'est moche
	yuna_write_to_sin(yuna,buf,tmp);
      }
      
      if(m->type == TYPE_GET_LISTE_RULE){
	//on re recu une demande de listage des regles
	//il n'y a pas de payload, a priori..
	myprintfDebugGuardians("On a recu une demande de listage des rules\n");
	
	rule_t * rule_tab = create_tab_from_rule_list(guardians->kimari->policy);
	int size = rule_list_size(guardians->kimari->policy);
	
	yuna_send_smth_to_ihm(yuna, TYPE_LISTE_RULE,size*sizeof(struct rule),(u8*)(rule_tab));
	
	//on libere le tableau des regles
	free(rule_tab);
      }
      
      if(m->type == TYPE_REMOVE_RULE){
	//on re recu une demande de supression de rule
	//payload a la taille u32 normalement
	u32 id;
	u32 *id_p = (u32*)(&(m->payload));
	id = *id_p;
	myprintfDebugGuardians("On a recu une demande de supression de regles: %d\n",id);
	kimari_delete_rule(kimari,id);
      }
    }else{//si on a zero
      myprintfDebugGuardians("On a recu un message de taille nule, on le drop...\n");
    }
  }
  //on close le fd de ihm_to_guardians
  yuna_close_ihm_to_guardians(yuna);
  return;
}


void guardians_handle_sin(guardians_t *guardians) {
  unsigned char buf[BUFSIZE];
  int status;
  message_t *m;
  yuna_p yuna = guardians->yuna;
  kimari_p kimari = guardians->kimari;
  tidus_p tidus = guardians->tidus;

  status = yuna_read_sin(yuna, buf);
  
  m=(message_t*)buf;
  myprintfDebugGuardians("Get Type:%d Len:%d\n",m->type,m->len);

  switch(m->type){
    ipq_packet_msg_t * m_ini;
  case TYPE_IPQ:
    m_ini = (ipq_packet_msg_t*)(&(m->payload));
    
    myprintfDebugGuardians("\n***************Paquet intercepte:****************\n");
    print_ipq(m_ini);
    
    caract_t c = tidus_create_caract_from_ipq(tidus,m_ini);
    //print_caract(c);
    
    unsigned short res;
    if( (res = kimari_is_matched_caract(kimari,c) ) == NOT_MATCHED){
      myprintfDebugGuardians("Non dans la policy\n");
      yuna_send_smth_to_ihm(yuna,TYPE_CARACT,sizeof(struct caract),(u8*)&c);
      
    }else{//Si ca a matche
      if(res == TO_DROP){
	myprintfDebugGuardians("A matche avec: DROP\n");
	myprintfDebugGuardians("Numero du paquet a DROPER: %lu\n", c.id );
	
	choix_t ch = creer_choix(c.id,TO_DROP);
	yuna_send_smth_to_sin(yuna,TYPE_CHOIX,sizeof(choix_t),(u8*)&ch);
      }else{
	myprintfDebugGuardians("A matche avec: ACCEPT\n");
	myprintfDebugGuardians("Numero du paquet a ACCEPTER: %lu\n", c.id );

	//on genere le choix et on le passe a sin pour application
	choix_t ch = creer_choix(c.id,TO_ACCEPT);
	yuna_send_smth_to_sin(yuna,TYPE_CHOIX,sizeof(choix_t),(u8*)&ch);
      }
    }
    break;
  default:
    myprintfDebugGuardians("Type non ipq\n");
    break;
  }//fin du switch
}


void guardians_do_time_work(guardians_t *guardians){
  auron_p auron = guardians->auron;
  
  myprintfDebugGuardians("On fait le do_time_work\n");
  
  int nbAction = time_work_asked_list_size(auron->work_to_do);
  if(nbAction){
    myprintfDebugGuardians("Il y a %d actions a faire, donc on les vides\n",nbAction);
    auron_do_time_work(guardians->auron);
    time_work_asked_list_flush(auron->work_to_do);
    auron->work_to_do = time_work_asked_list_create();
  }
  
}
