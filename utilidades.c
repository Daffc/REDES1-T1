#include "utilidades.h"
#include "comandos.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


/**
 * --------------------------------
 * ----- FUNÇÕES INTERNAS. --------
 * --------------------------------
*/
void removeEspacos(char * origem, char * destino){
    int i = 0;
    /**
    * Removendo espaços da string de origem e salva resutado em destino.
    */
    do
    {
        if (*origem != ' ')   
        {
            destino[i] = *origem;
            i++;
        }
        
    } while(*origem++);
    destino[i+1] = '\0';
}



/**
 * --------------------------------
 * ----- FUNÇÕES EXTERNAS. --------
 * --------------------------------
*/

/**
 * Devolve o tamanho da mensaem para envio (Necessário uma vez que mensagens de tamanho menor que 14 não são enviadas)
*/
int tamanhoMensagem(Mensagem *msg){

    // Varidfica se mensagem não tem tamanho necessario (minimo = 14) e retorna tamanho mínimo.
    if(msg->controle.tamanho  < 10)
        return 14;
    return (msg->controle.tamanho + 4);
}

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


int local_cd(char * comando, char * local){
    // verificar permissão/existência
    // int access(const char *, int); retorno errno
    // printar na tela o ERRO possivel

    char        operador[500], semEspacos[500];
    struct      stat path_stat;

    /**
     * Remove espaços de comando e guarda resultado em operador.
    */
    removeEspacos(comando, semEspacos);

    strcpy(operador, local);
    strcat(operador, "/");    
    strcat(operador, &semEspacos[2]);

    /**
     * Verifica existencia e permissão de leitura ao caminho indicado.
    */
    if(!access(operador, F_OK | R_OK)){        
        /**
         * Caso usuário deseje voltar um diretório.
        */
        if(strstr(comando + 3, "..")){
            /**
             * Procura pela última ocorrencia da '/' a string local e a substitui por '\0'
            */
            *(strrchr(local, '/')) = '\0';

            /**
             * Retorna 0, informando que operação foi concluida com sucesso
            */
            return 0;
        }
        else{
            stat(operador, &path_stat);
            if(S_ISDIR(path_stat.st_mode)){
                /**
                 * Redefine local
                */
                if(strstr(comando + 3, "..")){
                    /**
                     * Procura pela última ocorrencia da '/' a string local e a substitui por '\0'
                    */
                    *(strrchr(local, '/')) = '\0';
                }
                else{
                    /**
                     * Caso contrário, string analizada é copiada para local.
                    */
                    strcpy(local, operador);            
                }

                /**
                 * Retorna 0, informando que operação foi concluida com sucesso
                */
                return 0;
            }

            /**
             * Caso caminho seja válido porem não esteja relacionad a um diretório.
            */
            else{
                /**
                 * Retorna -1, informando que operação não pode ser 
                 * concluida devido ao tipo não condizer com um diretório.
                */
                return -1;
            }  
        }
    }
    /**
     * Caso não consiga acessar, printa para usuário erro.
    */
    else{
        /**
         * Retorna erro gerado pelo acesso indevido (Inexistência / Sem permissão de leitura)
        */
        return errno;
    }
}

int local_ls(char * comando, char * local, char *bufferSaida){
    char    retorno, operador[100];
    int     i = 0;

    FILE    *fpls = NULL;

    /**
     * Remove espaços de comando e guarda resultado em operador.
    */
    removeEspacos(comando, operador);

    //Busca por parametro indicado e inicializa conexão com stream de resposta.
    if(operador[2] == '\0'){
        fpls = IniciaDescritorLs("ls", local);
    }     
    if(strstr(operador+2, "-l")){
        fpls = IniciaDescritorLs("ls -l", local);
    }
    if(strstr(operador+2, "-a")){
        fpls = IniciaDescritorLs("ls -a", local);
    }
    if(strstr(operador+2, "-al") || strstr(operador+2, "-la")){
        fpls = IniciaDescritorLs("ls -la", local);
    }

    /**
     * Caso Stream conectando a resposta seja definida, imprime resuldato.
    */
    if(fpls){
        /*Le caracter por caracter do arquivo de resposta do ls aberto guarda-o em bufferSaida*/
        while ((retorno = getc(fpls)) != EOF) {
            bufferSaida[i] = retorno ;
            i++;
        }
        bufferSaida[i] ='\0';
        
        // Finaliza descritor utilizado.
        FinalizaDescritorLs(fpls);
        return 0;  
    }

    /**
     * Caso comando seja desconhecido, informa ao usuário.
    */
    else
        return 1;
}

void remote_cd(){
    // Recebe chamada de mestre, invoca servidor com pedido de 'cd' e espera por resposta.
    // servidor invoca cd local e retorna para mestre resposta do 'cd' solicitado.
    // Recebe resposta e retorna '0' caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
}

void remote_ls(){
    // Recebe chamada de mestre, invoca servidor com pedido de 'ls' e espera por resposta.
    // servidor invoca cd local e retorna para mestre resposta do 'ls' solicitado.
    // Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
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

