#pragma once
#include<WinSock2.h>	//�����
#pragma comment(lib,"ws2_32.lib")	//���ļ�
#include<stdbool.h>
#define SPORT 8888	//�������˿ں�

//������
enum MsgTag {
	MsgFilename = 1,
	MsgOpenfileFailed = 6,

	MsgSize = 2,
	MsgReady = 3,
	MsgSend = 4,
	MsgSucceed = 5
};

//��װ��Ϣͷ
struct MsgHeader {
	enum MsgTag msgID;	//��Ϣ����
	struct {
		int filesize;		//�ļ���С
		char fileName[256];	//�ļ���
	}fileInfo;
};

//��ʼ��socket��
bool initSocket();
//�ر�socket��
bool closeSocket();

void connectToHost();
//������Ϣ
bool processMsg(SOCKET);

void downloadFileName(SOCKET);
void readyread(SOCKET, struct MsgHeader*);

