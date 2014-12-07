/**************************************************************************************
@author: �²�
@content: Dispatch�Կͻ������ӵļ���socket����
**************************************************************************************/
#ifndef __CC_CLIENT_SERVER_SOCKET_H__
#define __CC_CLIENT_SERVER_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate�����ĵ����ͻ������Ӷ���
*
*/
class CDGClient : public CClientConnector
{
public:
	CDGClient();
	virtual ~CDGClient();
	void ForceClose();
	void OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg = "");
	unsigned short GetSelectMaskServerID();
	int GetSelectRealServerID();
	bool GetIsGMIP();
	void SetGMIP(const bool bFlag);
	int GetEncodeIdx();
	int GetClientType();
	unsigned char GetNetType();
	void SendToClientPeer(unsigned short usIdent, void* pData, unsigned short usDataLen);
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void CMSelectServer(char* pBuf, unsigned short usBufLen);
	void CMCloseWindow(char* pBuf, unsigned short usBufLen);
	void ProcessReceiveMsg(char* pHeader, char* pData, int iDataLen);
private:
	unsigned long m_ulLastConnectTick;
	unsigned long m_ulForceCloseTick;
	unsigned short m_usSelectMaskServerID;       //���ѡ��ķ������������Եı��
	int m_iEncodeIdx;                            //���ܱ��
	int m_iSelectRealServerID;                   //���ѡ��ķ�������ʵ���
	bool m_bIsGMIP;								 //�ж��Ƿ�ΪgmIP��
	int m_iClientType;							 //��ͬ�Ŀͻ������ͣ�ʹ�ò�ͬ�İ�ͷ��Ƿ�
	unsigned char m_ucNetType;                   //�ͻ��˵���������---ͨ���ͻ��˶Թ̶���ַ�Ľ��������жϲ�ͬ����������
};

const int MAX_NET_TYPE_CONFIG = 9;

/**
*
* DispatchGate�Կͻ��˵ļ���������
*
*/
class CClientServerSocket : public CIOCPServerSocketManager
{
public:
	CClientServerSocket();
	virtual ~CClientServerSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	bool IsMasterIP(std::string &sIP);
	void SMSelectServer(int iSocketHandle, char* pBuf, unsigned short usBufLen);  // ����ѡ����Ϣ
	TIpType GetDefaultRule();
	unsigned char GetNetType(int nAddr);
protected:
	virtual void DoActive();
private:
	void CheckIpConfig(unsigned long ulTick);
	void LoadIpConfigFile(const std::string& sFileName);
	void Clear();
	void AddIpRuleNode(const std::string& sIP, TIpType ipType);
	bool CheckConnectIP(const std::string& sIP);

	CClientConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
private:
	unsigned long m_ulLastCheckTick;
	int m_iIPConfigFileAge;								 //��¼ipconfig�ļ��İ汾��
	TIpType m_DefaultRule;								 //Ĭ�ϵ�ip����
	std::string m_sWarWarning;							 //����ս����ʾ
	std::mutex m_IPRuleLockCS;							 //iprule������ٽ�������ʹ�õĻ�����
	std::list<PIpRuleNode> m_IPRuleList;				 //iprule����
	unsigned long m_NetTypes[MAX_NET_TYPE_CONFIG];      //���õĸ��ݿͻ��˶��ڹ̶������Ĳ�ͬ�������жϵ���������
};

extern CClientServerSocket* pG_GateSocket;

#endif //__CC_CLIENT_SERVER_SOCKET_H__