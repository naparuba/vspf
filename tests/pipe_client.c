#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#include "messages.h"

#define BUFSIZE 2048
#define FIFO_IHM_TO_CORE "../core_to_ihm"



int main(int argc, char ** argv){
  int fd;
  char mbuf[BUFSIZE];
  int i;
  
  fd = open(FIFO, O_WRONLY | O_NONBLOCK);
  assert(fd >= 0);
  
  for(i=1;i< argc; i++){
    strncpy(mbuf, argv[i], MSIZE -1);
    mbuf[MSIZE-1] = '\0';
    write(fd, mbuf, MSIZE);
  }

  return 0;
}
