/**************************************************************************************
@author: �²�
@content:
**************************************************************************************/

#include "CCTcpServerSocket.h"
#include "stdlib.h"
#pragma comment(lib, "ws2_32.lib")

const int DEFAULT_CLIENT_CORPSE_TIME = 20 * 60 * 1000;      // Ĭ�Ϸ�������ͻ�����ͨ�ź����ʱ��
const int MAX_HASH_BUCKETS_SIZE = 5071;                     // ��������Hash������ֵ
const unsigned long MAX_CLIENT_DELAY_TIME = 15 * 1000;      // SocketHandle�ĳ�ʱ����ʱ��
const unsigned long MAX_BLOCK_CONTINUE_TIME = 10000;        // ������������ʱ��
const unsigned long SEND_NODE_CACHE_SIZE = 16 * 1024;      // ÿ���ڵ�ķ�������С

/************************Start Of CSubIOCPWorker****************************************************/
CSubIOCPWorker::CSubIOCPWorker(PHANDLE ph, TNotifyEvent evt, std::string& sName) : m_pHIOCP(ph), m_OnSocketClose(evt)
{
	m_sThreadName = sName;
}

CSubIOCPWorker::~CSubIOCPWorker()
{
	WaitThreadExecuteOver();
}

void CSubIOCPWorker :: DoExecute()
{
	SetThreadLocale(0X804);
	while (!IsTerminated())
	{
		try
		{			
			unsigned long ulBytesTansfered = 0;
			ULONG_PTR key = NULL;
			PBlock pRBlock = nullptr;
			int Ret = GetQueuedCompletionStatus(*m_pHIOCP, &ulBytesTansfered, &key, (LPOVERLAPPED*)&pRBlock, INFINITE);
			//PostQueuedCompletionStatus���͵Ĺر���Ϣ
			if (SHUTDOWN_FLAG == (unsigned long)(pRBlock))
			{
				SendDebugString(m_sThreadName + ":receive SHUTDOWN_FLAG");
				break;
			}

			if (NULL != key)
			{
				/*
				MSDN:
				BOOL GetQueuedCompletionStatus(HANDLE CompletionPort, LPDWORD lpNumberOfBytes, PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped, DWORD dwMilliseconds);
				1.�����������ɶ˿�ȡ��һ���ɹ�I/O��������ɰ�������ֵΪ��0��������ָ��lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped�Ĳ����д洢�����Ϣ��
				2.��� *lpOverlappedΪ�ղ��Һ���û�д���ɶ˿�ȡ����ɰ�������ֵ��Ϊ0�������򲻻���lpNumberOfBytes and lpCompletionKey��ָ��Ĳ����д洢��Ϣ��
				3.��� *lpOverlapped��Ϊ�ղ��Һ�������ɶ˿ڳ���һ��ʧ��I/O��������ɰ�������ֵΪ0��������ָ��lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped�Ĳ���ָ���д洢�����Ϣ��
				*/
				if ((0 == Ret) || (ulBytesTansfered == 0))
				{
					//��Ϊkey!=NULL  ���Բ����ǵڶ������
					//������Ҫ���������������ر�socket
					if (nullptr != m_OnSocketClose)
						m_OnSocketClose((void*)key);
					continue;
				}
				//������պͷ���
				CClientConnector* client = (CClientConnector*)key;
				switch (pRBlock->Event)
				{
					case soRead:
						pRBlock->Event = soIdle;
						if (!client->IocpReadback(ulBytesTansfered))
						{
							if (nullptr != m_OnSocketClose)
								m_OnSocketClose(client);
						}
						break;
					case soWrite:
						pRBlock->Event = soIdle;
						client->IocpSendback(ulBytesTansfered);
						break;
					default:
						if (nullptr != m_OnSocketClose)
							m_OnSocketClose(client);
						break;
				}

			}
		}
		catch(...)
		{
			//SendDebugString('CSubIOCPWorker :: Execute Exception');
		}
	}
}

void CSubIOCPWorker :: ShutDown()
{
	m_OnSocketClose = nullptr;
	PostQueuedCompletionStatus(*m_pHIOCP, 0, 0, (LPOVERLAPPED)SHUTDOWN_FLAG);
	SendDebugString(m_sThreadName + ":send SHUTDOWN_FLAG");
}

