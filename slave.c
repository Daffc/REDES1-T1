#include "ConexaoRawSocket.h"
#include "utilidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <pwd.h>


void handshakeMsg(Mensagem * msg){

        /**
         * Recupera nome do usuário atual.
        */
        char local_dir[500];
        
        getcwd(local_dir, 500);
    
        /**
         * Define mensagem com nome do usuario atual.
        */
        msg->marcador_inicio = 126;
        msg->controle.tamanho = strlen(local_dir) + 1;
        memcpy(msg->dados, local_dir, strlen(local_dir) + 1);
        msg->controle.sequencia++;
        
        /**
         * Calcular CRC AQUI !!!.
        */
        /**/msg->crc = 81;
        /*---------------------*/
}


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
            
            printf("%d\t", msg.marcador_inicio);
            printf("%d\t%d\t%d\t", msg.controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
            printf("%s\t", (char *)msg.dados); 
            printf("%d\n", msg.crc);

            switch(msg.controle.tipo){
                case HANDSHAKE:
                    handshakeMsg(&msg);
                    defineBuffer(&msg, buffer);
                    write(fileslave, buffer, tamanhoMensagem(&msg));
                    memset(buffer, 0, TAMANHO_MAXIMO);
                    break;
            }            
        }       
    }   
    return 0;
}