#pragma once
#include<WinSock2.h>	//网络库
#pragma comment(lib,"ws2_32.lib")	//库文件
#include<stdbool.h>
#define SPORT 8888	//服务器端口号

//定义标记
enum MsgTag {
	MsgFilename = 1,
	MsgOpenfileFailed = 6,

	MsgSize = 2,
	MsgReady = 3,
	MsgSend = 4,
	MsgSucceed = 5
};

//封装消息头
struct MsgHeader {	
	enum MsgTag msgID;	//消息类型
	struct {
		int filesize;		//文件大小
		char fileName[256];	//文件名
	}fileInfo;
};

//初始化socket库
bool initSocket();
//关闭socket库
bool closeSocket();
//监听客户端连接
void listenToClient();
//处理消息
bool processMsg(SOCKET);
//读取文件，获取文件大小
bool readFile(SOCKET, struct MsgHeader*);