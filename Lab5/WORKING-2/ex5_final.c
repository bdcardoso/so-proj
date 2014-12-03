#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define READ 0
#define WRITE 1
#define BUFFSIZE 1024
#define NB_FILES 5

int main(){
    int pipe_fd[2];
    char *buffer=(char*)malloc(sizeof(char)*BUFFSIZE);
    pid_t pid_child1, pid_child2;
    int status;
    int nbytes;
    int i;
    sem_t *semaphores[NB_FILES];
    char *sem_name=(char*)malloc(sizeof(char)*7);
    
    /*for(i=0; i<NB_FILES; i++){
        sprintf(sem_name, "/semaf%d", i);
        if ( (semaphores[i] = sem_open(sem_name, O_CREAT, O_RDWR, 1) ) == SEM_FAILED){
            printf("no es pupu\n");
        }
    }/*
    sem_wait((sem_t*)3);

    /*child process #1 launch for writer*/
    if( (pid_child1 = fork()) == 0){
        
        execl("ex5_escritor", "ex5_escritor", NULL);
        exit(0);
    }
   
   /*terminal pipe_fd[0] -> reading terminal pipe_fd[1] -> writing*/
    if( pipe(pipe_fd) == -1 ){
    	printf("Error creating pipe\n");
    	free(buffer);
    	exit(-1);
    }
    printf("%d - %d\n", pipe_fd[0], pipe_fd[1]);

    /*child process #2 launch for reader*/
    if( (pid_child2 = fork()) == 0){
        close(pipe_fd[WRITE]);
        close(STDIN);
        dup(pipe_fd[READ]);
        close(pipe_fd[READ]);
        
        execl("ex5_leitor", "ex5_leitor", NULL);
        
        exit(0);
    }
/*
    close(pipe_fd[READ]);
    close(STDOUT);
    dup(pipe_fd[WRITE]);
    close(pipe_fd[WRITE]);
*/
    while(1){
        memset(buffer, '\0', BUFFSIZE);
        nbytes = read(STDIN, buffer, BUFFSIZE);
        printf("-> Bytes lidos %d\n", nbytes);
        if( strstr(buffer, "sair") != NULL){
            kill(pid_child1, SIGTSTP);
            waitpid(pid_child1, &status, 0);
            break;
        }
        if( strstr(buffer, "il") != NULL){
            kill(pid_child1, SIGUSR1);
            continue;
        }
        if( strstr(buffer, "ie") != NULL){
            kill(pid_child1, SIGUSR2);
            continue;
        }
        buffer[nbytes-1] = '\0';
        write(pipe_fd[WRITE], buffer, nbytes);
    }
    
    /*for(i=0; i<NB_FILES; i++){
        sprintf(sem_name, "/semaf%d", i);
        sem_close(semaphores[i]);
        sem_unlink(sem_name);
        memset(sem_name, '\0', 7);
    }*/
    
    kill(pid_child2, SIGSTOP);
    free(buffer);
    exit(0);
}
