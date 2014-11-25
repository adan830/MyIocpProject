/**************************************************************************************
@author: �²�
@content: tcp�������ӵĵײ��-�ͻ��˶˿ڹ���ͷ�ļ�
**************************************************************************************/
#ifndef __CC_TCP_CLIENT_SOCKET_H__
#define __CC_TCP_CLIENT_SOCKET_H__

#include <mutex>
#include "CCUtils.h"
#include "CCTcpSocketCommon.h"
#include "CCProtocol_Server.h"

/**
*  
* �ͻ��˵�Socket���������õ��߳������¼�ģ��ʵ�� ---- ע�����ģ�͵�ʵ�ֻ���Ҫ����ĵ�����������
*
*/
class CNetworkEventClientSocketManager
{
public:
    CNetworkEventClientSocketManager();
	virtual ~CNetworkEventClientSocketManager();
    unsigned int SendBuf(const char* pBuf, unsigned int Len, bool BoFree = false);
    bool Open();
    bool Close();
    bool Execute();  
	bool IsActive(){ return m_BoActive; }
	bool IsConnected(){ return m_BoConnected; }
public:
    std::string m_LocalAddress;			// ����ip
    std::string m_Address;				// Զ��ip
    int m_Port;							// Զ��port
    TOnSocketRead m_OnRead;
	TOnSocketError m_OnError;
    TNotifyEvent m_OnConnect;
    TNotifyEvent m_OnDisConnect;    
private:
    void DoRead();
    void DoWrite();
    bool DoError(TSocketErrorType seType);
    bool DoInitialize();
    void DoSend();
private:
	SOCKET m_CSocket;                   // ԭʼ�׽���
	HANDLE m_Event;                     // �����¼����
    std::mutex m_SendCS;                // ���Ͷ���ʹ�õĻ�����
    PSendBufferNode m_First, m_Last;    // ���ͻ���������
    int m_Count;                        // ���Ͷ��еĸ���
    bool m_BoActive;                    // WSAConnect�ɹ�����״̬, Closeʱ�ر�
	bool m_BoConnected;					// DoWrite����״̬, Closeʱ�ر�
};

/**
*  
* �ͻ��˵�Socket����������IOCPģ��ʵ��
*
* ע��������ͨ����������ʵ���ٽ����Ĺ���
*	lock_guard �����ٽ����Ĵ����������쳣���������µ�����
*	{			
*		std::lock_guard<std::mutex> guard(m_SendCS); 
*		//�����Ĵ����
*	}
*   �뿪�������飬guard��Ҫ�ͷ�
*
* ����ῼ��ͨ����������ʵ�֣�CCTcpSocketCommon.h �ж���� LOCK �� UNLOCK
* ��������sleep������Ч�ʸ��ߣ�����Ϊ�쳣���������Ŀ����Ըо�����һЩ�������Ҫ��һ������
*/
class CIOCPClientSocketManager : public CExecutableBase
{
public:
	CIOCPClientSocketManager();
	virtual ~CIOCPClientSocketManager();
    int SendBuf(const char* pBuf, int Count);
    bool Open();
    void Close(bool BoClearBuffer = true);
	bool IsActive(){ return m_BoActive; }
	bool IsConnected(){ return m_BoConnected; }
	void SetReconnectInterval(unsigned long interval){ m_Reconnect_Interval = interval; }
    void DoExecute();						//���಻���ش˷���
public:
    TOnSocketRead m_OnRead;
	TOnSocketError m_OnError;
    TNotifyEvent m_OnConnect;
    TNotifyEvent m_OnDisConnect;  
    std::string m_LocalAddress;			// ����ip
    std::string m_Address;				// Զ��ip
    int m_Port;							// Զ��port
	int m_TotalBufferlen;               // buffer�ܳ���
protected:
	int ParseSocketReadData(int iType, const char* pBuf, int iCount);                                //�������onsocketread����
	virtual void ProcessReceiveMsg(PServerSocketHeader pHeader, const char* pData, int iDataLen){};  //����������Ϣ�����ݣ�����ʵ��
private:
	bool DoInitialize();
	bool DoError(TSocketErrorType seType);
	bool DoConnect();
	void DoQueued();
	void DoClose();
	void Clear();
	void PrepareSend(int iUntreated, int iTransfered);
	bool PrepareRecv();
	void IocpSendback(int Transfered);
	bool IocpReadback(int Transfered);
private:
    bool m_BoActive;                    // 
	bool m_BoConnected;					// ��Զ�˶˿ڵ����ӱ��
	bool m_Sending;                     //
	HANDLE m_hIOCP;                     // iocp�ľ��
	SOCKET m_CSocket;                   // ԭʼ�׽���	
	std::mutex m_LockCS;                // ���в���ʹ�õĻ�����
	unsigned long m_SendLock;			// ���������� ���� ����δʹ�ã�
	unsigned long m_RecvLock;           // ���������� ���� ����δʹ�ã�
	TSendBufferLinkedList m_SendList;   // ���ͻ���������
	TBlock m_SendBlock;                 // ���ڷ��͵�
	TBlock m_RecvBlock;                 // ���ڽ��յ�
    SOCKADDR_IN m_SocketAddr;					   // 
	unsigned long m_Reconnect_Interval;			   // �������
	CC_UTILS::PBufferStream m_pReceiveBuffer;      // ����socket���ݽ��յ�buffer
};

#endif //__CC_TCP_CLIENT_SOCKET_H__