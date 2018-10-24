#include "ConexaoRawSocket.h"
#include "utilidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>



int main(){

    int fileslave;
    int saidaread;
    int estado, estado2;
    void *buffer;
    fileslave = ConexaoRawSocket("lo");

    Mensagem    msg;
    msg.dados = malloc(127);

    buffer = malloc(TAMANHO_MAXIMO);

    while(1){
        
        estado = read(fileslave, buffer, TAMANHO_MAXIMO);

        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if(*((unsigned char *)buffer) == 126){
            recuperaMensagem(&msg, buffer);

            if(msg.controle.tipo == PUT){
                trata_put(fileslave,msg);
            }
            
            printf("%d\t", msg.marcador_inicio);
            printf("%d\t%d\t%d\t", msg.controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
            printf("%s\t", (char *)msg.dados); 
            printf("%d\n", msg.crc);
            
        }   
        printf("**********************\n");
    
    }   
    return 0;
}