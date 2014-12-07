/**************************************************************************************
@author: �²�
@content: tcp�������ӵĵײ����� ͷ�ļ�
**************************************************************************************/
#ifndef __CC_TCP_SOCKET_COMMON_H__
#define __CC_TCP_SOCKET_COMMON_H__

#define _WINSOCKAPI_
#include <Windows.h>
#include <WinSock2.h>
#include <thread>
#include <functional>

//��������
const unsigned long SHUTDOWN_FLAG = 0XFFFFFFFF;   // iocp�˿ڹرձ�־
const int MAX_IOCP_BUFFER_SIZE = 8 * 1024;        // IOCPͶ�ݻ�������С��һ�����ó�8k���ܽϼ�
const int MAX_CLIENT_SEND_BUFFER_SIZE = MAX_IOCP_BUFFER_SIZE * 10 * 1024;  // �ͻ��˿��Է��͵�������ݻ��壬Ҳ�Ƿ�����������������ͻ��˻�����  80M 

//����TcpSocket�Ļص�����
typedef std::function<void (void* Sender)> TNotifyEvent;
typedef std::function<void (void* Sender, const char* pBuf, int iBufLen)> TOnSocketRead;   
typedef std::function<void (void* Sender, int& iErrorCode)> TOnSocketError;
typedef std::function<bool (const std::string& sIp)> TOnCheckAddress;
typedef std::function<void (HANDLE hIocp, SOCKET hSocket, std::string& sRAddress, int iRPort)> TOnSocketAccept;
class CClientConnector;
typedef std::function<CClientConnector* (const std::string& sIP)> TOnCreateClient;

//����dll�лص���������
typedef bool(*TServiceDllCallBackFunc)(void* Sender);
//����CCWindowsService.dll�����������ͨ��ģ��
typedef void(__stdcall *TServiceManagerFunc)(char* ServiceName, char* ServiceDesc, TServiceDllCallBackFunc OnStartFunc, TServiceDllCallBackFunc OnStopFunc);

/**
* ���ͻ���ڵ�ṹ
*/
typedef struct _TSendBufferNode
{
	char* szBuf;             
	int nBufLen;             // ����
	int nStart;              // ��ʼλ��
	_TSendBufferNode* Next;  // �¸��ڵ�ָ�� 
}TSendBufferNode, *PSendBufferNode;

/**
* ���ͽڵ���ɵ�����
*/
typedef struct _TSendBufferLinkedList
{
public:
	void DoInitial(const int iSize);
	void DoFinalize();
	inline bool IsEmpty(){ return nullptr == m_pFirst; };
	void AddBufferToList(const char* pBuf, int iCount);
	int GetBufferFromList(char* pDesBuf, int iBufMaxSize, int iBufUntreatedBytes);
private:
	PSendBufferNode m_pFirst;   //����ͷ�ڵ�
	PSendBufferNode m_pLast;    //����β�ڵ�
	int m_iNodeCacheSize;       //�ڵ�Ĵ洢��С
}TSendBufferLinkedList;

//�����ö������
enum TSocketErrorType {seConnect, seRead, seSend, seClose};

//socket�¼���ö������
enum TSocketEvent {soIdle, soWrite, soRead};

/**
* �ص�io�Ľṹ��
*/
typedef struct _TBlock
{
	OVERLAPPED Overlapped;					//�ص�
	WSABUF wsaBuffer;						//ϵͳ����
	TSocketEvent Event;						//���Socket��д
	char Buffer[MAX_IOCP_BUFFER_SIZE - 1];	//�û�����
}TBlock, *PBlock;

/*
* ��������ʵ�֣��Ȼ���������Ч���������ߣ�,��ռ��cpu
*/
enum {LOCK_IS_FREE = 0, LOCK_IS_TAKEN = 1};
#define LOCK(l) while(InterlockedCompareExchange(&l, LOCK_IS_TAKEN, LOCK_IS_FREE) == 1) {};
#define UNLOCK(l) InterlockedExchange(&l, LOCK_IS_FREE);

//ͨ��WSAStartup������ɶ�Winsock����ĳ�ʼ��
bool DoInitialWinSocket();

//��ֹWinsock 2 DLL (Ws2_32.dll) ��ʹ��
void DoFinalizeWinSocket();

//Debug��Ϣ
void SendDebugString(const std::string& sInfo);

//�жϵ�ǰip�Ƿ�������ip�����ݵ�ǰ��������������
bool IsInternetIP(const u_long ulIP);

//���ر����Ķ���ip��ַ
std::string GetInternetIP(const std::string& sDefaultIP = "");

/*
* ���ص��߳������еĶ������
*
*/
class CExecutableBase
{
public:
	CExecutableBase();
	virtual ~CExecutableBase();
	void InitialWorkThread();
	void Execute();
	virtual void DoExecute() = 0;
	std::thread* m_pThread;          // �ڲ�ִ���߳�ָ��
protected:
	void WaitThreadExecuteOver();
	void Terminate();
	bool IsTerminated();
protected:
	HANDLE m_Event;                  // �����¼����
	std::string m_sThreadName;       // �߳�����
private:
	bool m_bTerminated;              // ֹͣExecute������ѭ��ִ�б��
	bool m_bExecuteOver;             // Executeִ�����
};

#endif //__CC_TCP_SOCKET_COMMON_H__