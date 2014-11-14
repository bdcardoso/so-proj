#include "leitor_3.h"

char filename[15];

/*-------------------------------------------------------------------------------------
| fixed strings
+-------------------------------------------------------------------------------------*/
void * thread_code (void * arguments);


/*-------------------------------------------------------------------------------------
| MAIN
+-------------------------------------------------------------------------------------*/

int main(int argc, char* args[]){
    
    if (argc <= 1) {
        printf("por favor insira o nome do ficheiro\n");
        return -1;
    }
    
	struct timeval st , et;
	int i, error;
	pthread_t threads[K];
	int returnValues[K];
    int linetogo[K];
    
	gettimeofday(&st , NULL);
    
    strncpy(filename, args[1],15);


	for(i=0;i<K;i++){
		linetogo[i] = i*(PORTION) * (sizeof(char)* 11);
		error = pthread_create(&threads[i], NULL, thread_code, (void*)&linetogo[i]);
	}

    int sum = 0;
	for(i=0;i<K;i++){
        
		pthread_join(threads[i], (void*)&returnValues[i]);
        sum += returnValues[i];
        
        printf("%d\n", returnValues[i]);
		if (returnValues[i] == 1){
			printf("It's ok!!!\n");
		} else {
			printf("It's wrong\n");
		}
	}
	
    if (sum==K) { // sum equals number of threads
        printf("FICHEIRO CORRECTO\n");
    } else {
        printf("FICHEIRO ERRADO\n");
    }

	gettimeofday(&et , NULL);
   	printf("Running time of program: %d microseconds\n",(et.tv_usec - st.tv_usec));

    //free(arguments);
	return 0;
}


/*-------------------------------------------------------------------------------------
| thread_code
+-------------------------------------------------------------------------------------*/

void * thread_code (void * x) {
    
    char* file_to_open = malloc(15);
    file_to_open = /*(char*)arguments->filename; */ filename;
	printf("%s\n", file_to_open);
    int   fd;
    struct timeval tvstart; /* start time */


    /* Initialize the seed of random number generator but use gettimeofday */
    if (gettimeofday(&tvstart, NULL) == -1) {
        perror("Could not get time of day, exiting.");
        pthread_exit((void*)-1);
    }
    srandom ((unsigned)tvstart.tv_usec);
    //  srandom ((unsigned) time(NULL));


    fd  = open (file_to_open, O_RDONLY);
    printf("-------> %d\n", (*(int*) x));
    lseek(fd, (*(int*) x), SEEK_SET);

    printf("Monitor will check if file %s is consistent...\n", file_to_open);

    if (fd == -1) {
        perror ("Error opening file");
        pthread_exit ((void*)-1);
    }
    else {
        char string_to_read[STRING_SZ];
        char first_string[STRING_SZ];
        int  i, nbytes;

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
		pthread_exit ((void*)-1);
      }
    }
    /* Retry lock, this time blocking */
    if (flock(fd, LOCK_SH) < 0) {
      perror ("Error locking file");
      pthread_exit ((void*)-1);
    }

    if (read (fd, first_string, STRING_SZ) == -1) {
      perror ("Error reading file");
      pthread_exit ((void*)-1);
    }
      
    for (i=0; i< PORTION; i++) {
      
      if ((nbytes = read (fd, string_to_read, STRING_SZ)) == -1) {
		perror ("Error reading file");
		pthread_exit ((void*)-1);
      }
      if (nbytes == 0){
		fprintf (stderr, "\nInconsistent file (too few lines): %s\n", file_to_open);
		pthread_exit ((void*)-1);
      }

      if (strncmp(string_to_read, first_string, STRING_SZ)) {
		fprintf (stderr, "\nInconsistent file: %s\n", file_to_open);
          printf("%s %s", first_string, string_to_read);
		pthread_exit ((void*)-1);
      }
    }
    
    // COMENTADO PORQUE EXISTEM LINHAS PARA LER A SEGUIR QUE NAO E SUPOSTO LER
    //if (read (fd, string_to_read, STRING_SZ) > 0){
	//	fprintf (stderr, "\nInconsistent file (too many lines): %s\n", file_to_open);
	//	pthread_exit ((void*)-1);
    //}
    
    /*Falta teste para ver se existem mais do que 1024 linhas*/
 
    if (flock(fd, LOCK_UN) < 0) {
      perror ("Error unlocking file");
      pthread_exit ((void*)-1);
    }

    if (close (fd) == -1)  {
      perror ("Error closing file");
      pthread_exit ((void*)-1);
    }
  }
  pthread_exit((void*)1);
}
