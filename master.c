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

int main()
{

    int retorno, resp;
    char comando[100], local[500], remoto[500], *uName, *bufferLS;
    void *buffer_send, *buffer_read;
    Mensagem msg;

    struct pollfd fds[1];

    fds[0].fd = ConexaoRawSocket("eno1");
    fds[0].events = POLLIN;

    buffer_send = malloc(TAMANHO_MAXIMO);
    buffer_read = malloc(TAMANHO_MAXIMO);

    calcula_tabela_crc();

    msg.dados = malloc(127);

    // Definindo mensagem inicial.
    msg.marcador_inicio = 126;
    msg.controle.tamanho = 0;
    msg.controle.sequencia = 0;
    msg.controle.tipo = HANDSHAKE;
    msg.crc = 81;

    defineBuffer(&msg, buffer_send);

    write(fds[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho));

    int received = 1;
    while (received)
    {

        //Envia mensagem solicitando Informações do servidor (Caminho do iretório onde servido esta sendo executado).

        resp = timeout(fds, buffer_read);

        if (!resp)
        {
            write(fds[0].fd, buffer_send, tamanhoMensagem(msg.controle.tamanho));
        }

        //Somente le mensagem caso marcador de inicio sejá '0111 1110'
        if ((*((unsigned char *)buffer_read) == 126) && resp)
        {
            if (recuperaMensagem(&msg, buffer_read))
            {

                uName = getpwuid(geteuid())->pw_dir;
                strcpy(remoto, msg.dados);
                getcwd(local, 500);
                received = 0;
            }
            else
            {

                msg.controle.tipo = NACK;
                msg.marcador_inicio = 126;
                msg.controle.tamanho = 0;
                msg.controle.sequencia = 0;
                defineBuffer(&msg, buffer_send);
            }
        }
    }

    // TESTES COM COMANDOS !!!
    while (1)
    {
        imprimeLocalizacao(local, remoto);

        // Le linha de comando indicada por usuario.
        scanf(" %99[^\n]", comando);

        /**
         * Caso comando inicie com a String "ls".
        */
        if (strstr(comando, "ls") == comando)
        {
            retorno = local_ls(comando, local, &bufferLS);

            // Caso retorno de função seja diferente de 0, informar o erro ao usuário.
            if (retorno)
            {
                printf("Comando '%s' inválido\n", comando);
            }
            else
            {
                // Caso operação tenha ocorricdo como esperado, imprime resuldado do comando informado.
                printf("%s", bufferLS);
                free(bufferLS);
            }
        }

        /**
         * Caso comando inicie com a String "cd".
        */
        else if (strstr(comando, "cd") == comando)
        {
            retorno = local_cd(comando, local);

            // Verifica se houve erro na operação.
            if (retorno)
            {
                // Verifica se erro está relacionado ao tipo apontado por comando solicitado.
                if (retorno < 0)
                {
                    printf("O caminho indicado não aponta para um diretório.\n");
                }
                // Caso erro esteja relacionado a permissão de leitura ou existência do caminho solicitado
                else
                {
                    printf("Não foi possivel concluir operação em no caminho indicado : %s\n", strerror(retorno));
                }
            }
        }

        /**
         * Caso comando inicie com a String "rls".
        */
        else if (strstr(comando, "rls") == comando)
        {
            retorno = remote_ls(fds, remoto, comando, msg.controle.sequencia);

            // Caso retorno de função seja diferente de 0, informar o erro ao usuário.
            if (retorno)
            {
                printf("Comando '%s' inválido\n", comando);
            }
        }

        /**
         * Caso comando inicie com a String "rcd".
        */
        else if (strstr(comando, "rcd") == comando)
        {
            remote_cd(fds, remoto, comando, msg.controle.sequencia);
        }

        /**
         * Caso comando inicie com a String "get".
        */
        else if (strstr(comando, "get") == comando)
        {
            get(fds, local, remoto, comando, msg.controle.sequencia);
        }

        /**
         * Caso comando inicie com a String "put".
        */
        else if (strstr(comando, "put") == comando)
        {
            put(fds, local, remoto, comando);
        }

        /**
         * Caso comando informado não condiza com nenhum dos listados acima (cd, ls e suas variações).
        */
        else
        {
            printf("Comando '%s' inválido\n", comando);
        }
    }

    free(buffer_send);
    free(buffer_read);
    free(msg.dados);
    return 0;
}
