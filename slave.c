#include "ConexaoRawSocket.h"
#include "utilidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>



int main(){

    int fileslave;
    int saidaread;
    fileslave = ConexaoRawSocket("lo");

    Mensagem* msg;

    msg = malloc(sizeof(Mensagem));
    
    while(1){
        
        recv(fileslave, msg, 14, 0);
        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if(msg->marcador_inicio == 126)   
            printf("%d, %d, %d, %d, %d\n", msg->marcador_inicio, msg->tamanho, msg->sequencia, msg->tipo, msg->crc);
        printf("**********************\n");
    
    }   
    return 0;
}