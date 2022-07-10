#include<stdio.h>
#include "ftpServer.h"

char g_recvBuf[1024];//�������տͻ��˷��͵���Ϣ
int g_fileSize;	 //�ļ���С
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

	//��ȡ���յ�����Ϣ
	struct MsgHeader* msg = (struct MsgHeader*)g_recvBuf;
	switch (msg -> msgID) {
	case MsgFilename:
		printf("%s\n", msg->fileInfo.fileName);
		readFile(clifd, msg);
	//case MsgSize:

	//case :
	defalt:
		break;
	}

	return true;
}

//��ȡ�ļ�����ȡ�ļ���С
bool readFile(SOCKET clifd, struct MsgHeader* pmsg) {
	FILE* pread = fopen(pmsg->fileInfo.fileName, "rb");
	if (pread == NULL) {
		printf("�Ҳ���[%s]�ļ�...\n",pmsg->fileInfo.fileName);
		struct MsgHeader msg = { .msgID = MsgOpenfileFailed };
		if (SOCKET_ERROR == send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0)) {
			printf("send failed:%d\n", WSAGetLastError());
		}
		return false;
	}
	//����ļ���ȡ�ɹ������ȡ�ļ���С
	fseek(pread, 0, SEEK_END);
	g_fileSize = ftell(pread);
	fseek(pread, 0, SEEK_SET);
	
	//���ļ���С�����ͻ���
	struct MsgHeader msg = { .msgID = MsgSize,.fileInfo.filesize = g_fileSize };
	char tfname[200] = { 0 }, text[100];
	_splitpath(pmsg->fileInfo.fileName, NULL, NULL, tfname, text);
	strcat(tfname, text);
	strcpy(msg.fileInfo.fileName, tfname);

	send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0);

	//�����ļ�

	fclose(pread);
}