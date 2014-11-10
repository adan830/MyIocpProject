/**************************************************************************************
@author: �²�
@content: Dispatch��Ϊ�ͻ��˷�����PIG�������Ķ˿�
**************************************************************************************/
#include "CPigClientSocket.h"

/************************Start Of CPigClientSocket******************************************/

CPigClientSocket::CPigClientSocket()
{
	SendDebugString("CSampleClientManager ����");
	m_OnConnect = std::bind(&CPigClientSocket::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CPigClientSocket::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CPigClientSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CPigClientSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
}

CPigClientSocket::~CPigClientSocket()
{
	SendDebugString("CSampleClientManager ����");
}

void CPigClientSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{

}

void CPigClientSocket::SendBuffer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen)
{

}

void CPigClientSocket::DoHeartBeat()
{
	
}

void CPigClientSocket::OnSocketConnect(void* Sender)
{
	SendDebugString("���ӳɹ���");
}

void CPigClientSocket::OnSocketDisconnect(void* Sender)
{
	SendDebugString("�����ѶϿ���");
}

void CPigClientSocket::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{}

void CPigClientSocket::OnSocketError(void* Sender, int& iErrorCode)
{}

/************************End Of CPigClientSocket********************************************/