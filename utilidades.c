#include "utilidades.h"
#include "comandos.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>


int tamanhoMensagem(int i){

    // Varidfica se mensagem não tem tamanho necessario (minimo = 14) e retorna tamanho mínimo.
    if(i  < 10)
        return 14;
    return (i + 4);
}



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
// int tamanhoMensagem(Mensagem *msg){

//     // Varidfica se mensagem não tem tamanho necessario (minimo = 14) e retorna tamanho mínimo.
//     if(msg->controle.tamanho  < 10)
//         return 14;
//     return (msg->controle.tamanho + 4);
// }

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

    printf("%d\n", *((char *)buffer + 3));

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

void remote_cd(int filedesk, char *local,char *comando, int sequencia){

    char        operador[500], semEspacos[500];
    int         envio,resposta,reading;
    Mensagem    msg;
    void        *buffer;

    msg.dados = malloc(127);    
    buffer = malloc(TAMANHO_MAXIMO);

    /**
     * Remove espaços de comando e guarda resultado em operador.
    */
    removeEspacos(comando, semEspacos);

    /**
     * Concatena em operador caminho que deseja-se acessar. 
    */
    strcpy(operador, local);
    strcat(operador, "/");
    strcat(operador, &semEspacos[3]);
    
    /**
     * Define mensagem com codigo 'CD' e com caminho desejado.
    */
    msg.marcador_inicio = 126;
    msg.controle.tipo = CD;
    msg.controle.tamanho = strlen(operador) + 1;
    msg.controle.sequencia = sequencia + 1;
    strcpy(msg.dados, operador);
    msg.crc = 81;

    defineBuffer(&msg, buffer);

    envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

    *((unsigned char *)buffer) = 0;

    reading = 1;
    while(reading){
        resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
            if(*((unsigned char *)buffer) == 126){
            
            recuperaMensagem(&msg, buffer);

            // Caso solicitação de 'CD' seja aceita.
            if(msg.controle.tipo == OK){
                reading = 0;

                // Caso o solicitado seja voltar um diretório.
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
                    strcat(local,"/");
                    strcat(local,&semEspacos[3]);
                }
                return;    
            }
            // Caso solicitação de 'CD' não tenha sido aceita.
            else if(msg.controle.tipo == ERRO){
                printf("ger rekt no err\n");
                int erro = *((char *) msg.dados);
                if(erro){
                // Verifica se erro está relacionado ao tipo apontado por comando solicitado.
                    if(erro < 0){
                        printf("O caminho indicado não aponta para um diretório.\n");
                    }
                    // Caso erro esteja relacionado a permissão de leitura ou existência do caminho solicitado
                    else{
                        printf("Não foi possivel concluir operação em no caminho indicado : %s\n", strerror(erro));
                    }
                }
                free(msg.dados);
                return;
            }else{
                // calcula novo tempo para enviar o dado
                reading = 0;
                free(msg.dados);
                return;
            }
        }
        *((unsigned char *)buffer) = 0;
    }
    // Recebe chamada de mestre, invoca servidor com pedido de 'cd' e espera por resposta.
    // servidor invoca cd local e retorna para mestre resposta do 'cd' solicitado.
    // Recebe resposta e retorna '0' caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.

}

void trata_cd(int filedesk, Mensagem *first_mensagem){

    Mensagem    msg;
    void *buffer;
    buffer = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(127);

    msg.marcador_inicio = 126;
    msg.controle.sequencia = first_mensagem->controle.sequencia + 1;
    msg.crc = 0;

    printf("CAMINHO \n %s\n ",  (char *)first_mensagem->dados);


    int try = local_cd("", first_mensagem->dados);

    if(try){
        msg.controle.tipo = ERRO;
        msg.controle.tamanho = 1;
        *((char *)msg.dados) = try;
        printf("ERRO %d\n", try);
    }else{
        msg.controle.tipo = OK;
        msg.controle.tamanho = 0;
        msg.dados = 0;
        printf("OK\n");
    }
    defineBuffer(&msg, buffer);
    int envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

    free(msg.dados);
}

void remote_ls(){
    // Recebe chamada de mestre, invoca servidor com pedido de 'ls' e espera por resposta.
    // servidor invoca cd local e retorna para mestre resposta do 'ls' solicitado.
    // Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
}

