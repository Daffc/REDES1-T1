#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

    int fileslave;
    int saidaread;
    fileslave = ConexaoRawSocket("lo");

    char* msg;
    msg = malloc(sizeof(char)*20);
    
    while(1){
        saidaread = read(fileslave, msg, 20);
        printf("%s \n",msg);
    }

    return 0;
}