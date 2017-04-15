// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include<stdio.h>
#include<exception>
#include<string.h>
#include<string>
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
bool ifValid(string ip){
	int len = ip.length();
	int cnt = 0;
	int last = 0;
	for (int i = 0; i < len; i++){
		if (ip[i] == '.'){
			string tmp = ip.substr(last, (i - last + 1));
			last = i;
			cnt++;
			int num = atoi(tmp.c_str());
			if (num>255 || num < 0)return false;
		}
	}
	if (cnt != 3)return false;
	return true;
}
int TCP(){
	InitSock();
	SOCKET sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//建立socket,AF_INET为地址描述，SOCK_STREAM表示有序的基于连接的可靠的全双工的字节流，TCP表示协议类型
	if (sck == INVALID_SOCKET){
		printf("Failed socket, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Please input server's IP address: ");
	char ipAddr[100];
	scanf("%s", ipAddr);
	fflush(stdin);
	string ip = ipAddr;
	if (!ifValid(ip)){
		printf("Invalid IP\n");
		return 1;
	}
	printf("Please input server's port number: ");
	int port = 0;
	scanf("%d", &port);
	fflush(stdin);
	printf("\n");
	sockaddr_in servAddr;//socket的地址形式
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);//port number
	servAddr.sin_addr.S_un.S_addr = inet_addr(ipAddr);//ip address
	int connectres = connect(sck, (sockaddr*)&servAddr, sizeof(servAddr));
	if (connectres == -1){
		printf("Failed connect, error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Connect to server!\n");
	char RecvBuffer[256];//读写缓冲区大小为256
	char SendText[256];
	char bye[4] = "bye";
	try{
		while (true){
			printf("send: ");
			scanf("%[^\n]", SendText);
			fflush(stdin);//清空缓冲区
			if (send(sck, SendText, strlen(SendText), 0) == -1){
				printf("Failed send, error %d\n", WSAGetLastError());
				throw(WSAGetLastError());
			}
			if (!strcmp(SendText, bye))
				break;
			printf("\nWaiting for message...\n");
			int RecvLen = recv(sck, RecvBuffer, 256, 0);
			if (RecvLen > 0){
				RecvBuffer[RecvLen] = 0;
				printf("receive: %s\n", RecvBuffer);
				if (!strcmp(RecvBuffer, bye))
					break;
			}
			else{
				printf("Failed receive, error %d\n", WSAGetLastError());
				throw(WSAGetLastError());
			}
		}
	}
	catch (int error){
		printf("exception: %d\n", error);
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
	printf("Please input server's IP address: ");
	char ipAddr[100];
	scanf("%s", ipAddr);
	fflush(stdin);
	string ip = ipAddr;
	if (!ifValid(ip)){
		printf("Invalid IP\n");
		return 1;
	}
	printf("Please input server's port number: ");
	int port = 0;
	scanf("%d", &port);
	fflush(stdin);
	printf("\n");
	sockaddr_in ServerAddr;//server的socket的地址形式
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);//port number
	ServerAddr.sin_addr.s_addr = inet_addr(ipAddr);//可以接受任何ip的连接
	int ServerAddrSize = sizeof(ServerAddr);
	char RecvBuffer[256];//读写缓冲区大小为256
	char SendText[256];
	char bye[4] = "bye";
	try{
		while (true){
			printf("send: ");
			scanf("%[^\n]", SendText);//遇到换行才停止 可以输入空格
			fflush(stdin);
			if (sendto(sck, SendText, strlen(SendText), 0, (sockaddr*)&ServerAddr, ServerAddrSize) == -1){
				printf("Failed send, error %d\n", WSAGetLastError());
				throw(WSAGetLastError());
			}
			if (!strcmp(SendText, bye))
				break;
			printf("\nWaiting for message...\n");
			int RecvLen = recvfrom(sck, RecvBuffer, 256, 0, (sockaddr*)&ServerAddr, &ServerAddrSize);
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
		}
	}
	catch (int error){
		printf("exception: %d\n", error);
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
		printf("\nPlease choose protocol, input:  1:UDP  2:TCP  Exit: stop the client\n");
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
	}
	return 0;
}
