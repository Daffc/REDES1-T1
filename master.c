#include "ConexaoRawSocket.h"
#include "utilidades.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

int main(){

    int filedesk;

    filedesk = ConexaoRawSocket("lo");

    Mensagem msg;

        msg.marcador_inicio = 126;
        msg.tamanho = 10;
        msg.sequencia = 15;
        msg.tipo = 1;
        msg.crc = 81;

        printf( "Mensagem: %d\n", sizeof(Mensagem));
        

    //NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    int resp = write(filedesk, &msg, 14);
    printf("%d\n", resp);

    


    return 0;
}
