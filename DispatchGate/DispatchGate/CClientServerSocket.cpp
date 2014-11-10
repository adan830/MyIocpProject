/**************************************************************************************
@author: �²�
@content: Dispatch�Կͻ������ӵļ���socket����
**************************************************************************************/
#include "stdafx.h"
#include "CClientServerSocket.h"

/************************Start Of CDGClient********************************************************/
void CDGClient::Execute(unsigned long ulTick)
{
	if (ulTick - m_ulLastRunTick >= 10000)
	{
		m_ulLastRunTick = ulTick;
		SendDebugString("10��ִ��");
	}
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	SendDebugString("CSampleConnector ��ȡ����");
}

CDGClient::CDGClient() :m_ulLastRunTick(0)
{
	SendDebugString("CSampleConnector ����");
}

CDGClient::~CDGClient()
{
	SendDebugString("CSampleConnector ����");
}

/************************End Of CDGClient***********************************************************/


/************************Start Of CClientServerSocket************************************************/
CClientServerSocket::CClientServerSocket()
{
	SendDebugString("CSampleServerManager ����");
	m_OnCheckAddress = std::bind(&CClientServerSocket::OnCheckIPAddress, this, std::placeholders::_1);
	m_OnCreateClient = std::bind(&CClientServerSocket::OnCreateClientSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CClientServerSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CClientServerSocket::OnClientConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CClientServerSocket::OnClientDisconnect, this, std::placeholders::_1);
}

CClientServerSocket::~CClientServerSocket()
{
	SendDebugString("CSampleServerManager ����");
}

void CClientServerSocket::DoActive()
{

}

bool CClientServerSocket::OnCheckIPAddress(const std::string& sIP)
{
	SendDebugString("CheckIPAddress");
	return true;
}

CDGClient* CClientServerSocket::OnCreateClientSocket(const std::string& sIP)
{
	SendDebugString("OnCreateClientSocket");
	return new CDGClient;
}

void CClientServerSocket::OnSocketError(void* Sender, int& iErrorCode)
{

}

void CClientServerSocket::OnClientConnect(void* Sender)
{
	SendDebugString("OnClientConnect");
}

void CClientServerSocket::OnClientDisconnect(void* Sender)
{
	SendDebugString("OnClientDisconnect");
}

/************************End Of CClientServerSocket****************************************************/
