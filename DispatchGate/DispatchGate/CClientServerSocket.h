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
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	unsigned long m_ulLastRunTick;
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