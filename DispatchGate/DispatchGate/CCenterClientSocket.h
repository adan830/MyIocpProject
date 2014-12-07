/**************************************************************************************
@author: �²�
@content: Dispatch��Ϊ�ͻ��˷�����CenterServer�������Ķ˿�
**************************************************************************************/
#ifndef __CC_CENTER_CLIENT_SOCKET_H__
#define __CC_CENTER_CLIENT_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate��Center�����������Ӷ˿�
*
*/

class CCenterClientSocket : public CIOCPClientSocketManager
{
public:
	CCenterClientSocket();
	virtual ~CCenterClientSocket();
	void LoadConfig(CWgtIniFile* pIniFileParser);
	void SendToServerPeer(unsigned short usIdent, int iParam, void* pBuf, unsigned short usBufLen);
	void DoHeartBeat();					    // ��������
protected:
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen);
private:
	void OnSocketConnect(void* Sender);
	void OnSocketDisconnect(void* Sender);
	void OnSocketRead(void* Sender, const char* pBuf, int iCount);
	void OnSocketError(void* Sender, int& iErrorCode);
	void Reconnect();                       // ����
	void SendRegisterServer();              // ע�������
private:
	unsigned long m_ulCheckTick;
	int m_iPingCount;
	int m_iWorkIndex;
	TServerAddress m_ServerArray[MAX_CENTER_SERVER_COUNT];        // �����ö��CenterServer           
};

extern CCenterClientSocket* pG_CenterSocket;

#endif //__CC_CENTER_CLIENT_SOCKET_H__