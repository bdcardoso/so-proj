#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <sys/file.h> /* flock() */
#include <errno.h>

#define NUM_CHILDREN 3
#define NB_FILES       5
#define NB_ENTRIES     1024
#define STRING_SZ      10
#define NB_STRINGS     10

#define NUM_REQUEST 10
#define BUFFSCANF 13
#define NUM_SCAN 5

#define STDIN 0
#define STDOUT 1

int reader(char  *file_to_open) {
    int fd;
    //char *buf = (char*)malloc(sizeof(char)*(BUFFSCANF-2));
    //buf = file_to_open;
    //file_to_open = buf;
    //strncpy(buf, file_to_open, 2);
    //strncpy(file_to_open,buf, 2);
    //file_to_open[BUFFSCANF] = '\0';
    //printf("Monitor will check if file %s is consistent...", file_to_open);
    
    file_to_open[BUFFSCANF-1]= '\0';
    
    printf("------->>>>>>> %s <<<<<<<<<--------\n", file_to_open);
    
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
        
        for (i=0; i<NB_ENTRIES-1; i++) {
            
            if (read (fd, string_to_read, STRING_SZ) != 10) {
                fprintf (stderr,"\nError reading file: %s\n", file_to_open);
                printf("--- %s --- %d\n",string_to_read, i);
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


void *func_thread( void * argumentos){
    char *buffer = (char*)malloc(sizeof(char)*BUFFSCANF);
    while(1){
        read(STDIN, buffer, BUFFSCANF);
    
        reader(buffer);
    }
    pthread_exit(0);
}


int main(){
    //char *buffer = (char*) malloc(sizeof(char)*NUM_REQUEST*BUFFSCANF);
    //char *buffer_scanf = (char *) malloc(sizeof(char)*NUM_SCAN*BUFFSCANF);
    /*int n_read = 0, count=0; */
    int i;
    //char *ptr;
    //char buffer_mat[NUM_SCAN][BUFFSCANF];
    pthread_t *t_id = (pthread_t*) malloc(sizeof(pthread_t)*NUM_SCAN);
    
/*
    read(STDIN, buffer_scanf, NUM_SCAN*BUFFSCANF);
    
    printf("%s\n", buffer_scanf);
    
    ptr = buffer_scanf;
    do{
        n_read = sscanf(ptr, "%s ", buffer_mat[count]);
        printf("N_Read = %d\t%s\n", n_read, buffer_mat[count]);
        ptr++;
        count++;
    }while( ( ptr = strstr(ptr, " ")) != NULL);
*/
    for( i=0; i < NUM_SCAN; i++){
        if (pthread_create(&t_id[i], NULL, func_thread, NULL) != 0){
            printf("Erro a criar a string");
            exit(-1);
        }
    }
    
    for(i=0; i< NUM_SCAN; i++){
        if(pthread_join(t_id[i], NULL)){
            printf("Erro no join");
            exit(-1);
        }
    }
    
    
    exit(0);
}
