/**************************************************************************************
@author: �²�
@content: Dispatch��Ϊ�ͻ��˷�����CenterServer�������Ķ˿�
**************************************************************************************/
#include "stdafx.h"
#include "CCenterClientSocket.h"
#include "CDBServerSocket.h"

using namespace CC_UTILS;

CCenterClientSocket* pG_CenterSocket;

/************************Start Of CCenterClientSocket******************************************/

CCenterClientSocket::CCenterClientSocket() : m_iPingCount(0)
{
	SendDebugString("CCenterClientSocket ����");
	m_OnConnect = std::bind(&CCenterClientSocket::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CCenterClientSocket::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CCenterClientSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CCenterClientSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
}

CCenterClientSocket::~CCenterClientSocket()
{
	SendDebugString("CCenterClientSocket ����");
}

void CCenterClientSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{
	bool bConfigOK = false;
	if (pIniFileParser != nullptr)
	{
		std::string sServerStr;		
		std::vector<std::string> vec;
		int iPort;
		for (int i = 0; i < MAX_CENTER_SERVER_COUNT; i++)
		{
			std::string sKeyName("Server");
			sKeyName.append(to_string(i));
			sServerStr = pIniFileParser->getString("CenterServer", sKeyName, "");
			if (sServerStr == "")
				break;

			vec.clear();
		    SplitStr(sServerStr, ":", &vec);  //vec[0]��ip vec[1]��port
			iPort = CC_UTILS::StrToIntDef(vec[1], 0);
			if (iPort > 0)
			{
				memcpy_s(m_ServerArray[i].IPAddress, sizeof(m_ServerArray[i].IPAddress), vec[0].c_str(), vec[0].length() + 1);
				m_ServerArray[i].iPort = iPort;
				bConfigOK = true;
			}
		}
	}
	m_iWorkIndex = 0;
	if (!bConfigOK)
		Log("CenterServer δ����!", lmtError);
}

void CCenterClientSocket::SendToServerPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen)
{
	int iDataLen = sizeof(TServerSocketHeader) + usBufLen;
	char* pData = (char*)malloc(iDataLen);
	if (pData != nullptr)
	{
		try
		{
			((PServerSocketHeader)pData)->ulSign = SS_SEGMENTATION_SIGN;
			((PServerSocketHeader)pData)->usIdent = usIdent;
			((PServerSocketHeader)pData)->iParam = iParam;
			((PServerSocketHeader)pData)->usBehindLen = usBufLen;
			if (usBufLen > 0)
				memcpy(pData + sizeof(TServerSocketHeader), pBuf, usBufLen);

			SendBuf(pData, iDataLen);
			free(pData);
		}
		catch (...)
		{
			free(pData);
		}
	}
}

void CCenterClientSocket::DoHeartBeat()
{
	unsigned long ulInterval = 12 * 1000;
	if (!IsConnected())
		ulInterval = 3 * 1000;

	unsigned long ulTick = GetTickCount();
	if (ulTick - m_ulCheckTick >= ulInterval)
	{
		m_ulCheckTick = ulTick;
		if (IsConnected())
		{   //����״̬�����������
			if (m_iPingCount >= 3)
			{
				m_iPingCount = 0;
				Close();
			}
			else
			{
				SendToServerPeer(SM_PING, pG_DBSocket->GetPlayerTotalCount(), nullptr, 0);
				m_iPingCount += 1;
			}
		}
		else
		{	//�Ͽ�״̬��������
			Reconnect();
		}
	}
}

void CCenterClientSocket::OnSocketConnect(void* Sender)
{
	Log("��CCenterClientSocket(" + m_Address + ")���ӳɹ�");
	SendRegisterServer();
}

void CCenterClientSocket::OnSocketDisconnect(void* Sender)
{
	Log("��CCenterClientSocket(" + m_Address + ")�Ͽ�����");
}

void CCenterClientSocket::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{
	//�յ���Ϣ,ping��������
	m_iPingCount = 0;
	//�ڻ������������ݰ���������ProcessReceiveMsg����߼���Ϣ����
	int iErrorCode = ParseSocketReadData(1, pBuf, iCount);
	if (iErrorCode > 0)
		Log("CCenterClientSocket Socket Read Error, Code = " + to_string(iErrorCode), lmtError);
}

void CCenterClientSocket::ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen)
{
	PServerConnectInfo pInfo;
	int iCount;
	switch (pHeader->usIdent)
	{
	case SM_PING:
		break;
	case SM_SERVER_CONFIG:
		pInfo = (PServerConnectInfo)pData;
		iCount = pHeader->iParam;
		if (iCount > MAX_RESSERVER_COUNT)
			iCount = MAX_RESSERVER_COUNT;
		for (int i = 0; i < iCount; i++)
		{
			G_ResServerInfos[i] = *pInfo;
			pInfo += 1;
		}
		if (iCount > 0)
		{
			for (int i = iCount; i < MAX_RESSERVER_COUNT; i++)
				G_ResServerInfos[i].Addr.iPort = 0;
		}
		break;
	default:
		std::string temps("�յ�δ֪CenterServerЭ�飬Ident=");
		temps.append(to_string(pHeader->usIdent));
		Log(temps, lmtWarning);
		break;
	}
}

void CCenterClientSocket::OnSocketError(void* Sender, int& iErrorCode)
{
	Log("CCenterClientSocket Socket Error, Code = " + to_string(iErrorCode), lmtError);
	iErrorCode = 0;
}

void CCenterClientSocket::Reconnect()
{
	if (m_iWorkIndex >= MAX_CENTER_SERVER_COUNT)
		m_iWorkIndex = 0;

	if (!IsActive())
	{
		m_Address = m_ServerArray[m_iWorkIndex].IPAddress;
		m_Port = m_ServerArray[m_iWorkIndex].iPort;
		if (m_Port > 0)
		{		
			Log("Connect to CenterServer(" + m_Address + ":" + to_string(m_Port) + ")", lmtError);
			Open();
		}
	}
	m_iWorkIndex += 1;
}

void CCenterClientSocket::SendRegisterServer()
{
	std::string sIP = GetInternetIP();
	TServerAddress address;
	memcpy_s(address.IPAddress, sizeof(address.IPAddress), sIP.c_str(), sIP.length() + 1);
	address.iPort = pG_GateSocket->m_iListenPort;
	SendToServerPeer(SM_REGISTER, 0, &address, sizeof(TServerAddress));
}

/************************End Of CCenterClientSocket********************************************/