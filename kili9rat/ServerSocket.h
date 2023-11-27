#pragma once
#include "pch.h"
#include "framework.h"

class CPacket
{
public:
    CPacket():sHead(0), nLength(0), sCmd(0), sSum(0) {  //成员变量初始化
    }
    CPacket(WORD nCmd, const BYTE* pData, size_t nSize) { //用来封包
        sHead = 0XFEFF;
        nLength = nSize + 2 + 2;  //nLength strData+sCmd+sSum
        sCmd = nCmd;
        strData.resize(nSize);
        memcpy((void*)strData.c_str(), pData, nSize);
        sSum = 0;

    };
    CPacket(const CPacket& pack) {//赋值构造函数用来CPacket的赋值传递
        sHead = pack.sHead;
        nLength = pack.nLength;
        sCmd = pack.sCmd;
        strData = pack.strData;
        sSum = pack.sSum;
    }

    CPacket(const BYTE* pData, size_t& nSize) { //用来解析包数据
        size_t i = 0;
        for (; i < nSize; i++) {
            if(*(WORD*)(pData+i) == 0xFEFF){ //*(WORD*)：这部分是一个类型强制转换。它将pData中偏移i处的内存解释为一个WORD类型（通常是2个字节的整数），并通过*操作符来获得该     内存位置上的值。
                sHead = *(WORD*)(pData + i);
                i += 2; //未找到头有效退出 假如nSize里面有2bit(FEFF) 如果i没有+2,i=0,nSize=2 后面没有数据了再去解析长度程序就会崩溃 
                break;
            }
        }
        //+4长度 +2命令 +2和校验
        if (i + 4 + 2 + 2 >= nSize) {//包数据可能不全，或者包头未能完全接收到   
            nSize = 0;
            return; }
        //有数据继续往下读
        nLength = *(DWORD*)(pData + i); i += 4;
        if (nLength + i > nSize) {//包未完全接收到就返回，解析失败
            nSize = 0;
            return;
        }
        sCmd = *(WORD*)(pData + i); i + 2;
        if(nLength > 4)
        {
            strData.resize(nLength - 2 - 2); //sCmd到sSum
            memcpy((void*)strData.c_str(), pData + i, nLength - 4);
            i += nLength - 4;
        }
        sSum = *(WORD*)(pData + i); i += 2; //i是标记用到哪了 nLength + 2 + 4在前面可能有废数据 后面解析出的一个包前面用过的应该被销毁掉 < i后面的移动到buff使buff继续工作 
        WORD sum = 0;
        for (int j = 0; j < strData.size(); j++)
        {
            sum += BYTE(strData[i]) & 0xFF;
        }
        if (sum == sSum) {
            nSize = i; //+ nLength + 2 + 4;//包长 == head length data(不知道但是length标明了)...
            return;
        }
        nSize = 0;
    }
    ~CPacket(){}
    //等于号运算符重载
    CPacket& operator=(const CPacket& pack) {
        if (this != &pack) {
            sHead = pack.sHead;
            nLength = pack.nLength;
            sCmd = pack.sCmd;
            strData = pack.strData;
            sSum = pack.sSum;
        }
        return *this;
    }
public:
    WORD sHead; //固定位FE FF
    DWORD nLength; //包长度 (从控制命令开始, 到和校验结束)
    WORD sCmd; //控制命令
    std::string strData; //包数据
    WORD sSum; //和校验
};

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

#define BUFFER_SIZE 4096
    int DealCommand() {
        if (m_client == -1)return -1;
        //char buffer[1024] = "";
        char* buffer = new char[4096];
        memset(buffer, 0, 4096);
        size_t index = 0;
        while (true)
        {
            size_t len = recv(m_client, buffer + index, 4096 - index, 0);
            if (len <= 0) {
                return -1;
            }
            index += len;
            len = index;
            m_packet = CPacket((BYTE*)buffer, len);
            if (len > 0) {
                memmove(buffer, buffer + len, BUFFER_SIZE - len);
                index -= len;
                return m_packet.sCmd;
            }
        }
        return -1;
        //TODO 处理命令

    }

    bool Send(const char* pData, int nSize) {
        if (m_client == -1)return false;
        return send(m_client, pData, nSize, 0) > 0;
    }


    //单例
private:
    SOCKET m_client;
    SOCKET m_sock;
    CPacket m_packet;
    // 防止拷贝构造 既不能赋值也不能让外部调用构造
    CServerSocket& operator=(const CServerSocket& ss) {};
    //复制构造函数  
    CServerSocket(const CServerSocket& ss) {
        m_sock = ss.m_sock;
        m_client = ss.m_client;    //初始化
    };

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