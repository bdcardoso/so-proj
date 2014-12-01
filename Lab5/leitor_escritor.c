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

sigset_t new_set, old_set;

void func_SIGUSR1(){
    printf("Inverting file lock utilization\n");
    return;
}
void func_SIGUSR2(){
    printf("Inverting error write\n");
    return;
}

void func_SIGSTOP(){
    printf("Going to exit child %d\n", getpid());
    return;
}

int main(){
    int pipe_fd[2];
    char *buffer=(char*)malloc(sizeof(char)*BUFFSIZE);
    pid_t pid_child1;
    int status;
    
    signal(SIGUSR1, func_SIGUSR1);
    signal(SIGUSR2, func_SIGUSR2);
    signal(SIGSTOP, func_SIGSTOP);
    /*child process #1 launch for writer*/
    if( (pid_child1 = fork()) == 0){
        
        sigfillset(&new_set);
        sigdelset(&new_set, SIGUSR1);
        sigdelset(&new_set, SIGUSR2);
        sigdelset(&new_set, SIGSTOP);
        sigprocmask(SIG_BLOCK, &new_set, &old_set);
        
        getchar();
        exit(0);
    }
   
   /*terminal pipe_fd[0] -> reading terminal pipe_fd[1] -> writing*/
    if( pipe(pipe_fd) == -1 ){
    	printf("Error creating pipe\n");
    	free(buffer);
    	exit(-1);
    }
   
    /*child process #2 launch for reader*/
    if( fork() == 0){
        close(pipe_fd[WRITE]);
        close(STDIN);
        dup(pipe_fd[READ]);
        close(pipe_fd[READ]);        
        while(1){
            read(STDIN, buffer, BUFFSIZE);
            printf("%s\n", buffer);
        }
        free(buffer);
        exit(0);
    }

    close(pipe_fd[READ]);
    close(STDOUT);
    dup(pipe_fd[WRITE]);
    close(pipe_fd[WRITE]);
    
    while(1){
        memset(buffer, '\0', BUFFSIZE);
        read(STDIN, buffer, BUFFSIZE);
        if( strstr(buffer, "sair") != NULL){
            kill(pid_child1, SIGSTOP);
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
        write(STDOUT, buffer, BUFFSIZE);
    }
    
    free(buffer);
    exit(0);
}
