#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define N 10
#define BUFFSIZE 15
#define NB_MSG 1000

char *buffer_comunication;
char *buffer_info[NB_MSG];
sem_t semaphore;
pthread_mutex_t allow;


void* verifica(){
    
    sem_wait(&semaphore);
    printf("thread created");
    pthread_mutex_lock(&allow); /*Lock acess to the buffer*/
    /*Read function from buffer*/
    pthread_mutex_unlock(&allow);   /*Free acess to the buffer*/
    /*Check the file code*/

    pthread_exit(NULL);
}



int main(){
    pthread_t *t_id;
    int i;

    t_id = (pthread_t*)malloc(sizeof(pthread_t)*N);

    buffer_comunication = (char*)malloc(sizeof(char)*BUFFSIZE);

    pthread_mutex_init(&allow, NULL);
    sem_init(&semaphore, 0, 0); /*We only want threads acessing the semaphore once the buffer has been written to*/
    
    for (i = 0; i<NB_MSG; i++) {
        buffer_info[i] = malloc(15);
        printf("insert file name:\n");
    }

    for(i=0; i<N;i++){
        if(pthread_create(&t_id[i], NULL, verifica, NULL) != 0 ){
            printf("Error creating thread\n");
            exit(-1);
        }
    }
    
    /*Make this a loop while you want sent info to the buffer*/
    printf("insert file name:\n");
    int n=scanf("%s", buffer_comunication);
    while (strcmp(buffer_comunication, "sair")!=0) {
        printf("insert file name:\n");
        n=scanf("%s", buffer_comunication);
        
    }

    //write(buffer_comunication, filename, SizeBuffer);
    sem_post(&semaphore);

    /*end loop*/
    
    for(i=0; i<N; i++){
        if(pthread_join(t_id[i], NULL) != 0){
            printf("Error joining with thread %d\n", i);
            exit(-1);
        }
    }




    exit(0);
}
