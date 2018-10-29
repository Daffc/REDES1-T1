#include "utilidades.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
// #include <sys/stat.h>

int tamanhoMensagem(int i){

    // Varidfica se mensagem não tem tamanho necessario (minimo = 14) e retorna tamanho mínimo.
    if(i  < 10)
        return 14;
    return (i + 4);
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

void put(int filedesk, char *name){


    int try_send_name = 1;
    int try_send_fd = 1;
    int try_send_data = 1;
    int try_send_fim = 1;
    int has_data_to_sent = 1;
    int last_seq;
    int envio;
    int resposta;
    int reading;
    void *buffer;
    void *buffer0;
    void *buffer1;
    void *buffer2;
    int indice;
    int sequencia0,sequencia1,sequencia2;
    int tamanho_da_mensagem;
    int tam0,tam1,tam2;


    char **dados;

    dados = malloc(3);
    //
    dados[0] = malloc(TAMANHO_MAXIMO);
    dados[1] = malloc(TAMANHO_MAXIMO);
    dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);
    Mensagem    msg;
    msg.dados = malloc(127);
    msg.marcador_inicio = 126;
    msg.controle.tamanho = strlen(name) + 1;






    FILE *fd = NULL;
    fd = fopen(name,"r");
    if(fd == NULL){
        printf("erro ao criar filidescriptor do arquivo : %s \n",name);
        return;
    }
    // recebe o tamanho da mensagem
    // struct stat bufferino;
    // stat(fd,&bufferino);
    fseek(fd,0,SEEK_END);
    tamanho_da_mensagem = ftell(fd);

    printf("tamanho da mensagem %d\n",tamanho_da_mensagem);
    // devolve o ponteiro no inicio do file descriptor
    rewind(fd);

        last_seq = msg.controle.sequencia;


        while(try_send_name){
            // monta mensagem para enviar o nome
            msg.marcador_inicio = 126;
            msg.controle.tamanho = strlen(name) + 1;
            msg.controle.sequencia = last_seq;
            msg.controle.tipo = PUT;
            strcpy(msg.dados, name);
            msg.crc = 81;
            defineBuffer(&msg, buffer);
            // envia a mensagem
            envio = send(filedesk, buffer, 14, 0);

            printf("debug:\"Verificação envio do nome do arquivo: %d\n", envio);

            // evitar loop infinito
            *((unsigned char *)buffer) = 0;
            // inicio de processo para aguardar OK ou reenviar a o nome
            reading = 1;
            // memset(buffer,0,TAMANHO_MAXIMO);
            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    // sucesso ao receber o ok do servidor
                    if(msg.controle.tipo == OK){
                        printf("debug:\"Mensagem recebida pelo servidor\n");
                        try_send_name = 0;
                        reading = 0;
                    }else{
                        // calcula novo tempo para enviar o dado
                        printf("debug:\"Mensagem não recebida pelo servidor envia novamente\n");
                        reading = 0;
                    }
                    // evitar loop infinito
                    *((unsigned char *)buffer) = 0;
                }
            }
        }

        last_seq++;

        // apos enviar o nome tenta enviar o file descriptor
        while(try_send_fd){

            // monta mensagem para enviar file descriptor
            msg.marcador_inicio = 126;
            msg.controle.tamanho = 1;
            msg.controle.sequencia = last_seq;
            msg.controle.tipo = FD;
            strcpy(msg.dados, name);
            msg.crc = 81;
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);


            printf("debug:\"Verificação envio do FD: %d\n", envio);

            reading = 1;

            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                    if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    if(msg.controle.tipo == OK){
                        reading = 0;
                        try_send_fd = 0;
                    }else{
                        // calcula novo tempo para enviar o dado
                        // reading = 0;
                        // return;
                    }
                }
                *((unsigned char *)buffer) = 0;
            }
        }

        //
        int i = 0;
        while(has_data_to_sent){

            sequencia0 = msg.controle.sequencia;
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < 127){
                dados[0][indice] = getc(fd);
                i++;
                indice++;
            }
            tam0 = indice;
            sequencia1 = sequencia0 + 1;
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < 127){
                dados[1][indice] = getc(fd);
                i++;
                indice++;
            }
            tam1 = indice;
            sequencia2 = sequencia1 + 1;
            indice = 0;
            while(i <= tamanho_da_mensagem && indice < 127){
                dados[2][indice] = getc(fd);
                i++;
                indice++;
            }
            tam2 = indice;

            printf("i depois de criar as tres mensagens  =>> %d",i);
            printf("debug:\"Verificação antes de entrar no primeiro loop do envio \n");
            printf("Conteudo da primeira parte da mensagem : %s\n",dados[0]);
            printf("Conteudo da segunda parte da mensagem : %s\n",dados[1]);
            printf("Conteudo da terceira parte da mensagem : %s\n",dados[2]);

                    msg.marcador_inicio = 126;
                    msg.controle.tipo = DADOS;
                    msg.controle.sequencia = sequencia0;
                    msg.controle.tamanho = tam0;
                    strcpy(msg.dados,dados[0]);
                    defineBuffer(&msg, buffer0);
                    printf("debug:\"Cria a primeira mensagem\n");
                    msg.controle.sequencia = sequencia1;
                    msg.controle.tamanho = tam1;
                    strcpy(msg.dados,dados[1]);
                    defineBuffer(&msg, buffer1);
                    printf("debug:\"Cria a segunda mensagem\n");
                    msg.controle.sequencia = sequencia2;
                    msg.controle.tamanho = tam2;
                    strcpy(msg.dados,dados[2]);
                    defineBuffer(&msg, buffer2);
                    printf("debug:\"Cria a terceira mensagem\n");

                try_send_data = 1;

                while(try_send_data){
                    printf("tamanhos enviados %d,%d,%d",tam0,tam1,tam2);
                    printf("sequencias enviadas %d,%d,%d",sequencia0,sequencia1,sequencia2);
                    envio = send(filedesk, buffer0, tamanhoMensagem(tam0), 0);
                    envio = send(filedesk, buffer1, tamanhoMensagem(tam1), 0);
                    envio = send(filedesk, buffer2, tamanhoMensagem(tam2), 0);

                    printf("debug:\"Enviei as tres mensagens aguarda status do server : %d\n", envio);
                    reading = 1;
                        while(reading){
                            resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer) == 126){
                                printf("controle da variavel que itera arquivo i : %d\n",i);
                                printf("recebi mensagem do servidor tratar...\n");
                                recuperaMensagem(&msg, buffer);
                                printf("%d\n",msg.controle.tipo);
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                                printf("mensagem do tipo ack bro vai para proxima");
                                // incrementa a sequencia em tres somente após a garantia que enviei as tres
                                msg.controle.sequencia = (msg.controle.sequencia + 3);
                            }
                            if(msg.controle.tipo == NACK){
                                reading = 0;
                                printf("mensagem do tipo nack bro envia novamente");
                            }
                            // calcula temporização aqui tambem
                        }
                        *((unsigned char *)buffer) = 0;
                        printf("Aguardando resposta do servidor... \n");
                }
                if(i >= tamanho_da_mensagem){
                    has_data_to_sent = 0;
                }
            }
        }

        while(try_send_fim){
            msg.marcador_inicio = 126;
            msg.controle.tipo = FIM;
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, tamanhoMensagem(sizeof(FIM)), 0);


            printf("Verificação envio: %d\n", envio);
            reading = 1;
            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    if(msg.controle.tipo == OK){
                        try_send_fim = 0;
                    }else{
                        // calcula novo tempo para enviar o dado
                        reading = 0;
                        // e envia novamente
                    }
                *((unsigned char *)buffer) = 0;
                }
            }
        }

}

