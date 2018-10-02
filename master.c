#include "ConexaoRawSocket.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){


    int filedesk;

    filedesk = ConexaoRawSocket("lo");

    MensagemContent msg;
    msg.a = 10;
    msg.b = 11;
    msg.c = 12;
    msg.d = 13;
    msg.e = 14;
    msg.f = 15;

    //NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    int resp = write(filedesk, &msg, sizeof(MensagemContent));
    printf("%d\n", resp);

    


    return 0;
}
