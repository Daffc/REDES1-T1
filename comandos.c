#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Chama por rotina nativa e retorna ponteiro para streaming contendo resposta
 */

FILE * IniciaDescritorLs(char *comando, char *local)
{    
    FILE *fp;
    char resultado[1000];

    strcpy(resultado, comando);
    strcat(resultado, " ");
    strcat(resultado, local);

    /* Chama comando e devolve *FILE para saida de programa chamado.*/
    fp = popen(resultado, "r");

    if (fp == NULL) {
        fprintf(stderr, "Não foi possível executar comando\n" );
        exit(1);
    }

    return fp;
}
/**
 * Finaliza ponteiro de streaming
 */
void FinalizaDescritorLs(FILE *fp){
    if(fp != NULL){}
        pclose(fp);    
}