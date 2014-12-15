/**************************************************************************************
@author: �²�
@content: 
**************************************************************************************/
#include "CCTcpClientSocket.h"
#pragma comment(lib, "ws2_32.lib")

const int NETWORK_EVENT_RECEIVE_BUFFER_SIZE = 16 * 1024;					// �����¼�ģ���µĽ���buffer��С
const int MAX_CACHE_SIZE    = 16 * 1024;							        // ÿ���ڵ�ķ�������С

/************************Start Of CNetworkEventClientSocketManager************************************/
CNetworkEventClientSocketManager::CNetworkEventClientSocketManager():m_CSocket(INVALID_SOCKET)
{
}

CNetworkEventClientSocketManager::~CNetworkEventClientSocketManager()
{ 
	Close(); 
}

unsigned int CNetworkEventClientSocketManager :: SendBuf(const char* pBuf, unsigned int Len, bool BoFree)
{
	unsigned int sendLen = 0;
	if (m_BoConnected && (Len > 0))
	{
		PSendBufferNode nNode = new TSendBufferNode();
		nNode->Next = nullptr;
		nNode->nStart = 0;
		nNode->nBufLen = Len;
		if (! BoFree)
		{
			nNode->szBuf = (char*)malloc(Len);
			memcpy(nNode->szBuf, pBuf, Len);
		}
		else
		{
			nNode->szBuf = const_cast<char*>(pBuf);
		}
		sendLen = Len;
		bool BoSendNow = false;

		{			
			std::lock_guard<std::mutex> guard(m_SendCS); 
			if (m_Last != nullptr)
			{
				m_Last->Next = nNode;
			}
			else
			{
				m_First = nNode;
				BoSendNow = true;
			}
			m_Last = nNode;
			m_Count++;
		}

		if (BoSendNow)
		{
			DoSend();
		}
	}
	else if (BoFree)
	{
		free(const_cast<char*>(pBuf));
	}
	return sendLen;
}

bool CNetworkEventClientSocketManager :: Open()
{
	bool retflag = false;
	if ((!m_BoActive) && (m_Address.length() > 0) && (m_Port > 0))
	{
		if (DoInitialize()) 
		{
			SOCKADDR_IN Addr; 
			Addr.sin_family = AF_INET;
			Addr.sin_addr.s_addr = inet_addr(m_Address.c_str());
			if (Addr.sin_addr.s_addr == u_long(INADDR_NONE))
			{
				PHOSTENT HostEnt = gethostbyname(m_Address.c_str());
				if (HostEnt != nullptr)
				{
					char** puAddr = HostEnt->h_addr_list;
					do
					{
						Addr.sin_addr.s_addr = *(*puAddr);
						//���������Ҫ�ӻ�ȥ������������
						//if (std::rand(2) == 0)
						//    Break;
						puAddr++;
					} while ((*puAddr) == nullptr);
				}
				else
				{
					DoError(seConnect);
				}
			}

			Addr.sin_port = htons(m_Port);
			retflag = (WSAConnect(m_CSocket, (PSOCKADDR)&Addr, sizeof(Addr), nullptr, nullptr, nullptr, nullptr) == 0);
			if (retflag)
			{
				m_Event = WSACreateEvent();
				WSAEventSelect(m_CSocket, m_Event, FD_READ | FD_WRITE | FD_CLOSE);
				m_BoActive = true;
			}
			else
			{
				closesocket(m_CSocket);
				m_CSocket = INVALID_SOCKET;
			}				
		}
	}
	return retflag;
}

bool CNetworkEventClientSocketManager :: Close()
{
	bool retflag = false;
	if (m_CSocket != INVALID_SOCKET)
	{
		if (shutdown(m_CSocket, SD_BOTH) != SOCKET_ERROR)
			retflag = true;
		else
			DoError(seClose);
		
		m_BoActive = false;
		if (m_Event > 0)
		{
			WSACloseEvent(m_Event);
			m_Event = 0;
		}
		closesocket(m_CSocket);
		m_CSocket = INVALID_SOCKET;
		if (m_BoConnected)
		{
			m_BoConnected = false;
			if (nullptr != m_OnDisConnect)
				m_OnDisConnect(this);
		}
	}

	{
		std::lock_guard<std::mutex> guard(m_SendCS); 
		m_Count = 0;
		PSendBufferNode nNode;
		while (m_First != nullptr)
		{
			nNode = m_First;
			m_First = nNode->Next;
			free(nNode->szBuf);
			delete nNode;
		}
		m_Last = nullptr;
	}
	return retflag;
}

