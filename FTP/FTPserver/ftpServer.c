#include<stdio.h>
#include "ftpServer.h"

char g_recvBuf[1024];//用来接收客户端发送的消息
int g_fileSize;	 //文件大小
int main() {
	initSocket();

	listenToClient();

	closeSocket();

	return 0;
}

//初始化socket库
bool initSocket() {
	WSADATA wsadata;
	//打开异步套接字
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {//协议版本,传输参数
		printf("WSAStartup faild:%d\n", WSAGetLastError());	//工具→错误码：可以根据错误码查询错误
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
void listenToClient() {
	/*
	创建socket套接字
		系统内核中为我们分配的内存空间，存有地址、端口号等
	返回的是无符号整型
		INVALID_SOCKET SOCKET_ERROR
	*/
	SOCKET serfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//这里选择的ipv4,流式套接字，tcp
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
	serAddr.sin_addr.S_un.S_addr = ADDR_ANY;
	//ip地址 这里选择的是监听本机所有网卡
	if (0 != bind(serfd, (struct sockaddr*)&serAddr, sizeof(serAddr))) {
		printf("bind faild:%d\n", WSAGetLastError());
		return;
	}

	/*
	监听客户端链接
	*/
	if (0 != listen(serfd, 10)) {//第二个参数是队列长度
		printf("listen faild:%d\n", WSAGetLastError());
		return;
	}
	//如果有客户端连接，接受链接
	struct sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	SOCKET clifd = accept(serfd, (struct sockaddr*)&cliAddr, &len);//可以获得客户端的ip和端口号
	if (INVALID_SOCKET == clifd) {
		printf("accept faild:%d\n", WSAGetLastError());
		return;
	}
	//开始处理消息
	while (processMsg(clifd)) {
		Sleep(5000);
	}
}

//处理消息
bool processMsg(SOCKET clifd) {
	//成功接收消息，返回接受到的字节数，接受失败返回0
	int nRes = recv(clifd, g_recvBuf, 1023, 0);
	if (nRes <= 0) {
		printf("客户端下线...%d\n", WSAGetLastError());
	}

	//获取接收到的消息
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

//读取文件，获取文件大小
bool readFile(SOCKET clifd, struct MsgHeader* pmsg) {
	FILE* pread = fopen(pmsg->fileInfo.fileName, "rb");
	if (pread == NULL) {
		printf("找不到[%s]文件...\n",pmsg->fileInfo.fileName);
		struct MsgHeader msg = { .msgID = MsgOpenfileFailed };
		if (SOCKET_ERROR == send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0)) {
			printf("send failed:%d\n", WSAGetLastError());
		}
		return false;
	}
	//如果文件读取成功，则获取文件大小
	fseek(pread, 0, SEEK_END);
	g_fileSize = ftell(pread);
	fseek(pread, 0, SEEK_SET);
	
	//把文件大小发给客户端
	struct MsgHeader msg = { .msgID = MsgSize,.fileInfo.filesize = g_fileSize };
	char tfname[200] = { 0 }, text[100];
	_splitpath(pmsg->fileInfo.fileName, NULL, NULL, tfname, text);
	strcat(tfname, text);
	strcpy(msg.fileInfo.fileName, tfname);

	send(clifd, (char*)&msg, sizeof(struct MsgHeader), 0);

	//发送文件

	fclose(pread);
}