all: bankingServer bankingClient

bankingServer:
	gcc -g -fsanitize=address -pthread bankingServer.c -o bankingServer

bankingClient:
	gcc -g -fsanitize=address -pthread bankingClient.c -o bankingClient


clean:
	rm -f bankingClient
	rm -f bankingServer
