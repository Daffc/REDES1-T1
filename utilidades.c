#include "utilidades.h"

#include <string.h>
#include <stdio.h>


void defineBuffer(Mensagem * msg, void * buffer){

    memcpy(buffer, &(msg->marcador_inicio), 1);
    memcpy(buffer + 1, &(msg->controle), 2);
    memcpy(buffer + 3, msg->dados, msg->controle.tamanho); 
    memcpy(buffer + 3 + msg->controle.tamanho, &(msg->crc), 1);    
}


void recuperaMensagem(Mensagem * msg, void * buffer){

    // Guarda Marcador de início;
    memcpy(&(msg->marcador_inicio), buffer, 1);

    // Guarda informações de controle (tamanho, sequencia, tipo)
    memcpy(&(msg->controle), buffer + 1, 2);

    //Guarda dados de acordo com o informado em controle.tamanho
    memcpy(msg->dados, buffer + 3, msg->controle.tamanho); 

    // Guarda crc.   
    memcpy(&(msg->crc),buffer + 3 + msg->controle.tamanho, 1);    
    
}