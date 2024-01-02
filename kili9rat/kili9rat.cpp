// kili9rat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "kili9rat.h"
#include "ServerSocket.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment (linker, "/subsystem;windows /entry:WinMainCRTStartup") //子系统 入口函数
//#pragma comment (linker, "/subsystem;windows /entry:mainCRTStartup") //调用main函数
//#pragma comment (linker, "/subsystem;console /entry:mianCRTStartup") //纯后台
//#pragma comment (linker, "/subsystem;console /entry:WinMainCRTStartup")

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;
void Dump(BYTE* pData, size_t nSize){
    std::string strOut;
    for (size_t i = 0; i < nSize; i++)
    {
        char buf[8] = "";
        if (i > 0 && (i % 16 == 0))strOut += "\n";
        snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xFF);//这里取0xFF防止负数所产生意向外的影响
        strOut += buf;
    }
    strOut += "\n";
    OutputDebugStringA(strOut.c_str());
}

//获取磁盘分区信息
int MakeDriverInfo()       //1==>A 2==>B 3==>C...26==>Z
{
    std::string result;
    for (int i = 1; i <= 26; i++) {
        //返回0是代表切换成功
        if (_chdrive(i) == 0) { 
            if (result.size() > 0) result += ',';
            result += 'A' + i - 1;
        }
    }
    //发送命令 需要先封包   CPacket封解包
    CPacket pack(1, (BYTE*)result.c_str(), result.size());
    Dump((BYTE*)pack.Data(),pack.Size());
    //CServerSocket::getInstance()->Send(pack);
    return 0;
}

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
                //CServerSocket local;
                //单例 > 规范规定 语法规定 硬件规定
                //CServerSocket::getInstance();

            };
            //TODO: 在此处为应用程序的行为编写代码。
            //程序初始化就初始化 销毁就销毁
            //静态变量（首次被调用会初始化 一定是程序销毁才会销毁），全局静态变量在main被调用前会初始化 main结束会被析构
            //pserver(服务器创建单例) > InitSocket(初始化套接字) > AccepClient(获取客服端) > DealCommand(处理命令)
            //CServerSocket* pserver = CServerSocket::getInstance();
            //int count = 0;
            //if (pserver->InitSocket() == false) {
            //    MessageBox(NULL, _T("网络初始化异常未能成功初始化，请检查网络状态"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
            //    exit(0);
            //}
            //while (CServerSocket::getInstance() != NULL) {
            //    if (pserver->AccepClient() == false) {
            //        if (count >= 3) {
            //            MessageBox(NULL, _T("多次重试无非正常接入网络，结束程序!"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
            //            exit(0);
            //        }
            //        MessageBox(NULL, _T("无非正常接入网络，自动重试"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
            //        count++;
            //    }
            //    int ret = pserver->DealCommand();
            //    //TODO
            //}

            //实现功能
            int nCmd = 1;
            switch (nCmd){
            case 1: //查看磁盘分区
            default:
                MakeDriverInfo();
                break;
            }
            

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