int ordena(int *min,int *med, int *max){

    int tmp0 = *min;
    int tmp1 = *med;
    int tmp2 = *max;

    int medio = tmp1;
    int maior1 = medio++ % 32;
    int maior2 = maior1++ % 32;
    int menor1 = medio-- % 32;
    int menor2 = menor1-- % 32;

        if(tmp0 == menor1 && tmp2 == maior1){
            *min = tmp0;
            *med = tmp1;
            *max = tmp2;
            return 1;
        }
        if(tmp0 == maior1 && tmp2 == menor1){
            *min = tmp2;
            *med = tmp1;
            *max = tmp0;
            return 1;
        }
        if(tmp0 == maior1 && tmp2 == maior2){
            *min = tmp1;
            *med = tmp0;
            *max = tmp2;
            return 1;
        }
        if(tmp0 == maior2 && tmp2 == maior1){
            *min = tmp1;
            *med = tmp2;
            *max = tmp0;
            return 1;

        }
        if(tmp0 == menor1 && tmp2 == menor2){
            *min = tmp2;
            *med = tmp0;
            *max = tmp1;
            return 1;
        }
        if(tmp0 == menor2 && tmp2 == menor1){
            *min = tmp0;
            *med = tmp2;
            *max = tmp1;
            return 1;
        }
        printf("Script de ordernar has a breach !!!\n");
        return 0;

}

