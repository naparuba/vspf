#include "caract_list.h"
#include "print.h"
#include "rule.h"

#include <stdlib.h>
#include <stdio.h>


//O(1)
caract_list_p caract_list_create(void){
  return NULL;
}

//O(1)
caract_list_p caract_list_add(caract_list_p p, caract_t c){
  caract_list_p tmp;
  if ((tmp=(caract_list_p)malloc(sizeof(caract_queue)))==NULL)
    {
      printf("impossible d'allouer la memoire");
      return (caract_list_p)EXIT_FAILURE;
    }
  tmp->c = c;
  tmp->prec = p;
  return p=tmp;
}

//O(1)
caract_list_p caract_list_remove(caract_list_p p){
  caract_list_p tmp;
  if (!caract_list_is_void(p))
    {
      tmp=p->prec;
      free(p);
      //TODO: gerer le paquet -> DROP je suppose
      return p=tmp;
    }else{
      printf("impossible de depiler une pile vide");
      return (caract_list_p)EXIT_FAILURE;
    }
}

//O(1)
//FIXE: Heu si on gerai l erreur?
caract_t caract_list_get(caract_list_p p){
  caract_t c;
  if (!caract_list_is_void(p))
    return p->c;
  return c;
}

//O(1)
caract_list_p caract_list_next(caract_list_p p){
  if(!caract_list_is_void(p))
    return p->prec;
  return NULL;
}

//O(1)
int caract_list_is_void(caract_list_p p){
  return p==NULL;
}

//O(Taille p)
void caract_list_print(caract_list_p p){
  caract_list_p p_temp;
  p_temp=p;
  while(!caract_list_is_void(p_temp))
    {
      print_caract(caract_list_get(p_temp));
      p_temp=caract_list_next(p_temp);
    }
  return;
}

//O(Taille p)
u32 caract_list_size(caract_list_p p){
  caract_list_p p_temp;
  u32 i=0;
  p_temp=p;
  while(!caract_list_is_void(p_temp))
    {
      i++;
      p_temp=caract_list_next(p_temp);
    }
  return i;
}

//TODO: renommage
//O(Taille p)
caract_list_p caract_list_remove_from_id(caract_list_p p, unsigned int id){
  caract_list_p res = p;
  caract_list_p p_temp = p;
  caract_list_p p_prec = NULL;
  caract_list_p p_temp2 = NULL;
  
  while(!caract_list_is_void(p_temp))
    {
      if( (p_temp->c).id == id){//si on chope le bon element
	//on verifie si on est ds le permeier element
	if(p_temp == p){// si on est ds le permier element
	  //on free et renvoi le deuxieme element comme nouvelle pile
	  res = caract_list_next(p_temp);
	  free(p_temp);
	  return res;
	}else{//on est pas ds le premier
	  //donc on libère
	  p_temp2 = caract_list_next(p_temp);
	  free(p_temp);
	  p_prec->prec = p_temp2; 
	  return res;
	}
      }else{
	p_prec = p_temp;
	p_temp = caract_list_next(p_temp);
      }
    }
  return res;
}

//O(Taille p)
void caract_list_flush(caract_list_p p){
  while(!caract_list_is_void(p))
    {p=caract_list_remove(p);}
  return;
}


void caract_list_accept_all(int fd, caract_list_p p){
  int status;
  caract_list_p p_temp;
  p_temp = p;
  while(!caract_list_is_void(p_temp))
    {
      printf("On renvoi TO_ACCEPT au packet:\n");
      message_t m_choix;
      u8* packet_choix;
      //pareil on envoi un choix
      choix_t c = creer_choix( (p_temp->c).id,TO_ACCEPT);
      m_choix=creer_message_choix(c);
      packet_choix=creer_packet(m_choix);
      printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
      print_choix(c);
      status=write(fd, packet_choix,sizeof(struct choix)+2*sizeof(u16));
      
      //FIXE ME: enlever le sync, c'est trop violent
      sync();
      printf("Status: %d\n",status);
      /*
      printf("On accept: %lu\n",p_temp->m->packet_id);
      status = ipq_set_verdict(h, p_temp->m->packet_id,NF_ACCEPT, 0, NULL);
      if (status < 0) {
	ipq_perror("passer");
	exit(1);
      }
      printf("On accepte un paquet\n");
      */
      //printf("Non implemente\n");
      p_temp=caract_list_next(p_temp);
    }
  return;
}

