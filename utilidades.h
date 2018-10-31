
#ifndef _Utilidades_
#define _Utilidades_

#include <stdio.h>

    #define TAMANHO_MAXIMO 131

    typedef struct ControleMSG{
        unsigned short tamanho:7;
        unsigned short sequencia:5;
        unsigned short tipo:4;

    } ControleMSG;

    typedef struct Mensagem{

        unsigned char  marcador_inicio;
        ControleMSG controle;
        void *dados;
        unsigned char crc;

    }Mensagem;

    enum Tipos{
        NACK,
        ACK,
        OK,
        DADOS,
        HANDSHAKE,
        FD,  // file descriptor
        NONE2,
        SHOW_SCREEN,
        NONE3,
        NONE4,
        GET,
        PUT,
        CD,
        LS,
        ERRO,
        FIM
    };

    enum Erros{
        NONE6,
        INEXISTENTE, // access(const char *pathname, F_OK);
        PERMICAO, // access(const char *pathname, R_OK || W_OK);
        QUOTA,
        TIPO,
    };

    /**
     * Devolve o tamanho da menssagem para envio (Necessário uma vez que mensagens
     *  de tamanho menor que 14 não são enviadas)
    */
    int tamanhoMensagem(int i);
    /**
     * Transfere Mensagem para buffer que será transmitido
    */
    void defineBuffer(Mensagem * msg, void * buffer);
    void put(int filedesk, char *name);
    void trata_put(int filedesk,Mensagem *first_message);
    /**
     * Recupera Mensagem de buffer recebido
    */
    void recuperaMensagem(Mensagem * msg, void * buffer);

    /**
     * imprime locais correntes do usuário (LOCAL E REMOTO)
    */
    void imprimeLocalizacao(char * local,char * remoto);

    /**
     * Trata e executa ls local.
    */
    void local_ls(char * comando, char * local);

    /**
     * Trata e executa ls local.
    */
    void local_cd(char * comando, char * local);

#endif
