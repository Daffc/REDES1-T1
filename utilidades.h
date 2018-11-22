
#ifndef _Utilidades_
#define _Utilidades_

#include <stdio.h>
#include <poll.h>
#include <linux/types.h>

    #define TAMANHO_MAXIMO 131
    #define TIMEOUT 1

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
        MOSTRA_TELA,
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
        FD_err,
    };

    /**
     * Devolve o tamanho da menssagem para envio (Necessário uma vez que mensagens
     *  de tamanho menor que 14 não são enviadas)
    */
    int tamanhoMensagem(int i);
    /**
     * Transfere Mensagem para buffer que será transmitido
    */
    void defineBuffer(Mensagem *msg, void * buffer);
    /**
     * Retira o nome do arquivo que sera enviado da variavel comando
     * E realiza o envio de mensagens entre o master/slave
    */
    void put(struct pollfd fds[], char *local , char*remoto , char *name);
    /**
     * Recebe o nome do arquivo que recebe da mensagem enviada pelo master
     * E realiza o recebimento de mensagens entre o slave/master
     */
    void trata_put(int filedesk,Mensagem *first_message);
    /**
     * Recupera Mensagem de buffer recebido e verifica crc de bonus
    */
    int recuperaMensagem(Mensagem * msg, void * buffer);

    /**
     * Trata timeout de envio de mensagem, informada em 'buffer_send' e seu tamanho informado 
     * em 'tamanho', caso mensagem não retorne a tempo função retornará 0, caso mensagem volte
     * a tempo função retornará 1 e e buffer_read conterá mensagem recebida.
    */
    int timeout(struct pollfd fds[], void *buffer_read);


    /**
     * imprime locais correntes do usuário (LOCAL E REMOTO)
    */
    void imprimeLocalizacao(char * local,char * remoto);

    /**
     * Trata e executa ls local, retornando '0' no caso de sucesso e '-1' no caso de falha.
     * 'comando' está relacionado à String da operação solicitada.
     * 'local' indica posição de memória com localização atual do usuário.
     * 'bufferSaida' armazenará resposta do 'ls' caso operação seja concluida com sucesso.
    */
    int local_ls(char * comando, char * local, char **bufferSaida);

    /**
     * Trata e executa cd local, retornando -1 caso apontado não seja um diretório, 
     * o código gerado pelo acesso representando erro de existência ou à permissão 
     * de leitura ou 0 caso operação tenha ocorrido com sucesso.
     * 'comando' está relacionado à String da operação solicitada.
     * 'local' indica posição de memória com localização atual do usuário.
    */
    int local_cd(char * comando, char * local);
    /**
     * função que trata requisição de rls no lado do servidor, utilizando da função local_ls
     */
    void trata_ls(struct pollfd conexao[], Mensagem *first_mensagem);
    /**
     * Recebe chamada de mestre, invoca servidor com pedido de 'ls' e espera por resposta.
     * servidor invoca ls local e retorna para mestre resposta do 'ls' solicitado.
     * Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
     */
    int remote_ls(struct pollfd conexao[], char *remoto, char *comando, int sequencia);
    /**
     * Recebe chamada de mestre, invoca servidor com pedido de 'cd' e espera por resposta.
     * servidor invoca cd local e retorna para mestre resposta do 'cd' solicitado.
     * Recebe resposta, guardando resultado em buffer e retornando '0' no caso operação tenha ocorrido com sucesso ou erro informado pelo servidor.
     */
    void remote_cd(struct pollfd conexao[], char *local,char *comando, int sequencia);
    /**
     * função que trata requisição de rcd no lado do servidor, utilizando da função local_cd
     */
    void trata_cd(int filedesk,Mensagem *msg);
    /**
     * Master envia requisição ao slave, o nome do arquivo requisitado, esta na variavel comando
     * Espera slave verificar a existencia/permissão do arquivo
     * Recebe os dados caso slave retorne ok
     */
    void get(struct pollfd conexao[],char *local_remote,char *local_local,char *comando,int sequencia);
    /**
     * Slave recebe requisição do master e faz a verificação de existencia/permissão
     * Caso sucesso envia a(s) mensagem(s) para o master
     */
    void trata_get(struct pollfd conexao[], Mensagem *msg);
    /**
     * Executa calculo de crc e retorna o valor
     */

    unsigned char calcula_crc(__int8_t *dados,int tamanho);
    /**
     * Cria a tabela de crc, baseada no polinomio cujo valor binario 7
     */
    void calcula_tabela_crc();

    

#endif
