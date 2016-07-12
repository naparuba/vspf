#include "ipq_list.h"
#include "print.h"
#include "rule.h"

#include <stdlib.h>
#include <stdio.h>


//O(1)
ipq_list_p ipq_list_create(void){
  return NULL;
}

//O(1)
ipq_list_p ipq_list_add(ipq_list_p p, ipq_packet_msg_t * m){
  ipq_list_p tmp;
  if ((tmp=(ipq_list_p)malloc(sizeof(ipq_list_t)))==NULL)
    {
      printf("impossible d'allouer la memoire");
      return (ipq_list_p)EXIT_FAILURE;
    }
  tmp->m = m;
  tmp->prec = p;
  return p=tmp;
}

//O(1)
ipq_list_p ipq_list_remove(ipq_list_p p){
  ipq_list_p tmp;
  if (!ipq_list_is_void(p))
    {
      tmp=p->prec;
      free(p);
      //TODO: gerer le paquet -> DROP je suppose
      return p=tmp;
    }else{
      printf("impossible de depiler une pile vide");
      return (ipq_list_p)EXIT_FAILURE;
    }
}

//O(1)
ipq_packet_msg_t * ipq_list_get(ipq_list_p p){
  if (!ipq_list_is_void(p))
    return p->m;
  return NULL;
}

//O(1)
ipq_list_p ipq_list_next(ipq_list_p p){
  if(!ipq_list_is_void(p))
    return p->prec;
  return NULL;
}

//O(1)
int ipq_list_is_void(ipq_list_p p){
  return p==NULL;
}

//O(Taille p)
void ipq_list_print(ipq_list_p p){
  ipq_list_p p_temp;
  p_temp=p;
  while(!ipq_list_is_void(p_temp))
    {
      print_ip(ipq_list_get(p_temp));
      p_temp=ipq_list_next(p_temp);
    }
  return;
}

//O(Taille p)
u32 ipq_list_size(ipq_list_p p){
  ipq_list_p p_temp;
  u32 i=0;
  p_temp=p;
  while(!ipq_list_is_void(p_temp))
    {
      i++;
      p_temp=ipq_list_next(p_temp);
    }
  return i;
}

//O(Taille p)
void ipq_list_flush(ipq_list_p p){
  while(!ipq_list_is_void(p))
    {p=ipq_list_remove(p);}
  return;
}

void ipq_list_accept_all(struct ipq_handle *h, ipq_list_p p){
  int status;
  ipq_list_p p_temp;
  p_temp = p;
  while(!ipq_list_is_void(p_temp))
    {
      printf("On accept: %lu\n",p_temp->m->packet_id);
      status = ipq_set_verdict(h, p_temp->m->packet_id,NF_ACCEPT, 0, NULL);
      if (status < 0) {
	ipq_perror("passer");
	exit(1);
      }
      printf("On drop un paquet\n");
      p_temp=ipq_list_next(p_temp);
    }
  return;
}

void ipq_list_drop_all(struct ipq_handle *h, ipq_list_p p){
  int status;
  ipq_list_p p_temp;
  p_temp = p;
  while(!ipq_list_is_void(p_temp))
    {
      printf("On drop: %lu\n",p_temp->m->packet_id);

      status = ipq_set_verdict(h, p_temp->m->packet_id,NF_DROP, 0, NULL);
      if (status < 0) {
	ipq_perror("passer");
	exit(1);
      }
      printf("On drop un paquet\n");
      p_temp=ipq_list_next(p_temp);
    }
  return;
}

void ipq_list_apply(struct ipq_handle *h, ipq_list_p p, choix_t c){
  int status;
  ipq_list_p p_temp;
  p_temp = p;
  
  //si le verdict ne convient pas on quitte
  if(c.verdict != TO_DROP && c.verdict != TO_ACCEPT){
    printf("Verdict not matched pour %lu\n",c.ipq_id);
    return;
  }
  
  while(!ipq_list_is_void(p_temp))
    {//on regarde si on a le paquet qu'il faut
      // il n'y en a qu'un
      if(p_temp->m->packet_id == c.ipq_id){
	if(c.verdict == TO_DROP){
	  printf("On drop: %lu\n",p_temp->m->packet_id);
	  status = ipq_set_verdict(h, p_temp->m->packet_id,NF_DROP, 0, NULL);
	}
	if(c.verdict == TO_ACCEPT){
	  printf("On accept: %lu\n",p_temp->m->packet_id);
	  status = ipq_set_verdict(h, p_temp->m->packet_id,NF_ACCEPT, 0, NULL);
	}
	if (status < 0) {
	  ipq_perror("passer");
	  exit(1);
	}
	//un seul peut matcher, on quitte
	return;
      }
      p_temp=ipq_list_next(p_temp);
    }
  printf("Aucun packet en attente n'a matche\n");
  return;
}
