#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket {
public:
    // ��̬���������ڻ�ȡ����ʵ��
    static CServerSocket* getInstance() {
        //m_instance == NULL
        if (!m_instance) {//��̬����û��thisָ�� �����޷�ֱ�ӷ��ʳ�Ա���� ��Ҫ��Ա����ҲΪ��̬
            m_instance = new CServerSocket;
        }
        return m_instance;
    };

    //socket��ʼ��
    bool InitSocket() {

        if (m_sock == -1)return false;

        //׼����ַ
        sockaddr_in serv_adr;
        memset(&serv_adr, 0, sizeof(serv_adr)); //serv_adr��ʼ���ڴ���0
        serv_adr.sin_family = AF_INET;   //ʹ�õ�ַ��
        //serv_adr.sin_addr.S_un.S_addr ����ʹ��s_addr��������
        serv_adr.sin_addr.s_addr = INADDR_ANY;  //�����������ϵ�����IP(���ܶ���������IP)
        serv_adr.sin_port = htons(9527);

        //�� ǿ������ת��
        if (bind(m_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
            return false;

        }
        //����1��1
        if (listen(m_sock, 1) == -1) {
            return false;

        }
        return true; //socket��ʼ�����
    }

    //client��ʼ��
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
        //TODO ��������

    }

    bool Seed(const char* pData, int nSize) {
        return send(m_client, pData, nSize, 0) > 0;
    }


    //����
private:
    // ��ֹ�������� �Ȳ��ܸ�ֵҲ�������ⲿ���ù���
    CServerSocket& operator=(const CServerSocket& ss) {};
    //���ƹ��캯��  
    CServerSocket(const CServerSocket& ss) {
        m_sock = ss.m_sock;
        m_client = ss.m_client;    //��ʼ��
    };
    SOCKET m_client;
    SOCKET m_sock;
    //���캯��
    CServerSocket() {
        m_client = INVALID_SOCKET; //-1

        if (InitSockEnv() == FALSE) {
            MessageBox(NULL, _T("�޷���ʼ���׽��ֻ���,������������"), _T("��ʼ������"), MB_OK | MB_ICONERROR);
            exit(0);    //��������Ϣ����ϵͳ������ Ӳ��������
        }
        //serv_sock > member
        //�����׽���
        m_sock = socket(PF_INET, SOCK_STREAM, 0);
    };    //���캯��

    BOOL InitSockEnv() {
        //sockt����
        WSADATA data;
        if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
            return FALSE;
        };  //TODO: ����ֵ����
        return TRUE;
    }

    //�� ��������ʵ��ָ��
    static CServerSocket* m_instance;


    ~CServerSocket() {
        closesocket(m_sock);
        WSACleanup();
        //��������WSAStartup WSACleanup�ɶԳ���
    };    //��������

    static void releaseInstance() {
        if (m_instance != NULL) {
            CServerSocket* tmp = m_instance;
            m_instance = NULL;
            delete tmp;
        }
    }
    //��������
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

extern CServerSocket server;    //����һ���ⲿ�ı��� ������cpp�Ϳ���ȥ������ⲿ����