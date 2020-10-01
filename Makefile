INCL=-I ./head/
all: server client

random_port.o:
	gcc -o ./src/random_port.o ${INCL} -c ./src/random_port.c

client: random_port.o
	gcc -o ./client/client ${INCL} ./client/client.c ./src/random_port.o

server: random_port.o
	gcc -o ./server/server ${INCL} ./server/server.c ./src/random_port.o

clean:
	rm ./src/*.o
	rm ./client/*.o
	rm ./server/*.o
	
	 
