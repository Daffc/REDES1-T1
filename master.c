#include "ConexaoRawSocket.h"
#include "utilidades.h"
#include "comandos.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

int main(){

    int         filedesk;
    char        retorno[1035], comando[500];
    Mensagem    msg;
    FILE        *fpls;

    // filedesk = ConexaoRawSocket("lo");


    // msg.marcador_inicio = 126;
    // msg.tamanho = 10;
    // msg.sequencia = 15;
    // msg.tipo = 1;
    // msg.crc = 81;
    

    //NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    // int resp = write(filedesk, &msg, 14);

    while(1){
        // Le linha de comando indicada por usuario.
        scanf(" %99[^\n]", comando);

        // Inicializa descritor com resposta do comando indicado.
        fpls = IniciaDescritorComando(comando);

            /*Le linha por linha arquivo de resposta do ls aberto e gerencia tratamento*/
            while (fgets(retorno, 127, fpls) != NULL) {
                printf("%s|", retorno);
            }

        // Finaliza descritor utilizado.
        FinalizaDescritorComando(fpls);
    }    

    
    return 0;
}
