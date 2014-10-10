/**************************************************************************************
@author: �²�
@content: ʹ��IOCP��������ܿ��ٹ���������
		ͨ���� CIOCPServerSocketManager �� CClientConnector�ļ̳п��ٹ���������
**************************************************************************************/
#ifndef __CC_IOCP_SAMPLE_SERVER_H__
#define __CC_IOCP_SAMPLE_SERVER_H__

#include "CCTcpServerSocket.h"

/**
*  
* CClientConnector������sample
*
*/
class CSampleConnector : public CClientConnector
{
public:
	virtual void OnCreate();
	virtual void OnDestroy();
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	unsigned long m_ulLastRunTick;
};

/**
*  
* CIOCPServerSocketManager������sample
*
*/
class CSampleServerManager : public CIOCPServerSocketManager
{
public:
	virtual void OnCreate();
	virtual void OnDestroy();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CSampleConnector* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_IOCP_SAMPLE_SERVER_H__