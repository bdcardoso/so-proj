/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 3, programa LEITOR
|
|
+-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
| includes
+-------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <sys/file.h> /* flock() */
#include <errno.h>

#include "exercicio1.h"

/*-------------------------------------------------------------------------------------
| fixed strings
+-------------------------------------------------------------------------------------*/


char* myfiles[NB_FILES];


/*-------------------------------------------------------------------------------------
| init_myfiles
+-------------------------------------------------------------------------------------*/

void init_myfiles () {
  int i;

  for (i=0; i< NB_FILES; i++) {
    myfiles[i] = malloc (15);
    sprintf (myfiles[i], "SO2014-%1d.txt", i);
  }
}


/*-------------------------------------------------------------------------------------
| main
+-------------------------------------------------------------------------------------*/

int main (int argc, char** argv) {
  char  *file_to_open;
  int   fd;
  struct timeval tvstart; /* start time */

  init_myfiles ();

  /* Initialize the seed of random number generator but use gettimeofday */
  if (gettimeofday(&tvstart, NULL) == -1) {
    perror("Could not get time of day, exiting.");
    exit(-1);
  }
  srandom ((unsigned)tvstart.tv_usec);
  //  srandom ((unsigned) time(NULL));

  file_to_open = myfiles[get_random(NB_FILES)];
  fd  = open (file_to_open, O_RDONLY);

  printf("Monitor will check if file %s is consistent...", file_to_open);

  if (fd == -1) {
    perror ("Error opening file");
    exit (-1);
  }
  else {
    char string_to_read[STRING_SZ];
    char first_string[STRING_SZ];
    int  i;

/* Shared lock - Blocking
    if (flock(fd, LOCK_SH) < 0) {
      perror ("Error locking file");
      exit (-1);
    }
*/

/* Shared lock - Non blocking */
    if (flock(fd, LOCK_SH | LOCK_NB) < 0) {
      if (errno == EWOULDBLOCK)
	perror ("Wait unlock");
      else {
	perror ("Error locking file");
	exit (-1);
      }
    }
    /* Retry lock, this time blocking */
    if (flock(fd, LOCK_SH) < 0) {
      perror ("Error locking file");
      exit (-1);
    }

    if (read (fd, first_string, STRING_SZ) == -1) {
      perror ("Error reading file");
      exit (-1);
    }
    for (i=0; i<NB_ENTRIES-1; i++) {
      
      if (read (fd, string_to_read, STRING_SZ) == -1) {
	perror ("Error reading file");
	exit (-1);
      }

      if (strncmp(string_to_read, first_string, STRING_SZ)) {
	fprintf (stderr, "\nInconsistent file: %s\n", file_to_open);
	exit (-1);
      }
    }
 
    if (flock(fd, LOCK_UN) < 0) {
      perror ("Error unlocking file");
      exit (-1);
    }

    if (close (fd) == -1)  {
      perror ("Error closing file");
      exit (-1);
    }
  }
  printf("YES.\n");
}
