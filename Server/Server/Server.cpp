// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include<stdio.h>
#include<exception>
#include<string.h>
#pragma commment(lib,"Ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable:4996)
using namespace std;
//初始化socket
int InitSock(BYTE minorVer = 2, BYTE majorVer = 2){
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if (WSAStartup(sockVersion, &wsaData) != 0)exit(0);//WSAStartup启动winsock编程软件
	return 1;
}
int TCP(){
	InitSock();
	SOCKET sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//sck为服务器监听的socket
	//建立socket,AF_INET为地址描述，SOCK_STREAM表示有序的基于连接的可靠的全双工的字节流，TCP表示协议类型
	if (sck == INVALID_SOCKET){
		printf("Failed socket, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Please input your port number: ");
	int port = 0;
	scanf("%d", &port);
	fflush(stdin);
	sockaddr_in Addr;//socket的地址形式
	Addr.sin_family = AF_INET; 
	Addr.sin_port = htons(port);//port number
	Addr.sin_addr.s_addr = INADDR_ANY;//可以接受任何ip的连接
	if (bind(sck, (LPSOCKADDR)&Addr, sizeof(Addr)) == SOCKET_ERROR){
		printf("Failed bind, error %d\n", WSAGetLastError());
		return 1;
	}
	if (listen(sck, 2) == SOCKET_ERROR){
		printf("Failed listen, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Server start!\n\n");
	while (true){
		sockaddr_in clientAddr;
		memset((void *)&clientAddr, 0, sizeof(clientAddr));
		int clientAddrLen = sizeof(clientAddr);
		SOCKET clientSck;
		printf("Waiting for client...\n");
		clientSck = accept(sck, (SOCKADDR*)&clientAddr, &clientAddrLen);
		printf("Accept a connection: %s\n", inet_ntoa(clientAddr.sin_addr));
		char RecvBuffer[256];//读写缓冲区大小为256
		char SendText[256];
		char bye[4] = "bye";
		try{
			while (true){
				if (clientSck == INVALID_SOCKET){
					printf("Failed accept, error %d\n", WSAGetLastError());
					return 1;
				}
				printf("\nWaiting for message...\n");
				int RecvLen = recv(clientSck, RecvBuffer, 256, 0);
				if (RecvLen > 0){
					RecvBuffer[RecvLen] = 0;
					printf("receive: %s\n", RecvBuffer);
					if (!strcmp(RecvBuffer, bye))
						break;
				}
				else{
					int error = WSAGetLastError();
					printf("Failed receive, error %d\n", WSAGetLastError());
					throw(WSAGetLastError());
				}

				printf("send: ");
				scanf("%[^\n]", SendText);//遇到换行才停止 可以输入空格
				fflush(stdin);
				if (send(clientSck, SendText, strlen(SendText), 0) == -1){
					printf("Failed send, error %d\n", WSAGetLastError());
					throw(WSAGetLastError());
				}
				if (!strcmp(SendText, bye))
					break;
				
			}
		}
		catch (int error){
			printf("exception: %d\n", error);
		}
		closesocket(clientSck);
		printf("\n");
		printf("Continue or not? input Y to continue and wait for another client\n");
		char ans[100];
		scanf("%s", ans);
		fflush(stdin);
		if (strcmp(ans, "Y") != 0)break;
	}
	closesocket(sck);
	WSACleanup();
	return 0;
}
int UDP(){
	InitSock();
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//sck为服务器监听的socket
	//建立socket,AF_INET为地址描述，SOCK_DGRAM表示无保障的面向消息的socket，TCP表示协议类型
	if (sck == INVALID_SOCKET){
		printf("Failed socket, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Please input your port number: ");
	int port=0;
	scanf("%d", &port);
	fflush(stdin);
	sockaddr_in Addr;//server的socket的地址形式
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(port);//port number
	Addr.sin_addr.s_addr = INADDR_ANY;//可以接受任何ip的连接
	if (bind(sck, (LPSOCKADDR)&Addr, sizeof(Addr)) == SOCKET_ERROR){
		printf("Failed bind, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Server start!\n");
	char RecvBuffer[256];//读写缓冲区大小为256
	char SendText[256];
	char bye[4] = "bye";
	sockaddr_in ClientAddr;
	int ClientAddrSize = sizeof(ClientAddr);
	while (true){
		try{
			while (true){
				printf("\nWaiting for message...\n");
				int RecvLen = recvfrom(sck, RecvBuffer, 256, 0, (sockaddr*)&ClientAddr, &ClientAddrSize);
				if (RecvLen > 0){
					RecvBuffer[RecvLen] = 0;
					printf("receive: %s\n", RecvBuffer);
					if (!strcmp(RecvBuffer, bye))
						break;
				}
				else{
					int error = WSAGetLastError();
					printf("Failed receive, error %d\n", WSAGetLastError());
					throw(WSAGetLastError());
				}
				printf("send: ");
				scanf("%[^\n]", SendText);//遇到换行才停止 可以输入空格
				fflush(stdin);
				if (sendto(sck, SendText, strlen(SendText), 0, (sockaddr*)&ClientAddr, ClientAddrSize) == -1){
					printf("Failed send, error %d\n", WSAGetLastError());
					throw(WSAGetLastError());
				}
				if (!strcmp(SendText, bye))
				{
					break;
				}

			}
		}
		catch (int error){
			printf("exception: %d\n", error);
		}
		printf("\n");
		printf("Continue or not? input Y to continue and wait for another client\n");
		char ans[100];
		scanf("%s", ans);
		fflush(stdin);
		if (strcmp(ans, "Y") != 0)break;
	}
	closesocket(sck);
	WSACleanup();
	return 0;
}
int main(){
	while (true){
		printf("\n**********************************************************************\n");
		printf("*                              Welcome                               *\n");
		printf("**********************************************************************\n");
		printf("\nPlease choose protocol, input:  1:UDP  2:TCP  Exit: stop the server\n");
		char choose[100];
		bool flag = false;
		scanf("%s", choose);
		fflush(stdin);
		if (!strcmp(choose, "1")){
			flag = true;
			UDP();
		}
		else if (!strcmp(choose, "2")){
			flag = true;
			TCP();
		}
		else if ((!strcmp(choose, "exit")) || (!strcmp(choose, "Exit")) || (!strcmp(choose, "EXIT")))
			break;
		if (!flag){
			printf("invalid input!\n");
		}
		printf("\n");
	}
	return 0;
}
	