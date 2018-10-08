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

    filedesk = ConexaoRawSocket("lo");

    buffer = malloc(300);
    
    Mensagem    msg;
    msg.dados = malloc(127);
        

    msg.marcador_inicio = 126;
    msg.controle.tamanho = 127;
    msg.controle.sequencia = 15;
    msg.controle.tipo = 1;
    strcpy(msg.dados, "Surprise steepest recurred landlord mr wandered amounted of. Continuing devonshire but considered its. Rose past oh shew roof is song neat. Do depend better");
    msg.crc = 81;

    defineBuffer(&msg, buffer);
    
    //NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    int resp = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
    printf("Verificação envio: %d\n", resp);
    

    // TESTES COM COMANDOS !!!
    // while(1){
    //     // Le linha de comando indicada por usuario.
    //     scanf(" %99[^\n]", comando);

    //     // Inicializa descritor com resposta do comando indicado.
    //     fpls = IniciaDescritorComando(comando);

    //         /*Le linha por linha arquivo de resposta do ls aberto e gerencia tratamento*/
    //         while (fgets(retorno, 127, fpls) != NULL) {
    //             printf("%s|", retorno);
    //         }

    //     // Finaliza descritor utilizado.
    //     FinalizaDescritorComando(fpls);
    // }    

    
    return 0;
}
