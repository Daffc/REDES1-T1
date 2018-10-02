#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main(){

    int fileslave;
    int saidaread;
    fileslave = ConexaoRawSocket("lo");

    MensagemContent* msg;

    msg = malloc(sizeof(MensagemContent));
    
    
    while(1){
        saidaread = read(fileslave, msg, sizeof(MensagemContent));
        printf("%d, %d, %d, %d, %d, %d\n", msg->a, msg->b, msg->c, msg->d, msg->e, msg->f);
        sleep(2);         
    }   

    return 0;
}