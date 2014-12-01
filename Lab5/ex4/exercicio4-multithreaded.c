/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 4, programa LEITOR paralelo 2
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
#include <pthread.h>
#include "exercicio4.h"
#include "exercicio4-leitor.h"


/*-------------------------------------------------------------------------------------
| main
+-------------------------------------------------------------------------------------*/

extern char* myfiles[NB_FILES];

int main (int argc, char** argv) {

  pthread_t tid[NUM_CHILDREN];
  int runningChildren;
  long int retVals[NUM_CHILDREN]; // On 32 bit machines this could be safely declared as an array of ints.
				  // This is declared as an array of long ints as on 64 bits architectures
				  // pointers take 8 bytes and pthread_join expects to receive a return value
				  // of type void*. Recall: sizeof(int)=4; sizeof(long int)=8

  time_t start, end;

  time(&start);
  
  init_myfiles();

  for (runningChildren = 0; runningChildren < NUM_CHILDREN; runningChildren++) {
    	if (pthread_create(&tid[runningChildren], NULL, reader_wrapper, (void*) myfiles[runningChildren]) != 0)
	{
		printf("Error creating thread %d", runningChildren);
		return -1;
	}

  }

  for (runningChildren = 0; runningChildren < NUM_CHILDREN; runningChildren++) {
	pthread_join(tid[runningChildren], (void**)&retVals[runningChildren]);
	printf("Multithreaded verifier - thread %d/%d returned :%d\n",runningChildren, NUM_CHILDREN, (int) retVals[runningChildren]);
  }
  time(&end);
  double dif = difftime (end,start);
  printf ("Your calculations took %.2lf seconds to run.\n", dif );

  return 0;
 }
