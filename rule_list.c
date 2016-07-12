#include <stdlib.h>
#include <stdio.h>

#include "rule_list.h"
#include "print.h"
#include "rule.h"


//O(1)
rule_list_p rule_list_create(void){
  return NULL;
}

//O(1)
rule_list_p rule_list_add(rule_list_p p, rule_t x){
  rule_list_p tmp;
  if ((tmp=(rule_list_p)malloc(sizeof(rule_list_t)))==NULL)
    {
      printf("impossible d'allouer la memoire");
      return (rule_list_p)EXIT_FAILURE;
    }
  tmp->rule=x;
  tmp->prec=p;
  return p=tmp;
}

//O(1)
rule_list_p rule_list_remove(rule_list_p p){
  rule_list_p tmp;
  if (!rule_list_is_void(p))
    {
      tmp=p->prec;
      free(p);
      return p=tmp;
    }else{
      printf("impossible de rule_list_remove une pile vide");
      return (rule_list_p)EXIT_FAILURE;
    }
}

//O(1)
rule_t rule_list_get(rule_list_p p){
  if (!rule_list_is_void(p))
    return p->rule;
  return create_void_rule();
}

//O(1)
rule_list_p rule_list_next(rule_list_p p){
  if(!rule_list_is_void(p))
    return p->prec;
  return NULL;
}

//O(1)
int rule_list_is_void(rule_list_p p){
  return p==NULL;
}

//O(Taille p)
void rule_list_print(rule_list_p p){
  rule_list_p p_temp;
  p_temp = p;
  while(!rule_list_is_void(p_temp))
    {
      print_rule(rule_list_get(p_temp));
      p_temp=rule_list_next(p_temp);
    }
  return;
}

//O(Taille p)
u32 rule_list_size(rule_list_p p){
  rule_list_p p_temp;
  u32 i=0;
  p_temp=p;
  while(!rule_list_is_void(p_temp))
    {
      i++;
      p_temp=rule_list_next(p_temp);
    }
  return i;
}

//O(Taille p)
void rule_list_flush(rule_list_p p){
  while(!rule_list_is_void(p))
    {p=rule_list_remove(p);}
  return;
}

//O(taille p)
u16 rule_list_is_matched(caract_t c,rule_list_p p){
  u16 res;
  rule_list_p tmp;
  tmp = p;
  while(!rule_list_is_void(tmp))
    {
      if( (res=is_matched(c,rule_list_get(tmp))) != NOT_MATCHED)
	return res;
      else
	tmp=rule_list_next(tmp);
    }
  return NOT_MATCHED;
}



//on va parcourir la rule_list:
//avoir le nombre total d elements
//reserver nbelement*sizeof struct rule
//parcourir la liste pour en creer un tab
//O(taille p)
rule_t * create_tab_from_rule_list(rule_list_p p){
  u32 size;
  u32 pos = 0;
  rule_list_p tmp;
  tmp = p;
  size = rule_list_size(tmp);
  rule_t * res = (rule_t *)malloc(size*sizeof(rule_t));
  
  while(!rule_list_is_void(tmp))
    {
      rule_t r = rule_list_get(tmp);
      //memcpy (void *dest, const void *src, size_t n);
      memcpy((res+pos),&r,sizeof(struct rule));
      pos++;
      tmp = rule_list_next(tmp);
    }
  return res;
}

//O(Taille p)
rule_list_p rule_list_remove_from_id(rule_list_p p, u32 id){
  rule_list_p res = p;
  rule_list_p p_temp = p;
  rule_list_p p_prec = NULL;
  rule_list_p p_temp2 = NULL;
  
  while(!rule_list_is_void(p_temp))
    {
      if( (p_temp->rule).id == id){//si on chope le bon element
	//on verifie si on est ds le permeier element
	if(p_temp == p){// si on est ds le permier element
	  //on free et renvoi le deuxieme element comme nouvelle pile
	  res = rule_list_next(p_temp);
	  free(p_temp);
	  return res;
	}else{//on est pas ds le premier
	  //donc on libère
	  p_temp2 = rule_list_next(p_temp);
	  free(p_temp);
	  p_prec->prec = p_temp2; 
	  return res;
	}
      }else{
	p_prec = p_temp;
	p_temp = rule_list_next(p_temp);
      }
    }
  return res;
}
