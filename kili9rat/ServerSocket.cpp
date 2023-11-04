#include "pch.h"
#include "ServerSocket.h"

//程序初始化就初始化 销毁就销毁
//静态变量（首次被调用会初始化 一定是程序销毁才会销毁），全局静态变量在main被调用前会初始化 main结束会被析构

//CServerSocket server;
//这样就得到一个全局的m_instance 静态是可以全局访问的 所以可以直接类名加方法 非静态限制于this需要.或者指正才能调用
//无论类还是子类静态成员都是共用的
CServerSocket* CServerSocket::m_instance = NULL; //显式
CServerSocket::CHelper CServerSocket::m_helper;

CServerSocket* pserver = CServerSocket::getInstance(); //初始化