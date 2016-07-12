#include "rule_list.h"
#include "caract.h"

#ifndef KIMARI_H
#define KIMARI_H

typedef struct{
  int nbRulesInList;
  size_t sizeModule;
} kimari_stats_t,*kimari_stats_p;


typedef struct {
  rule_list_p policy;

  kimari_stats_p stats;
} kimari_t, * kimari_p;

kimari_p kimari_init();
void kimari_exit(kimari_p);

void policy_init(kimari_p);

void kimari_add_rule(kimari_p kimari, rule_t r);
void kimari_delete_rule(kimari_p kimari, u32 id);
unsigned short kimari_is_matched_caract(kimari_p kimari,caract_t c);

#endif
