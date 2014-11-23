/**************************************************************************************
@author: �²�
@content: Dispatch��DB���������ӵļ���socket����
**************************************************************************************/
#ifndef __CC_DB_SERVER_SOCKET_H__
#define __CC_DB_SERVER_SOCKET_H__

#include "CCTcpServerSocket.h"

/**
*
* DispatchGate�����ĵ���DBServer�����Ӷ���
*
*/
class CDBConnector : public CClientConnector
{
public:
	CDBConnector();
	~CDBConnector();
	int GetServerID();
	int GetPlayerCount();
	void SendToClientPeer(unsigned short usIdent, int iParam, char* pBuf, unsigned short usBufLen);
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckClientIP(const std::string& sIP);
protected:
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void SendHeartBeat(int iCount);                // ��������
	void RegisterDBServer(int iServerID);          // ע��DBServer
	void ClearIPRule(TIpType ipType);
	void ReceiveConfig(int iParam, char* pBuf, unsigned short usBufLen);
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
	CDBServerSocket();
	virtual ~CDBServerSocket();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CDBConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_DB_SERVER_SOCKET_H__