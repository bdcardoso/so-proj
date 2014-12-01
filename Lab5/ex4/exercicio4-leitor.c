/*-------------------------------------------------------------------------------------
| SO, Solução do Exercício 4, programa LEITOR paralelo 1
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

#include "exercicio4-leitor-util.h"
#include "exercicio4.h"

extern char* myfiles[NB_FILES];

int parseInputParams(int argc, char** argv) {

	int index=-1;

	if (argc!=2) {
		perror("Wrong number of input params --- Terminating.");
		exit(-1);
	}

	index=atoi(argv[1]);

	if (index<0 || index >4) {
		perror("Error while parsing the file index --- Terminating.");
		exit(-1);
	}
				
	return index;

}

int main (int argc, char** argv) {

  int file_index=parseInputParams(argc, argv);

  init_myfiles ();

  return reader(myfiles[file_index]);
}
