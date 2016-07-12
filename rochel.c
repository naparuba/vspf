#include <stdlib.h>
#include "rochel.h"

rochel_p rochel_init(){
  rochel_p rochel = (rochel_p)malloc(sizeof(rochel_t));
  return rochel;
}

void rochel_exit(rochel_p rochel){
  free(rochel);
}
