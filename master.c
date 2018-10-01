#include "ConexaoRawSocket.h"
#include <unistd.h>
#include <stdlib.h>


int main(){


    int filedesk;

    passar jao;
    jao.a = 1;
    jao.b = 5;
    jao.c = 19;

    filedesk = ConexaoRawSocket("lo");

    char* msg;
    msg = malloc(sizeof(char)*20);
    msg = "walkinghere";

    write(filedesk, &jao, sizeof(passar));
    


    return 0;
}
