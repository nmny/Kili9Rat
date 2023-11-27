#pragma once
#include "pch.h"
#include "framework.h"

class CPacket
{
public:
    CPacket():sHead(0), nLength(0), sCmd(0), sSum(0) {  //��Ա������ʼ��
    }
    CPacket(WORD nCmd, const BYTE* pData, size_t nSize) { //�������
        sHead = 0XFEFF;
        nLength = nSize + 2 + 2;  //nLength strData+sCmd+sSum
        sCmd = nCmd;
        strData.resize(nSize);
        memcpy((void*)strData.c_str(), pData, nSize);
        sSum = 0;

    };
    CPacket(const CPacket& pack) {//��ֵ���캯������CPacket�ĸ�ֵ����
        sHead = pack.sHead;
        nLength = pack.nLength;
        sCmd = pack.sCmd;
        strData = pack.strData;
        sSum = pack.sSum;
    }

    CPacket(const BYTE* pData, size_t& nSize) { //��������������
        size_t i = 0;
        for (; i < nSize; i++) {
            if(*(WORD*)(pData+i) == 0xFEFF){ //*(WORD*)���ⲿ����һ������ǿ��ת��������pData��ƫ��i�����ڴ����Ϊһ��WORD���ͣ�ͨ����2���ֽڵ�����������ͨ��*����������ø�     �ڴ�λ���ϵ�ֵ��
                sHead = *(WORD*)(pData + i);
                i += 2; //δ�ҵ�ͷ��Ч�˳� ����nSize������2bit(FEFF) ���iû��+2,i=0,nSize=2 ����û����������ȥ�������ȳ���ͻ���� 
                break;
            }
        }
        //+4���� +2���� +2��У��
        if (i + 4 + 2 + 2 >= nSize) {//�����ݿ��ܲ�ȫ�����߰�ͷδ����ȫ���յ�   
            nSize = 0;
            return; }
        //�����ݼ������¶�
        nLength = *(DWORD*)(pData + i); i += 4;
        if (nLength + i > nSize) {//��δ��ȫ���յ��ͷ��أ�����ʧ��
            nSize = 0;
            return;
        }
        sCmd = *(WORD*)(pData + i); i + 2;
        if(nLength > 4)
        {
            strData.resize(nLength - 2 - 2); //sCmd��sSum
            memcpy((void*)strData.c_str(), pData + i, nLength - 4);
            i += nLength - 4;
        }
        sSum = *(WORD*)(pData + i); i += 2; //i�Ǳ���õ����� nLength + 2 + 4��ǰ������з����� �����������һ����ǰ���ù���Ӧ�ñ����ٵ� < i������ƶ���buffʹbuff�������� 
        WORD sum = 0;
        for (int j = 0; j < strData.size(); j++)
        {
            sum += BYTE(strData[i]) & 0xFF;
        }
        if (sum == sSum) {
            nSize = i; //+ nLength + 2 + 4;//���� == head length data(��֪������length������)...
            return;
        }
        nSize = 0;
    }
    ~CPacket(){}
    //���ں����������
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
    WORD sHead; //�̶�λFE FF
    DWORD nLength; //������ (�ӿ������ʼ, ����У�����)
    WORD sCmd; //��������
    std::string strData; //������
    WORD sSum; //��У��
};

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
        //TODO ��������

    }

    bool Send(const char* pData, int nSize) {
        if (m_client == -1)return false;
        return send(m_client, pData, nSize, 0) > 0;
    }


    //����
private:
    SOCKET m_client;
    SOCKET m_sock;
    CPacket m_packet;
    // ��ֹ�������� �Ȳ��ܸ�ֵҲ�������ⲿ���ù���
    CServerSocket& operator=(const CServerSocket& ss) {};
    //���ƹ��캯��  
    CServerSocket(const CServerSocket& ss) {
        m_sock = ss.m_sock;
        m_client = ss.m_client;    //��ʼ��
    };

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