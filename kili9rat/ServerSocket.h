#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket
{

public:
	CServerSocket(){
		if (InitSockEnv() == FALSE) {
			MessageBox(NULL,_T("无法初始化套接字环境,请检查网络设置"),_T("初始化错误！"),MB_OK|MB_ICONERROR);
			exit(0);	//输出这个消息就是系统性问题 硬件故障了
		}
	};	//构造函数
	~CServerSocket(){
		WSACleanup();
	};	//析构函数
	BOOL InitSockEnv() {
		//sockt环境s
		WSADATA data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0){
			return FALSE;
		};  //TODO: 返回值处理
		return TRUE;

	};
};

extern CServerSocket server;	//声明一个外部的变量 其他的cpp就可以去用这个外部变量