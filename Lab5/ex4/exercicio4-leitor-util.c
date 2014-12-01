/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 4, ficheiro com funções utilizadas por vários exercícios
|
| Autor: Paolo Romano
+-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------
| includes
+-------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/file.h> /* flock() */
#include <errno.h>

#include "exercicio4.h"

/*-------------------------------------------------------------------------------------
| fixed strings
+-------------------------------------------------------------------------------------*/


char* myfiles[NB_FILES];
char mystrings[STRING_SZ][NB_STRINGS] = { "aaaaaaaaa\n", 
				"bbbbbbbbb\n",
				"ccccccccc\n",
				"ddddddddd\n",
				"eeeeeeeee\n",
				"fffffffff\n",
				"ggggggggg\n",
				"hhhhhhhhh\n",
				"iiiiiiiii\n",
				"jjjjjjjjj\n" };

/*-------------------------------------------------------------------------------------
| isLineLegal
+-------------------------------------------------------------------------------------*/

int isLineLegal(const char* str) {

	int i=0;
	for (;i< NB_STRINGS; i++) {
		if (strncmp(str,mystrings[i],STRING_SZ)==0)
			return 0;
	}

	return -1;
}


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
| reader
+-------------------------------------------------------------------------------------*/



int reader(char  *file_to_open) {
  int fd;

  printf("Monitor will check if file %s is consistent...", file_to_open);

  fd  = open (file_to_open, O_RDONLY);

  if (fd == -1) {
    perror ("Error opening file");
    return -1;
  }
  else {
    char string_to_read[STRING_SZ];
    char first_string[STRING_SZ];
    int  i;

/* Shared lock - Non blocking */
    if (flock(fd, LOCK_SH | LOCK_NB) < 0) {
      if (errno == EWOULDBLOCK)
	perror ("Wait unlock");
      else {
	perror ("Error locking file");
	return -1;
      }
    }
    /* Retry lock, this time blocking */
    if (flock(fd, LOCK_SH) < 0) {
      perror ("Error locking file");
      return -1;
    }

    if (read (fd, first_string, STRING_SZ) != 10) {
      perror ("Error reading file");
      return -1;
    }

    sleep(1);
    for (i=0; i<NB_ENTRIES-1; i++) {
      
      if (read (fd, string_to_read, STRING_SZ) != 10) {
	fprintf (stderr,"\nError reading file: %s\n", file_to_open);
	return -1;
      }

      if (strncmp(string_to_read, first_string, STRING_SZ)) {
	fprintf (stderr, "\nInconsistent file: %s\n", file_to_open);
	return -1;
      }
    }

    if (read (fd, string_to_read, STRING_SZ) != 0) {
	fprintf (stderr, "\nInconsistent file (extra lines): %s\n", file_to_open);
	return -1;
      }

/* AMC - to be replaced by flock() - UNLOCK 
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = NB_ENTRIES*STRING_SZ ;

    if (fcntl(fd, F_SETLK, &fl) == -1){
	perror ("Error unlocking file");
	return -1;
      }
*/
    if (flock(fd, LOCK_UN) < 0) {
      perror ("Error unlocking file");
      return (-1);
    }

    if (close (fd) == -1)  {
      perror ("Error closing file");
      return (-1);
    }
  }
  printf("YES for file %s.\n",file_to_open);
  return 0;
}



void* reader_wrapper(void* ptr) {

	return (void*) reader((char*) ptr);
}


