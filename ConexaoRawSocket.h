// adicionar IFDEF

int ConexaoRawSocket(char *device);

typedef struct Mensagem{
    
    unsigned char marcador_inicio;
    unsigned char tamanho;
    unsigned char sequencia;
    unsigned char tipo;
    char dados;
    unsigned char crc;
    
}Mensagem;

enum Tipos{
    NACK,
    ACK,
    OK,
    DADOS,
    NONE,
    FD,  // file descriptor
    NONE,
    SHOW_SCREEN,
    NONE,
    NONE,
    GET,
    PUT,
    CD,
    LS,
    ERRO,
    FIM
}

enum Erros{
    NONE,
    INEXISTENTE,
    PERMICAO,
    QUOTA,
    NONE
}