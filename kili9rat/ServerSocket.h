#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket
{

public:
	CServerSocket(){
		if (InitSockEnv() == FALSE) {
			MessageBox(NULL,_T("�޷���ʼ���׽��ֻ���,������������"),_T("��ʼ������"),MB_OK|MB_ICONERROR);
			exit(0);	//��������Ϣ����ϵͳ������ Ӳ��������
		}
	};	//���캯��
	~CServerSocket(){
		WSACleanup();
	};	//��������
	BOOL InitSockEnv() {
		//sockt����s
		WSADATA data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0){
			return FALSE;
		};  //TODO: ����ֵ����
		return TRUE;

	};
};

extern CServerSocket server;	//����һ���ⲿ�ı��� ������cpp�Ϳ���ȥ������ⲿ����