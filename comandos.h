#ifndef _COMANDOS_
#define _COMANDOS_

    #include <stdlib.h>
    #include <stdio.h>
    
    /**
     * Retorna ponteiro para descritor de arquivo com resposta.
    */
    FILE * IniciaDescritorComando( );
    
    /**
    * Finaliza ponteiro previamente aberto.
    */
    void FinalizaDescritorComando(FILE *fp);
#endif