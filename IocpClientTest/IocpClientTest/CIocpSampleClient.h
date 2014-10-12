/**************************************************************************************
@author: �²�
@content: ʹ��IOCP�ͻ��˿�ܿ��ٹ�������iocp�Ŀͻ���
		  ͨ���� CIOCPClientSocketManager�ļ̳п��ٹ����ͻ��˵����ӹ���
**************************************************************************************/
#ifndef __CC_IOCP_SAMPLE_CLIENT_H__
#define __CC_IOCP_SAMPLE_CLIENT_H__

#include "CCTcpClientSocket.h"

/**
*  
* ����IOCP�Ŀͻ��˵�һ������������
*
*/
class CSampleClientManager : public CIOCPClientSocketManager
{
public:
	CSampleClientManager();
	virtual ~CSampleClientManager();
	void ConnectToServer(const std::string& sIP, const int iPort);
	void Disconnect();
	bool SendToServer();
private:
	void OnSocketConnect(void* Sender);
	void OnSocketDisconnect(void* Sender);
	void OnSocketRead(void* Sender, const char* pBuf, int iCount);
	void OnSocketError(void* Sender, int& iErrorCode);
};

#endif //__CC_IOCP_SAMPLE_CLIENT_H__