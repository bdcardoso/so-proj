/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 4, programa LEITOR paralelo 3
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
#include "exercicio4-leitor-util.h"
#include <sys/file.h> /* flock() */
#include <errno.h>
#include <string.h>
#include <sys/time.h>




extern char* myfiles[NB_FILES];

char* fileName;
char common_string[STRING_SZ];

/********************************************
parallel reader
********************************************/

int parallelReader(void* input_param) {
  int fd;
  int threadID = (int) input_param;
 

  fd  = open (fileName , O_RDONLY);

  if (fd == -1) {
    perror ("Error opening file");
    return -1;
  }
  else {
    char string_to_read[STRING_SZ];
    int  i;

    // Computed offset to skip via lseek and number of lines to parse
    int pos = NB_ENTRIES/NUM_CHILDREN * threadID * STRING_SZ;
    int linesToRead = (threadID == NUM_CHILDREN - 1) ? NB_ENTRIES / NUM_CHILDREN + NB_ENTRIES % NUM_CHILDREN : NB_ENTRIES / NUM_CHILDREN;


    if (lseek(fd,pos,SEEK_SET)== -1) {
     perror ("Error seeking file");
     return -1;
    }


    // As an optimization the first thread may actually skip reading the first line

   
    for (i=0; i<linesToRead; i++) {
      
      if (read (fd, string_to_read, STRING_SZ) != 10) {
	fprintf (stderr,"\n%d: Error reading file: %s\n", threadID, fileName);
	return -1;
      }

      if (strncmp(string_to_read, common_string, STRING_SZ)) {
	fprintf (stderr, "\n%d: Inconsistent file: %s\n", threadID, fileName);
	return -1;
      }
    }

    // The last thread checks for any additional trailing data
    if (threadID == NUM_CHILDREN - 1 && read (fd, string_to_read, STRING_SZ) != 0) {
	fprintf (stderr, "\n%d: Inconsistent file (extra lines): %s\n", threadID, fileName);
	return -1;
      }


    if (close (fd) == -1)  {
      perror ("Error closing file");
      return (-1);
    }
  }

  return 0;

}




/********************************************
Parse input params
********************************************/

int parseInputParams(int argc, char** argv) {

	int index=-1;

	if (argc!=2) {
		fprintf(stderr,"Wrong number of input params --- Terminating.\n");
		exit(-1);
	}

	index=atoi(argv[1]);

	if (index<0 || index >4) {
		fprintf(stderr,"Error while parsing the file index --- Terminating.\n");
		exit(-1);
	}
				
	return index;

}


/********************************************
Main
********************************************/

int main (int argc, char** argv) {

  pthread_t tid[NUM_CHILDREN];
  int runningChildren;
  long int retVals[NUM_CHILDREN]; // On 32 bit machines this could be safely declared as an array of ints.
				  // This is declared as an array of long ints as on 64 bits architectures
				  // pointers take 8 bytes and pthread_join expects to receive a return value
				  // of type void*. Recall: sizeof(int)=4; sizeof(long int)=8

  int fd;

  struct timeval tvstart; /* data de inicio */
  struct timeval tvend; /* data de fim */
  struct timeval tvduration; /* diferenca entre as duas datas */
  unsigned int duration; /* diferenca entre as datas em microssegundos */


  init_myfiles();
	
  int file_index=parseInputParams(argc, argv);

  fileName=myfiles[file_index];

  printf("Parallel reader: inspecting file %s\n",fileName);

  gettimeofday(&tvstart, NULL);   

  fd  = open (fileName , O_RDONLY);


  if (flock(fd, LOCK_SH) < 0) {
     perror ("Error locking file");
     return -1;
  }


  if (read (fd, common_string, STRING_SZ) != 10) {
     perror ("Error reading file");
     flock(fd, LOCK_UN);
     close(fd);
     return -1;
  }

  // verify first line
  if (isLineLegal(common_string)==-1) {
     fprintf(stderr, "First line in the file is corrupted.\n");
     flock(fd, LOCK_UN);
     close(fd);
     return -1;
  }

  for (runningChildren = 0; runningChildren < NUM_CHILDREN; runningChildren++) {
	
    	if (pthread_create(&tid[runningChildren], NULL, (void*(*)(void*)) parallelReader, (void*) runningChildren) != 0)
	{
		printf("Error creating thread %d", runningChildren);
		return -1;
	}

  }

  for (runningChildren = 0; runningChildren < NUM_CHILDREN; runningChildren++) {
	pthread_join(tid[runningChildren], (void**)&retVals[runningChildren]);
	printf("Multithreaded verifier - thread %d/%d returned :%d\n",runningChildren, NUM_CHILDREN, (int) retVals[runningChildren]);
  }
  

    if (flock(fd, LOCK_UN) < 0) {
      perror ("Error unlocking file");
      return (-1);
    }

    if (close (fd) == -1)  {
      perror ("Error closing file");
      return (-1);
    }

  gettimeofday(&tvend, NULL);

  tvduration.tv_sec = tvend.tv_sec - tvstart.tv_sec;
  tvduration.tv_usec = tvend.tv_usec - tvstart.tv_usec;
  duration = tvduration.tv_sec * 1000000 + tvduration.tv_usec;

  printf("duracao: %d microssegundos\n", duration);


  return 0;
 }