bool CNetworkEventClientSocketManager :: Execute()
{
	bool retflag = false;
	WSANETWORKEVENTS fdset;
	while (WaitForSingleObject(m_Event, 0) == WAIT_OBJECT_0)
	{
	    //�����ָ�����׽ӿ��������¼��ķ���
		if (WSAEnumNetworkEvents(m_CSocket, m_Event, &fdset) == 0)
		{
			retflag = true;
			/*
			* iErrorCode ����ָ������һ������������飬ͬ lNetworkEvents �е��¼�������һ�����ÿ�������¼����ͣ���������һ��������¼��������������¼����͵�
			* �������ƣ�ֻ��Ҫ���¼����ֺ������һ����_BIT����׺�ִ����ɡ����磬�� FD_READ �¼�������˵��iErrorCode �����������ʶ������ FD_READ_BIT
			*/
			if (((fdset.lNetworkEvents | FD_READ) != 0) && (fdset.iErrorCode[FD_READ_BIT] == 0))
			{
				DoRead();
			}
			if (((fdset.lNetworkEvents | FD_WRITE) != 0) && (fdset.iErrorCode[FD_WRITE_BIT] == 0))
			{				
				DoWrite();
				//����д�¼���ʱ�򣬲�����ر��¼�
				return retflag;				
			}
			//�յ��˳����źţ������۽���Ƿ��б������˳�
			if ((fdset.lNetworkEvents | FD_CLOSE) != 0)
			{
				Close();
			}
		}
	}
	return retflag;
}

void CNetworkEventClientSocketManager :: DoRead()
{
	char Buf[NETWORK_EVENT_RECEIVE_BUFFER_SIZE - 1];
	WSABUF wsaBuffer = {NETWORK_EVENT_RECEIVE_BUFFER_SIZE, &Buf[0]};
	unsigned int Flags = 0;
	unsigned int Transfer = 0;
	if (WSARecv(m_CSocket, &wsaBuffer, 1, (LPDWORD)&Transfer, (LPDWORD)&Flags, nullptr, nullptr) == SOCKET_ERROR) 
	{
		if (DoError(seRead)) 
		{
			Close();
			return;
		}
	}
	else if (nullptr != m_OnRead)
	{
		m_OnRead(this, Buf, Transfer);
	}
}

void CNetworkEventClientSocketManager :: DoWrite()
{
	if (!m_BoConnected) 
	{
		m_BoConnected = true;
		if (nullptr != m_OnConnect)
			m_OnConnect(this);
	}
	else
	{
		DoSend();
	}
}

void CNetworkEventClientSocketManager :: DoSend()
{
	std::lock_guard<std::mutex> guard(m_SendCS); 
	if (m_Count > 0)
	{
		//ע������� wsaBuffers�Ƿ���Ҫ���ⲿ����һ����ĳ��ӣ���������ÿ��new
		WSABUF* wsaBuffers = new WSABUF[m_Count];
		PSendBufferNode nNode;
		nNode = m_First;
		for (int i=0; i<m_Count; i++)
		{
			wsaBuffers[i].len = nNode->nBufLen;
			wsaBuffers[i].buf = nNode->szBuf;
			nNode = nNode->Next;
		}
		unsigned int Transfer = 0;
		if (WSASend(m_CSocket, &wsaBuffers[0], m_Count, (LPDWORD)&Transfer, 0, nullptr, nullptr) == SOCKET_ERROR)
		{
			if (DoError(seSend))
				Close();
		}
		else if (Transfer > 0)
		{
			m_Count = 0;
			m_Last = nullptr;
			while (m_First != nullptr)
			{
				nNode = m_First;
				m_First = nNode->Next;
				free(nNode->szBuf);
				delete nNode;			
			}
		}
		delete[] wsaBuffers;
		wsaBuffers = nullptr;
	}
}


bool CNetworkEventClientSocketManager :: DoError(TSocketErrorType seType)
{
	int ErrorCode = WSAGetLastError();
	if (((seType == seRead) || (seType == seSend)) && (ErrorCode == WSAEWOULDBLOCK))
		return false;
	if (nullptr != m_OnError)
		m_OnError(this, ErrorCode);
	return true;
}

