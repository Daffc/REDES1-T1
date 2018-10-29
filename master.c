#include "ConexaoRawSocket.h"
#include "utilidades.h"
#include "comandos.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

int main(){

    int         filedesk;
    char        retorno[1035], comando[500];
    void        *buffer;
    FILE        *fpls;

    filedesk = ConexaoRawSocket("eno1");

    // buffer = malloc(TAMANHO_MAXIMO);
    
    // Mensagem    msg;
    // msg.dados = malloc(127);
        

    // msg.marcador_inicio = 126;
    // msg.controle.tamanho = 127;
    // msg.controle.sequencia = 15;
    // msg.controle.tipo = PUT;
    // strcpy(msg.dados, "dados");
    // msg.crc = 81;

    // defineBuffer(&msg, buffer);

    put(filedesk,"dados");
    
    //NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    // int resp =  send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
    // printf("Verificação envio: %d\n", resp);


    
    return 0;
}

