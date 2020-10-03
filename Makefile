INCL=-I ./head/
all: server client

encrypt.o:
	gcc -o ./src/encrypt.o ${INCL} -c ./src/encrypt.c
config.o:
	gcc -o ./src/config.o ${INCL} -c ./src/config.c

random_port.o:
	gcc -o ./src/random_port.o ${INCL} -c ./src/random_port.c

client: random_port.o config.o
	gcc -o ./client/client ${INCL} ./client/client.c ./src/random_port.o ./src/config.o ./src/encrypt.o 

server: random_port.o config.o encrypt.o
	gcc -o ./server/server ${INCL} ./server/server.c ./src/random_port.o ./src/config.o ./src/encrypt.o 

clean:
	rm ./src/*.o
	rm ./client/*.o
	rm ./server/*.o
	
	 
