#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

unsigned int __stdcall  ClientThread(void *data);

WSADATA wsaData;
int iResult;
sockaddr_in addr;
SOCKET sConnect, sClient;
char * Name;

int _tmain(int argc, _TCHAR* argv[])
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2222);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

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

	iResult = connect(sConnect, (SOCKADDR*)&addr, sizeof(sockaddr_in));
	if (iResult)
	{
		printf("Connect failed %u", WSAGetLastError());
		closesocket(sConnect);
		return 0;
	}
	//// autorization
	Name = new char[256];
	printf("Your Name: ");
	scanf("%s", Name);

	iResult = send(sConnect, Name, 256, NULL);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(sConnect);
		WSACleanup();
		return 1;
	}
	////////////

	char *message = new char[256];
	ZeroMemory(message, 256);
	recv(sConnect, message, 256, NULL);

	printf("%s %s", message, Name);

	printf("Press enter to continue\n"); getchar();
	system("cls");

	printf("let's chat %s\n", Name);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientThread, 0, 0, 0);

	while (true)
	{
		char * msg = new char[256];
		ZeroMemory(msg, 256);
		fgets(msg, 256, stdin);

		iResult = send(sConnect, msg, 256, NULL);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(sConnect);
			WSACleanup();
			return 1;
		}
		Sleep(100);
	}

	closesocket(sConnect);
	WSACleanup();
	return 0;
}

unsigned int __stdcall ClientThread(void *)
{
	char * buffer = new char[256];
	int size = 0;
	while (true)
	{
		ZeroMemory(buffer, 256);
		if (size = recv(sConnect, buffer, 256, NULL) > 0)
		{
			puts(buffer);
		}
		Sleep(100);
	}

	return 0;
}