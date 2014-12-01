#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1
#define READ 0
#define WRITE 1
#define BUFFSIZE 1024

int main(){
    int pipe_fd[2];
    char *buffer=(char*)malloc(sizeof(char)*BUFFSIZE);

    
    /*child process launch for writer*/
    if( fork() == 0){
        exit(0);
    }
   
   /*terminal pipe_fd[0] -> reading terminal pipe_fd[1] -> writing*/
    if( pipe(pipe_fd) == -1 ){
    	printf("Error creating pipe\n");
    	free(buffer);
    	exit(-1);
    }
   
    /*child process launch for reader*/
    if( fork() == 0){
        close(pipe_fd[WRITE]);
        close(STDIN);
        dup(pipe_fd[READ]);
        close(pipe_fd[READ]);        

        read(STDIN, buffer, 11);
        printf("%s\n", buffer);
        free(buffer);
        exit(0);
    }

    close(pipe_fd[READ]);
    close(STDOUT);
    dup(pipe_fd[WRITE]);
    close(pipe_fd[WRITE]);

    write(STDOUT, "Hello son!", 11);

    getchar();
    free(buffer);
    exit(0);
}
