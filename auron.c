#include <unistd.h>
#include "auron.h"
#include "print.h"

/*struct timeval { 
    long    tv_sec;        
    long    tv_usec;       
};
*/


auron_p auron_init(){
  auron_p auron = (auron_p)malloc(sizeof(auron_t));
  auron->stats = (auron_stats_p)malloc(sizeof(auron_t));
  
  auron->waiting_work = time_work_asked_list_create();
  auron->work_to_do = time_work_asked_list_create();

  //on lui met un timeout de 1h
  (auron->timeout).tv_sec=3600;
  (auron->timeout).tv_usec=0;
  
  return auron;
}

void auron_exit(auron_p auron){
  time_work_asked_list_flush(auron->waiting_work);
  time_work_asked_list_flush(auron->work_to_do);
  free(auron);
}

void auron_add_time_work(auron_p auron,u16 time,u16 type,u16 data_length,u8* payload){
  time_work_asked_t t = time_work_asked_create(time,type,data_length,payload);

  auron->waiting_work = time_work_asked_list_add(auron->waiting_work,t);  
}

//on parcoure la liste des to_do et on prend le min des time
//si il n'y en a pas on met 5sec
void auron_generate_timeout(auron_p auron){
  //(auron->timeout).tv_sec = time_work_asked_list_get_min_time(auron->work_to_do);
  (auron->timeout).tv_sec = time_work_asked_list_get_min_time(auron->waiting_work);
  myprintfDebugGuardians("On a mit auron timeout a jour:%d\n",(auron->timeout).tv_sec);
}

//on met a jour suivant le temps qui s'est passe depusi le dernier tour
void auron_update_work_from_to_do(auron_p auron , u32 time){
  myprintfDebugGuardians("Dans Auron::update wl = %p  todo = %p\n",auron->waiting_work,auron->work_to_do);
  u32 size = time_work_asked_list_size(auron->waiting_work);
  myprintfDebugGuardians("La size est de %d\n",size);
  
  //  myprintfDebugGuardians("apres la size\n");
  //deja on rempli la to_do_list
  auron->work_to_do = time_work_asked_list_waiting_to_to_do(auron->waiting_work,
							    auron->work_to_do,
							    time);
  myprintfDebugGuardians("Dans Auron::update::2 wl = %p  todo = %p\n",auron->waiting_work,auron->work_to_do);
  
  //puis on "netoy" la waiting_list
  //myprintfDebugGuardians("On update Auron de %d\n",time);
  myprintfDebugGuardians("Avant le netoyage wl = %p\n",auron->waiting_work);
  auron->waiting_work = time_work_asked_list_waiting_update(auron->waiting_work,time);
  myprintfDebugGuardians("Apres le netoyage wl = %p\n",auron->waiting_work);

  //  myprintfDebugGuardians("Apres Taille waiting:%d\n",time_work_asked_list_size(auron->waiting_work));
  //myprintfDebugGuardians("Apres Taille to_do:%d\n",time_work_asked_list_size(auron->work_to_do));
}

//on parcoure la to do list
//on regarde les datas
/*typedef struct{
  u16 time; // on oublie ici ca sert a rien
  u16 type;
  u16 data_length;
  u8* payload;
} time work*/
void auron_do_time_work(auron_p auron){
  time_work_asked_list_p p_temp;
  p_temp = auron->work_to_do;
  while(!time_work_asked_list_is_void(p_temp))
    {
      myprintfDebugGuardians("On a une action: type = %d, data = %d\n",(p_temp->t).type,(int)*((p_temp->t).payload));
      p_temp = time_work_asked_list_next(p_temp);
    }
}
