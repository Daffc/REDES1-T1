#include "utilidades.h"

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

void local_ls(){
    // não precisa verificar autorização
    // utilizar ls do sistema
    // folder atual
    // nosso pwd relativo
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

void put(FILE * filedesk, Mensagem * msg){

    int try_send_name = 1;
    int try_send_fd = 1;
    int try_send_data = 1;
    int try_send_fim = 1;
    int has_data_to_sent = 1;
    int envio;
    int resposta;
    int reading;
    void *buffer;
    char **dados;
    int indice;
    int sequencia0,sequencia1,sequencia2;
    int tamanho_da_mensagem = 0;
    *dados = malloc(3);
    *dados[0] = malloc(TAMANHO_MAXIMO);
    *dados[1] = malloc(TAMANHO_MAXIMO);
    *dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    Mensagem    msg;
    msg.dados = malloc(127);

    msg.marcador_inicio = 126;
    msg.controle.tamanho = 127;
    // msg.controle.sequencia = 15;
    msg.controle.tipo = 1;
    strcpy(msg.dados, "Nome do arquivo");
    msg.crc = 81;
    defineBuffer(&msg, buffer);         
        
        while(try_send_name){
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
            printf("Verificação envio: %d\n", envio);
            reading = 1;
            memset(buffer,0,TAMANHO_MAXIMO);            
            while(reading){
                resposta = read(fileslave, buffer, TAMANHO_MAXIMO);
                if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    if(msg.controle.tipo == OK){
                        try_send_name = 0;
                        reading = 0;
                    }else{
                        // calcula novo tempo para enviar o dado
                        reading = 0;
                    }
                    memset(buffer,0,TAMANHO_MAXIMO);
                }                
            }           
        }

        while(try_send_fd){
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
            printf("Verificação envio: %d\n", resp);
            reading = 1;
            while(reading){
                resposta = read(fileslave, buffer, TAMANHO_MAXIMO);
                recuperaMensagem(&msg, buffer);
                msg.controle.tipo == OK{
                    try_send_name = 0
                }else{
                    // calcula novo tempo para enviar o dado
                    reading = 0;
                }
            } 
        }
        int i = 0;
        while(has_data_to_sent){
            
            
            sequencia0 = msg.controle.sequencia;      
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < TAMANHO_MAXIMO){
                *buffer[0] = getc(filedesk);
                i++;
                indice++;
            }
            tam0 = indice;
            sequencia1 = msg.controle.sequencia++;             
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < TAMANHO_MAXIMO){
                *buffer[1] = getc(filedesk);
                i++;
                indice++;
            }
            tam1 = indice;
            sequencia2 = msg.controle.sequencia++;
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < TAMANHO_MAXIMO){
                *buffer[2] = getc(filedesk);
                i++;
                indice++;
            }
            tam2 = indice;

                while(try_send_data){                    
                    msg.controle.tipo = DADOS;                    
                    msg.controle.sequencia = sequencia0;
                    defineBuffer(&msg, *buffer[0]);
                    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
                    printf("Verificação envio da primeira mensagem: %d\n", resp);
                    if(strlen(*buffer[1]) == 0){
                        msg.controle.tipo = NONE1;
                    }
                    msg.controle.sequencia = sequencia1;
                    defineBuffer(&msg, *buffer[1]);
                    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
                    printf("Verificação envio da segunda mensagem: %d\n", resp);
                    if(strlen(*buffer[2]) == 0){
                        msg.controle.tipo = NONE1;
                    }
                    msg.controle.sequencia = sequencia2;
                    defineBuffer(&msg, *buffer[2]);
                    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
                    printf("Verificação envio da terceira mensagem: %d\n", resp);
                
                    reading = 1;
                        while(reading){
                            resposta = read(fileslave, buffer, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer) == 126){
                                recuperaMensagem(&msg, buffer);
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                                msg.controle.sequencia = (msg.controle.sequencia + 3);
                            }
                            if(msg.controle.tipo == NACK){
                                reading = 0;
                            }
                            // calcula temporização aqui tambem
                        } 
                }
                if(i == tamanho_da_mensagem){
                    has_data_to_sent = 0;
                }   
        }        

        while(try_send_fim){
            msg.controle.tipo = FIM;
            envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
            printf("Verificação envio: %d\n", resp);
            reading = 1;            
            while(reading){
                resposta = read(fileslave, buffer, TAMANHO_MAXIMO);
                recuperaMensagem(&msg, buffer);
                msg.controle.tipo == OK{
                    try_send_fim = 0
                }else{
                    // calcula novo tempo para enviar o dado
                    reading = 0;
                    // e envia novamente
                }
            }
        }
    } 

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

