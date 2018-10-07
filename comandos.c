#include <stdio.h>
#include <stdlib.h>


FILE * IniciaDescritorComando(char *comando)
{    
    FILE *fp;
    
    /* Chama comando e devolve *FILE para saida de programa chamado.*/
    fp = popen(comando, "r");

    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

  return fp;
}

void FinalizaDescritorComando(FILE *fp){

    if(fp != NULL)
    pclose(fp);    

}