/************************End Of CSubIOCPWorker******************************************************/



/************************Start Of CMainIOCPWorker***************************************************/

//WSAAccept�������ж�Զ�������Ƿ�ɽ���
int CALLBACK ConditionFunc(
  IN LPWSABUF lpCallerId,			//The lpCallerId parameter points to a WSABUF structure that contains the address of the connecting entity. 
									//The buf portion of the WSABUF pointed to by lpCallerId points to a sockaddr. The sockaddr structure is interpreted 
									//according to its address family (typically by casting the sockaddr to some type specific to the address family).
  IN LPWSABUF lpCallerData,			//����ʵ�帽������������[һ������Э�鲻֧��]
  IN OUT LPQOS lpSQOS,				//��������ʱ��Ҫ�Ĵ���
  IN OUT LPQOS lpGQOS,				//�׽�����������Ҫ�Ĵ���[һ������Э�鲻֧��]
  IN LPWSABUF lpCalleeId,			//The lpCalleeId is a parameter that contains the local address of the connected entity. �ṹ��lpCallerId��ͬ
  OUT LPWSABUF lpCalleeData,		//������Ϊ���������һ���ַ��ظ��ͻ���[һ������Э�鲻֧��]
  OUT GROUP FAR *g,
  IN DWORD_PTR dwCallbackData		//The dwCallbackData parameter value passed to the condition function is the value passed as the dwCallbackData parameter in the original WSAAccept call.
									//This value is interpreted only by the Windows Socket version 2 client. This allows a client to pass some context information from the WSAAccept call site 
									//through to the condition function. This also provides the condition function with any additional information required to determine whether to accept the 
									//connection or not. A typical usage is to pass a (suitably cast) pointer to a data structure containing references to application-defined objects with which
									//this socket is associated.
)
{
	std::string sIPAddress = inet_ntoa(((PSOCKADDR_IN)(lpCallerId->buf))->sin_addr);
	int iPort = ntohs(((PSOCKADDR_IN)(lpCallerId->buf))->sin_port);
	int iIPAddress = inet_addr(sIPAddress.c_str());
	CIOCPServerSocketManager* pServer = (CIOCPServerSocketManager*)dwCallbackData;
	//����ֻ�ж�Զ�����ӵ�IP��ַ�Ƿ�Ϸ�
	if ((pServer != nullptr) && (pServer->DoCheckConnect(sIPAddress)))
		return CF_ACCEPT;
	else
		return CF_REJECT;
}

CMainIOCPWorker::CMainIOCPWorker(void* parent) : m_OnSocketError(nullptr), m_OnSocketClose(nullptr), m_OnReady(nullptr),
m_OnSocketAccept(nullptr), m_Parent(parent), m_hIOCP(0), m_Socket(INVALID_SOCKET), m_iSubThreadCount(0), m_SubWorkers(nullptr)
{
}

CMainIOCPWorker :: ~CMainIOCPWorker()
{
	//�ȹرն˿ڣ������ñ�֤WSAAccept����
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	WaitThreadExecuteOver();
}

