# DOUGLAS AFFONSO CLEMENTINO
# GRR20175877

CC     = gcc -g

EXECS  = master slave
 
# arquivos-objeto
objects = master.o slave.o ConexaoRawSocket.o comandos.o utilidades.o

all:  $(EXECS)

master: master.o ConexaoRawSocket.o comandos.o utilidades.o
	$(CC) master.o ConexaoRawSocket.o comandos.o utilidades.o -o master 

slave: slave.o ConexaoRawSocket.o comandos.o utilidades.o
	$(CC) slave.o ConexaoRawSocket.o comandos.o utilidades.o -o slave 

master.o : master.c ConexaoRawSocket.h utilidades.h comandos.h
	$(CC) -c master.c 

slave.o : slave.c ConexaoRawSocket.h utilidades.h
	$(CC) -c slave.c 

ConexaoRawSocket.o : ConexaoRawSocket.c ConexaoRawSocket.h
	$(CC) -c ConexaoRawSocket.c 

comandos.o : comandos.c comandos.h
	$(CC) -c comandos.c 

utilidades.o : utilidades.c utilidades.h
	$(CC) -c utilidades.c 


clean:
	-rm $(objects)
	-rm *~

purge: clean
	-rm $(EXECS)