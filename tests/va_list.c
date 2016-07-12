#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>

void monprintf(char* message,...)
{
  va_list args;
  va_start(args,message);
  vprintf(message,args);
  va_end(args);
}


int main(){
  monprintf("Test1: %i %i %i\n",1,2,3);
}