void CMainIOCPWorker :: DoExecute()
{
	SetThreadLocale(0X804);
	//listen�ĵڶ��������ǵȴ����Ӷ��е���󳤶ȣ�Ĭ��ֵһ����5����windows�����������õ�200�����߸��ߣ����ݲ�ͬ�汾������
	//��Ϊ�������ˣ�һ��accept���������ޣ�һ��ֻ�ܴ���һ���������󣬺�������û���������������Ҳ��ϵͳѹ��һ���Ժ���������ˣ�����Ͷ�ݵ�accept���󶼻�õ��ܿ췵��
	int iError = 0;
	if (listen(m_Socket, 5) != 0)
	{
		iError = GetLastError();
		if (nullptr != m_OnSocketError)
			m_OnSocketError(this, iError);
		Terminate();
		return;
	}
	else
	{
		//�����˿ڿ����󣬴����ӹ�������---�ڲ��߳�
		MakeWorkers();
	}

	SOCKET stClientSocket = INVALID_SOCKET;
	while (!IsTerminated())
	{
		try
		{
			if (INVALID_SOCKET == m_Socket)
			{
				Terminate();
				break;
			}

			SOCKADDR_IN ToAddress; 			
			memset(&ToAddress, 0, sizeof(ToAddress));
			int iAddressLen = sizeof(ToAddress);
			//�������ؽ���һ�����ӻ���״̬�����ķ���ֵ,ѡ�񴴽������һ���׽�����
			stClientSocket = WSAAccept(m_Socket, (sockaddr*)&ToAddress, &iAddressLen, &ConditionFunc, (unsigned long)m_Parent);
			if (IsTerminated())
				break;
			if (INVALID_SOCKET != stClientSocket)
			{
				if (inet_ntoa(ToAddress.sin_addr) != nullptr)
				{
					std::string sIPAddress = inet_ntoa(ToAddress.sin_addr);
					unsigned short iPort = ntohs(ToAddress.sin_port);
					if (m_OnSocketAccept != nullptr)
						m_OnSocketAccept(m_hIOCP, stClientSocket, sIPAddress, iPort);
				}
				else
					closesocket(stClientSocket);
			}
			else
			{
				iError = GetLastError();
				if (nullptr != m_OnSocketError)
					m_OnSocketError(this, iError);
			}
		}
		catch(...)
		{
			if (nullptr != m_OnSocketError)
				m_OnSocketError(this, iError);
		}
	}

	//��ִ���������رյ���Ӧ��Դ
	Close();
}

void CMainIOCPWorker :: MakeWorkers()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_iSubThreadCount = si.dwNumberOfProcessors * 2 + 1;
	m_SubWorkers = new CSubIOCPWorker*[m_iSubThreadCount];
	for (int i=0; i<m_iSubThreadCount; i++)
	{
		m_SubWorkers[i] = new CSubIOCPWorker(&m_hIOCP, m_OnSocketClose, std::to_string(i));
		m_SubWorkers[i]->InitialWorkThread();
	}
	if (nullptr != m_OnReady)
		m_OnReady(this);
}

void CMainIOCPWorker :: Close()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	if (m_SubWorkers != nullptr)
	{
		//ע�������ڹر����̵߳�ʱ��Ҫ��ͳһShutDown�����ͷ�CSubIOCPWorker����ָ��
		//���ͷ�CSubIOCPWorker��ʱ�����ж��߳�ִ����ϻ��ߵȴ�m_Event�ź���
		//���ShutDown��ͬʱ�ͷţ�����ܵ���û�з����㹻��SHUTDOWN_FLAG����ɶ˿ڣ�
		//�Ӷ��������̻߳���������״̬
		for (int i=m_iSubThreadCount-1; i>=0; i--)
		{
			if (m_SubWorkers[i] != nullptr)
				m_SubWorkers[i]->ShutDown();
		}
		for (int i=m_iSubThreadCount-1; i>=0; i--)
		{
			if (m_SubWorkers[i] != nullptr)
				delete m_SubWorkers[i];
		}
		m_SubWorkers = nullptr;
	}

	if (m_hIOCP > 0)
	{
		CloseHandle(m_hIOCP);
		m_hIOCP = 0;
	}
}

bool CMainIOCPWorker :: Start(const std::string& sIP, int iPort)
{
	bool retflag = false;
	if (m_hIOCP > 0)
		return retflag;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_hIOCP > 0)
	{
		m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (m_Socket != INVALID_SOCKET)
		{
			SOCKADDR_IN Addr; 
			memset(&Addr, 0, sizeof(Addr));
			Addr.sin_family = AF_INET;
			if ((sIP.compare("") == 0) || (sIP.compare("0.0.0.0") == 0))
				Addr.sin_addr.s_addr = INADDR_ANY;
			else
				Addr.sin_addr.s_addr = inet_addr(sIP.c_str());

			if (Addr.sin_addr.s_addr = (ULONG)INADDR_NONE)
			{
				PHOSTENT HostEnt = gethostbyname(sIP.c_str());
				if (HostEnt != nullptr) 
					Addr.sin_addr.s_addr = ((PIN_ADDR)(HostEnt->h_addr))->s_addr;
				else
					Addr.sin_addr.s_addr = INADDR_ANY;
			}
			WSAHtons(m_Socket, iPort, &Addr.sin_port);
			retflag = (bind(m_Socket, (sockaddr *)&Addr, sizeof(Addr)) == 0);
		}
	}
	if (!retflag)
	{
		int iError = GetLastError();
		if (nullptr != m_OnSocketError)
			m_OnSocketError(this, iError);
		Close();
	}
	return retflag;
}
/************************End Of CMainIOCPWorker*****************************************************/



