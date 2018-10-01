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

    passar derp;
    
    while(1){
        saidaread = read(fileslave, &derp, sizeof(passar));
        printf("%d %d %d\n",derp.a,derp.b, derp.c);
    }

    return 0;
}