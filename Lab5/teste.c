#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){
    char buffer[1];
    scanf("%s", &buffer);
    while (strcmp(buffer,"sair")!=0){
        printf("%s\n",buffer);
        scanf("%s", &buffer);
    }
    return 0;
}