/************************Start Of CClientConnector**************************************************/

CClientConnector :: CClientConnector():m_Socket(INVALID_SOCKET), m_sRemoteAddress(""), m_iRemotePort(0), m_SocketHandle(0),
	m_bSending(false), m_bSafeClose(false), m_OnSocketError(nullptr), m_iTotalBufferLen(0), m_ulActiveTick(GetTickCount()),
	m_ulLastSendTick(0), m_ulBufferFullTick(0)
{
	memset(&m_SendBlock, 0, sizeof(m_SendBlock));
	memset(&m_RecvBlock, 0, sizeof(m_RecvBlock));
	m_SendList.DoInitial(SEND_NODE_CACHE_SIZE);
	m_pReceiveBuffer = new CC_UTILS::TBufferStream;
	m_pReceiveBuffer->Initialize();
}

CClientConnector :: ~CClientConnector()
{
	Close();
	Clear();
	m_pReceiveBuffer->Finalize();
	delete(m_pReceiveBuffer);
}

std::string& CClientConnector::GetRemoteAddress()
{
	return m_sRemoteAddress;
}

void CClientConnector :: Close()
{
	SOCKET s = m_Socket;
	m_Socket = INVALID_SOCKET;
	if (s != INVALID_SOCKET)
		closesocket(s);
}

void CClientConnector :: SafeClose()
{
	m_bSafeClose = true;
}

int CClientConnector :: SendBuf(const char* pBuf, int iCount)
{
	int iSendLen = 0;
	if (iCount > 0)
	{
		std::lock_guard<std::mutex> guard(m_LockCS); 
		iSendLen = iCount;
		m_iTotalBufferLen += iCount;
		m_SendList.AddBufferToList(pBuf, iCount);
	}
	return iSendLen;
}

int CClientConnector :: SendText(const std::string& s)
{
	int iSendLen = 0;
	int iStrLen = s.length();
	if (iStrLen > 0)
	{
		//------------------------------------
		//------------------------------------
		//�ַ�����β'/0'��������������δ���ͳ�ȥ
		char* pBuf = (char*)malloc(iStrLen + 1);
		memcpy(pBuf, s.c_str(), iStrLen + 1);
		iSendLen = SendBuf(pBuf, iStrLen);
		free(pBuf);
	}
	return iSendLen;
}

void CClientConnector :: UpdateActive()
{
	m_ulActiveTick = GetTickCount();
}

void CClientConnector :: Clear()
{
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_SendList.DoFinalize();
}

void CClientConnector :: PrepareSend(int iUntreated, int iTransfered)
{
	if (INVALID_SOCKET == m_Socket)
		return;
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_iTotalBufferLen -= iTransfered;
	if (m_iTotalBufferLen < 0)
		m_iTotalBufferLen = 0;
	iUntreated = m_SendList.GetBufferFromList(m_SendBlock.Buffer, MAX_IOCP_BUFFER_SIZE, iUntreated);

	m_bSending = false;
	if (iUntreated > 0)
	{
		UpdateActive();
		m_SendBlock.Event = soWrite;
		m_SendBlock.wsaBuffer.len = iUntreated;
		m_SendBlock.wsaBuffer.buf = m_SendBlock.Buffer;
		memset(&m_SendBlock.Overlapped, 0, sizeof(m_SendBlock.Overlapped));
		if (m_Socket != INVALID_SOCKET)
		{
			if (WSASend(m_Socket, &m_SendBlock.wsaBuffer, 1, (LPDWORD)&iTransfered, 0, &m_SendBlock.Overlapped, nullptr) == SOCKET_ERROR)
			{
				int iErrorCode = WSAGetLastError();
				//ERROR_IO_PENDING -- �ص�I/O���صı�־��,��ʾ���ڵȴ�I/O���Ժ�ͻ᷵��
				if (iErrorCode != ERROR_IO_PENDING)
				{
					if (nullptr != m_OnSocketError)
						m_OnSocketError(this, iErrorCode);
					Close();
					return;
				}
			}
			m_bSending = true;
		}
	}
}

