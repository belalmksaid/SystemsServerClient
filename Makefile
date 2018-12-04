all: bankingServer bankingClient

bankingServer: bankingServer.c
	gcc -g -fsanitize=address -w -pthread bankingServer.c -o bankingServer

bankingClient: bankingClient.c
	gcc -g -fsanitize=address -pthread bankingClient.c -o bankingClient


clean:
	rm -f bankingClient
	rm -f bankingServer
