/**************************************************************************************
@author: �²�
@content: 
**************************************************************************************/
#include "CIocpSampleClient.h"
#pragma comment(lib, "ws2_32.lib")

/************************Start Of CIocpSampleClient******************************************/

CSampleClientManager::CSampleClientManager()
{
	SendDebugString("CSampleClientManager ����");
	m_OnConnect = std::bind(&CSampleClientManager::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CSampleClientManager::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CSampleClientManager::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CSampleClientManager::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
}

CSampleClientManager::~CSampleClientManager()
{
	SendDebugString("CSampleClientManager ����");
}

void CSampleClientManager::ConnectToServer(const std::string& sIP, const int iPort)
{
	m_Address = sIP;
	m_Port = iPort;
	SetReconnectInterval(10000);
	SendDebugString("��ʼ���ӡ�����");
}

void CSampleClientManager::Disconnect()
{
	SendDebugString("�����Ͽ�");
	Close(false);
}

bool CSampleClientManager::SendToServer()
{
	return true;
}

void CSampleClientManager::OnSocketConnect(void* Sender)
{
	SendDebugString("���ӳɹ���");
}

void CSampleClientManager::OnSocketDisconnect(void* Sender)
{
	SendDebugString("�����ѶϿ���");
}

void CSampleClientManager::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{}

void CSampleClientManager::OnSocketError(void* Sender, int& iErrorCode)
{}

/************************End Of CIocpSampleClient********************************************/