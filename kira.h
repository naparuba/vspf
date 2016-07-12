#include "ipq_list.h"

#ifndef KIRA_H
#define KIRA_H


typedef struct {
  struct ipq_handle *ipq_h;
  
  int guardians_to_sin_fd;
  int sin_to_guardians_fd;
} kira_t,*kira_p;

void kira_exit(kira_p kira);
kira_p kira_init();

int kira_open_sin_to_guardians(kira_p kira);
int kira_open_guardians_to_sin(kira_p kira);

void kira_declare_guardians_to_sin(kira_p kira,fd_set * read_fds);
int kira_read_guardians(kira_p kira,char * buf);

void kira_close_guardians_to_sin(kira_p kira);
int kira_ipq_read(kira_p kira,unsigned char *buf);
ipq_packet_msg_t *kira_ipq_get_packet(unsigned char *buf);

int kira_write_to_guardians(kira_p kira,char * buf, int size);
int kira_send_smth_to_guardians(kira_p kira, u16 type, u16 length, u8* payload);

#endif
