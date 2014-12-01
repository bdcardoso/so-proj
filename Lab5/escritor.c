#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

sigset_t new_set, old_set;

/*One must know if after recieving a signal the threads must start using the new definitions or this action will only take place AFTER the current loop finishes*/

void invert(){
    /*Flag that makes the writer ignore locks or use locks(?)*/
}

void errors(){
    /*Switches a flag so that erros are now writing to the file or that ignore writing erros to the file*/
}

void stop(){
    /*Let the process finish*/
    /*For this to happen we can use a integrer that allows us to see how many threads are active and how many threads are idle once the number of active threads is 0 then we know we can end the program*/
}


int main(){

    signal(SIGUSR1, invert);
    signal(SIGUSR2, errors);
    signal(SIGSTOP, stop);

    /*If we want to ignore ALL other signals add the code below*/
    sigfillset(&new_set);
    sigdelset(&new_set, SIGUSR1);
    sigdelset(&new_set, SIGUSR2);
    sigdelset(&new_set, SIGSTOP);
    sigprogmask(SIG_BLOCK, &new_set, &old_set);
    /*End of code to ignore signals*/

    /*Add code that launches the paralel writer*/

    exit(0);
}
