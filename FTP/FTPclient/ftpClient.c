#include<stdio.h>
#include "ftpClient.h"
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

//�����ͻ�������
void connectToHost() {
	/*
	����socket�׽���
	*/
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == serfd) {
		printf("socket faild:%d\n", WSAGetLastError());
		return;
	}

	/*
	��socket��IP��ַ���˿ں�
	*/
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	//Э������ ����ʹ���socketָ����һ��
	serAddr.sin_port = htons(SPORT);
	//�˿ں� ���ڴ�˱�ַ��С�˱�ַ��Ӱ�죬��Ҫ�ѱ����ֽ���ת��Ϊ�����ֽ���
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //��������ip��ַ ���ػػ� ��Ҫ�ѵ��ʮ����תΪ2����
	
	//���ӵ�������
	if (0 != connect(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		printf("connect faild:%d\n", WSAGetLastError());
		return;
	}

	//��ʼ������Ϣ
	while (processMsg(serfd)) {
		Sleep(5000);
	}
}
//������Ϣ
bool processMsg(SOCKET serfd) {
	printf("hello !\n");
	char sendBuf[1024] = "hello ���ǿͻ���";
	send(serfd, sendBuf, strlen(sendBuf)+1, 0);//+1��'\0'
	return true;
}