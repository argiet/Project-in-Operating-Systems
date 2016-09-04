all: server client script

server: server11.c 
	gcc -o server server11.c -pthread

client: client1.c
	gcc -o client client1.c

script: scr.sh
	chmod +x scr.sh

clean:
	rm -f server
	rm -f client