bool CNetworkEventClientSocketManager :: DoInitialize()
{
	bool retflag = false;
	if (m_CSocket == INVALID_SOCKET)
	{
		m_CSocket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		retflag = (m_CSocket != INVALID_SOCKET);
		if (retflag)		
		{
			if (m_LocalAddress.length() > 0)
			{
				SOCKADDR_IN Addr; 
				memset(&Addr, 0, sizeof(Addr));
				Addr.sin_family = PF_INET;
				Addr.sin_port = htons(INADDR_ANY);           
				Addr.sin_addr.s_addr = inet_addr(m_LocalAddress.c_str()); 
				//ע�ͣ�std�����ռ�����Ҳ�� bind�����������ʹ�����о���������
				retflag = ((bind(m_CSocket, (sockaddr *)&Addr, sizeof(Addr)) == 0));
			}
		}
		if (!retflag) 
		{
			DoError(seConnect);
		}
	}
	return retflag;
}
/************************End Of CNetworkEventClientSocketManager************************************/



/************************Start Of CIOCPClientSocketManager******************************************/
CIOCPClientSocketManager::CIOCPClientSocketManager() : m_OnRead(nullptr), m_OnError(nullptr), m_OnConnect(nullptr), m_OnDisConnect(nullptr),
	m_LocalAddress(""), m_Address(""), m_Port(), m_TotalBufferlen(0), m_BoActive(false), m_BoConnected(false), m_Sending(false),
	m_hIOCP(0), m_CSocket(INVALID_SOCKET), m_RecvLock(0), m_SendLock(0), m_Reconnect_Interval(10*1000)
{
	memset(&m_SendBlock, 0, sizeof(m_SendBlock));
	memset(&m_RecvBlock, 0, sizeof(m_RecvBlock));
	memset(&m_SocketAddr, 0, sizeof(m_SocketAddr));
	m_SendList.DoInitial(MAX_CACHE_SIZE);
	m_pReceiveBuffer = new CC_UTILS::TBufferStream;
	m_pReceiveBuffer->Initialize();
}

CIOCPClientSocketManager :: ~CIOCPClientSocketManager()
{
	DoClose();
	WaitThreadExecuteOver();
	m_pReceiveBuffer->Finalize();
	delete(m_pReceiveBuffer);
}

int CIOCPClientSocketManager :: SendBuf(const char* pBuf, int iCount)
{
	unsigned int sendLen = 0;
	if ((iCount > 0) && (m_TotalBufferlen < MAX_CLIENT_SEND_BUFFER_SIZE))
	{
		std::lock_guard<std::mutex> guard(m_LockCS); 
		sendLen = iCount;
		m_TotalBufferlen += iCount;
		m_SendList.AddBufferToList(pBuf, iCount);
		if (m_BoConnected && (!m_Sending))
			PrepareSend(0, 0);
	}
	return sendLen;
}

int  CIOCPClientSocketManager :: ParseSocketReadData(int iType, const char* pBuf, int iCount)
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
			iPackageLen = sizeof(TServerSocketHeader) + pHeader->usBehindLen;
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

bool CIOCPClientSocketManager :: DoInitialize()
{
	bool retflag = false;
	if (m_CSocket == INVALID_SOCKET)
	{
		m_CSocket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_IP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		retflag = (m_CSocket != INVALID_SOCKET);
		if (retflag)		
		{
			if (m_LocalAddress.length() > 0)
			{
				SOCKADDR_IN Addr; 
				memset(&Addr, 0, sizeof(Addr));
				Addr.sin_family = PF_INET;
				Addr.sin_port = htons(INADDR_ANY);           
				Addr.sin_addr.s_addr = inet_addr(m_LocalAddress.c_str()); 
				//ע�ͣ�std�����ռ�����Ҳ�� bind�����������ʹ�����о���������
				retflag = ((bind(m_CSocket, (sockaddr *)&Addr, sizeof(Addr)) == 0));
			}
		}
		if (!retflag) 
			DoError(seConnect);
	}
	return retflag;
}

bool CIOCPClientSocketManager :: Open()
{
	bool retflag = false;
	if ((!m_BoActive) && (m_Address.length() > 0) && (m_Port > 0))
	{
		if (DoInitialize()) 
		{
			memset(&m_SocketAddr, 0, sizeof(m_SocketAddr));
			m_SocketAddr.sin_family = AF_INET;
			m_SocketAddr.sin_addr.s_addr = inet_addr(m_Address.c_str());
			if (m_SocketAddr.sin_addr.s_addr == u_long(INADDR_NONE))
			{
				PHOSTENT HostEnt = gethostbyname(m_Address.c_str());
				if (HostEnt != nullptr)
					m_SocketAddr.sin_addr.s_addr = ((PIN_ADDR)(HostEnt->h_addr))->s_addr;
			}
			retflag = ((m_SocketAddr.sin_addr.s_addr != u_long(INADDR_NONE)) && (WSAHtons(m_CSocket, m_Port, &(m_SocketAddr.sin_port)) == 0));
			if (retflag) 
				m_BoActive = true;		
		}
	}
	return retflag;
}

