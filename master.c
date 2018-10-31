#include "ConexaoRawSocket.h"
#include "utilidades.h"
#include "comandos.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <pwd.h>

int main(){

    int         conexao;
    char        retorno, comando[100], local[500], remoto[500], *uName;
    void        *buffer;
    Mensagem    msg;

    conexao = ConexaoRawSocket("eno1");

    buffer = malloc(TAMANHO_MAXIMO);
    
    msg.dados = malloc(127);
        

    // Definindo mensagem inicial.
    msg.marcador_inicio = 126;
    msg.controle.tamanho = 0;
    msg.controle.sequencia = 0;
    msg.controle.tipo = HANDSHAKE;
    msg.crc = 81;
    
    defineBuffer(&msg, buffer);
    int resp = write(conexao, buffer, tamanhoMensagem(msg.controle.tamanho));

    if(resp < 0){
        printf("Erro ao enviar Mensagem: HANDSHAKE\n");
        exit(-1);
    }
    
    memset(buffer, 0, TAMANHO_MAXIMO);  
    while(!msg.controle.sequencia){

        resp = read(conexao, buffer, 4 + TAMANHO_MAXIMO);

        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if(*((unsigned char *)buffer) == 126){
            
            recuperaMensagem(&msg, buffer);

            printf("%d\t", msg.marcador_inicio);
            printf("%d\t%d\t%d\t", msg.controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
            printf("%s\t", (char *)msg.dados); 
            printf("%d\n", msg.crc);

            // strcpy(local, "/home/");
            // strcpy(remoto, "/home/");
            
            uName = getpwuid(geteuid ())->pw_dir; 

            getcwd(local, 500);
            strcpy(remoto, msg.dados);      
        }       
    }          


    

    
    // // NÃO ENVIA MSG SE TAMANHO DA MENSAGEM FOR MENOR QUE 14(BYTES)

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
            local_cd(comando, local);
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
            put(conexao,"dados");
        } 
    }    
    
    
    return 0;
}