void put(int filedesk, char *name){



    int check_file;
    check_file = access(name,F_OK);
    if( errno == ENOENT){
        printf("No such file or directory \n");
        return;
    }

    struct stat path_stat;
    stat(name, &path_stat);
    if(!S_ISREG(path_stat.st_mode)){
            printf("Erro arquivo invalido : TIPO\n");
        return;
    }

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
    dados[0] = malloc(TAMANHO_MAXIMO);
    dados[1] = malloc(TAMANHO_MAXIMO);
    dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);
    Mensagem    msg;
    msg.dados = malloc(127);

    FILE *fd = NULL;
    fd = fopen(name,"r");
    if(fd == NULL){
        printf("erro ao criar fd do arquivo : %s \n",name);
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
        last_seq += 1;


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

            *((unsigned char *)buffer) = 0;

            reading = 1;

            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    // sucesso ao receber o ok do servidor
                    if(msg.controle.tipo == OK){
                        try_send_name = 0;
                        reading = 0;
                    }else if(msg.controle.tipo == ERRO){
                        // Arrumar uma maneira de arrumar isso
                        printf("Ocorreu um erro : %s\n",(char *) msg.dados);
                        return;
                    }else{
                        // calcula novo tempo para enviar o dado
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
            msg.controle.tamanho = 19;
            msg.controle.sequencia = last_seq;
            *((int *) msg.dados) = tamanho_da_mensagem;
            msg.controle.tipo = FD;
            msg.crc = 81;
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

            reading = 1;

            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                    if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    if(msg.controle.tipo == OK){
                        reading = 0;
                        try_send_fd = 0;
                    }else if(msg.controle.tipo == ERRO){
                        printf("Ocorreu um erro : %s\n",(char *) msg.dados);
                        return;
                    }else{
                        // calcula novo tempo para enviar o dado
                        // reading = 0;
                        // return;
                    }
                }
                *((unsigned char *)buffer) = 0;
            }
        }

        // memset(buffer,0,TAMANHO_MAXIMO);

        int i = 0;

        // memset(msg.dados,0,TAMANHO_MAXIMO);

        while(has_data_to_sent){



            sequencia0 = msg.controle.sequencia;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[0][indice] = getc(fd);
                i++;
                indice++;
            }
            tam0 = indice;
            sequencia1 = sequencia0 + 1;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[1][indice] = getc(fd);
                i++;
                indice++;
            }
            tam1 = indice;
            sequencia2 = sequencia1 + 1;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[2][indice] = getc(fd);
                i++;
                indice++;
            }
            tam2 = indice;

                msg.marcador_inicio = 126;
                msg.controle.tipo = DADOS;
                msg.controle.sequencia = sequencia0;
                msg.controle.tamanho = tam0;
                strcpy(msg.dados,dados[0]);
                defineBuffer(&msg, buffer0);
                msg.controle.sequencia = sequencia1;
                msg.controle.tamanho = tam1;
                strcpy(msg.dados,dados[1]);
                defineBuffer(&msg, buffer1);
                msg.controle.sequencia = sequencia2;
                msg.controle.tamanho = tam2;
                strcpy(msg.dados,dados[2]);
                defineBuffer(&msg, buffer2);

                try_send_data = 1;

                while(try_send_data){

                    envio = send(filedesk, buffer0, tamanhoMensagem(tam0), 0);
                    envio = send(filedesk, buffer1, tamanhoMensagem(tam1), 0);
                    envio = send(filedesk, buffer2, tamanhoMensagem(tam2), 0);

                    reading = 1;
                        while(reading){
                            resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer) == 126){
                                recuperaMensagem(&msg, buffer);
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                                // incrementa a sequencia em tres somente após a garantia que enviei as tres
                                msg.controle.sequencia = (msg.controle.sequencia + 3);
                            }
                            if(msg.controle.tipo == NACK){
                                reading = 0;
                            }
                            // calcula temporização aqui tambem
                        }
                        *((unsigned char *)buffer) = 0;
                }
                if(i >= tamanho_da_mensagem){
                    has_data_to_sent = 0;
                }
            }
        }

        msg.marcador_inicio = 126;
        msg.controle.tipo = FIM;
        defineBuffer(&msg, buffer);
        int temp = 0;
        // tenta enviar mensagem de fim 5 vezes
        while(temp < 5){
            envio = send(filedesk, buffer, tamanhoMensagem(sizeof(FIM)), 0);
            temp++;
        }


    free(dados[0]);
    free(dados[1]);
    free(dados[2]);
    free(dados);
    free(buffer);
    free(buffer0);
    free(buffer1);
    free(buffer2);
    free(msg.dados);

}

