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

void CDGClient::SetGMIP(const bool bFlag)
{
	m_bIsGMIP = bFlag;
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

void CDGClient::ProcessReceiveMsg(char* pHeader, char* pData, int iDataLen)
{
	switch (((PClientSocketHead)pHeader)->usIdent)
	{
	case CM_PING:
		SendToClient(CM_PING, nullptr, 0);
		break;
	case CM_SELECT_SERVER_OLD:
		OpenWindow(cwMessageBox, 0, "�ͻ��˰汾����ȷ�����ȸ��£�");
		ForceClose();
		break;
	case CM_SELECT_SERVER:
		CMSelectServer(pData, iDataLen);
		break;
	case CM_CLOSE_WINDOW:
		CMCloseWindow(pData, iDataLen);
		break;
	case CM_QUIT:
		ForceClose();
		break;
	default:
		//----------------------------------------------------
		//----------------------------------------------------
		//----------------------------------------------------
		//----------------------------------------------------
		//Log(Format('�յ�δ֪�ͻ���Э�飬IP=%s Ident=%d', [RemoteAddress, Ident]), lmtWarning);
		Log("�յ�δ֪�ͻ���Э��", lmtWarning);
		break;
	}
}

void CDGClient::SocketRead(const char* pBuf, int iCount)
{
	if (m_ulForceCloseTick > 0)
		return;
	if (iCount >= sizeof(TClientSocketHead))
	{
		PClientSocketHead pHead = (PClientSocketHead)pBuf;
		if (pHead->usPackageLen < sizeof(TClientSocketHead))
			return;
		if (CS_SEGMENTATION_CLIENTSIGN == pHead->ulSign)
			m_iClientType = 1;
		else if (CS_SEGMENTATION_CLIENTSIGN + 1 == pHead->ulSign)
			m_iClientType = 2;
		else
			return;

		char* pData = (char*)pHead + sizeof(TClientSocketHead);
		int iDataLen = pHead->usPackageLen - sizeof(TClientSocketHead);
		ProcessReceiveMsg((char*)pHead, pData, iDataLen);
	}
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
	m_ucNetType = 0;
	if (sizeof(int) == usBufLen)
	{
		m_usSelectMaskServerID = *((int*)pBuf);
	}
	else if (sizeof(TCMSelectServer) == usBufLen)
	{
		m_usSelectMaskServerID = ((PCMSelectServer)pBuf)->iMaskServerID;
		//---------------------------------------------------------
		//---------------------------------------------------------
		//---------------------------------------------------------
		//m_ucNetType = G_GateSocket.GetNetType(((PCMSelectServer)pBuf)->iMaskServerID);
	}
	else
	{
		OpenWindow(cwMessageBox, 0, "ѡ�������ʧ��");
		std::string temps("ѡ������ʧ��: Area = ");
		temps.append(to_string(m_usSelectMaskServerID));
		Log(temps.c_str());
		return;
	}

	//-------------------------------------------------
	//-------------------------------------------------
	//-------------------------------------------------
	//����Դ���������и��ؾ���
	//m_iSelectRealServerID = G_DBSocket.SelectServer(this);
	if (m_iSelectRealServerID > 0)
	{
		OpenWindow(cwWarRule, 0, War_Warning);
		int iMinCount = 100000;
		TServerAddress address;
		memset(&address, 0, sizeof(TServerAddress));
		for (int i = 0; i < MAX_RESSERVER_COUNT; i++)
		{
			if (0 == G_ResServerInfos[i].Addr.iPort)
				break;

			if (iMinCount > G_ResServerInfos[i].iConnectCount)
			{
				iMinCount = G_ResServerInfos[i].iConnectCount;
				address = G_ResServerInfos[i].Addr;
			}
		}
		if (address.iPort > 0)
			SendToClient(SCM_RESSERVER_INFO, (char*)&address, sizeof(TServerAddress));
	}
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
CClientServerSocket::CClientServerSocket() :m_ulLastCheckTick(0), m_iIPConfigFileAge(0), m_DefaultRule(itUnKnow), m_sWarWarning("")				 
{
	SendDebugString("CClientServerSocket ����");
	//m_OnCheckAddress = std::bind(&CClientServerSocket::OnCheckIPAddress, this, std::placeholders::_1);
	m_OnCreateClient = std::bind(&CClientServerSocket::OnCreateClientSocket, this, std::placeholders::_1);
	m_OnClientError = std::bind(&CClientServerSocket::OnSocketError, this, std::placeholders::_1, std::placeholders::_2);
	m_OnConnect = std::bind(&CClientServerSocket::OnClientConnect, this, std::placeholders::_1);
}

CClientServerSocket::~CClientServerSocket()
{
	Clear();
	SendDebugString("CClientServerSocket ����");
}

void CClientServerSocket::LoadConfig(CWgtIniFile* pIniFileParser)
{
	if (pIniFileParser != nullptr)
	{
		int iPort = pIniFileParser->getInteger("Setup", "GatePort", DEFAULT_DispatchGate_CLIENT_PORT);
		std::string sTemp;
		std::string sKeyName("Type_");
		for (int i = 0; i < MAX_NET_TYPE_CONFIG; i++)
		{
			sTemp = pIniFileParser->getString("NetType", sKeyName + to_string(i), "");
			if ("" == sTemp)
				break;
			m_NetTypes[i] = inet_addr(sTemp.c_str());
		}

		if (!IsActive())
		{
			m_sLocalIP = "0.0.0.0";
			m_iListenPort = iPort;
			sTemp = "���ܿͻ�������, Port = ";
			sTemp.append(to_string(iPort));
			Log(sTemp.c_str(), lmtMessage);
			Open();
		}
	}
}

bool CClientServerSocket::IsMasterIP(std::string& sIP)
{
	bool retFlag = false;
	std::string sTempIP(sIP);
	sTempIP.append(".");	

	PIpRuleNode pNode;
	std::list<PIpRuleNode>::iterator vIter;
	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	for (vIter = m_IPRuleList.begin(); vIter != m_IPRuleList.end(); ++vIter)
	{
		pNode = (PIpRuleNode)*vIter;
		if (pNode->ipType != itMaster)
			continue;
		//--------------------------------------
		//--------------------------------------
		//����ж���Ҫ���Լ���һ��
		if (sTempIP.find(pNode->sMatchIP) == 1)
		{
			retFlag = true;
			break;
		}
	}
	return retFlag;
}

void CClientServerSocket::SMSelectServer(int iSocketHandle, char* pBuf, unsigned short usBufLen)
{
	if (usBufLen = sizeof(TNextGateInfo))
	{
		std::lock_guard<std::mutex> guard(m_LockCS);
		CDGClient* pClient = (CDGClient*)ValueOf(iSocketHandle);
		if (pClient != nullptr)
		{
			if ((0 == ((PNextGateInfo)pBuf)->iGateAddr) || (0 == ((PNextGateInfo)pBuf)->iGatePort))
			{
				pClient->OpenWindow(cwMessageBox, 0, "Ŀǰ����������ά���У���");
				pClient->ForceClose();
			}
			else
			{
				pClient->SendToClient(CM_SELECT_SERVER, pBuf, usBufLen);
			}
		}
	}
	else
	{
		Log("SM_SELECT_SERVER Response: invalid length=", lmtWarning);
	}
}

TIpType CClientServerSocket::GetDefaultRule()
{
	return m_DefaultRule;
}

void CClientServerSocket::DoActive()
{
	CIOCPServerSocketManager::DoActive();
	CheckIpConfig(GetTickCount());
}

void CClientServerSocket::CheckIpConfig(unsigned long ulTick)
{
	if ((0 == m_ulLastCheckTick) || (ulTick - m_ulLastCheckTick >= 20 * 1000))
	{
		m_ulLastCheckTick = ulTick;
		std::string sIPConfigFileName(G_CurrentExePath + "config.ini");
		int iAge = CC_UTILS::GetFileAge(sIPConfigFileName);

		if ((iAge != -1) && (iAge != m_iIPConfigFileAge))
		{
			if (m_iIPConfigFileAge > 0)
				Log("Reload IPConfig File...", lmtMessage);

			m_iIPConfigFileAge = iAge;
			LoadIpConfigFile(sIPConfigFileName);
		}
	}
}

void CClientServerSocket::LoadIpConfigFile(const std::string& sFileName)
{

}

void CClientServerSocket::Clear()
{
	PIpRuleNode pNode;
	std::list<PIpRuleNode>::iterator vIter;
	
	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	for (vIter = m_IPRuleList.begin(); vIter != m_IPRuleList.end(); ++vIter)
	{
		pNode = (PIpRuleNode)*vIter;
		delete(pNode);
	}
	m_IPRuleList.clear();	
}

void CClientServerSocket::AddIpRuleNode(const std::string& sIP, TIpType ipType)
{
	std::string sTempIP;
	//������ַ���������Ҫ�ټ��---------------------
	//������ַ���������Ҫ�ټ��---------------------
	//������ַ���������Ҫ�ټ��---------------------
	int iPos = sIP.find('*');
	if (iPos != string::npos)
		sTempIP = sIP.substr(0, iPos-1);
	else
		sTempIP = sIP;	

	if ("" == sTempIP)
		return;
	if (sTempIP.at(sTempIP.length()) != '.')
		sTempIP = sTempIP + '.';
	
	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	PIpRuleNode pNode = new TIpRuleNode;
	pNode->ipType = ipType;
	pNode->sMatchIP = sIP;
	m_IPRuleList.push_back(pNode);
}

/*
���أ�
0 : δ֪
1 : Ĭ��(����)
2 : ����
3 : ��ͨ
4 : �ƶ�
5 : ������
*/
unsigned short CClientServerSocket::GetNetType(int nAddr)
{
	unsigned short result = 0;
	for (int i = 0; i < MAX_NET_TYPE_CONFIG; i++)
	{
		if (0 == m_NetTypes[i])
			break;
		if (nAddr == m_NetTypes[i])
		{
			result = i;
			break;
		}
	}
	return result;
}

bool CClientServerSocket::CheckConnectIP(const std::string& sIP)
{
	bool retFlag = (m_DefaultRule != itDeny);
	std::string sTempIP(sIP);
	sTempIP.append(".");

	PIpRuleNode pNode;
	std::list<PIpRuleNode>::iterator vIter;
	std::lock_guard<std::mutex> guard(m_IPRuleLockCS);
	for (vIter = m_IPRuleList.begin(); vIter != m_IPRuleList.end(); ++vIter)
	{
		pNode = (PIpRuleNode)*vIter;
		//--------------------------------------
		//--------------------------------------
		//����ж���Ҫ���Լ���һ��
		if (sTempIP.find(pNode->sMatchIP) == 1)
		{
			switch (pNode->ipType)
			{
			case itDeny:
				retFlag = false;
				break;
			case itAllow:
			case itMaster:
				retFlag = true;
				break;
			default:
				retFlag = false;
				break;
			}
		}
	}

	if (!retFlag)
	{
		std::string sTemp(sIP + " ���ӱ���ֹ!");
		Log(sTemp.c_str(), lmtWarning);
	}
	return retFlag;
}

CDGClient* CClientServerSocket::OnCreateClientSocket(const std::string& sIP)
{
	SendDebugString("OnCreateClientSocket");
	return new CDGClient;
}

void CClientServerSocket::OnSocketError(void* Sender, int& iErrorCode)
{
	if (iErrorCode != 10054)
	{
		std::string sInfo("Server Socket Error, Code = ");
		sInfo.append(to_string(iErrorCode));
		Log(sInfo.c_str(), lmtError);
	}
	iErrorCode = 0;
}

void CClientServerSocket::OnClientConnect(void* Sender)
{
	CDGClient* client = (CDGClient*)Sender;
	if (!CheckConnectIP(client->GetRemoteAddress()))
	{
		client->OpenWindow(cwMessageBox, 0, "��Ŀǰ�޷�����");
		client->ForceClose();
	}
	else
	{
		//-------------------------------------------------------------
		//-------------------------------------------------------------
		//-------------------------------------------------------------
		client->SetGMIP(/*G_GateSocket.IsMasterIP(Client.RemoteAddress)*/"");
	}
}

/************************End Of CClientServerSocket****************************************************/

