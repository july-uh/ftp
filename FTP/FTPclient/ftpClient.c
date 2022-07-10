#include<stdio.h>
#include "ftpClient.h"

char g_recvBuf[1024];

int main() {
	initSocket();

	connectToHost();

	closeSocket();

	return 0;
}

//��ʼ��socket��
bool initSocket() {
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		printf("WSAStartup faild:%d\n", WSAGetLastError());
		return false;
	}
	return true;
}
//�ر�socket��
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
//������Ϣ
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
	char fileName[1024] = "hello ���ǿͻ���";
	gets_s(fileName, 1023);				//��ȡҪ���ص��ļ�

	struct MsgHeader file = { .msgID = MsgFilename };
	strcpy(file.fileInfo.fileName, fileName);

	send(serfd, (char*)&file, sizeof(struct MsgHeader), 0);//+1��'\0'
}

void readyread(SOCKET serfd, struct MsgHeader* pmsg) {
	printf("size:%d filename:%s \n", pmsg->fileInfo.filesize, pmsg->fileInfo.fileName);
	//׼���ڴ� pmsg->fileInfo.filesize
	//�������� ���� MsgReady
		
}
