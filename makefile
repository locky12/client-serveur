all: server.bin client.bin clean

server.bin: server.o
	gcc -o server.bin server.o

client.bin: client.o
	gcc -o client.bin client.o

client.o: client.c client.h
	gcc -c client.c

server.o: server.c server.h
	gcc -c server.c 


clean: 
	rm -f  *.o *~
