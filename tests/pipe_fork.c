/**********************************************************/
/**********************************************************/
/*PROGRAMME SUR L'UTILISATION DE PIPES/TUBES ET DE FORK() */
/**********************************************************/
/*But : Ecrire dans un pipe/tube de communication une url */
/*(adresse internet) qui devra être lancée a l'aide de */
/*lynx par un autre processus */
/**********************************************************/
  



/*
int setgid (gid_t);    // real gid
int setuid (uid_t);    // real uid
int setegid (gid_t);   // effective gid
int seteuid(uid_t);    //effective uid
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

int fifo_father_to_son_fd;
int fifo_son_to_father_fd;


int fils();
int fd_son_to_father[2];

int fd_father_to_son[2];

int give_up_root(void) 
{
  /* get the real uid and give up root */
  uid_t uid;
  int err;
  
  
  
  err=seteuid(1000);
  uid=getuid();
  
  printf("UID: %d\nErr:%d\n",(int)uid,err);  
  
  
  return (err);
}


/**********************************/
/* fils 1 : Ecriture dans le tube */
/**********************************/
int fils()
{
  char versfather[100];
  char defather[100];
  
  int i;
  for(i=0;i<100;i++)
    versfather[i]=48;
  
  
  fifo_father_to_son_fd = fd_father_to_son[0];
  
  fifo_son_to_father_fd = fd_son_to_father[0];
  
  
  //close(fifo_father_to_son_fd);
  printf("Son ecrit sur %d\n",fifo_son_to_father_fd);
  write(fifo_son_to_father_fd, versfather, 100);
  
  sleep(5);
  printf("son lit %d\n",fifo_father_to_son_fd);
  read(fifo_father_to_son_fd, defather, 100);  
  
  
  printf("On a ecrit sur pere\n");
  sleep(5);
  
  exit(0);
}



//le pere ecrit ds le fils


/***********************/
/* Fonction Principale */
/***********************/
int main()    
{ /* Processus Pere */
  if (pipe(fd_father_to_son) != 0) /* fd devient descripteur de fichier */
    {
      fprintf(stderr,"Problemes dans l'ouverture de Pipe \n");
      exit(1);
    }
  
  if (pipe(fd_son_to_father) != 0) /* fd devient descripteur de fichier */
    {
      fprintf(stderr,"Problemes dans l'ouverture de Pipe \n");
      exit(1);
    }

  give_up_root();
  
  
  

  // Lancement d'un fils
  if (    fork() == 0 )
    { /* Processus Fils 1 */
      fils();
    }else{//on est ds le pere
      char versson[100];
      char deson[100];


      //attention, interversion ici
      fifo_father_to_son_fd = fd_father_to_son[1];
      fifo_son_to_father_fd = fd_son_to_father[1];

      
      int i;
      for(i=0;i<100;i++)
	versson[i]=47;
      
      //close(fifo_father_to_son_fd);
      printf("Pere lit %d\n",fifo_son_to_father_fd);
      read(fifo_son_to_father_fd, deson, 100);
      
      sleep(5);
      printf("Pere ecrit sur %d\n",fifo_father_to_son_fd);
      write(fifo_father_to_son_fd,versson,100);
    }
  
  /*  
  close(fd[0]);
  close(fd[1]);
  fprintf(stderr, "attente de la fin des processus!\n");
  i=wait(&s);
  i=wait(&s);
  printf("fin du programme\n");
  */
  exit(EXIT_SUCCESS);
}