//��ʽ�ص���Close�����Send Buffer
void CIOCPClientSocketManager :: Close(bool BoClearBuffer)
{	
	DoClose();
	if (BoClearBuffer)
		Clear();
}

void CIOCPClientSocketManager :: DoExecute()
{
	SetThreadLocale(0X804);
	unsigned int LastconnectTick = 0;
	while (!IsTerminated())  
	{
		try
		{
			if (m_BoConnected)
			{
				DoQueued();
				if (m_CSocket == INVALID_SOCKET)
				{
					m_BoConnected = false;
					if (nullptr != m_OnDisConnect)
						m_OnDisConnect(this);
				}
			}
			else
			{
				if (m_BoActive)
				{
					if (DoConnect())
						continue;
				}
				else
				{
					if (m_Reconnect_Interval > 0)
					{
						unsigned int Tick = GetTickCount();
						if (Tick - LastconnectTick >= m_Reconnect_Interval)
						{
							LastconnectTick = Tick;
							Open();
						}
					}
				}
				WaitForSingleObject(m_Event, 100);
			}
		}
		catch(...)
		{
			SendDebugString("CIOCPClientSocketManager :: Execute Exception");
		}
	}
	Clear();
}

bool CIOCPClientSocketManager :: DoError(TSocketErrorType seType)
{
	bool retflag = false;
	int ErrorCode = WSAGetLastError();
	if ((((seType == seRead) || (seType == seSend)) && (ErrorCode == ERROR_IO_PENDING)) ||
		((seType == seConnect) && (ErrorCode == WSAEWOULDBLOCK)))
	{
		return retflag;
	}

	retflag = true;
	if ((seType == seConnect) && (ErrorCode == WSAECONNREFUSED))
	{
		if (nullptr != m_OnError)
			m_OnError(this, ErrorCode);
	}
	if (seType != seClose)
	{
		DoClose();
	}
	return retflag;
}

bool CIOCPClientSocketManager :: DoConnect()
{
	bool retflag = false;
	if ((m_hIOCP == 0) && (m_CSocket != INVALID_SOCKET))
	{
		//������ɶ˿� 
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (m_hIOCP > 0)
		{
			//������ɶ˿�
			if (CreateIoCompletionPort(HANDLE(m_CSocket), m_hIOCP, unsigned int(this), 0) == 0)
			{
				DoClose();
			}
			else
			{
				if (WSAConnect(m_CSocket, (PSOCKADDR)&m_SocketAddr, sizeof(m_SocketAddr), nullptr, nullptr, nullptr, nullptr) == SOCKET_ERROR)
				{
					if (DoError(seConnect))
						return retflag;
				}
				m_BoConnected = true;
				if (nullptr != m_OnConnect)
				{
					m_OnConnect(this);
				}
				if (m_CSocket != INVALID_SOCKET)
				{
					retflag = PrepareRecv();
					if (retflag && (!m_Sending))
					{
						PrepareSend(0, 0);
					}
				}
				else if (nullptr != m_OnDisConnect)
				{
					m_OnDisConnect(this);
				}
			}
		}
	}
	return retflag;
}

void CIOCPClientSocketManager :: DoQueued()
{
	PBlock PRBlock = nullptr;
	unsigned int dwBytesXfered = 0;
	unsigned int dwCompletionKey = 0;
	bool retflag = (GetQueuedCompletionStatus(m_hIOCP, (LPDWORD)&dwBytesXfered, (LPDWORD)&dwCompletionKey, (LPOVERLAPPED*)&PRBlock, INFINITE) > 0);

	if ((IsTerminated()) || ((unsigned int)PRBlock == SHUTDOWN_FLAG))
		return;

	if (dwCompletionKey > 0)
	{
		//�ͻ��˶Ͽ�����
		if ((!retflag) || (dwBytesXfered == 0))
		{
			DoClose();
			return;
		}
		switch (PRBlock->Event)
		{
			case soWrite:
				PRBlock->Event = soIdle;
				IocpSendback(dwBytesXfered);
				break;
			case soRead:
				PRBlock->Event = soIdle;
				if (!IocpReadback(dwBytesXfered))
					DoClose();
				break;
			default:
				break;
		}
	}
	else
	{
		DoClose();
	}
}

