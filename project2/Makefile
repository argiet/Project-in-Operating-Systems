all: server client script

server: server11.c 
	gcc -o server server4.c -pthread

client: client1.c
	gcc -o client client.c

script: run_tests.sh
	chmod +x run_tests.sh

clean:
	rm -f server
	rm -f client
