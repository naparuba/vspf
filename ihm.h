#ifndef IHM_H
#define IHM_H


typedef struct {
  caract_list_p wai_caract;
  int core_to_ihm_fd;
  int ihm_to_core_fd;
  struct rule * rule_tab;
  int rule_tab_size;
} ihm_t;

#endif //IHM_H
