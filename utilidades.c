#include "utilidades.h"
#include "comandos.h"

#include <string.h>
#include <stdio.h>


void defineBuffer(Mensagem * msg, void * buffer){

    memcpy(buffer, &(msg->marcador_inicio), 1);
    memcpy(buffer + 1, &(msg->controle), 2);
    memcpy(buffer + 3, msg->dados, msg->controle.tamanho); 
    memcpy(buffer + 3 + msg->controle.tamanho, &(msg->crc), 1);    
}


void recuperaMensagem(Mensagem * msg, void * buffer){

    // Guarda Marcador de início;
    memcpy(&(msg->marcador_inicio), buffer, 1);

    // Guarda informações de controle (tamanho, sequencia, tipo)
    memcpy(&(msg->controle), buffer + 1, 2);

    //Guarda dados de acordo com o informado em controle.tamanho
    memcpy(msg->dados, buffer + 3, msg->controle.tamanho); 

    // Guarda crc.   
    memcpy(&(msg->crc),buffer + 3 + msg->controle.tamanho, 1);    
    
}

/**
 * Imprime localidades LOCAL e REMOTA.
*/
void imprimeLocalizacao(char *local,char *remoto){
    printf("\x1b[32m" "LOCAL" "\x1b[0m" ":\t");
    printf("%s \n", local);
    printf("\x1b[32m" "REMOTO" "\x1b[0m" ":\t");
    printf("%s \n", remoto);
}



/*
    Inicialização:
        - /
        String acumula a posição atual
*/

/*
    Todas devem possuir

    imprimir o local atual acessado por cd/ls
    Estados:
        - $ local
        - $ remoto

            
*/

/*
    Comentarios Rafael noob : olhar onde o programa esta sendo executado

*/
void local_cd(){
    // verificar permissão/existência
    // int access(const char *, int); retorno errno
    // printar na tela o ERRO possivel
}

void local_ls(char * comando, char * local){
    char    retorno;
    FILE    *fpls;
    

    // Inicializa descritor com resposta do comando indicado.
    fpls = IniciaDescritorLs(comando, local);

    /*Le caracter por caracter do arquivo de resposta do ls aberto e gerencia tratamento*/
    while ((retorno = getc(fpls)) != EOF) {
        printf("%c", retorno);
    }
    
    // Finaliza descritor utilizado.
    FinalizaDescritorLs(fpls);
}

void remote_cd(){
    // verificar permissão/existência
    // int access(const char *, int); retorno errno
    // devolver como mensagem slave > master
}

void remote_ls(){
    // não precisa verificar autorização
    // devolver como mensagem slave > master

}

void put(){
    
    // envia o nome
    // abre write com esse nome
    // espera pela resposta OK
    // enviar descritor/tamanho do arquivo / verificar espaço
    // espera resposta ok ou erro se erro olhar dados
    // começa enviar dados 
    // le 127 em 127 adiciona em uma mensagem incrementa , tam/127
    // if < 127 
    // getchar ++ até construir mensagem , deu 127 faz não deu trata 
}

void get(){
    // envia o nome
    // abre write com esse nome
    // espera pela resposta OK
    // enviar descritor/tamanho do arquivo / verificar espaço
    // espera resposta ok ou erro se erro olhar dados
    // começa enviar dados 
    // le 127 em 127 adiciona em uma mensagem incrementa , tam/127
    // if < 127 
    // getchar ++ até construir mensagem , deu 127 faz não deu trata 
}

