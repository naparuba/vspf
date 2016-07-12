#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "time_work_asked_list.h"


//O(1)
time_work_asked_list_p time_work_asked_list_create(void){
  return NULL;
}

//O(1)
time_work_asked_list_p time_work_asked_list_add(time_work_asked_list_p p, time_work_asked_t t){
  time_work_asked_list_p tmp;
  if ((tmp=(time_work_asked_list_p)malloc(sizeof(time_work_asked_list_t)))==NULL)
    {
      printf("impossible d'allouer la memoire");
      return (time_work_asked_list_p)EXIT_FAILURE;
    }
  tmp->t = t;
  tmp->prec = p;
  return p=tmp;
}

//O(1)
time_work_asked_list_p time_work_asked_list_remove(time_work_asked_list_p p){
  time_work_asked_list_p tmp;
  if (!time_work_asked_list_is_void(p))
    {
      tmp=p->prec;
      //on netoye le t egalement
      time_work_asked_remove(p->t);
      free(p);
      //TODO: gerer le paquet -> DROP je suppose
      return p = tmp;
    }else{
      printf("impossible de depiler une pile vide");
      return (time_work_asked_list_p)EXIT_FAILURE;
    }
}

//O(1)
//FIXE: Heu si on gerai l erreur?
time_work_asked_t time_work_asked_list_get(time_work_asked_list_p p){
  time_work_asked_t t;
  if (!time_work_asked_list_is_void(p))
    return p->t;
  return t;
}

//O(1)
time_work_asked_list_p time_work_asked_list_next(time_work_asked_list_p p){
  if(!time_work_asked_list_is_void(p))
    return p->prec;
  return NULL;
}

//O(1)
int time_work_asked_list_is_void(time_work_asked_list_p p){
  return p==NULL;
}

//O(Taille p)
void time_work_asked_list_print(time_work_asked_list_p p){
  time_work_asked_list_p p_temp;
  p_temp=p;
  while(!time_work_asked_list_is_void(p_temp))
    {
      //print_time_work_asked(time_work_asked_list_get(p_temp));
      p_temp = time_work_asked_list_next(p_temp);
    }
}

//O(Taille p)
u32 time_work_asked_list_size(time_work_asked_list_p p){
  time_work_asked_list_p p_temp;
  u32 i = 0;
  p_temp = p;
  while(!time_work_asked_list_is_void(p_temp))
    {
      i++;
      p_temp = time_work_asked_list_next(p_temp);
    }
  return i;
}

//O(Taille p)
void time_work_asked_list_flush(time_work_asked_list_p p){
  while(!time_work_asked_list_is_void(p))
    {p=time_work_asked_list_remove(p);}
  return;
}


time_work_asked_list_p time_work_asked_list_waiting_to_to_do(time_work_asked_list_p wl,
							     time_work_asked_list_p tdl,
							     u32 time){
  //on parcoure la liste waiting_work, si on a un time <=0
  //alors on place le t dans work_to_do
  //sinon on decremente le time
  time_work_asked_list_p wl_temp = wl;
  while(!time_work_asked_list_is_void(wl_temp))
    {
      if( (wl_temp->t).time <=time){
	time_work_asked_t t = time_work_asked_create((wl_temp->t).time,
						     (wl_temp->t).type,
						     (wl_temp->t).data_length,
						     (wl_temp->t).payload);
	tdl = time_work_asked_list_add(tdl,t);
	printf("On ajoute un travail a todo\n");
      }else{}
      wl_temp=time_work_asked_list_next(wl_temp);
    }
  return tdl;
}


//refaite en recursif
time_work_asked_list_p time_work_asked_list_waiting_update(time_work_asked_list_p wl, u32 time){
  if(time_work_asked_list_is_void(wl))
    return wl;
  
  if( (wl->t).time <= time){
    wl = time_work_asked_list_remove(wl);
    return time_work_asked_list_waiting_update(time_work_asked_list_next(wl),time);
  }else{
    (wl->t).time -= time;
    wl->prec = time_work_asked_list_waiting_update(time_work_asked_list_next(wl),time);
    return wl;
  }
}


//O(Taille p)
u32 time_work_asked_list_get_min_time(time_work_asked_list_p p){
  u32 res = 5;
  time_work_asked_list_p p_temp;
  p_temp=p;
  while(!time_work_asked_list_is_void(p_temp))
    {
      u32 time_temp;
      if( (time_temp =  (p->t).time)< res)
	res = time_temp;
      p_temp=time_work_asked_list_next(p_temp);
    }
  return res;
}
