// Client program

#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <thread>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEF_PORT "27015"
#define BUFFER_LEN 512

int main()
{
	char* sendBuffer = "Testing ths shit";
	char recvBuffer[BUFFER_LEN];

	WSADATA sockData;
	
	//Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &sockData))
		return 1;
	
	struct addrinfo* result = 0, *ptr = 0, hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Resolve address and port
	if (getaddrinfo("Name", DEF_PORT, &hints, &result))
	{
		WSACleanup();
		return 2;
	}

	//Create Socket
	SOCKET connection = INVALID_SOCKET;
	ptr = result;	//get the first address returned by getaddrinfo

	connection = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);	//Create a socket for connection to server

	if (connection == INVALID_SOCKET)
	{
		WSACleanup();
		return 3;
	}

	//Connect to server
	if (connect(connection, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
	{
		closesocket(connection);
		WSACleanup();
		return 4;
	}
	
	freeaddrinfo(result);

	//Send initial data
	if (send(connection, sendBuffer, strlen(sendBuffer), 0) == SOCKET_ERROR)
	{
		closesocket(connection);
		WSACleanup();
		return 5;
	}

	int res = 0;

	res = shutdown(connection, SD_SEND);

	if (res == SOCKET_ERROR)
	{
		closesocket(connection);
		WSACleanup();
		return 6;
	}

	while (res > 0)
	{
		res = recv(connection, recvBuffer, BUFFER_LEN, 0);
		if (res > 0)
		{
			printf("Bytes received: %d\n", res);
		}
		else if (res == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
		}
	}

	closesocket(connection);
	WSACleanup();

	return 0;
}