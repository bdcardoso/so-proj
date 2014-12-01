/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 4, programa LEITOR paralelo 1, processo pai
|
| Autor: Paolo Romano
+-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
| includes
+-------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "exercicio4.h"


/*-------------------------------------------------------------------------------------
| main
+-------------------------------------------------------------------------------------*/

int main (int argc, char** argv) {

  pid_t pid;
  int runningChildren;
  char file_id[10];
  time_t start, end;

  time(&start);
  
  for (runningChildren = 0; runningChildren < NUM_CHILDREN; runningChildren++) {

    sprintf(file_id,"%d",runningChildren);

    pid = fork();
    if (pid < 0) {
      perror("Could not fork a child.");
      exit(-1);
    } else if (pid == 0) {

      if (execl("exercicio4-leitor", "exercicio4-leitor", file_id, NULL) == -1) {
	perror("Could not execute child program.");
	exit(-1);
      }

      printf("it should not reach this point...\n");
    }
  }
  
  int status;

  printf("Parent will now wait for the children to exit.\n");

  while (runningChildren > 0) {
    pid = wait(&status);
    printf("Child with pid=%ld completed with status 0x%x.\n", (long)pid, status);
    runningChildren --;
  }

  time(&end);
  double dif = difftime (end,start);
  printf ("Your calculations took %.2lf seconds to run.\n", dif );

  return 0;
 }
