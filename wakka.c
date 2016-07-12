#include "wakka.h"


wakka_p wakka_init(tidus_stats_p tidus_stats,
		   yuna_stats_p yuna_stats,
		   kimari_stats_p kimari_stats,
		   auron_stats_p auron_stats){
  
  wakka_p wakka = (wakka_p)malloc(sizeof(wakka_t));
  wakka->tidus_stats = tidus_stats;
  wakka->yuna_stats = yuna_stats;
  wakka->kimari_stats = kimari_stats;
  wakka->auron_stats = auron_stats;

  return wakka;  
}
