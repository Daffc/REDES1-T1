# DOUGLAS AFFONSO CLEMENTINO
# GRR20175877

CC     = gcc -std=c11 -g

EXECS  = master slave
 
# arquivos-objeto
objects = master.o slave.o ConexaoRawSocket.o

all:  $(EXECS)

master: master.o ConexaoRawSocket.o 
	$(CC) master.o ConexaoRawSocket.o -o master 

slave: slave.o ConexaoRawSocket.o 
	$(CC) slave.o ConexaoRawSocket.o -o slave 

master.o : master.c ConexaoRawSocket.h
	$(CC) -c master.c 

slave.o : slave.c ConexaoRawSocket.h
	$(CC) -c slave.c 

ConexaoRawSocket.o : ConexaoRawSocket.c ConexaoRawSocket.h
	$(CC) -c ConexaoRawSocket.c 


clean:
	-rm $(objects)
	-rm *~

purge: clean
	-rm $(EXECS)