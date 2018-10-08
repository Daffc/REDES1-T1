
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

    void defineBuffer(Mensagem * msg, void * buffer);
    void recuperaMensagem(Mensagem * msg, void * buffer);    
    

#endif
