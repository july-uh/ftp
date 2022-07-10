#include<stdio.h>
#include "ftpServer.h"

char g_recvBuf[1024];//�������տͻ��˷��͵���Ϣ

int main() {
	initSocket();

	listenToClient();

	closeSocket();

	return 0;
}

//��ʼ��socket��
bool initSocket() {
	WSADATA wsadata;
	//���첽�׽���
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {//Э��汾,�������
		printf("WSAStartup faild:%d\n", WSAGetLastError());	//���ߡ������룺���Ը��ݴ������ѯ����
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
void listenToClient() {
	/*
	����socket�׽���
		ϵͳ�ں���Ϊ���Ƿ�����ڴ�ռ䣬���е�ַ���˿ںŵ�
	���ص����޷�������
		INVALID_SOCKET SOCKET_ERROR
	*/
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����ѡ���ipv4,��ʽ�׽��֣�tcp
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
	serAddr.sin_addr.S_un.S_addr = ADDR_ANY;
	//ip��ַ ����ѡ����Ǽ���������������
	if (0 != bind(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		printf("bind faild:%d\n", WSAGetLastError());
		return;
	}

	/*
	�����ͻ�������
	*/
	if (0 != listen(serfd, 10)) {//�ڶ��������Ƕ��г���
		printf("listen faild:%d\n", WSAGetLastError());
		return;
	}
	//����пͻ������ӣ���������
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	SOCKET clifd = accept(serfd, (struct sockaddr*)&cliAddr, &len);//���Ի�ÿͻ��˵�ip�Ͷ˿ں�
	if (INVALID_SOCKET == clifd) {
		printf("accept faild:%d\n", WSAGetLastError());
		return;
	}
	//��ʼ������Ϣ
	while (processMsg(clifd)) {
		Sleep(5000);
	}
}
//������Ϣ
bool processMsg(SOCKET clifd) {
	//�ɹ�������Ϣ�����ؽ��ܵ����ֽ���������ʧ�ܷ���0
	int nRes = recv(clifd, g_recvBuf, 1023, 0);
	if (nRes <= 0) {
		printf("�ͻ�������...%d\n", WSAGetLastError());
	}
	else {
		printf("%s\n", g_recvBuf);
	}
	return true;
}
