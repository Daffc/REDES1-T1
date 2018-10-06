
#ifndef _Utilidades_
#define _Utilidades_

    typedef struct Mensagem{
        
        unsigned char marcador_inicio;
        unsigned char crc;
        unsigned short tamanho:7;
        unsigned short sequencia:5;
        unsigned short tipo:4;
        //void *dados;
        
    }Mensagem;

    enum Tipos{
        NACK,
        ACK,
        OK,
        DADOS,
        NONE1,
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

#endif
