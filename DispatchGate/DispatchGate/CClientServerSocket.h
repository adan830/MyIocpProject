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
	~CDGClient();
	void ForceClose();
	void OpenWindow(TClientWindowType wtype, int iParam, const std::string& msg = "");
	unsigned short GetSelectMaskServerID();
	int GetSelectRealServerID();
	bool GetIsGMIP();
	int GetEncodeIdx();
	int GetClientType();
	unsigned char GetNetType();
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	void CMSelectServer(char* pBuf, unsigned short usBufLen);
	void CMCloseWindow(char* pBuf, unsigned short usBufLen);
	void SendToClient(unsigned short usIdent, char* pData, unsigned short usDataLen);
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
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CDGClient* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_CLIENT_SERVER_SOCKET_H__