#include "pch.h"
#include "ServerSocket.h"

//�����ʼ���ͳ�ʼ�� ���پ�����
//��̬�������״α����û��ʼ�� һ���ǳ������ٲŻ����٣���ȫ�־�̬������main������ǰ���ʼ�� main�����ᱻ����

//CServerSocket server;
//�����͵õ�һ��ȫ�ֵ�m_instance ��̬�ǿ���ȫ�ַ��ʵ� ���Կ���ֱ�������ӷ��� �Ǿ�̬������this��Ҫ.����ָ�����ܵ���
//�����໹�����ྲ̬��Ա���ǹ��õ�
CServerSocket* CServerSocket::m_instance = NULL; //��ʽ
CServerSocket::CHelper CServerSocket::m_helper;

CServerSocket* pserver = CServerSocket::getInstance(); //��ʼ��