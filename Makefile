INCL=-I ./head/
all: server client install_conf

validate.o:
	gcc -o ./src/validate.o ${INCL} -c ./src/validate.c

net_trans.o:
	gcc -o ./src/net_trans.o ${INCL} -c ./src/net_trans.c

encrypt.o:
	gcc -o ./src/encrypt.o ${INCL} -c ./src/encrypt.c

config.o:
	gcc -o ./src/config.o ${INCL} -c ./src/config.c

random_port.o:
	gcc -o ./src/random_port.o ${INCL} -c ./src/random_port.c

client: random_port.o config.o encrypt.o net_trans.o validate.o
	gcc -o ./client/client ${INCL} ./client/client.c ./src/random_port.o ./src/config.o ./src/encrypt.o ./src/net_trans.o ./src/validate.o -lpthread

server: random_port.o config.o encrypt.o net_trans.o
	gcc -o ./server/server ${INCL} ./server/server.c ./src/random_port.o ./src/config.o ./src/encrypt.o ./src/net_trans.o

install_conf:
	@if [ ! -d "/etc/sftt/" ]; then \
		mkdir -p /etc/sftt; \
	fi; \
	cp ./conf/* /etc/sftt/

clean:
	rm ./src/*.o
	rm ./client/*.o
	rm ./server/*.o
	
	 
