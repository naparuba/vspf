#include "print.h"
#include "common.h"
#include "belial.h"
#include <unistd.h>


belial_p belial_init(){
  belial_p belial = (belial_p)malloc(sizeof(belial_t));
  belial->stats = (belial_stats_p)malloc(sizeof(belial_t));
  
  belial->waiting_work = time_work_asked_list_create();
  belial->work_to_do = time_work_asked_list_create();

  //on lui met un timeout de 1h
  (belial->timeout).tv_sec=3600;
  (belial->timeout).tv_usec=0;
  
  return belial;
}

void belial_exit(belial_p belial){
  time_work_asked_list_flush(belial->waiting_work);
  time_work_asked_list_flush(belial->work_to_do);
  free(belial);
}

void belial_add_time_work(belial_p belial,u16 time,u16 type,u16 data_length,u8* payload){
  time_work_asked_t t = time_work_asked_create(time,type,data_length,payload);
  belial->waiting_work = time_work_asked_list_add(belial->waiting_work,t);  
}

//on parcoure la liste des to_do et on prend le min des time
//si il n'y en a pas on met 5sec
void belial_generate_timeout(belial_p belial){
  (belial->timeout).tv_sec = time_work_asked_list_get_min_time(belial->waiting_work);
}

//on met a jour suivant le temps qui s'est passe depusi le dernier tour
void belial_update_work_from_to_do(belial_p belial , u32 time){
  belial->work_to_do = time_work_asked_list_waiting_to_to_do(belial->waiting_work,
							     belial->work_to_do,
							     time);
  belial->waiting_work = time_work_asked_list_waiting_update(belial->waiting_work,time);
}

//on parcoure la to do list
//on regarde les datas
void belial_do_time_work(belial_p belial){
  time_work_asked_list_p p_temp;
  p_temp = belial->work_to_do;
  while(!time_work_asked_list_is_void(p_temp))
    {
      ul32 packet_id;
      switch((p_temp->t).type){
      case TYPE_TIME_REMOVE_IPQ:
	packet_id = (ul32)*((p_temp->t).payload);
	myprintfDebugSin("On a une action: type = Remove_Ipq, data = %lu\n",(p_temp->t).type,packet_id);
	break;
      default:
	myprintfDebugSin("Type de timeout inconnu\n");
	break;
      }
      p_temp = time_work_asked_list_next(p_temp);
    }
}
