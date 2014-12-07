/**************************************************************************************
@author: �²�
@content: Dispatch��Ϊ�ͻ��˷�����PIG�������Ķ˿�
**************************************************************************************/
#include "stdafx.h"
#include "CPigClientSocket.h"
#include "CDBServerSocket.h"

using namespace CC_UTILS;

CPigClientSocket* pG_PigSocket;

/************************Start Of CPigClientSocket******************************************/

CPigClientSocket::CPigClientSocket() : m_iPingCount(0)
{
	SendDebugString("CPigClientSocket ����");
	SetReconnectInterval(10 * 1000);

	m_OnConnect = std::bind(&CPigClientSocket::OnSocketConnect, this, std::placeholders::_1);
	m_OnDisConnect = std::bind(&CPigClientSocket::OnSocketDisconnect, this, std::placeholders::_1);
	m_OnRead = std::bind(&CPigClientSocket::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_OnError = std::bind(&CPigClientSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);	
}

CPigClientSocket::~CPigClientSocket()
{
	SendDebugString("CPigClientSocket ����");
}

void CPigClientSocket::LoadConfig(CWgtIniFile* pIniFileParser)
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

void CPigClientSocket::SendToServerPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen)
{
	int iDataLen = sizeof(TServerSocketHeader)+usBufLen;
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

void CPigClientSocket::DoHeartBeat()
{
	unsigned long ulInterval = 10 * 1000;
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
				SendToServerPeer(SM_PING, 0, nullptr, 0);
				m_iPingCount += 1;
			}
		}
		else
		{	//�Ͽ�״̬��������			
			Open();
		}
	}

}

void CPigClientSocket::OnSocketConnect(void* Sender)
{	
	std::string temps("��PigServer(");
	temps.append(m_Address);
	temps.append(")���ӳɹ�");
	Log(temps);
}

void CPigClientSocket::OnSocketDisconnect(void* Sender)
{
	std::string temps("��PigServer(");
	temps.append(m_Address);
	temps.append(")�Ͽ�����");
	Log(temps);
}

void CPigClientSocket::OnSocketRead(void* Sender, const char* pBuf, int iCount)
{
	//�յ���Ϣ,ping��������
	m_iPingCount = 0;     
	//�ڻ������������ݰ���������ProcessReceiveMsg����߼���Ϣ����
	int iErrorCode = ParseSocketReadData(1, pBuf, iCount);
	if (iErrorCode > 0)
	{
		std::string temps("CPigClientSocket Socket Read Error, Code = ");
		temps.append(to_string(iErrorCode));
		Log(temps, lmtError);
	}	
}

void CPigClientSocket::ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen)
{
	switch (pHeader->usIdent)
	{
	case SM_PING:
		break;
	case SM_PIG_MSG:
		//ע�⣺�����ת����Ҫ�������Ƿ�ı����������ṹ������
		//ע�⣺�����ת����Ҫ�������Ƿ�ı����������ṹ������
		pG_DBSocket->SendPigMsg(const_cast<char*>(pData), iDataLen);   //����Pig��Ϣ
		break;
	case SM_PIG_QUERY_AREA:
		pG_DBSocket->SendServerInfoToPig(this);   //��ѯ������Ϣ 
		break;
	default:
		std::string temps("�յ�δ֪PigServerЭ�飬Ident=");
		temps.append(to_string(pHeader->usIdent));
		Log(temps, lmtWarning);
		break;
	}
}

void CPigClientSocket::OnSocketError(void* Sender, int& iErrorCode)
{
	std::string temps("CPigClientSocket Socket Error, Code = ");
	temps.append(to_string(iErrorCode));
	Log(temps, lmtError);
	iErrorCode = 0;
}

/************************End Of CPigClientSocket********************************************/