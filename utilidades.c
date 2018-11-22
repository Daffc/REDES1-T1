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


unsigned char tabela[256];




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
 * Recebe mensagem e buffer destino , e popula buffer com conteudo da mensagem + crc.
 */
void defineBuffer(Mensagem * msg, void * buffer){

    void *buffer_crc;


    buffer_crc = malloc(TAMANHO_MAXIMO + 2);

    memcpy(buffer_crc,&(msg->controle),2);
    memcpy(buffer_crc + 2, msg->dados, msg->controle.tamanho);

    unsigned char crc = calcula_crc(buffer_crc,msg->controle.tamanho + 2);

    memcpy(buffer, &(msg->marcador_inicio), 1);
    memcpy(buffer + 1, &(msg->controle), 2);
    memcpy(buffer + 3, msg->dados, msg->controle.tamanho);
    memcpy(buffer + 3 + msg->controle.tamanho, &crc, 1);

    free(buffer_crc);
}

/**
 * Recebe buffer e mensagem destino , e popula mensagem com conteudo da buffer e retornando resultado de calculo de crc.
 */
int recuperaMensagem(Mensagem * msg, void * buffer){

    void *buffer_crc;
    buffer_crc = malloc(TAMANHO_MAXIMO + 2);

    // Guarda Marcador de início;
    memcpy(&(msg->marcador_inicio), buffer, 1);

    // Guarda informações de controle (tamanho, sequencia, tipo)
    memcpy(&(msg->controle), buffer + 1, 2);

    //Guarda dados de acordo com o informado em controle.tamanho
    memcpy(msg->dados, buffer + 3, msg->controle.tamanho);

    // Guarda crc.
    memcpy(&(msg->crc),buffer + 3 + msg->controle.tamanho, 1);

    memcpy(buffer_crc,&(msg->controle),2);
    memcpy(buffer_crc + 2, msg->dados, msg->controle.tamanho);


    int verifica_crc = (msg->crc == calcula_crc(buffer_crc,msg->controle.tamanho + 2 ));

    free(buffer_crc);

    return verifica_crc;

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

int timeout(struct pollfd fds[], void *buffer_read){
    int resp;

    // Verifica se existe mensagens a serem lidas até tempo limite
    resp = poll(fds, 1, TIMEOUT * 1000);

    // Caso ocorra algum erro na verificação de do timeout
    if(resp == -1){
        printf("Erro na consulta de socket!\n");
    }

    // Caso tempo de espera tenha chegado ao fim.
    if (!resp) {
        printf("DEMOROU DEMAIS\n");
        return 0;
    }
    // Caso tudo ocorra normalmente.
    if (fds[0].revents & POLLIN){
        resp = read(fds[0].fd, buffer_read, TAMANHO_MAXIMO);
        return 1;
    }
}

/**
 * Recebe tres valores sem ordem , e os devolve ordenados
 */

int ordena(int *min,int *med, int *max){

    int tmp0 = *min;
    int tmp1 = *med;
    int tmp2 = *max;

    int medio = tmp1;
    int maior1 = (medio + 1) % 32;
    int maior2 = (maior1 + 1) % 32;
    int menor1 = (medio + 31) % 32;
    int menor2 = (menor1 + 31)  % 32;

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

/**
 * --------------------------------
 * ----- FUNÇÕES EXTERNAS. --------
 * --------------------------------
*/

int tamanhoMensagem(int i){

    // Varidfica se mensagem não tem tamanho necessario (minimo = 14) e retorna tamanho mínimo.
    if(i  < 10)
        return 14;
    return (i + 4);
}

/**
 * Trata resultado de comando nativo ls + parametros e devolve resultado em alocação de buffer saida.
 * Retornando inteiro 0, caso operação tenha sido executada com sucesso, ou erro relacionado
 */

int local_ls(char * comando, char * local, char **bufferSaida){
    char    retorno, operador[100];
    long int     i = 0;

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

        *bufferSaida = malloc(1000);
        
        /*Le caracter por caracter do arquivo de resposta do ls aberto guarda-o em bufferSaida*/
        while ((retorno = getc(fpls)) != EOF) {
            (*bufferSaida)[i] = retorno ;
            i++;
            // Caso buffe não seja grande o suficiente, alocar mais posições de memória.
            if(i%1000 == 0)
                *bufferSaida = realloc(*bufferSaida, 1000 * ((i / 1000) +1 ));
        }
        (*bufferSaida)[i] ='\0';

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

/**
 * Recebe chamada de mestre, invoca servidor com pedido de 'ls' e espera por resposta.
 * servidor invoca ls local e retorna para mestre resposta do 'ls' solicitado.
 * Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
 */
int remote_ls(struct pollfd conexao[], char *remoto, char *comando, int sequencia){
    

    char        operador[500], semEspacos[500], impressor[128];
    long int    ponteiroResposta = 0;


    operador[0] = '\0';

    /**
     * Remove espaços de comando e guarda resultado em semEspacos.
    */
    removeEspacos(comando, semEspacos);


    //Busca por parametro indicado constroi String "operador" com conteúdo da menságem a ser enviada (parâmetros + caminho).
    if(semEspacos[3] == '\0'){
        strcpy(operador, " ");
        strcat(operador, remoto);
    }
    if(strstr(&semEspacos[3], "-l")){
        strcpy(operador, "-l ");
        strcat(operador, remoto);
    }
    if(strstr(&semEspacos[3], "-a")){
        strcpy(operador, "-a ");
        strcat(operador, remoto);
    }
    if(strstr(&semEspacos[3], "-al") || strstr(&semEspacos[3], "-la")){
        strcpy(operador, "-la ");
        strcat(operador, remoto);
    }


    /**
     * Caso operador possua algum conteúdo, inicia-se a operação.
    */
    if(*operador != '\0'){

        printf("iniciando RLS... \n");

        //strcpy(name,&semEspacos[3]);

        // printf("comando com endereço relativo remoto %s\n",operador);
        // printf("comando com endereço relativo local %s\n",name);

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
        int erro = 0;

        // lembre-se do free mate

        // tenta criar um arquivo e devolve resposta de sucesso com o nome...
        Mensagem msg,msg_resposta;
        msg_resposta.dados = malloc(TAMANHO_MAXIMO);

        msg.dados = malloc(TAMANHO_MAXIMO);
        buffer_send = malloc(TAMANHO_MAXIMO);
        buffer_read = malloc(TAMANHO_MAXIMO);

        // Caso a msg_resposta venha com campo dados completo, seja possível printar normalmente.
        impressor[127] = '\0';


        last_seq = sequencia + 1;

        strcpy(msg.dados, operador);

        msg.marcador_inicio = 126;
        msg.controle.sequencia = last_seq;
        msg.crc = 81;
        msg.controle.tipo = LS;
        msg.controle.tamanho = strlen(operador) + 1;


        defineBuffer(&msg, buffer_send);
        envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);


        int recebeMT;
        int loop_rls = 1;
        int resp;

        while(loop_rls){
            resp = timeout(conexao, buffer_read);

            if(!resp){
                envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
            }

            if(*((unsigned char *)buffer_read) == 126){
                if(recuperaMensagem(&msg, buffer_read)){
                    if(msg.controle.tipo == MOSTRA_TELA){
                        recebeMT = 0;
                        loop_rls = 0;
                    }
                }else{
                    msg.marcador_inicio = 126;
                    msg.controle.tipo = NACK;
                    msg.controle.tamanho = 0;
                    defineBuffer(&msg, buffer_send);
                    envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
                }
            }
        }

        while(main_loop){

            if(recebeMT){
                resposta = read(conexao[0].fd, buffer_read, TAMANHO_MAXIMO);
            }

            if(*((unsigned char *)buffer_read) == 126){
                if(recuperaMensagem(&msg, buffer_read)){
                    // printf(" ** %s\n", (char *)msg.dados);
                    switch(msg.controle.tipo){
                        case ERRO:
                            // printf("there has been a breach: %s \n",strerror(*((char *) msg.dados)));
                            main_loop = 0;
                            erro = 1;
                        break;
                        case MOSTRA_TELA:
                            // printf("Recebi o primeiro dado tratando\n");
                            // toda vez que chamar dados a primeira mensagem vai estar no buffer_read
                            recuperaMensagem(&msg_resposta, buffer_read);
                            // printf("Recebi as tres mensagens TCHAU\n");
                            // printf("%d %d %d \n",msg_resposta.controle.tamanho,msg_resposta.controle.tamanho,msg_resposta.controle.tamanho);

                            // Utilizar 'try' para verificar CRC.
                            int try = 1;
                            if(try){
                                memcpy(impressor, msg_resposta.dados, msg_resposta.controle.tamanho);
                                impressor[msg_resposta.controle.tamanho] = '\0';
                                printf("%s", impressor);

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
                            envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(1), 0);
                            // Habilita leitura de menságens dentro deste laço.
                            recebeMT = 1;
                        break;
                        case FIM:
                            // printf("\nRecebi FIM\n");
                            main_loop = 0;
                        break;
                        case NACK:
                            envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(1), 0);
                        break;
                    }
                }else{
                    msg.marcador_inicio = 126;
                    msg.controle.tipo = NACK;
                    msg.controle.tamanho = 0;
                    defineBuffer(&msg, buffer_send);
                    envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
                }
                *((unsigned char *)buffer_read) = 0;

                // printf("sai do case/switch\n");
            }
            // criar um temporizador e renviar a mensagem
            // mensagens de confirmação sempre tem o tamanho 1

        }
        // printf("saindo do get\n");


        free(msg_resposta.dados);
        free(msg.dados);
        free(buffer_send);
        free(buffer_read);

        /**
         * Informa ao programa principal que operação ocorreu com sucesso.
        */
        return 0;
    }

    /**
    * Caso comando seja desconhecido, retorna código de erro.
    */
    else{
        return 1;
    }
}

/**
 * função que trata requisição de rls no lado do servidor, utilizando da função local_ls
 */

void trata_ls(struct pollfd conexao[], Mensagem *first_mensagem){

    char comando[500], *bufferResposta;
    int sucess = 1;
    int try_send_data = 1;
    int has_data_to_sent = 1;
    int last_seq;
    int envio;
    int reading;
    unsigned long int ponteiroResposta = 0;
    void *buffer;
    void *buffer_envio;
    int indice;
    int tamanho_da_mensagem;
    int resp;


    char *leitor;

    leitor = malloc(128);
    leitor[127] = '\0';

    buffer = malloc(TAMANHO_MAXIMO);
    buffer_envio = malloc(TAMANHO_MAXIMO);

    Mensagem    msg;
    msg.dados = malloc(127);

    strcpy(comando, "ls ");

    /**
     * Verifica e recupera parâmetros caso seja requisitado pelo usuário.
    */
    char *parametro;
    parametro = strtok((char *)first_mensagem->dados, " ");

    /**
     * Caso o primeiro caracter da string parâmetro seja '-',
     * indicando que o que segue no conteúdo é um parâmetro de 'ls'.
    */
    printf("**%s\n", parametro);
    if(parametro[0] == '-'){
        strcat(comando, parametro);
        printf("--%s\n", comando);

        parametro = strtok(NULL, " ");

        printf("()%s\n", parametro);
        sucess = local_ls(comando, parametro, &bufferResposta);
    }
    else{
        printf("--%s\n", comando);
        sucess = local_ls(comando, parametro, &bufferResposta);
    }


    /**
     * envia para ls_local:
     * valor de comando com comando ls e os parâmetros indicados.
     * local onde o comado deve ser efetiado indicado pela substring de first_mensagem.dados após o espaço separador.
     * bufferResposta que armazenará resposta de ls om seus parâmetros.
     *
    */

    tamanho_da_mensagem = (strlen(bufferResposta));

    // printf("tamanho da mensagem %d\n",tamanho_da_mensagem);

    last_seq = first_mensagem->controle.sequencia;
    last_seq += 1;

    if(!sucess){

        int i = 0;
        while(has_data_to_sent){
            indice = 0;
            ponteiroResposta += strlen(strncpy(leitor, &bufferResposta[ponteiroResposta], 127));

            msg.marcador_inicio = 126;
            msg.controle.tipo = MOSTRA_TELA;
            msg.controle.sequencia = msg.controle.sequencia +1;
            msg.controle.tamanho = strlen(leitor);


            // DEFINIR CRC.

            strcpy(msg.dados,leitor);
            defineBuffer(&msg, buffer_envio);

            try_send_data = 1;
            while(try_send_data){
                send(conexao[0].fd, buffer_envio, tamanhoMensagem(msg.controle.tamanho), 0);

                reading = 1;
                while(reading){

                    resp = timeout(conexao, buffer);

                    if(!resp){
                        send(conexao[0].fd, buffer_envio, tamanhoMensagem(msg.controle.tamanho), 0);
                    }

                    if((*((unsigned char *)buffer) == 126 && resp)){

                        if(recuperaMensagem(&msg, buffer)){
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                                // incrementa a sequencia em tres somente após a garantia que enviei as tres
                                msg.controle.sequencia = (msg.controle.sequencia + 1);
                            }
                            if(msg.controle.tipo == NACK){
                                    reading = 0;
                            }
                            // calcula temporização aqui tambem
                        }else{
                            msg.marcador_inicio = 126;
                            msg.controle.tipo = NACK;
                            msg.controle.tamanho = 0;
                            defineBuffer(&msg, buffer_envio);
                            envio = send(conexao[0].fd, buffer_envio, tamanhoMensagem(msg.controle.tamanho), 0);
                        }
                    }
                    *((unsigned char *)buffer) = 0;
                }

                if(ponteiroResposta >= tamanho_da_mensagem){
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

    envio = send(conexao[0].fd, buffer, tamanhoMensagem(sizeof(char)), 0);

    clear_dados:


    free(bufferResposta);
    free(leitor);
    free(buffer);
    free(buffer_envio);
    free(msg.dados);
}

/**
 * função que trata requisição de cd, verificando permição e existencia de destino, e atualizando vetor que armazena caminho relativo.
 * Retorna 0 caso operação tenha sido efetuada com sucesso, ou valor correspondente ao código de erro.
 */
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

/**
 * Recebe chamada de mestre, invoca servidor com pedido de 'cd' e espera por resposta.
 * servidor invoca cd local e retorna para mestre resposta do 'cd' solicitado.
 * Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
 */

void remote_cd(struct pollfd conexao[], char *local, char *comando, int sequencia){

    char        operador[500], semEspacos[500];
    int         envio, resposta, reading;
    Mensagem    msg;
    void        *buffer;
    void        *buffer_send;
    int         resp;
    int         erro;

    msg.dados = malloc(127);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);

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

    defineBuffer(&msg, buffer_send);

    send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);

    *((unsigned char *)buffer) = 0;

    reading = 1;
    while(reading){

        resp = timeout(conexao, buffer);

        if(!resp){
            send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
        }

        if((*((unsigned char *)buffer) == 126) && resp){

            if(recuperaMensagem(&msg, buffer)){

                switch(msg.controle.tipo){
                    case OK:
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
                    break;
                    case ERRO:
                        reading = 0;
                        erro = *((char *) msg.dados);
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
                    break;
                }
            }else{
                send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
            }
            *((unsigned char *)buffer) = 0;
        }
    }

    free(msg.dados);
    free(buffer);
    free(buffer_send);

}

/**
 * função que trata requisição de rcd no lado do servidor, utilizando da função local_cd
 */

void trata_cd(int conexao, Mensagem *first_mensagem){

    Mensagem    msg;
    void *buffer;
    int sendingAnswer;
    buffer = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(127);

    msg.marcador_inicio = 126;
    msg.controle.sequencia = first_mensagem->controle.sequencia + 1;

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
    sendingAnswer = 1;

    int envio = send(conexao, buffer, tamanhoMensagem(msg.controle.tamanho), 0);

    free(msg.dados);
}

/**
 * Retira o nome do arquivo que sera enviado da variavel comando
 * E realiza o envio de mensagens entre o master/slave 
 */

void put(struct pollfd conexao[], char *local , char*remoto , char *comando){



    // printf("endereço relativo local %s\n",local);
    // printf("endereço relativo remoto %s\n",remoto);
    // printf("tipo do comando %s\n",comando);

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

    // printf("comando com endereço relativo remoto %s\n",operador);
    // printf("tamanho do operador : %ld\n",strlen(operador));
    // printf("comando com endereço relativo local %s\n",name);

    // char semEspacos[500];
    // removeEspacos(name, semEspacos);
    // printf("%s\n",name);
    if(access(name,F_OK)){
        printf("mensagem de erro :%s\n",strerror(errno));
        printf("No such file or directory \n");
        return;
    }

    struct stat path_stat;
    stat(name, &path_stat);
    if(!S_ISREG(path_stat.st_mode)){
        printf("Erro arquivo invalido : TIPO\n");
        return;
    }

    FILE *fd = NULL;
    fd = fopen(name,"r");
    if(fd == NULL){
        printf("erro ao criar fd do arquivo : %s \n",name);
        return;
    }

    // controla se o nome foi enviado
    int try_send_name = 1;

    // controla se o file desk foi enviado
    int try_send_fd = 1;

    // controla o envio dos dados
    int try_send_data = 1;

    // controla o envio do vim
    int try_send_fim = 1;

    // controla se existe dados para envio
    int has_data_to_sent = 1;

    // controla a sequencia de acordo com a ultima mensagem
    int last_seq;

    // guarda valor do send()
    int envio;

    // controla a espera de mensagem
    int reading;

    // auxiliar no timeout
    int resp;

    // buffer que recebe mensagem
    void *buffer;

    // buffer que envia mensagem
    void *buffer_send;

    // buffer de envio da primeira mensagem
    void *buffer0;

    // buffer de envio da segunda mensagem
    void *buffer1;

    // buffer de envio da terceira mensagem
    void *buffer2;

    // controla o tamanho da mensagem
    int indice;

    // controla a sequencia de cada mensagem
    int sequencia0,sequencia1,sequencia2;

    // contem tamnho da mensagem
    int tamanho_da_mensagem;

    // controla o tamanho das mensagens uma a uma
    int tam0,tam1,tam2;


    char **dados;

    dados = malloc(3);
    dados[0] = malloc(TAMANHO_MAXIMO);
    dados[1] = malloc(TAMANHO_MAXIMO);
    dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);
    Mensagem    msg;
    msg.dados = malloc(127);

    
    // recebe o tamanho da mensagem
    // struct stat bufferino;
    // stat(fd,&bufferino);
    fseek(fd,0,SEEK_END);
    tamanho_da_mensagem = ftell(fd);

    printf("tamanho da mensagem %d\n",tamanho_da_mensagem);
    // devolve o ponteiro no inicio do file descriptor
    rewind(fd);

    last_seq = (msg.controle.sequencia + 1) % 32;
    


    while(try_send_name){
        // monta mensagem para enviar o nome
        msg.marcador_inicio = 126;
        msg.controle.tamanho = strlen(operador) + 1;
        msg.controle.sequencia = last_seq;
        msg.controle.tipo = PUT;
        strcpy(msg.dados, operador);
        msg.crc = 81;
        defineBuffer(&msg, buffer_send);

        *((unsigned char *)buffer) = 0;

        reading = 1;

        envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
        while(reading){
            resp = timeout(conexao, buffer);

            if(!resp){
                envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
            }
            if((*((unsigned char *)buffer) == 126) && resp){
                if(recuperaMensagem(&msg, buffer)){
                // sucesso ao receber o ok do servidor
                    if(msg.controle.tipo == OK){
                        try_send_name = 0;
                        reading = 0;
                        last_seq = ( msg.controle.sequencia + 1 ) % 32;
                    }else if(msg.controle.tipo == ERRO){
                        // Arrumar uma maneira de arrumar isso
                        printf("Ocorreu um erro : %s\n",(char *) msg.dados);
                        last_seq = ( msg.controle.sequencia + 1 ) % 32;
                        goto saida_put;
                    }
                }else{
                    envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);                    
                }
                // evitar loop infinito
                *((unsigned char *)buffer) = 0;
            }
        }
    }
    

    // apos enviar o nome tenta enviar o file descriptor
    while(try_send_fd){

        // monta mensagem para enviar file descriptor
        msg.marcador_inicio = 126;
        msg.controle.tamanho = sizeof(int);
        msg.controle.sequencia = last_seq;
        *((int *) msg.dados) = tamanho_da_mensagem;
        msg.controle.tipo = FD;
        defineBuffer(&msg, buffer);

        reading = 1;

        envio = send(conexao[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho), 0);
        while(reading){

            resp = timeout(conexao, buffer);

            if(!resp){
                envio = send(conexao[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho), 0);
            }

            if((*((unsigned char *)buffer) == 126 && resp)){
                if(recuperaMensagem(&msg, buffer)){
                    if(msg.controle.tipo == OK){
                        reading = 0;
                        try_send_fd = 0;
                        last_seq = ( msg.controle.sequencia + 1 ) % 32;
                    }
                }else{
                    envio = send(conexao[0].fd, buffer, tamanhoMensagem(msg.controle.tamanho), 0);
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
            envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
            envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
            envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);

            reading = 1;
            while(reading){

                resp = timeout(conexao, buffer);
                if(!resp){
                    // printf("demorou reenviando...");
                    envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                    envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                    envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                }
                // resposta = read(conexao[0].fd, buffer, TAMANHO_MAXIMO);
                if((*((unsigned char *)buffer) == 126) && resp){
                    if(recuperaMensagem(&msg, buffer)){
                        if(msg.controle.tipo == ACK){
                            reading = 0;
                            try_send_data = 0;
                            // incrementa a sequencia em tres somente após a garantia que enviei as tres
                            msg.controle.sequencia = (msg.controle.sequencia + 1);
                            last_seq = msg.controle.sequencia;
                        }
                        if(msg.controle.tipo == NACK){
                                envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                                envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                                envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                        }
                    }else{
                        envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                        envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                        envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                    }
                    // calcula temporização aqui tambem'
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
    msg.controle.sequencia = last_seq;
    defineBuffer(&msg, buffer);
    int temp = 0;

    envio = send(conexao[0].fd, buffer, tamanhoMensagem(sizeof(FIM)), 0);

 

    saida_put:

    fclose(fd);
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

/**
 * Recebe o nome do arquivo que recebe da mensagem enviada pelo master
 * E realiza o recebimento de mensagens entre o slave/master
 */

void trata_put(int filedesk, Mensagem *first_msg){

    // printf("iniciando put ... \n");


    // int check_file = 0;


    // verifica existencia de erro
    int check_error = 0;

    // guarda valor do send()
    int envio;

    // guarda valor do read()
    int resposta;

    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;    // lembre-se do free mate

    // esse buffer fica reponsavel por receber os dados do cliente
    void *buffer_read;

    // controla o loop principal
    int main_loop = 1;

    // controla a sequencia de acordo com a ultima mensagem
    int last_seq;

    // variaveis auxiliares para identificar a sequencia da mensagem
    int min,med,max;

    // variavel auxiliar para contar as mensagens recebidas
    int cont = 0;

    // usada para pegar o tamanho da mensagem que esta sendo recebida
    int sizeofmessage;

    // redebe o nome do arquivo
    char name[500];

    // checa se os crcs estão sem erro
    int crcs[3];

    // tenta criar um arquivo e devolve resposta de sucesso com o nome...
    Mensagem msg,msgs[3];
    msgs[0].dados = malloc(TAMANHO_MAXIMO);
    msgs[1].dados = malloc(TAMANHO_MAXIMO);
    msgs[2].dados = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);

    last_seq = (first_msg->controle.sequencia + 1) % 32;

    strcpy(name,(char *) first_msg->dados);

    // cria ponteiro para file descriptor
    FILE *fd = NULL;
    fd = fopen(name,"w");
    if(fd == NULL){
        printf("erro ao criar fd do arquivo : %s \n",name);
        printf("mensagem de erro : %s", strerror(errno));
        goto saida_trataPut;
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
    msg.controle.sequencia = last_seq;
    msg.crc = 81;
    defineBuffer(&msg, buffer_send);
    envio = send(filedesk, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);

    while(main_loop){
        resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
        if(*((unsigned char *)buffer_read) == 126){
            if(recuperaMensagem(&msg, buffer_read)){
                switch(msg.controle.tipo){
                    case FD:
                        // sizeofmessage = (int *) msgs[0].dados;
                        // printf("Recebi o tamanho da mensagem %d\n",sizeofmessage);
                        sizeofmessage = *((int *) msg.dados);
                        msg.marcador_inicio = 126;
                        msg.controle.tamanho = 1;
                        msg.controle.sequencia = msg.controle.sequencia + 1;
                        last_seq = msg.controle.sequencia;
                        msg.controle.tipo = OK;
                        msg.crc = 81;
                        defineBuffer(&msg, buffer_send);
                        // adiciona o file descriptor ...
                        // em todo caso para teste ira enviar ok
                    break;
                    case DADOS:
                        // printf("Recebi o primeiro dado tratando\n");
                        // toda vez que chamar dados a primeira mensagem vai estar no buffer_read
                        crcs[0] = recuperaMensagem(&msgs[0], buffer_read);
                        cont = 1;
                            while(cont < 3){
                                resposta = read(filedesk, buffer_read, TAMANHO_MAXIMO);
                                if(*((unsigned char *)buffer_read) == 126){
                                    crcs[cont] = recuperaMensagem(&msgs[cont], buffer_read);
                                    cont++;
                                    *((unsigned char *)buffer_read) = 0;
                                }
                            }
                            // printf("Recebi as tres mensagens TCHAU\n");
                            min = msgs[0].controle.sequencia;
                            med = msgs[1].controle.sequencia;
                            max = msgs[2].controle.sequencia;

                            int try = ordena(&min,&med,&max);

                            // printf("crcs[0] = %d && crcs[1] = %d && crcs[2] = %d\n",crcs[0],crcs[1],crcs[2]);

                            if(try && crcs[0] && crcs[1] && crcs[2]){

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
                                msg.controle.sequencia = msgs[2].controle.sequencia + 1;
                                last_seq = msg.controle.sequencia;
                                msg.controle.tipo = ACK;
                                msg.crc = 81;
                                defineBuffer(&msg, buffer_send);
                            }else{
                                msg.marcador_inicio = 126;
                                msg.controle.tamanho = 1;
                                msg.controle.sequencia = msgs[2].controle.sequencia + 1;
                                last_seq = msg.controle.sequencia;
                                msg.controle.tipo = NACK;
                                msg.crc = 81;
                                defineBuffer(&msg, buffer_send);
                            }
                    break;
                    case FIM:
                        // printf("Recebi FIM\n");
                        main_loop = 0;
                    break;
                }
            }else{
                msg.marcador_inicio = 126;
                msg.controle.tamanho = 1;
                msg.controle.sequencia = last_seq + 1;
                msg.controle.tipo = NACK;
                msg.crc = 81;
                defineBuffer(&msg, buffer_send);
            }

            // printf("sai do case/switch\n");
        }
        // criar um temporizador e renviar a mensagem
        // mensagens de confirmação sempre tem o tamanho 1
        // printf("enviando mensagem ao cliente\n");
        envio = send(filedesk, buffer_send, tamanhoMensagem(1), 0);
    }
    // printf("saindo do put\n");

    fclose(fd);

    saida_trataPut:

    
    free(msgs[0].dados);
    free(msgs[1].dados);
    free(msgs[2].dados);
    free(msg.dados);
    free(buffer_send);
    free(buffer_read);
}

/**
 * Master envia requisição ao slave, o nome do arquivo requisitado, esta na variavel comando
 * Espera slave verificar a existencia/permissão do arquivo
 * Recebe os dados caso slave retorne ok
 */

void get(struct pollfd conexao[],char *local,char *remoto,char *comando,int sequencia){

    // guarda onde sera criado arquivo localrelativo/nomedoarquivo
    char name[500];

    char operador[500], semEspacos[500];

    removeEspacos(comando, semEspacos);
    strcpy(operador, remoto);
    strcat(operador, "/");
    strcat(operador, &semEspacos[3]);

    strcpy(name, local);
    strcat(name,"/");
    strcat(name,&semEspacos[3]);


    // descritor de arquivo
    FILE *fp = NULL;

    // checa erro de acesso
    int check_error = 0;

    // guarda o valor de saida do send()
    int envio;

    // guarda o valor de saida da read()
    int resposta;

    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;

    // esse buffer fica reponsavel por receber os dados do cliente
    void *buffer_read;

    // controla o loop principal
    int main_loop = 1;

    // controla a sequencia de acordo com a ultima mensagem
    int last_seq;

    // variaveis auxiliares para identificar a sequencia da mensagem
    int min,med,max;

    // variavel auxiliar para contar as mensagens recebidas
    int cont = 0;

    // usada para pegar o tamanho da mensagem que esta sendo recebida
    int sizeofmessage;

    // erro controla se vai ser usado fclose ou não, para evitar CORRUPTION ERROR
    int erro = 0;

    // checa se os crcs estão sem erro
    int crcs[3];

    // trata de garantir que não aconteça dois reads seguidos
    int recebeFD;

    // controla o envio do comando GET
    int loop_get = 1;

    // controla o timeout do envio da mensagem GET
    int resp;

    Mensagem msg,msgs[3];
    msgs[0].dados = malloc(TAMANHO_MAXIMO);
    msgs[1].dados = malloc(TAMANHO_MAXIMO);
    msgs[2].dados = malloc(TAMANHO_MAXIMO);
    msg.dados = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);

    last_seq = sequencia + 1;

    strcpy(msg.dados,operador);


    msg.marcador_inicio = 126;
    msg.controle.sequencia = last_seq;
    msg.controle.tipo = GET;
    msg.controle.tamanho = strlen(operador) + 1;
    msg.crc = 81;

    defineBuffer(&msg, buffer_send);

    envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
    while(loop_get){
        resp = timeout(conexao, buffer_read);

        if(!resp){
            envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
        }

        if(*((unsigned char *)buffer_read) == 126){
            if(recuperaMensagem(&msg, buffer_read)){
                if(msg.controle.tipo == FD){
                    recebeFD = 0;
                    loop_get = 0;
                    last_seq = (msg.controle.sequencia + 1) % 32;
                }
                if(msg.controle.tipo == ERRO){
                    recebeFD = 0;
                    loop_get = 0;
                    main_loop = 0;
                    erro = 1;
                    last_seq = (msg.controle.sequencia + 1) % 32;  
                    printf("mensagem de erro : %s\n", strerror(*((char*) msg.dados)));                  
                }
            }else{
                envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
            }
        }
    }


    while(main_loop){
        if(recebeFD){
            resposta = read(conexao[0].fd, buffer_read, TAMANHO_MAXIMO);
        }

        if(*((unsigned char *)buffer_read) == 126){
            if(recuperaMensagem(&msg, buffer_read)){
                switch(msg.controle.tipo){
                    case ERRO:
                        main_loop = 0;
                        erro = 1;
                        last_seq = (msg.controle.sequencia + 1) % 32;
                        printf("mensagem de erro : %s\n", strerror(*((char*) msg.dados))); 
                    break;
                    case FD:
                        fp = fopen(name,"w");
                        if(fp == NULL){
                            printf("erro ao criar fd do arquivo : %s \n",name);
                            printf("mensagem de erro : %s\n", strerror(errno));
                            main_loop = 0;
                            erro = 1;
                            last_seq = (msg.controle.sequencia + 1) % 32;
                            msg.controle.tipo = ERRO;
                            msg.controle.tamanho = 1;
                            *((char *)msg.dados) = errno;
                        }
                        else{
                            msg.controle.tipo = OK;
                            msg.controle.tamanho = 0;
                        }

                        sizeofmessage = *((int *) msg.dados);
                        msg.marcador_inicio = 126;                        
                        msg.controle.sequencia = last_seq;
                        
                        defineBuffer(&msg, buffer_send);
                        // adiciona o file descriptor ...
                        // em todo caso para teste ira enviar ok
                        envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(1), 0);
                        // Habilita leitura de menságens dentro deste laço.
                        recebeFD = 1;
                    break;
                    case DADOS:
                        // toda vez que chamar dados a primeira mensagem vai estar no buffer_read
                        crcs[0] = recuperaMensagem(&msgs[0], buffer_read);
                        cont = 1;
                        while(cont < 3){
                            resposta = read(conexao[0].fd, buffer_read, TAMANHO_MAXIMO);
                            if(*((unsigned char *)buffer_read) == 126){
                                crcs[cont] = recuperaMensagem(&msgs[cont], buffer_read);
                                cont++;
                                *((unsigned char *)buffer_read) = 0;
                            }
                        }
                        min = msgs[0].controle.sequencia;
                        med = msgs[1].controle.sequencia;
                        max = msgs[2].controle.sequencia;
                        int try = ordena(&min,&med,&max);

                        if(try && crcs[0] && crcs[1] && crcs[2]){

                            if(min == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[0].dados);
                            }else if(min == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[1].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[2].dados);
                            }

                            if(med == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[0].dados);
                            }else if(med == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[1].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[2].dados);
                            }

                            if(max == msgs[0].controle.sequencia){
                                fwrite((char *) msgs[0].dados,1,msgs[0].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[0].dados);
                            }else if(max == msgs[1].controle.sequencia){
                                fwrite((char *) msgs[1].dados,1,msgs[1].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[1].dados);
                            }else{
                                fwrite((char *) msgs[2].dados,1,msgs[2].controle.tamanho,fp);
                                // printf("%s\n",(char *) msgs[2].dados);
                            }

                            msg.marcador_inicio = 126;
                            msg.controle.tamanho = 1;
                            // msgs[2].controle.sequencia tem o valor da ultima sequencia recebida
                            msg.controle.sequencia = msgs[2].controle.sequencia + 1;
                            msg.controle.tipo = ACK;
                            msg.crc = 81;
                            defineBuffer(&msg, buffer_send);

                        }else{
                            msg.marcador_inicio = 126;
                            msg.controle.tamanho = 1;
                            // msgs[2].controle.sequencia tem o valor da ultima sequencia recebida
                            msg.controle.sequencia = msgs[2].controle.sequencia + 1;
                            msg.controle.tipo = NACK;
                            msg.crc = 81;
                            defineBuffer(&msg, buffer_send);
                        }
                        envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(1), 0);
                    break;
                    case FIM:
                        // printf("Recebi FIM\n");
                        main_loop = 0;
                    break;
                }
            }
            // *((unsigned char *)buffer_read) = 0;
        }
        // criar um temporizador e renviar a mensagem
        // mensagens de confirmação sempre tem o tamanho 1

    }
    // printf("saindo do get\n");

    if(!erro){
        fclose(fp);
    }
    free(msgs[0].dados);
    free(msgs[1].dados);
    free(msgs[2].dados);
    free(msg.dados);
    free(buffer_send);
    free(buffer_read);
}

