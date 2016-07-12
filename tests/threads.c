#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//gcc -D_REENTRANT -o threads threads.c -lpthread && ./threads
static pthread_mutex_t my_mutex;


void *my_thread_process (void * arg)
{
  int i;
  
  //declarer qu'on ne veut pas mourrir!
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  
  pthread_mutex_lock (&my_mutex);
  
  for (i = 0 ; i < 5 ; i++) {
    printf ("Thread %s: %d\n", (char*)arg, i);
    sleep(1);
  }
  
  pthread_mutex_unlock (&my_mutex);
  
  //dire que si on veut on peut mourrir
  pthread_testcancel ();
  
  printf("Le thread quitte normalement\n");
  pthread_exit (0);
}


main (int ac, char **av)
{
  pthread_t th1, th2;
  void *ret;
  
  //inititalisation du mutex
  pthread_mutex_init (&my_mutex, NULL);
  
  
  //cree une structure pour la configuration
  pthread_attr_t thread_attr;
  
  if (pthread_attr_init (&thread_attr) != 0) {
    fprintf (stderr, "pthread_attr_init error");
    exit (1);
  }
  
  if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
    fprintf (stderr, "pthread_attr_setdetachstate error");
    exit (1);
  }
  //fin de la creation de la structure pour le detache
  
  
  if (pthread_create (&th1, &thread_attr, my_thread_process, "1") < 0) {
    fprintf (stderr, "pthread_create error for thread 1\n");
    exit (1);
  }
  
  /*
  if (pthread_create (&th2, &thread_attr, my_thread_process, "2") < 0) {
    fprintf (stderr, "pthread_create error for thread 2\n");
    exit (1);
  }
  */
  
  printf("On a cree le threads\n");
  sleep(10);
  printf("On quitte le thread principal\n");
  
  /*
  (void)pthread_join (th1, &ret);
  printf("Apres join 1\n");
  (void)pthread_join (th2, &ret);
  printf("Apres join 2\n");
  */
  
  printf("On demande au thread de mourrir et attends que le thread meure\n"); 
  
  if (pthread_cancel (th1) != 0) {
    fprintf (stderr, "pthread_cancel error for thread 1\n");
    //exit (1);
  }
  
  /*
  if (pthread_cancel (th2) != 0) {
    fprintf (stderr, "pthread_cancel error for thread 1\n");
    exit (1);
  }
  */
  
}