int ordena(int *min,int *med, int *max){

    int tmp0 = *min;
    int tmp1 = *med;
    int tmp2 = *max;

    int medio = tmp1;
    int maior1 = (medio + 1) % 32;
    int maior2 = (maior1 + 1) % 32;
    int menor1 = (medio - 1) % 32;
    int menor2 = (menor1 - 1)  % 32;
    // printf("tmp0 = %d,tmp1 = %d,tmp2 = %d\n",tmp0,tmp1,tmp2);
    // printf(" menor2 = %d menor1 = %d medio = %d maior1 = %d maior2 = %d\n",menor2,menor1,medio,maior1,maior2);

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
    int check_file = 0;
    int check_error = 0;
    int envio;
    int resposta;
    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;
    // esse buffer fica reponsavel por receber os dados do cliente
    void *buffer_read;
    // controla o loop principal
    int main_loop = 1;
    int last_seq;
    int min,med,max;
    int cont = 0;
    int sizeofmessage;
    char *name;
    // lembre-se do free mate

    // tenta criar um arquivo e devolve resposta de sucesso com o nome...
    Mensagem msg,msgs[3];
    msgs[0].dados = malloc(TAMANHO_MAXIMO);
    msgs[1].dados = malloc(TAMANHO_MAXIMO);
    msgs[2].dados = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);

    last_seq = first_msg->controle.sequencia;

    strcpy(name,(char *) first_msg->dados);

    FILE *fd = NULL;
    fd = fopen(name,"w");
    if(fd == NULL){
        printf("erro ao criar fd do arquivo : %s \n",name);
        printf("mensagem de erro : %s", strerror(errno));
        return;
    }

    check_error = access(name, W_OK);

    if(check_error){
        msg.controle.tipo = ERRO;
        strcpy(msg.dados,"PERMICAO");
        msg.controle.tamanho = 9;
        main_loop = 0;
    }else{
        msg.controle.tipo = OK;
        msg.controle.tamanho = 1;
    }


    msg.marcador_inicio = 126;
    msg.controle.sequencia = 10;
    msg.crc = 81;
    defineBuffer(&msg, buffer_send);
    envio = send(filedesk, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);

    while(main_loop){
        resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
        if(*((unsigned char *)buffer_read) == 126){
            recuperaMensagem(&msg, buffer_read);
            switch(msg.controle.tipo){
                case FD:
                    // sizeofmessage = (int *) msgs[0].dados;
                    // printf("Recebi o tamanho da mensagem %d\n",sizeofmessage);
                    sizeofmessage = *((int *) msg.dados);
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
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                            }else if(min == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
                            }

                            if(med == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                            }else if(med == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
                            }

                            if(max == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                            }else if(max == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
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
            // *((unsigned char *)buffer_read) = 0;

            printf("sai do case/switch\n");
        }
        // criar um temporizador e renviar a mensagem
        // mensagens de confirmação sempre tem o tamanho 1
        printf("enviando mensagem ao cliente\n");
        envio = send(filedesk, buffer_send, tamanhoMensagem(1), 0);
    }
    printf("saindo do put\n");

    free(msgs[0].dados);
    free(msgs[1].dados);
    free(msgs[2].dados);
    free(msg.dados);
    free(buffer_send);
    free(buffer_read);
}
////////////////////////////////////////////////////////////////// GET REKT ////////////////////
void get(int filedesk,char *local,char *remoto,char *comando,int sequencia){
    // envia o nome
    // abre write com esse nome
    // espera pela resposta OK
    // enviar descritor/tamanho do arquivo / verificar espaço
    // espera resposta ok ou erro se erro olhar dados
    // começa enviar dados
    // le 127 em 127 adiciona em uma mensagem incrementa , tam/127
    // if < 127
    // getchar ++ até construir mensagem , deu 127 faz não deu trata
    printf("iniciando get ... \n");
    char name[500];
    char operador[500], semEspacos[500];
    removeEspacos(comando, semEspacos);
    strcpy(operador, remoto);
    strcat(operador, "/");
    strcat(operador, &semEspacos[3]);
    // aqui é para aproveitar o código

    strcpy(name, local);
    strcat(name,"/");
    strcat(name,&semEspacos[3]);

    //strcpy(name,&semEspacos[3]);


    int check_file = 0;
    int check_error = 0;
    int envio;
    int resposta;
    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;
    // esse buffer fica reponsavel por receber os dados do cliente
    void *buffer_read;
    // controla o loop principal
    int main_loop = 1;
    int last_seq;
    int min,med,max;
    int cont = 0;
    int sizeofmessage;
    
    // lembre-se do free mate

    // tenta criar um arquivo e devolve resposta de sucesso com o nome...
    Mensagem msg,msgs[3];
    msgs[0].dados = malloc(TAMANHO_MAXIMO);
    msgs[1].dados = malloc(TAMANHO_MAXIMO);
    msgs[2].dados = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);


    
    last_seq = sequencia + 1;

    strcpy(msg.dados,operador);

    

    // if(check_error){
    //     msg.controle.tipo = ERRO;
    //     strcpy(msg.dados,"PERMICAO");
    //     msg.controle.tamanho = 9;
    //     main_loop = 0;
    // }else{
    //     msg.controle.tipo = OK;
    //     msg.controle.tamanho = 1;
    // }
    
    // temporizar aqui tambem
    if(main_loop){

    msg.marcador_inicio = 126;
    msg.controle.sequencia = last_seq;
    msg.crc = 81;
    msg.controle.tipo = GET;
    msg.controle.tamanho = strlen(operador) + 1;
    defineBuffer(&msg, buffer_send);    
        envio = send(filedesk, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
    }

    while(main_loop){
        resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
        if(*((unsigned char *)buffer_read) == 126){
            recuperaMensagem(&msg, buffer_read);
            switch(msg.controle.tipo){
                case FD:                
                    FILE *fd = NULL;
                    fd = fopen(name,"w");
                    if(fd == NULL){
                        printf("erro ao criar fd do arquivo : %s \n",name);
                        printf("mensagem de erro : %s", strerror(errno));
                        main_loop = 0;
                    }
    
                    check_error = access(name, W_OK);
    
                    if(check_error){
                        printf("Erro de access no arquivo : %s\n", strerror(errno));
                        main_loop = 0;
                    }
                    // sizeofmessage = (int *) msgs[0].dados;
                    // printf("Recebi o tamanho da mensagem %d\n",sizeofmessage);
                    sizeofmessage = *((int *) msg.dados);
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
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else if(min == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }

                            if(med == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else if(med == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }

                            if(max == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else if(max == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fd);
                                printf("%s\n",(char *) msgs[0].dados);
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
            // *((unsigned char *)buffer_read) = 0;

            printf("sai do case/switch\n");
        }
        // criar um temporizador e renviar a mensagem
        // mensagens de confirmação sempre tem o tamanho 1
        printf("enviando mensagem ao servidor\n");
        envio = send(filedesk, buffer_send, tamanhoMensagem(1), 0);
    }
    printf("saindo do get\n");

    free(msgs[0].dados);
    free(msgs[1].dados);
    free(msgs[2].dados);
    free(msg.dados);
    free(buffer_send);
    free(buffer_read);
}


////////////////////////////////////////////////////////////////// TRATA GET REKT ////////////////////
void trata_get(int filedesk,Mensagem *first_mensagem){

    char name[500];
    int sucess = 1;
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
    dados[0] = malloc(TAMANHO_MAXIMO);
    dados[1] = malloc(TAMANHO_MAXIMO);
    dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);
    Mensagem    msg;
    msg.dados = malloc(127);

    // depende estou enviando o nome junto?
    strcpy(name,(char*) first_mensagem->dados);    

    int check_file;
    check_file = access(name,F_OK);
    if( errno == ENOENT){
        printf("No such file or directory \n");
        *((char *)buffer) = INEXISTENTE;
        sucess = 0;
    }

    struct stat path_stat;
    stat(name, &path_stat);
    if(!S_ISREG(path_stat.st_mode)){
            printf("Erro arquivo invalido : TIPO\n");
            *((char *)buffer) = TIPO;
        sucess = 0;
    }

    FILE *fd = NULL;
    fd = fopen(name,"r");
    if(fd == NULL){
        *((char *)buffer) = FD_err;
        printf("erro ao criar fd do arquivo : %s \n",name);
        sucess = 0;
    }else{
        fseek(fd,0,SEEK_END);
        tamanho_da_mensagem = ftell(fd);    
        printf("tamanho da mensagem %d\n",tamanho_da_mensagem);
        // devolve o ponteiro no inicio do file descriptor
        rewind(fd);
    }  

    last_seq = msg.controle.sequencia;
    last_seq += 1;

    if(sucess){
        // apos enviar o nome tenta enviar o file descriptor
        while(try_send_fd){

            // monta mensagem para enviar file descriptor
            msg.marcador_inicio = 126;
            msg.controle.tamanho = sizeof(char);
            msg.controle.sequencia = last_seq;
            *((int *) msg.dados) = tamanho_da_mensagem;
            msg.controle.tipo = FD;
            msg.crc = 81;
            defineBuffer(&msg, buffer);
            envio = send(filedesk, buffer, tamanhoMensagem(msg.controle.tamanho), 0);            

            reading = 1;

            while(reading){
                resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                    if(*((unsigned char *)buffer) == 126){
                    recuperaMensagem(&msg, buffer);
                    if(msg.controle.tipo == OK){
                        reading = 0;
                        try_send_fd = 0;
                    }else if(msg.controle.tipo == ERRO){
                        printf("Ocorreu um erro : %s\n",(char *) msg.dados);
                        goto clear_dados;
                    }else{
                        // calcula novo tempo para enviar o dado
                        // reading = 0;
                        // return;
                    }
                }
                *((unsigned char *)buffer) = 0;
            }
        }

        int i = 0;

        while(has_data_to_sent){

            sequencia0 = msg.controle.sequencia;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[0][indice] = getc(fd);
                i++;
                indice++;
            }
            tam0 = indice;
            sequencia1 = sequencia0 + 1;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[1][indice] = getc(fd);
                i++;
                indice++;
            }
            tam1 = indice;
            sequencia2 = sequencia1 + 1;
            indice = 0;
            while(i < tamanho_da_mensagem && indice < 127){
                dados[2][indice] = getc(fd);
                i++;
                indice++;
            }
            tam2 = indice;

                msg.marcador_inicio = 126;
                msg.controle.tipo = DADOS;
                msg.controle.sequencia = sequencia0;
                msg.controle.tamanho = tam0;
                strcpy(msg.dados,dados[0]);
                defineBuffer(&msg, buffer0);
                msg.controle.sequencia = sequencia1;
                msg.controle.tamanho = tam1;
                strcpy(msg.dados,dados[1]);
                defineBuffer(&msg, buffer1);
                msg.controle.sequencia = sequencia2;
                msg.controle.tamanho = tam2;
                strcpy(msg.dados,dados[2]);
                defineBuffer(&msg, buffer2);

                try_send_data = 1;

                while(try_send_data){

                    envio = send(filedesk, buffer0, tamanhoMensagem(tam0), 0);
                    envio = send(filedesk, buffer1, tamanhoMensagem(tam1), 0);
                    envio = send(filedesk, buffer2, tamanhoMensagem(tam2), 0);

                    reading = 1;
                        while(reading){
                            resposta = read(filedesk, buffer, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer) == 126){
                                recuperaMensagem(&msg, buffer);
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                                // incrementa a sequencia em tres somente após a garantia que enviei as tres
                                msg.controle.sequencia = (msg.controle.sequencia + 3);
                            }
                            if(msg.controle.tipo == NACK){
                                reading = 0;
                            }
                            // calcula temporização aqui tambem
                        }
                        *((unsigned char *)buffer) = 0;
                }
                if(i >= tamanho_da_mensagem){
                    has_data_to_sent = 0;
                }
            }
        }        
        msg.controle.tipo = FIM;
    }else{
        msg.controle.tipo = ERRO;
    }

    msg.marcador_inicio = 126;
    msg.controle.tamanho = sizeof(char);
    msg.controle.sequencia = msg.controle.sequencia + 1;
    msg.crc = 81;
    defineBuffer(&msg, buffer); 

    envio = send(filedesk, buffer, tamanhoMensagem(sizeof(char)), 0);

    clear_dados:


    free(dados[0]);
    free(dados[1]);
    free(dados[2]);
    free(dados);
    free(buffer);
    free(buffer0);
    free(buffer1);
    free(buffer2);
    free(msg.dados);


}