bool CClientConnector :: PrepareRecv()
{
	bool retflag = false;
	if (INVALID_SOCKET == m_Socket)
		return retflag;
	UpdateActive();
	int iErrorCode = 0;
	try
	{
		m_RecvBlock.Event = soRead;
		m_RecvBlock.wsaBuffer.len = MAX_IOCP_BUFFER_SIZE;
		m_RecvBlock.wsaBuffer.buf = m_RecvBlock.Buffer;
		memset(&m_RecvBlock.Overlapped, 0, sizeof(m_RecvBlock.Overlapped));
		unsigned long ulFlag = 0;
		unsigned long ulTransfered = 0;
		if (m_Socket != INVALID_SOCKET)
		{
			retflag = (WSARecv(m_Socket, &m_RecvBlock.wsaBuffer, 1, &ulTransfered, &ulFlag, &m_RecvBlock.Overlapped, nullptr) != SOCKET_ERROR);
			if (!retflag)
			{
				iErrorCode = WSAGetLastError();
				if (iErrorCode == ERROR_IO_PENDING)
					return true;
				else
				{
					if (nullptr != m_OnSocketError)
						m_OnSocketError(this, iErrorCode);
					Close();
				}
			}
		}
	}
	catch(...)
	{
		iErrorCode = GetLastError();
		if (nullptr != m_OnSocketError)
			m_OnSocketError(this, iErrorCode);
	}
	return retflag;
}

void CClientConnector :: IocpSendback(int iTransfered)
{
	int iRemainLen = m_SendBlock.wsaBuffer.len - iTransfered;
	if (iRemainLen > 0)
		memmove(&m_SendBlock.Buffer[0], &m_SendBlock.Buffer[iTransfered], iRemainLen);
	else
		iRemainLen = 0;
	PrepareSend(iRemainLen, iTransfered);
}

bool CClientConnector :: IocpReadback(int iTransfered)
{
	bool retflag = false;
	if (iTransfered > 0)
	{
		try
		{
			SocketRead(m_RecvBlock.Buffer, iTransfered);
			if (m_Socket != INVALID_SOCKET)
				retflag = PrepareRecv();
		}
		catch(...)
		{
			if (m_Socket != INVALID_SOCKET)
				retflag = PrepareRecv();
		}
	}
	return retflag;
}

void CClientConnector :: DoActive(unsigned long ulTick)
{
	if ((m_bSafeClose) && (!m_bSending) && (m_SendList.IsEmpty()))
	{
		Close();
		return;
	}
	if ((ulTick - m_ulLastSendTick >= 40) || (m_iTotalBufferLen >= MAX_IOCP_BUFFER_SIZE))
	{
		m_ulLastSendTick = ulTick;
		{
			std::lock_guard<std::mutex> guard(m_LockCS);
			if (!m_bSending)
				PrepareSend(0, 0);
		}
	}
	Execute(ulTick);
}

bool CClientConnector :: IsCorpse(unsigned long ulTick, unsigned long ulMaxCorpseTime)
{
	return ((ulTick > m_ulActiveTick) && (ulTick - m_ulActiveTick > ulMaxCorpseTime));
}

bool CClientConnector :: IsBlock(int iMaxBlockSize)
{
	bool retflag = (m_iTotalBufferLen > iMaxBlockSize);
	if (retflag)
	{
		if (m_ulBufferFullTick = 0)
		{
			m_ulBufferFullTick = GetTickCount();
			retflag = false;
		}
		else if (GetTickCount() < m_ulBufferFullTick + MAX_BLOCK_CONTINUE_TIME)
		{
			//��������ϲ��Զ�ˣ��ڴ��������ݳ����ʱ�򣬸���һ���Ļ���ʱ��
			retflag = false;
		}
	}
	else 
	{
		m_ulBufferFullTick = 0;
	}
	return retflag;
}