void trata_put(int filedesk, Mensagem *first_msg){

    printf("iniciando put ... \n");
    int envio;
    int resposta;
    void *buffer;
    void *buffer0;
    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;
    // esse buffer fica reponsavel por receber os dados do cliente
    void *buffer_read;
    // controla o loop principal
    int main_loop = 1;
    int last_seq;
    int tmp0,tmp1,tmp2;
    int min,med,max;
    int send_confirmation_fd = 1;
    int send_confirmation_name = 1;
    int wait_for_fd = 1;
    int wait_for_data = 1;
    int status = 1;
    int try_send_ack = 1;
    int try_send_nack = 1;
    int wait_for_end = 1;
    int cont = 0;
    // lembre-se do free mate
    buffer0 = malloc(TAMANHO_MAXIMO);
    // tenta criar um arquivo e devolve resposta de sucesso com o nome...

    Mensagem msg,msgs[3];
    msgs[0].dados = malloc(TAMANHO_MAXIMO);
    msgs[1].dados = malloc(TAMANHO_MAXIMO);
    msgs[2].dados = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);

    last_seq = first_msg->controle.sequencia;

    msg.marcador_inicio = 126;
    msg.controle.tamanho = 1;
    msg.controle.sequencia = 10;
    msg.controle.tipo = OK;
    msg.crc = 81;
    defineBuffer(&msg, buffer_send);
    envio = send(filedesk, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);

    while(main_loop){
        resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
        printf("tamanho da mensagem recebida  %d\n ",resposta);
        if(*((unsigned char *)buffer_read) == 126){
            recuperaMensagem(&msg, buffer_read);
            switch(msg.controle.tipo){
                case FD:
                    printf("Recebi FD\n");
                    msg.marcador_inicio = 126;
                    msg.controle.tamanho = 1;
                    msg.controle.sequencia = 11;
                    msg.controle.tipo = OK;
                    msg.crc = 81;
                    defineBuffer(&msg, buffer_send);
                    // adiciona o file descriptor ...
                    // em todo caso para teste ira enviar ok
                break;
                case DADOS:
                    printf("Recebi o primeiro dado tratando\n");
                    // toda vez que chamar dados a primeira mensagem vai estar no buffer_read
                    recuperaMensagem(&msgs[0], buffer_read);
                    cont = 1;
                        while(cont < 3){
                            resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer_read) == 126){
                                recuperaMensagem(&msgs[cont], buffer_read);
                                cont++;
                                *((unsigned char *)buffer_read) = 0;
                            }
                        }
                        printf("Recebi as tres mensagens TCHAU\n");
                        min = msgs[0].controle.sequencia;
                        med = msgs[1].controle.sequencia;
                        max = msgs[2].controle.sequencia;

                        int try = ordena(&min,&med,&max);
                        if(try){

                            if(min == msgs[0].controle.sequencia){
                                printf("Dado adicionado no 1°: %s\n",(char *) msgs[0].dados);
                                // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                            }else if(min == msgs[1].controle.sequencia){
                                printf("Dado adicionado no 1°: %s\n",(char *) msgs[1].dados);
                                // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                            }else{
                                printf("Dado adicionado no 1°: %s\n",(char *) msgs[2].dados);
                                // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                            }

                            if(med == msgs[0].controle.sequencia){
                                printf("Dado adicionado no 2°: %s\n",(char *) msgs[0].dados);
                                // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                            }else if(med == msgs[1].controle.sequencia){
                                printf("Dado adicionado no 2°: %s\n",(char *) msgs[1].dados);
                                // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                            }else{
                                printf("Dado adicionado no 2°: %s\n",(char *) msgs[2].dados);
                                // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                            }

                            if(max == msgs[0].controle.sequencia){
                                printf("Dado adicionado no 3°: %s\n",(char *) msgs[0].dados);
                                // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
                            }else if(max == msgs[1].controle.sequencia){
                                printf("Dado adicionado no 3°: %s\n",(char *) msgs[1].dados);
                                // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
                            }else{
                                printf("Dado adicionado no 3° : %s\n",(char *) msgs[2].dados);
                                // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
                            }

                            msg.marcador_inicio = 126;
                            msg.controle.tamanho = 1;
                            msg.controle.sequencia = last_seq + 1;
                            msg.controle.tipo = ACK;
                            msg.crc = 81;
                            defineBuffer(&msg, buffer_send);
                        }else{
                            msg.marcador_inicio = 126;
                            msg.controle.tamanho = 1;
                            msg.controle.sequencia = last_seq + 1;
                            msg.controle.tipo = NACK;
                            msg.crc = 81;
                            defineBuffer(&msg, buffer_send);
                        }
                break;
                case FIM:
                    printf("Recebi FIM\n");
                    main_loop = 0;
                break;
            }
            *((unsigned char *)buffer_read) = 0;
            printf("sai do case/switch\n");
        }
        // criar um temporizador e renviar a mensagem
        // mensagens de confirmação sempre tem o tamanho 1
        printf("enviando mensagem ao cliente\n");
        envio = send(filedesk, buffer_send, tamanhoMensagem(sizeof(OK)), 0);        
    }
    printf("saindo do put\n");
}







    // while( send_confirmation_name ){

    //     msg.marcador_inicio = 126;
    //     msg.controle.tamanho = 1;
    //     msg.controle.sequencia = last_seq + 1;
    //     // assumindo que eu posso criar o arquivo para debug
    //     msg.controle.tipo = OK;
    //     msg.crc = 81;
    //     defineBuffer(&msg, buffer);
    //     envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);


    //     printf("debug:\"Verificação envio da confirmação do nome: %d\n", envio);
    //     wait_for_fd = 1;
    //     while(wait_for_fd){
    //         resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
    //         if(*((unsigned char *)buffer) == 126){
    //         recuperaMensagem(&msg, buffer);
    //             if(msg.controle.tipo == FD){
    //                 wait_for_fd = 0;
    //                 send_confirmation_name = 0;
    //             }else{
    //                 wait_for_fd = 0;
    //             }
    //         }
    //         *((unsigned char *)buffer) = 0;
    //     }
    // }

    // last_seq = msg.controle.sequencia++;

    // while(send_confirmation_fd){
        // trata os dados que foram recebido e são FD
        // msg.marcador_inicio = 126;
        // msg.controle.tamanho = 1;
        // msg.controle.sequencia = last_seq;
        // assumindo que eu posso criar o arquivo para debug
        // msg.controle.tipo = OK;
        // msg.crc = 81;
        // defineBuffer(&msg, buffer);

    // envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);


        // wait_for_data = 1;
        // while(wait_for_data){
        //     cont = 0;
        //     while(cont < 3){
        //         resposta = read(filedesk, buffer0, TAMANHO_MAXIMO);
        //         if(*((unsigned char *)buffer0) == 126){
        //             recuperaMensagem(&msgs[cont], buffer0);
        //             printf("recebi a mensagem numero %d\n",cont);
        //             if(msgs[cont].controle.tipo == FIM){
        //                 wait_for_data = 0;
        //                 send_confirmation_fd = 0;
        //             }
                    // printf("cont : %d\n",cont);
                    // printf("%d\t", msgs[cont].marcador_inicio);
                    // printf("%d\t%d\t%d\t", msgs[cont].controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
                    // printf("%s\t", (char *)msgs[cont].dados);
                    // printf("%d\n", msgs[cont].crc);
                //     cont++;
                // }
                // memset(buffer0,0,TAMANHO_MAXIMO);
                // if(cont == 3){
                //     send_confirmation_fd = 0;
                //     printf("Aguardando por dados \n");
                // }else{
                //     // pensar em algo para tratar o fd
                //     // e colocar wait_for_data = 0;
                // }
                // printf("Aguardando por dados \n");
            // }
            // printf("Tenho os 3 dados agora irei trata-los\n");
            // printf("sequencias recebidas %d,%d,%d\n",msgs[0].controle.sequencia,msgs[1].controle.tipo,msgs[2].controle.sequencia);
            // if(msgs[0].controle.sequencia == msgs[1].controle.tipo || msgs[0].controle.sequencia == msgs[2].controle.sequencia || msgs[1].controle.sequencia == msgs[2].controle.sequencia ){
            //     printf("enviando nack e esperando pelas algum dos indices repetidos\n");
            //     try_send_nack = 1;
            //     while(try_send_nack){
            //         msg.controle.tipo = NACK;
            //         defineBuffer(&msg, buffer);
            //         envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

            //         try_send_nack = 0;
            //     }
    //         }else if(msgs[0].controle.tipo == DADOS && msgs[1].controle.tipo == DADOS && msgs[2].controle.tipo == DADOS){
    //             printf("Dados recebidos tratando them \n");
    //             // faz o CRC bolado antes de tratar as mensagens e se for tenta enviar o nack

    //             int tmp0 = msgs[0].controle.sequencia;
    //             int tmp1 = msgs[1].controle.sequencia;
    //             int tmp2 = msgs[2].controle.sequencia;

    //             min = tmp0;
    //             med = tmp1;
    //             max = tmp2;

    //             ordena(&min,&med,&max);
    //             if(min == msgs[0].controle.sequencia){
    //                 printf("Dado adicionado no 1°: %s\n",(char *) msgs[0].dados);
    //                 // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
    //             }else if(min == msgs[1].controle.sequencia){
    //                 printf("Dado adicionado no 1°: %s\n",(char *) msgs[1].dados);
    //                 // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
    //             }else{
    //                 printf("Dado adicionado no 1°: %s\n",(char *) msgs[2].dados);
    //                 // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
    //             }
    //             if(med == msgs[0].controle.sequencia){
    //                 printf("Dado adicionado no 2°: %s\n",(char *) msgs[0].dados);
    //                 // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
    //             }else if(med == msgs[1].controle.sequencia){
    //                 printf("Dado adicionado no 2°: %s\n",(char *) msgs[1].dados);
    //                 // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
    //             }else{
    //                 printf("Dado adicionado no 2°: %s\n",(char *) msgs[2].dados);
    //                 // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
    //             }
    //             if(max == msgs[0].controle.sequencia){
    //                 printf("Dado adicionado no 3°: %s\n",(char *) msgs[0].dados);
    //                 // adiciona (char *) msg0.dados de tamanho msg0.controle.tamanho no file desk
    //             }else if(max == msgs[1].controle.sequencia){
    //                 printf("Dado adicionado no 3°: %s\n",(char *) msgs[1].dados);
    //                 // adiciona (char *) msg1.dados de tamanho msg1.controle.tamanho no file desk
    //             }else{
    //                 printf("Dado adicionado no 3° : %s\n",(char *) msgs[2].dados);
    //                 // adiciona (char *) msg2.dados de tamanho msg2.controle.tamanho no file desk
    //             }
    //             try_send_ack = 1;
    //             while(try_send_ack){
    //             printf("tenta enviar ack\n");
    //                 msg.controle.tipo = ACK;
    //                 defineBuffer(&msg, buffer);
    //                 printf("tenta enviar ack\n");
    //                 envio = send(filedesk, buffer,tamanhoMensagem(msg.controle.tamanho), 0);

    //                 // se o ack for sucessful faça o teste abaixo , caso contrario tente enviar o ack
    //                 // vou parar quando um dos tamanhos seja != 127
    //                 try_send_ack = 0;
    //             }
    //         }
    //     }
    // }
//     while(wait_for_end){
//         msg.controle.tipo = FIM;
//         envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

//         wait_for_end = 0;
//     }
//     printf("Saindo da função trata_put\n");

// }
// }




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

