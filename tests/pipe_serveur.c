#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>


#define MSIZE 64
#define FIFO "/tmp/ihm_to_core"


int main(){
  int fd;
  char mbuf[MSIZE];
  int status;
  int nbytes;
  status = mkfifo(FIFO, 666);
  assert(status != -1 || errno == EEXIST);
  fd = open(FIFO, O_RDONLY);
  assert(fd >= 0);
  
  while(1){
    nbytes = read(fd, mbuf, MSIZE);
    assert(nbytes >= 0);
    if(nbytes > 0)
      printf(" %s\n", mbuf);

  }
  return 0;
}
