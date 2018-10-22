
#ifndef _Utilidades_
#define _Utilidades_

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
        INEXISTENTE,
        PERMICAO,
        QUOTA,
    };

    /**
     * Devolve o tamanho da menssagem para envio (Necessário uma vez que mensagens
     *  de tamanho menor que 14 não são enviadas)
    */
    int tamanhoMensagem(Mensagem *msg);
    
    /**
     * Transfere Mensagem para buffer que será transmitido
    */
    void defineBuffer(Mensagem * msg, void * buffer);

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
