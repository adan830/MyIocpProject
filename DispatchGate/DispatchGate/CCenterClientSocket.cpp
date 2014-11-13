/**************************************************************************************
@author: �²�
@content: Dispatch��Ϊ�ͻ��˷�����CenterServer�������Ķ˿�
**************************************************************************************/
#include "stdafx.h"
#include "CCenterClientSocket.h"

TServerConnectInfo G_ResServerInfos[MAX_RESSERVER_COUNT];

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
	if (pIniFileParser != nullptr)
	{
		m_Address = pIniFileParser->getString("PigServer", "IP", "");
		m_Port = pIniFileParser->getInteger("PigServer", "Port", DEFAULT_PIG_SERVER_PORT);
		if ("" == m_Address)
			Log("δ����PigServer��IP��");
		else
			Open();
	}
}

void CCenterClientSocket::SendToServer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen)
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
				//-------------------
				//SendToServer(SM_PING, G_DBSocket.PlayerTotalCount, nullptr, 0);
				//-------------------
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
	std::string temps("��CCenterClientSocket(");
	temps.append(m_Address);
	temps.append(")���ӳɹ�");
	Log(temps.c_str());
	SendRegisterServer();
}

void CCenterClientSocket::OnSocketDisconnect(void* Sender)
{
	std::string temps("��CCenterClientSocket(");
	temps.append(m_Address);
	temps.append(")�Ͽ�����");
	Log(temps.c_str());
}

void CCenterClientSocket::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{
	//�յ���Ϣ,ping��������
	m_iPingCount = 0;
	//�ڻ������������ݰ���������ProcessReceiveMsg����߼���Ϣ����
	int iErrorCode = ParseSocketReadData(1, pBuf, iCount);
	if (iErrorCode > 0)
	{
		std::string temps("CCenterClientSocket Socket Read Error, Code = ");
		temps.append(to_string(iErrorCode));
		Log(temps.c_str(), lmtError);
	}
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
		Log(temps.c_str(), lmtWarning);
		break;
	}
}

void CCenterClientSocket::OnSocketError(void* Sender, int& iErrorCode)
{
	std::string temps("CCenterClientSocket Socket Error, Code = ");
	temps.append(to_string(iErrorCode));
	Log(temps.c_str(), lmtError);
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
			std::string temps("Connect to CenterServer(");
			temps.append(m_Address);
			temps.append(":");
			temps.append(to_string(m_Port));
			temps.append(")");
			Log(temps.c_str(), lmtError);
			Open();
		}
	}
	m_iWorkIndex += 1;
}

void CCenterClientSocket::SendRegisterServer()
{
	TServerAddress address;
	/*
	  with Addr do
	  begin
		StrPLCopy(IPAddress, GetInternetIP, 15);
		nPort := G_GateSocket.Port;
	  end;
	*/
	SendToServer(SM_REGISTER, 0, (char*)&address, sizeof(TServerAddress));
}

/************************End Of CCenterClientSocket********************************************/