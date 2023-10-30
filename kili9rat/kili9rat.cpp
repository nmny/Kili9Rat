// kili9rat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "kili9rat.h"
#include "ServerSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {   //1 进度的可能 2 项目对接方便 3 可行性评估，提早暴露风险
            //服务器 > 套接字:socket、bind、listen、accept、read、write、close
            // 套接字初始化
            //server; //ServerSocket 这个全局变量就可以拿来用了
            {
                CServerSocket local;
            }
            //TODO: 在此处为应用程序的行为编写代码。
            SOCKET serv_sock = socket(PF_INET, SOCK_STREAM,0);
            //TODO: 校验是否创建成功
            //准备地址
            sockaddr_in serv_adr, client_adr;
            memset(&serv_adr, 0, sizeof(serv_adr)); //serv_adr初始化内存清0
            serv_adr.sin_family = AF_INET;   //使用地址簇
            //serv_adr.sin_addr.S_un.S_addr 可以使用s_addr宏来代替
            serv_adr.sin_addr.s_addr = INADDR_ANY;  //监听服务器上的所有IP(可能多个网卡多个IP)
            serv_adr.sin_port = htons(9527);   

            //绑定
            bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr));    //强制类型转换
            //TOFO: 返回值处理
            //前期准备工作
            listen(serv_sock, 1); //控制1对1
            char buffer[1024];
            int cli_sz = sizeof(client_adr);
            //SOCKET client = accept(serv_sock, (sockaddr*)&client_adr, &cli_sz);
            //recv(client, buffer, sizeof(buffer), 0); //windows接收 发送
            //send(client, buffer, sizeof(buffer), 0);
            closesocket(serv_sock);
            WSACleanup();   //清理网络WSAStartup WSACleanup成对出现
            //程序初始化就初始化 销毁就销毁
            //静态变量（首次被调用会初始化 一定是程序销毁才会销毁），全局静态变量在main被调用前会初始化 main结束会被析构


        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
