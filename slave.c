#include "ConexaoRawSocket.h"
#include "utilidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <pwd.h>
#include <poll.h>


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


int main(int argc, char *argv[]){

    int saidaread;
    int estado, estado2;
    void *buffer;

    struct pollfd fds[1];

    fds[0].fd = ConexaoRawSocket("eno1");
    fds[0].events = POLLIN;

    calcula_tabela_crc();

    Mensagem    msg;
    msg.dados = malloc(127);

    buffer = malloc(TAMANHO_MAXIMO);

    while(1){

        estado = read(fds[0].fd, buffer, TAMANHO_MAXIMO);

        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if(*((unsigned char *)buffer) == 126){
            if(recuperaMensagem(&msg, buffer)){

                printf("%d\t", msg.marcador_inicio);
                printf("%d\t%d\t%d\t", msg.controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
                printf("%s\t", (char *)msg.dados);
                printf("%d\n", msg.crc);

                switch(msg.controle.tipo){
                    case HANDSHAKE:
                        getcwd((char *)msg.dados, 500);

                        /**
                         * Define mensagem com nome do usuario atual.
                        */
                        msg.marcador_inicio = 126;
                        msg.controle.tamanho = strlen((char *)msg.dados) + 1;
                        // memcpy(msg.dados, local, strlen(local) + 1);
                        msg.controle.sequencia++;
                        
                        defineBuffer(&msg, buffer);
                        write(fds[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho));
                        memset(buffer, 0, TAMANHO_MAXIMO);
                        break;
                    case CD:
                        trata_cd(fds[0].fd,&msg);
                        break;
                    case LS:
                        trata_ls(fds,&msg);
                        break;
                    case GET:
                        trata_get(fds,&msg);
                        break;
                    case PUT:
                        trata_put(fds[0].fd,&msg);
                        break;
                    case NACK:
                        write(fds[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho));
                }
            }else{
                msg.marcador_inicio = 126;
                msg.controle.tamanho = 0;
                msg.controle.sequencia++;
                msg.controle.tipo = NACK;

                defineBuffer(&msg, buffer);

                write(fds[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho));
            }
        }
    }
    return 0;
}