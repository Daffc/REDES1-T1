#include "ConexaoRawSocket.h"
#include "utilidades.h"
#include "comandos.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <pwd.h>
#include <poll.h>

int main(){

    int         conexao, retorno, resp;
    char        comando[100], local[500], remoto[500], *uName, *bufferLS;
    void        *buffer;
    Mensagem    msg;

    // conexao = ConexaoRawSocket("eno1");
    struct pollfd fds[1];

    fds[0].fd = ConexaoRawSocket("eno1");
    fds[0].events = POLLIN;

    buffer = malloc(TAMANHO_MAXIMO);

    msg.dados = malloc(127);


    // Definindo mensagem inicial.
    msg.marcador_inicio = 126;
    msg.controle.tamanho = 0;
    msg.controle.sequencia = 0;
    msg.controle.tipo = HANDSHAKE;
    msg.crc = 81;

    defineBuffer(&msg, buffer);   
    
    // int resp = write(conexao, buffer, tamanhoMensagem(msg.controle.tamanho));

    // if(resp < 0){
    //     printf("Erro ao enviar Mensagem: HANDSHAKE\n");
    //     exit(-1);
    // }

    // resp = read(conexao, buffer, TAMANHO_MAXIMO);

    memset(buffer, 0, TAMANHO_MAXIMO);
    while(!msg.controle.sequencia){
        // resp = read(conexao, buffer, TAMANHO_MAXIMO);

        resp = poll(fds, 1, 10 * 1000);

        if(resp == -1){
            printf("DEU RUIM\n");
        }

        if (!resp) {
            printf("DEMOROU DEMAIS\n");
        }
        if (fds[0].revents & POLLIN)
            printf ("DEU BOM\n");
            
        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if(*((unsigned char *)buffer) == 126){

            recuperaMensagem(&msg, buffer);

            printf("%d\t", msg.marcador_inicio);
            printf("%d\t%d\t%d\t", msg.controle.sequencia, msg.controle.tamanho, msg.controle.tipo);
            printf("%s\t", (char *)msg.dados);
            printf("%d\n", msg.crc);

            uName = getpwuid(geteuid ())->pw_dir;
            strcpy(remoto, msg.dados);
            getcwd(local, 500);
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
            retorno = local_ls(comando, local, &bufferLS);

            // Caso retorno de função seja diferente de 0, informar o erro ao usuário.
            if(retorno){
                printf("Comando '%s' inválido\n", comando);
            }
            else{
                // Caso operação tenha ocorricdo como esperado, imprime resuldado do comando informado.
                printf("%s", bufferLS);
                free(bufferLS);
            }
        }

        /**
         * Caso comando inicie com a String "cd".
        */
        else if(strstr(comando, "cd") ==  comando){
            retorno = local_cd(comando, local);

            // Verifica se houve erro na operação.
            if(retorno){
                // Verifica se erro está relacionado ao tipo apontado por comando solicitado.
                if(retorno < 0){
                    printf("O caminho indicado não aponta para um diretório.\n");
                }
                // Caso erro esteja relacionado a permissão de leitura ou existência do caminho solicitado
                else{
                    printf("Não foi possivel concluir operação em no caminho indicado : %s\n", strerror(retorno));
                }
            }
        }

        /**
         * Caso comando inicie com a String "rls".
        */
        else if(strstr(comando, "rls") ==  comando){
            retorno = remote_ls(conexao, remoto, comando, msg.controle.sequencia);

            // Caso retorno de função seja diferente de 0, informar o erro ao usuário.
            if(retorno){
                printf("Comando '%s' inválido\n", comando);
            }
        }

        /**
         * Caso comando inicie com a String "rcd".
        */
        else if(strstr(comando, "rcd") ==  comando){
            printf("COMANDO RCD\n");
            remote_cd(conexao, remoto, comando, msg.controle.sequencia);
        }

        /**
         * Caso comando inicie com a String "get".
        */
        else if(strstr(comando, "get") ==  comando){
            printf("COMANDO GET\n");
            get(conexao,local,remoto,comando,msg.controle.sequencia);
        }

        /**
         * Caso comando inicie com a String "put".
        */
        else if(strstr(comando, "put") ==  comando){
            printf("COMANDO PUT\n");
            put(conexao,local,remoto,comando);
        }

        /**
         * Caso comando informado não condiza com nenhum dos listados acima (cd, ls e suas variações).
        */
        else{
            printf("Comando '%s' inválido\n", comando);
        }
    }


    return 0;
}