void caract_list_drop_all(int fd, caract_list_p p){
  int status;
  caract_list_p p_temp;
  p_temp = p;
  while(!caract_list_is_void(p_temp))
    {
      printf("On renvoi TO_ACCEPT au packet:\n");
      message_t m_choix;
      u8* packet_choix;
      //pareil on envoi un choix
      choix_t c = creer_choix( (p_temp->c).id,TO_DROP);
      m_choix=creer_message_choix(c);
      packet_choix=creer_packet(m_choix);
      printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
      print_choix(c);
      status=write(fd, packet_choix,sizeof(struct choix)+2*sizeof(u16));
      
      //FIXE ME: enlever le sync, c'est trop violent
      sync();
      printf("Status: %d\n",status);
      
      /*
      printf("On drop: %lu\n",p_temp->m->packet_id);

      status = ipq_set_verdict(h, p_temp->m->packet_id,NF_DROP, 0, NULL);
      if (status < 0) {
	ipq_perror("passer");
	exit(1);
      }
      printf("On drop un paquet\n");
      */
      //printf("Non implemente\n");
      p_temp=caract_list_next(p_temp);
    }
  return;
}

caract_list_p caract_list_apply(int fd, caract_list_p p, choix_t c){
  int status;
  caract_list_p p_temp;
  p_temp = p;
  
  printf("Non implemente\n");
  
  
  //si le verdict ne convient pas on quitte
  if(c.verdict != TO_DROP && c.verdict != TO_ACCEPT){
    //    printf("Verdict not matched pour %lu\n",c.ipq_id);
    return;
  }
  
  while(!caract_list_is_void(p_temp))
    {//on regarde si on a le paquet qu'il faut
      // il n'y en a qu'un
      if( (p_temp->c).id == c.ipq_id){
	if(c.verdict == TO_DROP){
	  printf("On drop: %lu\n",(p_temp->c).id);
	  //status = ipq_set_verdict(h, p_temp->m->packet_id,NF_DROP, 0, NULL);
	}
	if(c.verdict == TO_ACCEPT){
	  printf("On accept: %lu\n",(p_temp->c).id);
	  //status = ipq_set_verdict(h, p_temp->m->packet_id,NF_ACCEPT, 0, NULL);
	}



	message_t m_choix;
	u8* packet_choix;
	//pareil on envoi un choix
	m_choix=creer_message_choix(c);
	packet_choix=creer_packet(m_choix);
	printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
	print_choix(c);
	status=write(fd, packet_choix,sizeof(struct choix)+2*sizeof(u16));
	
	//FIXE ME: enlever le sync, c'est trop violent
	sync();
	printf("Status: %d\n",status);
      

	p=caract_list_remove_from_id(p,(p_temp->c).id);


	if (status < 0) {
	  printf("Erreur dans l'ecriture\n");
	  exit(1);
	}
	//un seul peut matcher, on quitte
	return p;
      }
      p_temp=caract_list_next(p_temp);
    }
  printf("Aucun packet en attente n'a matche\n");
  
  
  return p;
}


caract_t caract_list_get_from_id(caract_list_p p, unsigned long id){
  caract_list_p p_temp = p;
  caract_t c;
  
  while(!caract_list_is_void(p_temp))
    {//on regarde si on a le paquet qu'il faut
      // il n'y en a qu'un
      if( (p_temp->c).id == id)
	return p_temp->c;
      p_temp=caract_list_next(p_temp);
    }
  return c;
}