/**
 * Slave recebe requisição do master e faz a verificação de existencia/permissão
 * Caso sucesso envia a(s) mensagem(s) para o master
 */

void trata_get(struct pollfd conexao[],Mensagem *first_mensagem){

    // ira reter o nome com o caminho relativo
    char name[500];

    // controla se encontrou algum erro no caminho caso contrario fica 1
    int sucess = 1;

    // tenta enviar o file descriptor -- tamanho no caso
    int try_send_fd = 1;

    // controla o envio dos dados montados(ou seja não é necessario de montar o buffer de cada)
    int try_send_data = 1;

    // controla se todo arquivo ja foi lido
    int has_data_to_sent = 1;

    // controla a sequencia de acordo com a ultima mensagem
    int last_seq;

    // guarda valor do send()
    int envio;

    // controla loop's que aguardam resposta do master
    int reading;

    // buffer recebe mensagem
    void *buffer;

    // esse buffer fica responsavel por enviar sempre a ultima resposta
    void *buffer_send;

    // buffer de envio da primeira mensagem
    void *buffer0;

    // buffer de envio da segunda mensagem
    void *buffer1;

    // buffer de envio da terceira mensagem
    void *buffer2;

    // controla o tamanho da mensagem
    int indice;

    // controla a sequencia de cada mensagem
    int sequencia0,sequencia1,sequencia2;

    // contem o tamanho da mensagem
    int tamanho_da_mensagem;

    // controla o tamanho das mensagens uma a uma
    int tam0,tam1,tam2;

    // erro garante que não sera utilizado fclose para evitar CORRUPTION ERROR
    int erro = 0;

    // variavel auxiliar do timeout
    int resp;

    // vetor de string que guarda as mensagems 127 em 127 bytes
    char *dados[3];

    dados[0] = malloc(TAMANHO_MAXIMO);
    dados[1] = malloc(TAMANHO_MAXIMO);
    dados[2] = malloc(TAMANHO_MAXIMO);
    buffer = malloc(TAMANHO_MAXIMO);
    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer0 = malloc(TAMANHO_MAXIMO);
    buffer1 = malloc(TAMANHO_MAXIMO);
    buffer2 = malloc(TAMANHO_MAXIMO);

    // estrutura de controle da mensagem
    Mensagem    msg;
    msg.dados = malloc(127);

    
    strcpy(name,(char*) first_mensagem->dados);

    if(access(name,F_OK)){
        printf("No such file or directory \n");
        *((char *)msg.dados) = errno;
        erro = 1;
        goto label;
    }

    struct stat path_stat;
    stat(name, &path_stat);
    if(!S_ISREG(path_stat.st_mode)){
        printf("Erro arquivo invalido : TIPO\n");
        *((char *)msg.dados) = errno;
        erro = 1;
        goto label;
    }

    FILE *fd = NULL;
    fd = fopen(name,"r");
    if(fd == NULL){
        *((char *)buffer) = FD_err;
        printf("erro ao criar fd do arquivo : %s \n",name);
        erro = 1;
        goto label;
    }else{

        fseek(fd,0,SEEK_END);
        tamanho_da_mensagem = ftell(fd);
        // printf("tamanho da mensagem %d\n",tamanho_da_mensagem);
        // devolve o ponteiro no inicio do file descriptor
        rewind(fd);
    }

    last_seq = (msg.controle.sequencia + 1) % 32;
    

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
            defineBuffer(&msg, buffer_send);

            envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);

            reading = 1;

            while(reading){

                resp = timeout(conexao, buffer);

                if(!resp){
                    envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
                }

                if((*((unsigned char *)buffer) == 126) && resp){
                    if(recuperaMensagem(&msg, buffer)){
                        if(msg.controle.tipo == OK){
                            reading = 0;
                            try_send_fd = 0;
                            last_seq = (msg.controle.sequencia + 1) % 32;
                        }
                        if(msg.controle.tipo == ERRO){
                            reading = 0;
                            try_send_fd = 0;
                            last_seq = (msg.controle.sequencia + 1) % 32;
                            goto clear_dados;
                        }
                    }else{
                        envio = send(conexao[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho), 0);
                    }
                }
                *((unsigned char *)buffer) = 0;
            }
        }

        int i = 0;

        while(has_data_to_sent){

            sequencia0 = (msg.controle.sequencia + 1) % 32;
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

                envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);

                reading = 1;

                while(reading){
                    resp = timeout(conexao, buffer);

                    if(!resp){
                        envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                        envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                        envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                    }

                    if((*((unsigned char *)buffer) == 126) && resp){
                        if(recuperaMensagem(&msg, buffer)){
                            if(msg.controle.tipo == ACK){
                                reading = 0;
                                try_send_data = 0;
                            }
                            if(msg.controle.tipo == NACK){
                                    envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                                    envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                                    envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                            }
                        }else{
                            envio = send(conexao[0].fd, buffer0, tamanhoMensagem(tam0), 0);
                            envio = send(conexao[0].fd, buffer1, tamanhoMensagem(tam1), 0);
                            envio = send(conexao[0].fd, buffer2, tamanhoMensagem(tam2), 0);
                        }
                    }
                    if(i >= tamanho_da_mensagem){
                        has_data_to_sent = 0;
                    }
                }
            }
        }
        msg.controle.tipo = FIM;
        msg.controle.tamanho = 0;
    }else{
        label:
        msg.controle.tipo = ERRO;
        msg.controle.tamanho = 1;

    }

    msg.marcador_inicio = 126;
    msg.controle.sequencia = msg.controle.sequencia + 1;
    msg.crc = 81;
    defineBuffer(&msg, buffer);

    envio = send(conexao[0].fd, buffer, tamanhoMensagem(sizeof(char)), 0);

    clear_dados:

    if(!erro){
        fclose(fd);
    }
    free(dados[0]);
    free(dados[1]);
    free(dados[2]);
    free(buffer_send);
    free(buffer);
    free(buffer0);
    free(buffer1);
    free(buffer2);
    free(msg.dados);
}

/**
 * Cria a tabela de crc, baseada no polinomio cujo valor binario 7
 */

void calcula_tabela_crc(){

    __int8_t base = 0x7;
    __int8_t byteAtual;
    int max = 256;
    for(int i = 0 ; i < max ;i++){
        byteAtual = i;
        for (int bit = 0; bit < 8; bit++)
        {
            if ((byteAtual & 0x80) != 0)
            {
                byteAtual <<= 1;
                byteAtual ^= base;
            }
            else
            {
                byteAtual <<= 1;
            }
        }
        tabela[i] = byteAtual;
    }
}

/**
 * Executa calculo de crc e retorna o valor
 */

unsigned char calcula_crc(__int8_t *dados,int tamanho){

    __uint8_t crc = 0;


    for(int i = 0 ; i < tamanho; i++){
        __uint8_t data = (dados[i] ^ crc);
        crc = tabela[data];
    }
    return crc;
}
