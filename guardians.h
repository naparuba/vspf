#include "common.h"
#include <time.h>
#include "rule_list.h"
#include "yuna.h"
#include "tidus.h"
#include "kimari.h"
#include "wakka.h"
#include "auron.h"

#ifndef GUARDIANS_H
#define GUARDIANS_H


typedef struct {
  yuna_p yuna;
  tidus_p tidus;
  kimari_p kimari;
  wakka_p wakka;
  auron_p auron;
} guardians_t;

int guardians_thread(int, int);
void guardians_exit(guardians_t *guardians, int exit_code);
int guardians_init(guardians_t *guardians);
int guardians_wait_message(guardians_t *guardians, fd_set *read_fds);
void guardians_handle_message(guardians_t *guardians,fd_set *read_fds,struct timeval* t_before_select);
void guardians_handle_sin(guardians_t *guardians);
void guardians_handle_ihm(guardians_t *guardians);
void guardians_do_time_work(guardians_t *guardians);





#endif
