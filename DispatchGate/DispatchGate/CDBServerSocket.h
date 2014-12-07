/**************************************************************************************
@author: �²�
@content: Dispatch��DB���������ӵļ���socket����
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"
#include "CClientServerSocket.h"
#include "CPigClientSocket.h"

//dispatch��ʹ�õķ�������������
typedef struct _TServerConfigInfo
{
	int iMaskServerID;
	std::string sServerName;
	int iRealServerID;
	std::string sServerIP;
}TServerConfigInfo, *PServerConfigInfo;

/**
*
* DispatchGate�����ĵ���DBServer�����Ӷ���
*
*/
class CDBConnector : public CClientConnector
{
public:
	CDBConnector();
	virtual ~CDBConnector();
	int GetServerID();
	int GetPlayerCount();
	std::string& GetServerName();
	std::string& GetDenyHint();
	TIpType GetDefaultRule();
	void SetServerName(const std::string& sName);
	void SendToClientPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, char* pData, int iDataLen);
private:
	void SendHeartBeat(int iCount);                // ��������
	void RegisterDBServer(int iServerID);          // ע��DBServer
	void ClearIPRule(TIpType ipType);
	void ReceiveConfig(int iParam, const char* pBuf, unsigned short usBufLen);
private:
	int m_iServerID;                //������ʵ������
	int m_iPlayerCount;             //DB�ϵ��������
	TIpType m_DefaultRule;          
	std::string m_sDenyHint;			   //���ӷ��������Ĭ����ʾ
	std::string m_sServerName;             //����������
	std::mutex m_IPRuleLockCS;			   //iprule������ٽ�������ʹ�õĻ�����
	std::list<PIpRuleNode> m_IPRuleList;   //iprule����
};

/**
*
* DispatchGate��DBServer�ļ���������
*
*/
class CDBServerSocket : public CIOCPServerSocketManager
{
public:
	CDBServerSocket(const std::string& sName);
	virtual ~CDBServerSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	int SelectServer(CDGClient* pClient);
	void SendSelectServer(CDGClient* pClient);
	void SendServerInfoToPig(CPigClientSocket* pPigClient);
	void SendPigMsg(char* pBuf, unsigned short usBufLen);
	int GetPlayerTotalCount();
	void ShowDBMsg(int iServerID, int iCol, const std::string &msg);
	bool RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector* pDBServer);
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CClientConnector* OnCreateDBSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnDBConnect(void* Sender);
	void OnDBDisconnect(void* Sender);
	void OnSetListView(void* Sender);

	void LoadServerConfig();
	std::string OnLineDBServer(int iServerID);
	void RemoveServerInfo(void* pValue, const std::string &sKey);
	//-----------------------------
	//-----------------------------
	//-----------------------------
	//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);
private:
	std::string m_sAllowDBServerIP;				// �����IP
	int m_iSessionID;           
	std::string m_sServerName;
	int m_iConfigFileAge;
	unsigned long m_ulLastCheckTick;
	CC_UTILS::CLogSocket* m_pLogSocket;			// ������־����Ķ˿�
	CC_UTILS::CStringHash m_ServerHash;         // �����б� 
};

extern CDBServerSocket* pG_DBSocket;

#endif //__CC_DB_SERVER_SOCKET_H__