#include "ConexaoRawSocket.h"
#include "utilidades.h"
#include "comandos.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

int main(){

    int         conexao;
    char        retorno, comando[100], local[500], remoto[500];
    void        *buffer;

    conexao = ConexaoRawSocket("lo");

    // buffer = malloc(300);
    
    Mensagem    msg;
    msg.dados = malloc(127);
        

    strcpy(local, "~/");
    strcpy(remoto, "~/");

    // msg.marcador_inicio = 126;
    // msg.controle.tamanho = 127;
    // msg.controle.sequencia = 15;
    // msg.controle.tipo = 1;
    // msg.crc = 81;

    // defineBuffer(&msg, buffer);
    
    // // NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)
    // int resp = write(conexao, buffer, 4 + msg.controle.tamanho);
    // printf("Verificação envio: %d\n", resp);

    // TESTES COM COMANDOS !!!
    while(1){
        imprimeLocalizacao(local, remoto);

        // Le linha de comando indicada por usuario.
        scanf(" %99[^\n]", comando);

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "ls") ==  comando){
            local_ls(comando, local);
        } 

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "cd") ==  comando){
            printf("COMANDO CD\n");
        } 

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "rls") ==  comando){
            printf("COMANDO RLS\n");
        } 

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "rcd") ==  comando){
            printf("COMANDO RCD\n");
        } 

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "get") ==  comando){
            printf("COMANDO GET\n");
        } 

        /**
         * Caso comando inicie com a String "ls".
        */
        if(strstr(comando, "put") ==  comando){
            printf("COMANDO PUT\n");
        } 
    }    
    
    return 0;
}
