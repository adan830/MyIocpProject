/**************************************************************************************
@author: �²�
@content: Dispatch��DB���������ӵļ���socket����
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"
#include "CClientServerSocket.h"
#include "CPigClientSocket.h"

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
	void SendToClientPeer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen);
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
	std::hash<std::string> myhash;
/*
public:
	CDBServerSocket(const std::string& sName);
	virtual ~CDBServerSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	int SelectServer(CDGClient &client);
	void SendSelectServer(CDGClient &client);
	void SendAreaInfoToPig(CPigClientSocket &pigClient);
	void SendPigMsg(const char* pBuf, unsigned short usBufLen);
	int GetPlayerTotalCount();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CClientConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
	void OnSetListView(void* Sender);

	void LoadAreaConfig();
	bool RegisterDBServer(const std::string &sAddress, int iServerID, CDBConnector &dbServer);
	void ShowDBMsg(int iServerID, int iCol, const std::string &msg);
	std::string OnLineDBServer(int iServerID);

	//-----------------------------
	//-----------------------------
	//-----------------------------
	//procedure RemoveServerInfo(Pvalue: Pointer; const Key: ansistring);
	//procedure EnumAreaConfig(ElName: string; Elem: TlkJSONbase;data: pointer; var Continue: Boolean);
private:
	std::string m_sAllowDBServerIP;				// �����IP
	int m_iSessionID;           
	std::string m_sServerName;
	int m_iConfigFileAge;
	unsigned long m_ulLastCheckTick;
	//FLogSocket: TLogSocket;
	//FServerList: TNamesHash;                  //  �����б� 
*/
};

#endif //__CC_DB_SERVER_SOCKET_H__