void CIOCPClientSocketManager :: DoClose()
{
	try
	{
		SOCKET sock = m_CSocket;
		m_CSocket = INVALID_SOCKET;
		if (sock != INVALID_SOCKET)
			closesocket(sock);

		HANDLE hIOCP = m_hIOCP;
		m_hIOCP = 0;
		if (hIOCP > 0)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, (LPOVERLAPPED)SHUTDOWN_FLAG);
			CloseHandle(hIOCP);
		}
	}
	catch(...)
	{
		DoError(seClose);
	}
	m_BoActive = false;
	m_Sending = false;
}

void CIOCPClientSocketManager :: Clear()
{
	std::lock_guard<std::mutex> guard(m_LockCS); 
	m_SendList.DoFinalize();
}

void CIOCPClientSocketManager :: PrepareSend(int iUntreated, int iTransfered)
{
	std::lock_guard<std::mutex> guard(m_LockCS);
	m_TotalBufferlen -= iTransfered;
	if (m_TotalBufferlen < 0)
		m_TotalBufferlen = 0;
	if (m_CSocket != INVALID_SOCKET)
	{
		iUntreated = m_SendList.GetBufferFromList(m_SendBlock.Buffer, MAX_IOCP_BUFFER_SIZE, iUntreated);

		m_Sending = false;
		if (iUntreated > 0)
		{
			//�û�bufferת�Ƶ�ϵͳbuffer�� 
			m_SendBlock.Event = soWrite;
			m_SendBlock.wsaBuffer.len = iUntreated;
			m_SendBlock.wsaBuffer.buf = m_SendBlock.Buffer;
			memset(&m_SendBlock.Overlapped, 0, sizeof(m_SendBlock.Overlapped));
			if (WSASend(m_CSocket, &m_SendBlock.wsaBuffer, 1, (LPDWORD)&iTransfered, 0, &m_SendBlock.Overlapped, nullptr) == SOCKET_ERROR)
			{
				/*
				ԭ�������ӣ��������Ӧ�ò��ԣ�DoErrorʧ�ܵĻ���m_Sending Ҳ����false;
				if (DoError(seSend))
				  return;
				*/
				DoError(seSend);
				return;
			}
			m_Sending = true;
		}
	}
}

bool CIOCPClientSocketManager :: PrepareRecv()
{
	bool retflag = false;
	if (m_CSocket != INVALID_SOCKET)
	{
		m_RecvBlock.Event = soRead;
		m_RecvBlock.wsaBuffer.len = MAX_IOCP_BUFFER_SIZE;
		m_RecvBlock.wsaBuffer.buf = m_RecvBlock.Buffer;
		memset(&m_RecvBlock.Overlapped, 0, sizeof(m_RecvBlock.Overlapped));
		unsigned int Flags = 0;
		unsigned int Transfer = 0;
		retflag = (WSARecv(m_CSocket, &m_RecvBlock.wsaBuffer, 1, (LPDWORD)&Transfer, (LPDWORD)&Flags, &m_RecvBlock.Overlapped, nullptr) != SOCKET_ERROR);
		if (!retflag)
			retflag = (! DoError(seRead));
	}
	return retflag;
}

void CIOCPClientSocketManager :: IocpSendback(int Transfered)
{
	int RemainLen = m_SendBlock.wsaBuffer.len - Transfered;
	if (RemainLen > 0)
		memcpy(&(m_SendBlock.Buffer[0]), &(m_SendBlock.Buffer[Transfered]), RemainLen);
	else
		RemainLen = 0;

	PrepareSend(RemainLen, Transfered);
}

bool CIOCPClientSocketManager :: IocpReadback(int Transfered)
{
	bool retflag = false;
	if (Transfered > 0)
	{
		try
		{
			if (nullptr != m_OnRead)
				m_OnRead(this, m_RecvBlock.Buffer, Transfered);
			//˳�����������ٴ�Ͷ��
			if (m_CSocket != INVALID_SOCKET)
				retflag = PrepareRecv();
		}
		catch(...)
		{
			//�����쳣��Ҳ�����ٴ�Ͷ��
			if (m_CSocket != INVALID_SOCKET)
				retflag = PrepareRecv();
		}
	}
	return retflag;
}

/************************End Of CIOCPClientSocketManager********************************************/