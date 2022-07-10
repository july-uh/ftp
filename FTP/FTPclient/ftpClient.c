#include<stdio.h>
#include "ftpClient.h"
int main() {
	initSocket();

	connectToHost();

	closeSocket();

	return 0;
}

//初始化socket库
bool initSocket() {
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		printf("WSAStartup faild:%d\n", WSAGetLastError());
		return false;
	}
	return true;
}
//关闭socket库
bool closeSocket() {
	if (0 != WSACleanup()) {
		printf("WSACleanup faild:%d\n", WSAGetLastError());
		return false;
	}
	return true;
}

//监听客户端连接
void connectToHost() {
	/*
	创建socket套接字
	*/
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serfd) {
		printf("socket faild:%d\n", WSAGetLastError());
		return;
	}

	/*
	给socket绑定IP地址、端口号
	*/
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	//协议类型 必须和创建socket指定的一样
	serAddr.sin_port = htons(SPORT);
	//端口号 由于大端编址和小端编址的影响，需要把本地字节序转换为网络字节序
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //服务器的ip地址 本地回环 需要把点分十进制转为2进制
	
	//链接到服务器
	if (0 != connect(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		printf("connect faild:%d\n", WSAGetLastError());
		return;
	}

	//开始处理消息
	while (processMsg(serfd)) {
		Sleep(5000);
	}
}
//处理消息
bool processMsg(SOCKET serfd) {
	printf("hello !\n");
	char sendBuf[1024] = "hello 我是客户端";
	send(serfd, sendBuf, strlen(sendBuf)+1, 0);//+1是'\0'
	return true;
}