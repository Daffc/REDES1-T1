#ifndef _COMANDOS_
#define _COMANDOS_

    #include <stdlib.h>
    #include <stdio.h>
    
    /**
     * Retorna ponteiro para descritor de arquivo com resposta.
    */
    FILE * IniciaDescritorLs(char *comando, char *local);
    
    /**
    * Finaliza ponteiro previamente aberto.
    */
    void FinalizaDescritorLs(FILE *fp);
#endif