#include "common.h"
#include "time_work_asked.h"
#include "rule.h"
#include "time_work_asked_list.h"
#include "caract.h"
#include "messages.h"
#include "rule_list.h" 
#include "caract_list.h" 
#include "ipq_list.h" 
#include "packet.h" 
#include "choix.h"       
#include "print.h"


#define NB_TESTS 10000

int tab_test[NB_TESTS];
int indice = 0;


//sert a tester les elements
//on cree le elements, on le verifie, on les detruit, etc...

#define TEST_OK 0
#define TEST_FAILED 1 
//on renvoi 0: Ok ca va
//on renvoi 1: CA VA PAS :)


void testTimeWorkAskedList(){
  time_work_asked_list_p p = time_work_asked_list_create();
  time_work_asked_t t;
  time_work_asked_list_add(time_work_asked_list_p p,time_work_asked_t t);
}


int is_tab_test_ok(void){
  int i;
  int nbErr=0;
  for(i=0;i<NB_TESTS;i++)
    if(tab_test[i] == TEST_FAILED){
      nbErr++;
    }
  
  if(nbErr == 0)
    return TEST_OK;
  else{
    printf("%d Erreurs\n",nbErr);
    return TEST_FAILED;
  }
}

int main(){
  int i;
  for(i=0;i<NB_TESTS;i++)
    tab_test[i] = TEST_OK;

  if(is_tab_test_ok() == TEST_OK)
    printf("All tests OK\n");
  else
    printf("Pb lors des tests!!!!!!!!!!!\n");

  return 0;
}
	
