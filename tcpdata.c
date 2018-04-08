#define WIN32_LEAN_AND_MEAN

#include "tcpdata.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "3072"

SOCKET ConnectSocket = INVALID_SOCKET;
FILE * BinFile = NULL;
BOOL use_network = TRUE;

int __cdecl tcpdata_init(PCSTR hostname, int port)
{
	use_network = strcmp(hostname, "file") == 0 ? FALSE : TRUE;

	if (use_network)
	{

		struct sockaddr_in serveraddr;

		WSADATA wsaData;

		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory((char *)&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		//memcpy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
		serveraddr.sin_addr.s_addr = inet_addr(hostname);
		serveraddr.sin_port = htons(port);

		/* connect: create a connection with the server */
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(ConnectSocket, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		{
			int LasrError = WSAGetLastError();

			printf("ERROR connecting");
			return 1;
		}
	}
	else
	{
		BinFile = fopen("gpssim.bin", "wb");
		if (!BinFile)
		{
			printf("Unable to open file!");
			return 2;
		}
	}
	return 0;

}

void tcpdata_close()
{
	if (use_network)
	{
		closesocket(ConnectSocket);
		WSACleanup();
	}
	else
	{
		if (BinFile != NULL)
		{
			fclose(BinFile);
			BinFile = NULL;
		}
	}
}
_Bool __cdecl tcpdata_send_and_wait(char *p, int size)
{
	if (tcpdata_send(p, size))
	{
		tcpdata_receive(p, size);
		return TRUE;
	}
	return FALSE;
}

_Bool __cdecl tcpdata_send(char *p, int size)
{
	if (use_network)
	{
		int iResult = send(ConnectSocket, p, size, 0);
		if (iResult == SOCKET_ERROR)
		{
			return FALSE;
		}
	}
	else
	{
		if (BinFile != NULL)
		{
			fwrite(p, size, 1, BinFile);
			fflush(BinFile);
		}
	}
	return TRUE;
}

int __cdecl tcpdata_receive(char *p, int size)
{
	int count = 0;
	if (use_network)
	{
		char rbuf[50];
		count = recv(ConnectSocket, rbuf, 50, MSG_WAITALL);
		if (count == SOCKET_ERROR)
		{
			return 0;
		}
	}
	else
	{
		if (BinFile != NULL)
		{
			count = fread(p, size, 1, BinFile);
		}
	}
	return count;
}
