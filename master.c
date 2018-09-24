#include "ConexaoRawSocket.h"
#include <unistd.h>
#include <stdlib.h>

int main(){


    int filedesk;

    filedesk = ConexaoRawSocket("lo");

    char* msg;
    msg = malloc(sizeof(char)*20);
    msg = "walkinghere";

    write(filedesk, msg, 20); 
    


    return 0;
}
