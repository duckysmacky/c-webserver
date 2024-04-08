#include <stdio.h>
#include <string.h>
#include <windows.h>

#define ERR(msg, ...) { printf("[!] " msg "\n", ##__VA_ARGS__); return EXIT_FAILURE; }

int main(int argc, const char *argv[])
{
	WSADATA wsaData;
	SOCKET socket_open, socket_connected;
	struct sockaddr_in addr_server, addr_client;
	
    // Loding winsock library
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) ERR("Error occured while initialising Winsock: %d", WSAGetLastError())

    // Creating a socket of IPv4, connection-oriented, TCP protocol
    socket_open = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_open == INVALID_SOCKET) ERR("Error occured while creating a socket: %d", WSAGetLastError());

    // Setting up server
	addr_server.sin_addr.s_addr = INADDR_ANY; // target address -> ANY (VOID)
	addr_server.sin_family = AF_INET; // type -> ipv4
	addr_server.sin_port = htons(8888); // port -> 8888

	// Binding port
	if (bind(socket_open, (struct sockaddr*) &addr_server, sizeof(addr_server)) == SOCKET_ERROR) ERR("Error binding port: %d", WSAGetLastError())
	printf("Port binded successfully!\n");

	// Set socket to listen
	listen(socket_open, 3);

	// Accept and incoming connection
	printf("Waiting for incoming connections...\n");
	int addrlen = sizeof(struct sockaddr_in);
	while
	(
		(socket_connected = accept(socket_open, (struct sockaddr *) &addr_client, &addrlen))
		!= INVALID_SOCKET
	)
	{
		// Get client info
		char *clientIp = inet_ntoa(addr_client.sin_addr);
		u_short clientPort = addr_client.sin_port;
		printf("Client connected [%s:%d]\n", clientIp, clientPort);

		//Reply to the client
		char *welcomeMessage = "Connected to server!\n";
		send(socket_connected , welcomeMessage , strlen(welcomeMessage) , 0);
	}
	if (socket_connected == INVALID_SOCKET) ERR("Port accept failed with code: %d", WSAGetLastError())
	
	getchar();
	closesocket(socket_open);
	closesocket(socket_connected);
	WSACleanup();
	return EXIT_SUCCESS;
}