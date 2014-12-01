#include "leitor_2.h"

char* fila[FILA_MAX];
pthread_mutex_t mutex;
sem_t consumidor;
int index_produtor;
int index_consumidor;
int working=1;

/*-------------------------------------------------------------------------------------
| fixed strings
+-------------------------------------------------------------------------------------*/
void * thread_code (void * args);

char* myfiles[NB_FILES];

/*-------------------------------------------------------------------------------------
| init_myfiles
+-------------------------------------------------------------------------------------*/


void init_myfiles () {
  int i,n,counter;
  int numbers[NB_FILES];
  char* tmp;
  srand(time(NULL));

  counter= NB_FILES;
  for (i=0; i< NB_FILES; i++) {
    myfiles[i] = malloc (15);
    sprintf (myfiles[i], "SO2014-%1d.txt", i);
  }
  
  for(i=0;i< NB_THREADS;++i){
    n = rand() % counter;
    tmp = myfiles[n+i];
    myfiles[n+i] = myfiles[i];
    myfiles[i] = tmp;
    counter--;
  }
  
}

int main(){
	struct timeval st , et;
	int i, error;
	pthread_t threads[NB_THREADS];
	int returnValues[NB_THREADS];
	
	pthread_mutex_init(&mutex, NULL);
	sem_init(&consumidor,0,0);
	index_produtor=0;
	index_consumidor=0;

	init_myfiles ();
	gettimeofday(&st , NULL);

	for(i=0;i<NB_THREADS;i++){
		error = pthread_create(&threads[i], NULL, thread_code,NULL);	
	}
	//**
	char buffer[2];
	
	printf("Ficheiro:");
	int n=scanf("%s", &buffer);
	while (strcmp(buffer,"sair")!=0){
		for (i=0; i< NB_FILES; i++) {
			if(strcmp(myfiles[i], buffer)==0){
				pthread_mutex_lock(&mutex);
				
				fila[index_produtor] = malloc (15);
				sprintf (fila[index_produtor],buffer);
				index_produtor=(index_produtor+1)%FILA_MAX;
				
				sem_post(&consumidor);
				pthread_mutex_unlock(&mutex);	
			}
		}
		printf("Ficheiro:");
		n=scanf("%s", &buffer);
	}
	
	return 0;
	//**
	
	for(i=0;i<NB_THREADS;i++){
		pthread_join(threads[i], (void *) &returnValues[i]);
	}	

	gettimeofday(&et , NULL);
   	printf("Running time of program: %d microseconds\n",(et.tv_usec - st.tv_usec));
	
	pthread_mutex_destroy(&mutex);
	sem_destroy(&consumidor);
	return 0;
}


/*-------------------------------------------------------------------------------------
| thread_code
+-------------------------------------------------------------------------------------*/

void * thread_code (void * args) {
	int res;
	while (working){
	res=1;
	char* file_to_open = malloc(15);
	
	sem_wait(&consumidor);
	pthread_mutex_lock(&mutex);
	
	file_to_open = fila[index_consumidor];
	printf("%s",file_to_open);
	index_consumidor=(index_consumidor+1)%FILA_MAX;
	
	pthread_mutex_unlock(&mutex);
	
	printf("%s\n", file_to_open);
	int   fd;
	struct timeval tvstart; /* start time */

  

  /* Initialize the seed of random number generator but use gettimeofday */
  if (gettimeofday(&tvstart, NULL) == -1) {
    perror("Could not get time of day, exiting.");
    res= -1;
  }
  srandom ((unsigned)tvstart.tv_usec);
  //  srandom ((unsigned) time(NULL));


  fd  = open (file_to_open, O_RDONLY);

  printf("Monitor will check if file %s is consistent...\n", file_to_open);

  if (fd == -1) {
    perror ("Error opening file");
    res= -1;
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
		res= -1;
      }
    }
    /* Retry lock, this time blocking */
    if (flock(fd, LOCK_SH) < 0) {
      perror ("Error locking file");
      res= -1;
    }

    if (read (fd, first_string, STRING_SZ) == -1) {
      perror ("Error reading file");
      res= -1;
    }
    for (i=0; i<NB_ENTRIES-1; i++) {
      
      if ((nbytes = read (fd, string_to_read, STRING_SZ)) == -1) {
		perror ("Error reading file");
		res= -1;
		break;
      }
      if (nbytes == 0){
		fprintf (stderr, "\nInconsistent file (too few lines): %s\n", file_to_open);
		res= -1;
		break;
      }

      if (strncmp(string_to_read, first_string, STRING_SZ)) {
		fprintf (stderr, "\nInconsistent file: %s\n", file_to_open);
		res= -1;
		break;
      }
    }
    
    if (read (fd, string_to_read, STRING_SZ) > 0){
		fprintf (stderr, "\nInconsistent file (too many lines): %s\n", file_to_open);
		res= -1;
		break;
    }
    
    /*Falta teste para ver se existem mais do que 1024 linhas*/
 
    if (flock(fd, LOCK_UN) < 0) {
      perror ("Error unlocking file");
      res= -1;
	  break;
    }

    if (close (fd) == -1)  {
      perror ("Error closing file");
      res= -1;
	  break;
    }
  }
  if (res==1)
	printf("It's ok!!!\n");
  else	
	printf("It's wrong\n");
  
  }
  pthread_exit((void*)1);
  
}