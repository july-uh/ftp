#pragma once
#include<WinSock2.h>	//�����
#pragma comment(lib,"ws2_32.lib")	//���ļ�
#include<stdbool.h>
#define SPORT 8888	//�������˿ں�

//��ʼ��socket��
bool initSocket();
//�ر�socket��
bool closeSocket();
//�����ͻ�������
void connectToHost();
//������Ϣ
bool processMsg(SOCKET);