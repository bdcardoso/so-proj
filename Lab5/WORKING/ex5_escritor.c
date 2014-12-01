#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/file.h> /* flock() */
#include <errno.h>
#include <pthread.h>

sigset_t new_set, old_set;

#define T 1
#define F 0
#define NUM_THREADS 10
#define NB_FILES    5
#define NB_ENTRIES  1024
#define STRING_SZ   10
#define get_random(max) rand()%max
#define NUM_CHILDREN 10
#define NB_ITERATIONS   2000
#define NB_STRINGS      10

int lock_USR1;
int error_USR2;
int stp_stop;
pthread_mutex_t mux[NB_FILES], mux_files;

/*-------------------------------------------------------------------------------------
 | fixed strings
 +-------------------------------------------------------------------------------------*/

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
 | thread_escritor
 +-------------------------------------------------------------------------------------*/

void thread_escritor () {
    int   i;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int index;
    char *file_to_open;
    int fd;
    int it_was_locked;
    char *string_to_write;
    int j;
    
    while( stp_stop ){
        
        pthread_mutex_lock(&mux_files);
        index = get_random(NB_FILES);
        file_to_open = myfiles[index];
        string_to_write  = mystrings[get_random(NB_FILES)];
        pthread_mutex_unlock(&mux_files);
        
        fd  = open (file_to_open, O_RDWR | O_CREAT, mode);
        it_was_locked=0;
        
        if (fd == -1) {
            perror ("Error opening file");
            exit (-1);
        }
        
        
        if(lock_USR1){
            pthread_mutex_lock(&mux[index]);
          //  printf("A lockar %d\n", index);
            it_was_locked=1;
        }
        
        for (j=0; j<NB_ENTRIES; j++) {
            
            if ( error_USR2){
                if (j%2==0) {
                    string_to_write[0]='z';
                } else {
                    string_to_write[0]= string_to_write[1];
                }
            }else{
                string_to_write[0]= string_to_write[1];
            }

            if (write (fd, string_to_write, STRING_SZ) == -1) {
                perror ("Error writing file");
                exit (-1);
            }
        }
        
        if (close (fd) == -1)  {
            perror ("Error closing file");
            exit (-1);
        }
        
        if( it_was_locked){
         //   printf("Deslockar intensamente %d\n", index);
            pthread_mutex_unlock(&mux[index]);
            it_was_locked=0;
        }
    }
    pthread_exit(0);
}

/*One must know if after recieving a signal the threads must start using the new definitions or this action will only take place AFTER the current loop finishes*/

void func_SIGUSR1(){
    if(lock_USR1 == T){
        lock_USR1 = F;
    }
    if(lock_USR1 == F){
        lock_USR1 = T;
    }
    /*Flag that makes the writer ignore locks or use locks(?)*/
    signal(SIGUSR1, func_SIGUSR1);
}

void func_SIGUSR2(){
    /*Switches a flag so that erros are now writing to the file or that ignore writing erros to the file*/
    if(error_USR2 == T){
        error_USR2 = F;
    }
    if(error_USR2 == F){
        error_USR2 = T;
    }
    signal(SIGUSR2, func_SIGUSR2);
}

void func_SIGTSTP(){
    /*Let the process finish*/
    /*For this to happen we can use a integrer that allows us to see how many threads are active and how many threads are idle once the number of active threads is 0 then we know we can end the program*/
    stp_stop = F;
}


int main(){
    pthread_t *t_id = (pthread_t*) malloc(sizeof(pthread_t) * NUM_THREADS);
    int i;
    struct timeval tvstart; /* start time */
    
    lock_USR1 = T;
    error_USR2 = F;
    stp_stop = T;
    
    init_myfiles ();
    /* Initialize the seed of random number generator but use gettimeofday */
    if (gettimeofday(&tvstart, NULL) == -1) {
        perror("Could not get time of day, exiting.");
        exit(-1);
    }
    srandom ((unsigned)tvstart.tv_usec);
    
    for( i=0; i<NB_FILES; i++){
        pthread_mutex_init(&mux[i], NULL);
    }
    
    pthread_mutex_init(&mux_files, NULL);
    
    for( i=0; i < NUM_THREADS; i++){
        if (pthread_create(&t_id[i], NULL, thread_escritor, NULL) != 0){
            printf("Erro a criar a string");
            exit(-1);
        }
    }
    
    signal(SIGUSR1, func_SIGUSR1);
    signal(SIGUSR2, func_SIGUSR2);
    signal(SIGTSTP, func_SIGTSTP);
    signal(SIGINT, func_SIGTSTP);
    
    /*If we want to ignore ALL other signals add the code below*/
    sigfillset(&new_set);
    sigdelset(&new_set, SIGUSR1);
    sigdelset(&new_set, SIGUSR2);
    sigdelset(&new_set, SIGTSTP);
    sigdelset(&new_set, SIGINT);
    sigprocmask(SIG_BLOCK, &new_set, &old_set);
    /*End of code to ignore signals*/
    
    for(i=0; i< NUM_THREADS; i++){
        if(pthread_join(t_id[i], NULL)){
            printf("Erro no join");
            exit(-1);
        }
    }
    
    for( i=0; i<NB_FILES; i++){
        pthread_mutex_destroy(&mux[i]);
    }
    pthread_mutex_destroy(&mux_files);
    /*Add code that launches the paralel writer*/

    exit(0);
}