int CClientConnector::ParseSocketReadData(int iType, const char* pBuf, int iCount)
{
	m_pReceiveBuffer->Write(pBuf, iCount);
	char* pTempBuf = (char*)m_pReceiveBuffer->GetMemPoint();
	int iTempBufLen = m_pReceiveBuffer->GetPosition();
	int iErrorCode = 0;
	int iOffset = 0;
	int iPackageLen = 0;
	PServerSocketHeader pHeader = nullptr;
	while (iTempBufLen - iOffset >= sizeof(TServerSocketHeader))
	{
		pHeader = (PServerSocketHeader)pTempBuf;
		if (SS_SEGMENTATION_SIGN == pHeader->ulSign)
		{
			iPackageLen = sizeof(TServerSocketHeader)+pHeader->usBehindLen;
			//�������ݰ��������ӵ�
			if (iPackageLen >= MAXWORD)
			{
				iOffset = iTempBufLen;
				iErrorCode = 1;
				break;
			}
			//����m_pReceiveBuffer����ʱ���������µİ�����iPackageLen�ڵ�ǰλ��iOffset�ϳ���iTempBufLen
			if (iOffset + iPackageLen > iTempBufLen)
				break;
			//�����յ������ݰ�������ʵ��
			ProcessReceiveMsg(pHeader, pTempBuf + sizeof(TServerSocketHeader), pHeader->usBehindLen);
			//�ƶ�ָ�룬��������socket���������
			iOffset += iPackageLen;
			pTempBuf += iPackageLen;
		}
		else
		{	//����Ѱ�Ұ�ͷ
			iErrorCode = 2;
			iOffset += 1;
			pTempBuf += 1;
		}
	}
	m_pReceiveBuffer->Reset(iOffset);
	return iErrorCode;
}

/************************Start Of CClientConnector**************************************************/


/************************Start Of CIOCPServerSocketManager******************************************/

CIOCPServerSocketManager :: CIOCPServerSocketManager():m_sLocalIP(""), m_iListenPort(0), m_OnConnect(nullptr), m_OnDisConnect(nullptr),
	m_OnListenReady(nullptr), m_OnClientError(nullptr), m_OnCreateClient(nullptr), m_OnCheckAddress(nullptr), m_MainWorker(nullptr),
	m_iMaxCorpseTime(DEFAULT_CLIENT_CORPSE_TIME), m_iMaxBlockSize(MAX_CLIENT_SEND_BUFFER_SIZE), m_bDelayFree(false),
	m_DFNFirst(nullptr), m_DFNLast(nullptr), m_usNewCreateHandle(1000), m_iDelayFreeHandleCount(0)
{
	m_QueryClientHash.DoInitial(MAX_HASH_BUCKETS_SIZE);
}

CIOCPServerSocketManager :: ~CIOCPServerSocketManager()
{
	WaitThreadExecuteOver();
	m_QueryClientHash.ClearAllPortItems();
}

void CIOCPServerSocketManager :: Open()
{
	if (nullptr == m_MainWorker)
	{
		//����InitialWorkThread��������Start�������Լ����õ�
		CMainIOCPWorker *pWorker;
		pWorker = new CMainIOCPWorker(this);
		pWorker->m_OnSocketError = std::bind(&CIOCPServerSocketManager::DoAcceptError, this, std::placeholders::_1, std::placeholders::_2);
		pWorker->m_OnSocketAccept = std::bind(&CIOCPServerSocketManager::DoSocketAccept, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		pWorker->m_OnSocketClose = std::bind(&CIOCPServerSocketManager::DoSocketClose, this, std::placeholders::_1);
		pWorker->m_OnReady = std::bind(&CIOCPServerSocketManager::DoReady, this, std::placeholders::_1);

		if (pWorker->Start(m_sLocalIP, m_iListenPort))
		{
			m_MainWorker = pWorker;
			m_MainWorker->InitialWorkThread();
		}
		else
		{
			delete(pWorker);		
		}
	}
}

void CIOCPServerSocketManager :: Close()
{
	CClientConnector* client = nullptr;
	CMainIOCPWorker* pObject = m_MainWorker;
	if (pObject == nullptr)
		return;

	m_MainWorker = nullptr;	
	TOnCreateClient funOldCreateClient = m_OnCreateClient;		//�رչ�����ж�ص�m_OnCreateClient����ֹ����DoSocketAccept��������
	m_OnCreateClient = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_LockCS);
		std::list<void*>::iterator vIter;
		for (vIter=m_ActiveConnects.begin(); vIter!=m_ActiveConnects.end(); ++vIter)
		{
			client = (CClientConnector*)*vIter;
			if (client != nullptr)
				client->Close();
		}
	}

	for (int i=1; i<=100; i++)
	{
		if (0 == m_QueryClientHash.GetItemCount())
		  break;
		WaitForSingleObject(m_Event, 100);
	}
	delete(pObject);	//������ͷ�m_MainWorker

	{
		std::lock_guard<std::mutex> guard(m_LockCS);
		std::list<void*>::iterator vIter;
		for (vIter=m_ActiveConnects.begin(); vIter!=m_ActiveConnects.end(); ++vIter)
		{
			client = (CClientConnector*)*vIter;
			delete(client);
		}
		m_ActiveConnects.clear();
		m_QueryClientHash.ClearAllPortItems();
	}	
	m_OnCreateClient = funOldCreateClient;						//����װ����m_OnCreateClient
}

