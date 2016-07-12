#include "ipq_list.h"
#include "common.h"
#include "rule_list.h"
#include "kira.h"
#include "rochel.h"
#include "itachi.h"
#include "belial.h"

#ifndef SIN_H
#define SIN_H


typedef struct {
  //struct ipq_handle *ipq_h;
  ipq_list_p queued_packets;
  
  /*int guardians_to_sin_fd;
    int sin_to_guardians_fd;*/
  
  kira_p kira;
  itachi_p itachi;
  rochel_p rochel;
  belial_p belial;
  
} sin_t;

void sin_exit(sin_t *sin, int exit_code);
int sin_init(sin_t *sin);
int sin_wait_message(sin_t *sin, fd_set *read_fds);
void sin_handle_message(sin_t *sin, fd_set *read_fds,struct timeval* t_before_select);
void sin_handle_ipq(sin_t *sin);
void sin_handle_guardians(sin_t *sin);
void sin_do_time_work(sin_t *sin);
int sin_thread(int fifo_son_to_father_fd, int fifo_father_to_son_fd);

#endif
