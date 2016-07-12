#include "common.h"
#include "rule.h"

#ifndef RULE_LIST_H
#define RULE_LIST_H

// Définition des structure policy et policy_p

typedef struct rule_list
{
  struct rule rule;
  struct rule_list* prec;
}rule_list_t,* rule_list_p;


rule_list_p rule_list_create(void);

rule_list_p rule_list_add(rule_list_p p,rule_t r);

rule_list_p rule_list_remove(rule_list_p p);

rule_t rule_list_get(rule_list_p p);

rule_list_p rule_list_next(rule_list_p p);

int rule_list_is_void(rule_list_p p);

void rule_list_print(rule_list_p p);

u32 rule_list_size(rule_list_p p);

void rule_list_flush(rule_list_p p);

u16 rule_list_is_matched(caract_t c,rule_list_p p);

rule_list_p rule_list_remove_from_id(rule_list_p p, u32 id);

rule_t * create_tab_from_rule_list(rule_list_p p);

#endif // POLICY_H