void CIOCPServerSocketManager :: DoExecute()
{
	SetThreadLocale(0X804);
	unsigned long ulDelayTick = 0;
	unsigned long ulTick = 0;
	while (!IsTerminated())
	{
		ulTick = GetTickCount();
		try
		{
			{
				CClientConnector* client = nullptr;
				int iErrorCode = 0;
				std::lock_guard<std::mutex> guard(m_LockCS);
				std::list<void*>::iterator vIter;
				for (vIter=m_ActiveConnects.begin(); vIter!=m_ActiveConnects.end(); ++vIter)
				{
					client = (CClientConnector*)*vIter;
					if (client->IsCorpse(ulTick, m_iMaxCorpseTime))
					{
						iErrorCode = -100;
						DoClientError(client, iErrorCode);
						client->Close();
					}
					else if (client->IsBlock(m_iMaxBlockSize))
					{
						iErrorCode = -101;
						DoClientError(client, iErrorCode);
						client->Close();
					}
					else
						client->DoActive(ulTick);
				}
			}

			if (m_bDelayFree && (ulTick - ulDelayTick >= 2000))
			{
				ulDelayTick = ulTick;
				m_bDelayFree = DelayFreeClient(ulTick);
			}
			if ((!IsTerminated()) && (IsActive()))
				DoActive();
		}
		catch(...)
		{
			//SendDebugString('TTcpServerSocket.Execute Exception');
		}
		WaitForSingleObject(m_Event, 1);
	}
	//��ִ���������رյ���Ӧ��Դ
	Close();
}

bool CIOCPServerSocketManager :: DoCheckConnect(const std::string& sRemoteAddress)
{
	if (nullptr != m_OnCheckAddress)
		return m_OnCheckAddress(sRemoteAddress);
	else
		return true;
}

void* CIOCPServerSocketManager :: ValueOf(const int iKey)
{
	CC_UTILS::PPHashPortItem ppItem = m_QueryClientHash.FindPortItemPointer(iKey);
	if (ppItem != nullptr)
		return (*ppItem)->pItem;
	else
		return nullptr;
}

void CIOCPServerSocketManager :: DoReady(void* Sender)
{
	if (nullptr != m_OnListenReady)
		m_OnListenReady(Sender);
}

void CIOCPServerSocketManager :: DoSocketClose(void* Sender)
{
	PDelayFreeNode pNode;
	std::lock_guard<std::mutex> guard(m_LockCS);
	std::list<void*>::iterator vIter;
	for (vIter=m_ActiveConnects.begin(); vIter!=m_ActiveConnects.end(); ++vIter)
	{
		if (*vIter == Sender)  
		{
			pNode = new TDelayFreeNode;
			pNode->usHandle = ((CClientConnector*)Sender)->m_SocketHandle;
			pNode->ulAddTick = (unsigned long)GetTickCount();
			pNode->pObj = Sender;
			pNode->Next = nullptr;
			m_QueryClientHash.RemovePortItem(pNode->usHandle);
			if (m_DFNLast != nullptr)
				m_DFNLast->Next = pNode;
			else
				m_DFNFirst = pNode;
			m_DFNLast = pNode;
			++m_iDelayFreeHandleCount;
			m_bDelayFree = true;

			if (nullptr != m_OnDisConnect)
				m_OnDisConnect(this);
			((CClientConnector*)Sender)->Close();
			((CClientConnector*)Sender)->Clear();
			m_ActiveConnects.erase(vIter);
			break;
		}
	}
}

