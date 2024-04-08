#include <stdio.h>
#include <string.h>
#include <windows.h>

#define ERR(msg, ...) { printf("[!] " msg "\n", ##__VA_ARGS__); return EXIT_FAILURE; }

typedef struct ClientInfo {
	SOCKET client_socket;
	char *client_ip;
	int client_port;
} ClientInfo, *PClientInfo;

DWORD WINAPI clientThread(LPVOID param);

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
	if(
		bind(socket_open, (struct sockaddr*) &addr_server, sizeof(addr_server))
		== SOCKET_ERROR
	) ERR("Error binding port: %d", WSAGetLastError())
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
		PClientInfo cInfo = (PClientInfo) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PClientInfo));
		if (cInfo == NULL) ERR("Error occured while allocating memory: %lu\n", GetLastError());
		cInfo -> client_socket = socket_connected;
		cInfo -> client_ip = inet_ntoa(addr_client.sin_addr);
		cInfo -> client_port = addr_client.sin_port;
		printf("Client connected [%s:%d]\n", cInfo -> client_ip, cInfo -> client_port);

		// Reply to the client
		char *message = "Connected to server!\n";
		send(socket_connected, message, strlen(message), 0);

		// Create a thread for the client
		HANDLE hClient = CreateThread(NULL, 0, clientThread, (LPVOID) cInfo, 0, NULL);
		if (hClient == NULL) ERR("Error creating thread: %lu\n", GetLastError())
	}
	if (socket_connected == INVALID_SOCKET) ERR("Port accept failed with code: %d", WSAGetLastError())
	
	getchar();

	closesocket(socket_open);
	closesocket(socket_connected);
	WSACleanup();

	return EXIT_SUCCESS;
}

DWORD WINAPI clientThread(LPVOID param)
{
	PClientInfo cInfo;
	SOCKET sock_current;
	char *serverMessage, clientIn[1024];
	size_t msgsize;
	
	// Import params
	cInfo = (PClientInfo) param;
	sock_current = cInfo -> client_socket;

	// Send message
	serverMessage = "Thread created\n";
	send(sock_current, serverMessage, strlen(serverMessage), 0);

	// Recieve message from client
	while
	(
		(msgsize = recv(sock_current, clientIn, 1024, 0))
		> 0
	)
	{
		// send(sock_current, clientIn, strlen(clientIn), 0);
		printf("[%s:%d]: %s\n", cInfo -> client_ip, cInfo -> client_port, clientIn);
	}

	// Error handling
	if (msgsize == 0)
	{
		printf("Client [%s:%d] disconnected!\n", cInfo -> client_ip, cInfo -> client_port);

	}
	else if (msgsize == -1)
	{
		printf("Failed to read message from client [%s:%d]!\n", cInfo -> client_ip, cInfo -> client_port);
	}

	free(cInfo);

	return EXIT_SUCCESS;
}