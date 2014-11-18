/**************************************************************************************
@author: �²�
@content: Dispatch�Կͻ������ӵļ���socket����
**************************************************************************************/
#include "stdafx.h"
#include "CClientServerSocket.h"

const std::string IPCONFIG_FILE = "ipaddress.txt";        // IP�����ļ�
const int MAX_CONNECT_TIMEOUT = 30 * 1000;                // �������ʱ��
const int DELAY_DISCONNECT_TIME = 3000;                   // ��ʱ�Ͽ�ʱ��
const std::string War_Warning = "  ����Ϸ�������ɶ�սģʽ��������Ϸ���򣬿ɻ�ó�����Ϸ���顣</br>  �����������ѿ���<font color=\"0xFFDD0000\">������ϵͳ</font>����ϸ������ע��Ϸ������Ϣ��";

/************************Start Of CDGClient********************************************************/
CDGClient::CDGClient() :m_ulLastConnectTick(GetTickCount()), m_ulForceCloseTick(0), m_usSelectMaskServerID(0), m_iEncodeIdx(0),
						m_iSelectRealServerID(0), m_bIsGMIP(false), m_iClientType(0), m_ucNetType(0)
{
	SendDebugString("CDGClient ����");
}

CDGClient::~CDGClient()
{
	SendDebugString("CDGClient ����");
}

unsigned short CDGClient::GetSelectMaskServerID()
{
	return m_usSelectMaskServerID;
}

int CDGClient::GetSelectRealServerID()
{
	return m_iSelectRealServerID;
}

bool CDGClient::GetIsGMIP()
{
	return m_bIsGMIP;
}

int CDGClient::GetEncodeIdx()
{
	return m_iEncodeIdx;
}

int CDGClient::GetClientType()
{
	return m_iClientType;
}

unsigned char CDGClient::GetNetType()
{
	return m_ucNetType;
}

void CDGClient::ForceClose()
{
	m_ulForceCloseTick = GetTickCount();
}

void CDGClient::Execute(unsigned long ulTick)
{
	CClientConnector::Execute(ulTick);
	if (((ulTick > m_ulLastConnectTick) && (ulTick - m_ulLastConnectTick > MAX_CONNECT_TIMEOUT)) ||
		((m_ulForceCloseTick > 0) && (ulTick > m_ulForceCloseTick) && (ulTick - m_ulForceCloseTick > DELAY_DISCONNECT_TIME)))
	{
		Close();
	}

	if (ulTick < m_ulLastConnectTick)
		m_ulLastConnectTick = ulTick;
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	SendDebugString("CSampleConnector ��ȡ����");
}

void CDGClient::OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg)
{
	int iStrLen = msg.length();
	int iDataLen = sizeof(TClientWindowRec)+iStrLen + 1;
	char* pData = (char*)malloc(iDataLen);
	try
	{
		memset(pData, 0, iDataLen);
		((PClientWindowRec)pData)->WinType = wtype;
		((PClientWindowRec)pData)->Param = iParam;
		if (iStrLen > 0)
		{
			memcpy((void*)pData[sizeof(TClientWindowRec)], msg.c_str(), iStrLen);
			pData[iDataLen - 1] = '\0';
		}
		SendToClient(SCM_OPEN_WINDOW, pData, iDataLen);
		free(pData);
	}
	catch (...)
	{
		free(pData);
	}
}

void CDGClient::CMSelectServer(char* pBuf, unsigned short usBufLen)
{

}

void CDGClient::CMCloseWindow(char* pBuf, unsigned short usBufLen)
{
	if (usBufLen >= sizeof(TClientWindowRec))
	{
		if (cwWarRule == ((PClientWindowRec)pBuf)->WinType)
		{
			//--------------------------------
			//--------------------------------
			//--------------------------------
			//G_DBSocket.SendSelectServer(Self);
		}
	}
}

void CDGClient::SendToClient(unsigned short usIdent, char* pData, unsigned short usDataLen)
{
	unsigned short usBufLen = sizeof(TClientSocketHead)+usDataLen;
	char* pBuf = (char*)malloc(usBufLen);
	try
	{
		((PClientSocketHead)pBuf)->ulSign = CS_SEGMENTATION_CLIENTSIGN;
		((PClientSocketHead)pBuf)->usPackageLen = usBufLen;
		((PClientSocketHead)pBuf)->usIdent = usIdent;
		((PClientSocketHead)pBuf)->ulIdx = 0;
		if (usDataLen > 0)
		{
			memcpy((void*)pBuf[sizeof(TClientSocketHead)], pData, usDataLen);
		}		
		SendBuf(pBuf, usBufLen);
		free(pBuf);
	}
	catch (...)
	{
		free(pBuf);
	}
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