void CIOCPServerSocketManager :: DoSocketAccept(HANDLE hIOCP, SOCKET hSocket, const std::string& sRemoteAddress, int iRemotePort)
{
	bool bAcceptOK = false;
	CClientConnector* pClient = nullptr;
	unsigned short usHandle = AllocHandle();
	if (usHandle > 0)
	{
		int iError = 0;
		if (nullptr != m_OnCreateClient)
			pClient = m_OnCreateClient(sRemoteAddress);
		else
			pClient = new CClientConnector;

		if (CreateIoCompletionPort((HANDLE)hSocket, hIOCP, (ULONG_PTR)pClient, 0) > 0)
		{
			pClient->m_Socket = hSocket;
			pClient->m_sRemoteAddress = sRemoteAddress;
			pClient->m_iRemotePort = iRemotePort;
			pClient->m_SocketHandle = usHandle;
			pClient->m_OnSocketError = std::bind(&CIOCPServerSocketManager::DoClientError, this, std::placeholders::_1, std::placeholders::_2);
			bAcceptOK = true;

			AddClient(usHandle, pClient);
			if (nullptr != m_OnConnect)
				m_OnConnect(pClient);
			if (!pClient->PrepareRecv())
			{
				pClient->Close();
				DoSocketClose(pClient);
			}
		}
		else
		{
			iError = GetLastError();
			DoClientError(pClient, iError);
			delete(pClient);
		}
	}
	if (!bAcceptOK)
		closesocket(hSocket);
}

void CIOCPServerSocketManager :: DoAcceptError(void* Sender, int& iErrorCode)
{
	SendDebugString("DoAcceptError: ");
	iErrorCode = 0;
}

void CIOCPServerSocketManager :: DoClientError(void* Sender, int& iErrorCode)
{
	if ((10053 == iErrorCode) || (10054 == iErrorCode))
		return;
	if (nullptr != m_OnClientError)
		m_OnClientError(Sender, iErrorCode);
}

//���н��δ�ͷžͷ���true ��ֵ��m_BoDelayFree
bool CIOCPServerSocketManager :: DelayFreeClient(unsigned long ulTick)
{
	bool retflag = false;
	PDelayFreeNode pNode;
	PDelayFreeNode pNextNode;
	std::lock_guard<std::mutex> guard(m_LockCS);
	pNode = m_DFNFirst;
	while (pNode != nullptr)
	{
		pNextNode = pNode->Next;
		if (pNode->pObj != nullptr)
		{
			if ((ulTick > pNode->ulAddTick) && (ulTick - pNode->ulAddTick >= MAX_CLIENT_DELAY_TIME))
			{
				delete((CClientConnector*)pNode->pObj);   
				pNode->pObj = nullptr;
			}
			else
			{
				retflag = true;  
				break;
			}
		}
		pNode = pNextNode;
	}
	return retflag;
} 

unsigned short CIOCPServerSocketManager :: AllocHandle()
{
	unsigned short usRetHandle = 0;
	unsigned long ulTick = GetTickCount();
	std::lock_guard<std::mutex> guard(m_LockCS);
	PDelayFreeNode pNode;
	if (m_DFNFirst != nullptr)
	{
		pNode = m_DFNFirst;
		if (ulTick - pNode->ulAddTick >= MAX_CLIENT_DELAY_TIME)
		{
			m_DFNFirst = pNode->Next;
			if (nullptr == m_DFNFirst)
				m_DFNLast = nullptr;
			usRetHandle = pNode->usHandle;
			delete(pNode->pObj);
			pNode->pObj = nullptr;
			delete(pNode);
			--m_iDelayFreeHandleCount;
		}
	}
	if (0 == usRetHandle)
	{
		if (m_usNewCreateHandle < MAXWORD)
		{
			++m_usNewCreateHandle;
			usRetHandle = m_usNewCreateHandle;
		}
	}
	return usRetHandle;
}

void CIOCPServerSocketManager :: AddClient(int iHandle, void* pClient)
{
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_ActiveConnects.push_back(pClient);
	m_QueryClientHash.AddPortItem(iHandle, pClient);
}


/************************End Of CIOCPServerSocketManager********************************************/