#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <winsock2.h>
#include <process.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib" )

unsigned int __stdcall  ServsClient(void *data);

SOCKET sConnect, sClient, *sConnections;
WSADATA wsaData;
int iResult, counter = 0;
sockaddr_in addr;
char * Name[64];

int _tmain(int argc, _TCHAR* argv[])
{

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2222);
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.20.217");

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 

	if (iResult)
	{
		printf("WSA startup failed");
		return 0;
	}

	sConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sConnect == INVALID_SOCKET)
	{
		printf("Invalid socket");
		WSACleanup();
		return 0;
	}

	puts("Server Started..");

	iResult = bind(sConnect, (sockaddr*)&addr, sizeof(sockaddr_in));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed %u", GetLastError());
		closesocket(sConnect);
		WSACleanup();
		return 0;
	}

	iResult = listen(sConnect, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("iResult failed %u", GetLastError());
		closesocket(sConnect);
		WSACleanup();
		return 0;
	}

	sConnections = (SOCKET *)calloc(64, sizeof(SOCKET));
	while (sClient = accept(sConnect, 0, 0))
	{
		if (sClient == INVALID_SOCKET)
		{
			printf("invalid client socket", GetLastError());
			continue;
		}
		sConnections[counter] = sClient;
		Name[counter] = new char[256];
		ZeroMemory(Name[counter], 256);
		recv(sConnections[counter], Name[counter], 256, NULL);

		int sendRes = send(sConnections[counter], "Welcome", 256, NULL);
		if (sendRes == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(sConnections[counter]);
			WSACleanup();
			return 1;
		}

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ServsClient, (void *)counter, 0, 0);
		counter++;
	}

	for (int i = 0; i < counter; i++)
	{
		iResult = shutdown(sConnections[i], SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(sConnections[i]);
			WSACleanup();
			return 1;
		}
	}


	return 0;
}

unsigned int __stdcall ServsClient(void * p)
{
	int id = (int)p;
	char buffer[256];
	char * message = new char[256];
	int size = 0;
	while (true)
	{
		ZeroMemory(buffer, 256);
		if (size = recv(sConnections[id], message, 256, NULL) > 0)
		{
			for (int i = 0; i < counter; i++)
			{
				if (i == id)
				{
					ZeroMemory(buffer, 256);
					sprintf_s(buffer, "%s: %s", Name[i], message);
					puts(buffer);

					ZeroMemory(buffer, 256);
					sprintf_s(buffer, "You: %s", message);
					send(sConnections[i], buffer, 256, NULL);
				}
				else
				{
					ZeroMemory(buffer, 256);
					sprintf_s(buffer, "%s: %s", Name[id], message);
					send(sConnections[i], buffer, 256, NULL);
				}
			}
		}
	}

	return 0;
}