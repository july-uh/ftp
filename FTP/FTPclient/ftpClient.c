#include<stdio.h>
#include "ftpClient.h"

char g_recvBuf[1024];

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

void connectToHost() {
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serfd) {
		printf("socket faild:%d\n", WSAGetLastError());
		return;
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(SPORT);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	
	if (0 != connect(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		printf("connect faild:%d\n", WSAGetLastError());
		return;
	}

	while (processMsg(serfd)) {
		Sleep(5000);
	}
}
//处理消息
bool processMsg(SOCKET serfd) {

	downloadFileName(serfd);

	recv(serfd, g_recvBuf, 1023, 0);
	struct MsgHeader* msg = (struct MsgHeader*)g_recvBuf;
	switch (msg->msgID) {
	case MsgOpenfileFailed:
		downloadFileName(serfd);
		break;
	case MsgSize:
		readyread(serfd, msg);

		break;
	}

	return true;
}

void downloadFileName(SOCKET serfd) {
	char fileName[1024] = "hello 我是客户端";
	gets_s(fileName, 1023);				//获取要下载的文件

	struct MsgHeader file = { .msgID = MsgFilename };
	strcpy(file.fileInfo.fileName, fileName);

	send(serfd, (char*)&file, sizeof(struct MsgHeader), 0);//+1是'\0'
}

void readyread(SOCKET serfd, struct MsgHeader* pmsg) {
	printf("size:%d filename:%s \n", pmsg->fileInfo.filesize, pmsg->fileInfo.fileName);
	//准备内存 pmsg->fileInfo.filesize
	//给服务器 发送 MsgReady
		
}
