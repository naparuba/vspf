#include "common.h"
#include "choix.h"

#ifndef MESSAGES_H
#define MESSAGES_H

#define TYPE_CARACT 1
#define TYPE_RULE 2
#define TYPE_CHOIX 3
#define TYPE_GET_LISTE_RULE 4
#define TYPE_LISTE_RULE 5
#define TYPE_REMOVE_RULE 6
#define TYPE_IPQ 7

typedef struct message{
  u16 type;
  u16 len;
  u8 * payload;
} message_t;

message_t creer_message(u16 type, u16 len, u8* payload);
message_t creer_message_choix(choix_t c);

u8* creer_packet(message_t r);
message_t get_message(u8* m);



#endif
