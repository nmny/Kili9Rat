#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket {
public:
    // 静态函数，用于获取单例实例
    static CServerSocket* getInstance() {
        //m_instance == NULL
        if (!m_instance) {//静态函数没有this指针 所以无法直接访问成员变量 需要成员变量也为静态
            m_instance = new CServerSocket;
        }
        return m_instance;
    };

    //socket初始化
    bool InitSocket() {

        if (m_sock == -1)return false;

        //准备地址
        sockaddr_in serv_adr;
        memset(&serv_adr, 0, sizeof(serv_adr)); //serv_adr初始化内存清0
        serv_adr.sin_family = AF_INET;   //使用地址簇
        //serv_adr.sin_addr.S_un.S_addr 可以使用s_addr宏来代替
        serv_adr.sin_addr.s_addr = INADDR_ANY;  //监听服务器上的所有IP(可能多个网卡多个IP)
        serv_adr.sin_port = htons(9527);

        //绑定 强制类型转换
        if (bind(m_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
            return false;

        }
        //控制1对1
        if (listen(m_sock, 1) == -1) {
            return false;

        }
        return true; //socket初始化完成
    }

    //client初始化
    bool AccepClient() {
        sockaddr_in client_adr;
        int cli_sz = sizeof(client_adr);

        if (m_client = accept(m_sock, (sockaddr*)&client_adr, &cli_sz) == -1)return false;
        return true;
    }


    int DealCommand() {
        if (m_client == -1)return false;

        char buffer[1024] = "";
        while (true)
        {
            int len = recv(m_client, buffer, sizeof(buffer), 0);
            if (len <= 0) {
                return -1;
            }
        }
        //TODO 处理命令

    }

    bool Seed(const char* pData, int nSize) {
        return send(m_client, pData, nSize, 0) > 0;
    }


    //单例
private:
    // 防止拷贝构造 既不能赋值也不能让外部调用构造
    CServerSocket& operator=(const CServerSocket& ss) {};
    //复制构造函数  
    CServerSocket(const CServerSocket& ss) {
        m_sock = ss.m_sock;
        m_client = ss.m_client;    //初始化
    };
    SOCKET m_client;
    SOCKET m_sock;
    //构造函数
    CServerSocket() {
        m_client = INVALID_SOCKET; //-1

        if (InitSockEnv() == FALSE) {
            MessageBox(NULL, _T("无法初始化套接字环境,请检查网络设置"), _T("初始化错误！"), MB_OK | MB_ICONERROR);
            exit(0);    //输出这个消息就是系统性问题 硬件故障了
        }
        //serv_sock > member
        //创建套接字
        m_sock = socket(PF_INET, SOCK_STREAM, 0);
    };    //构造函数

    BOOL InitSockEnv() {
        //sockt环境
        WSADATA data;
        if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
            return FALSE;
        };  //TODO: 返回值处理
        return TRUE;
    }

    //拿 声明单例实例指针
    static CServerSocket* m_instance;


    ~CServerSocket() {
        closesocket(m_sock);
        WSACleanup();
        //清理网络WSAStartup WSACleanup成对出现
    };    //析构函数

    static void releaseInstance() {
        if (m_instance != NULL) {
            CServerSocket* tmp = m_instance;
            m_instance = NULL;
            delete tmp;
        }
    }
    //析构调用
    class CHelper {
    public:
        CHelper() {
            CServerSocket::getInstance();
        };
        ~CHelper() {
            CServerSocket::releaseInstance();
        };
    };
    static CHelper m_helper;
};

extern CServerSocket server;    //声明一个外部的变量 其他的cpp就可以去用这个外部变量