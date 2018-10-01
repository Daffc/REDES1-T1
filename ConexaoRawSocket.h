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

typedef struct passar{
    int a;
    int b;
    int c;
}passar;

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
    NONE7
};