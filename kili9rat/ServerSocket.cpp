#include "pch.h"
#include "ServerSocket.h"

//程序初始化就初始化 销毁就销毁
//静态变量（首次被调用会初始化 一定是程序销毁才会销毁），全局静态变量在main被调用前会初始化 main结束会被析构

CServerSocket server;
