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

void put(FILE * filedesk, char *name){

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
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
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
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                recuperaMensagem(&msg, buffer);
                if(msg.controle.tipo == OK){
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
                if(msg.controle.tipo == OK){
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

void ordena(int *min,int *med, int *max){

    int tmp0 = *min;
    int tmp1 = *med;
    int tmp2 = *max;

    int medio = tmp1;
    int maior1 = medio++ % 32;
    int maior2 = maior1++ % 32;
    int menor1 = medio-- % 32;
    int menor2 = menor1-- % 32;

        if(tmp0 == menor1 && tmp2 == maior1){
            //then sequencia:
            *min = tmp0;
            *med = tmp1;
            *max = tmp2;
            return;
        }
        if(tmp0 == maior1 && tmp2 == menor1){
            //then sequencia:
            *min = tmp2;
            *med = tmp1;
            *max = tmp0;
            return;
        }
        if(tmp0 == maior1 && tmp2 == maior2){
            //then sequencia:
            *min = tmp1;
            *med = tmp0;
            *max = tmp2;
            return;    
        }
        if(tmp0 == maior2 && tmp2 == maior1){
            //then sequencia:
            *min = tmp1;
            *med = tmp2;
            *max = tmp0;
            return;    

        }
        if(tmp0 == menor1 && tmp2 == menor2){
            //then sequencia:
            *min = tmp2;
            *med = tmp0;
            *max = tmp1;
            return;
        }
        if(tmp0 == menor2 && tmp2 == menor1){
            //then sequencia:
            *min = tmp0;
            *med = tmp2;
            *max = tmp1;
            return;
        }
        printf("Script de ordernar has a breach !!!\n");        

}

void trata_put(FILE *filedesk, Mensagem *first_msg){

    int envio;
    int resposta;
    void *buffer;
    void *buffer0;
    void *buffer1;
    void *buffer2;
    int tmp0,tmp1,tmp2;
    int min,med,max;
    int tam_min,tam_med,tam_max;
    int min,med,max;
    int send_confirmation_fd = 1;
    int send_confirmation_name = 1;
    int wait_for_fd = 1;    
    int wait_for_data = 1;
    int status = 1;
    int try_send_ack = 1;
    int try_send_nack = 1;
    int wait_for_end = 1;
    // lembre-se do free mate
    char **dados;
    *dados = malloc(3);
    *dados[0] = malloc(TAMANHO_MAXIMO);
    *dados[1] = malloc(TAMANHO_MAXIMO);
    *dados[2] = malloc(TAMANHO_MAXIMO);
    Mensagem msg0;
    Mensagem msg1;
    Mensagem msg2;
    msg0.dados = malloc(127);
    msg1.dados = malloc(127);
    msg2.dados = malloc(127);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);

    // tenta criar um arquivo e devolve resposta de sucesso com o nome...
    (char *) first_msg->dados;

    Mensagem msg;
    msg.dados = malloc(127);
    buffer = malloc(TAMANHO_MAXIMO);
    msg.marcador_inicio = 126;
    msg.controle.tamanho = 127;
    msg.controle.sequencia = first_msg->controle.sequencia++;
    msg.controle.tipo = OK;
    // strcpy(msg.dados, "Nome do arquivo");
    msg.crc = 81;
    defineBuffer(&msg, buffer);
    while(send_confirmation_name && status){
    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
        while(wait_for_fd){
            resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
            if(*((unsigned char *)buffer) == 126){
            recuperaMensagem(&msg, buffer);
                if(msg.controle.tipo == FD){
                    wait_for_fd = 0;
                    send_confirmation_name = 0;
                }
            }
        }
    }

    // aqui eu ja possui o file desk na mensagem
    while(send_confirmation_fd && status){
    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
        while(wait_for_data){
            resposta = read(filedesk, buffer0, TAMANHO_MAXIMO);
            resposta = read(filedesk, buffer1, TAMANHO_MAXIMO);
            resposta = read(filedesk, buffer2, TAMANHO_MAXIMO);
            if(*((unsigned char *)buffer0) == 126 && *((unsigned char *)buffer1) == 126 && *((unsigned char *)buffer2) == 126){
                recuperaMensagem(&msg0, buffer0);
                recuperaMensagem(&msg1, buffer1);
                recuperaMensagem(&msg2, buffer2);
                int tmp0 = msg0.controle.sequencia;
                int tmp1 = msg1.controle.sequencia;
                int tmp2 = msg2.controle.sequencia;

                // strcpy(dados[0],(char *) msg0.dados);
                // strcpy(dados[1],(char *) msg1.dados);
                // strcpy(dados[2],(char *) msg2.dados);

                // verificar com o douglas se precisa de memset

                min = tmp0;
                med = tmp1;
                max = tmp2;

                ordena(&min,&med,&max);

                if(min == msg0.controle.sequencia){
                    // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                }else if(min == msg1.controle.sequencia){
                    // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                }else{
                    // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                }
                if(med == msg0.controle.sequencia){
                    // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                }else if(med == msg1.controle.sequencia){
                    // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                }else{
                    // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                }
                if(max == msg0.controle.sequencia){
                    // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                }else if(max == msg1.controle.sequencia){
                    // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                }else{
                    // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                }
                try_send_ack = 1;
                while(try_send_ack){
                    envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
                    // se o ack for sucessful faça o teste abaixo , caso contrario tente enviar o ack
                    // vou parar quando um dos tamanhos seja != 127
                    if(msg0.controle.tamanho != 127 || msg0.controle.tamanho != 127 || msg0.controle.tamanho != 127){
                    // aqui eu tenho que garantir que o ack ou nack tenha sido recebido antes de eu poder setar essas variaveis abaixo                    
                        wait_for_data = 0;
                        send_confirmation_fd = 0;
                    }
                }                
                
            }
            
        }
        // definir quando exatamente manda nack
        while(try_send_nack){
            envio = send(filedesk, buffer, 4 + msg.controle.tamanho, 0);
            // arrumar uma garantia de enviar o nack...
        }
    }

    while(wait_for_end){
        resposta = read(filedesk, buffer0, TAMANHO_MAXIMO);
    }
}



// o problema não é eles terem saido ordenado e sim como relacionar com a mensagem em si                
// ordena as menasgens do capeta
// fazer uma comparação entre os buffers em relação ao 
// ordenar as mensagens 
// se uma das mensagens for de tamanho diferente de 127 isso quer dizer que estou aguardando a mensagem fim e wait_for_data = 0 e send_confirmation_fd = 0
// envia ACK E acaba com as mensagens



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

