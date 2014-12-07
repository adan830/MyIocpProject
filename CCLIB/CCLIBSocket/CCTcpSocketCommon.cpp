/**************************************************************************************
@author: �²�
@content: 
**************************************************************************************/

#include "CCTcpSocketCommon.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


//������ʵ�ֱ��붨�嵽cpp�ļ��У���Ȼ����ļ����ú󣬱���ʱ���Ƶ�obj�������涼�������������ʶ��������ʱ��ᵼ���ظ�
bool DoInitialWinSocket()
{
	WSADATA wsa;
	return (WSAStartup(0x2020, &wsa) == NO_ERROR);
}

void DoFinalizeWinSocket()
{
	WSACleanup();
}

void SendDebugString(const std::string& sInfo)
{
	std::cout << sInfo.c_str() << std::endl;
}

bool IsInternetIP(const u_long ulIP)
{
	bool retFlag = true;
	unsigned char ucFirst;
	unsigned char ucSecond;
	ucFirst = ulIP & 0xFF;
	ucSecond = (ulIP & 0xFF00) >> 8;
	if (192 == ucFirst)
		retFlag = false;
	else if (10 == ucFirst)
		retFlag = false;
	else if ((172 == ucFirst) && (ucSecond > 15) && (ucSecond < 33))
		retFlag = false;

	return retFlag;
}

std::string GetInternetIP(const std::string& sDefaultIP)
{
	std::string sRetIP("");
	char szHostName[256];
	if (0 == gethostname(szHostName, sizeof(szHostName)))
	{	
		if (sDefaultIP.find("127.") == 0)
		{
			sRetIP = sDefaultIP;
		}	
		else
		{
			HOSTENT* pHost = gethostbyname(szHostName);
			if (pHost != nullptr)
			{
				std::string sTempAddr;
				PIN_ADDR* pAddr = (PIN_ADDR*)pHost->h_addr_list;
				while (*pAddr != nullptr)
				{
					sTempAddr.assign(inet_ntoa(**pAddr));
					if (sDefaultIP.compare(sTempAddr) == 0)
					{
						sRetIP = sTempAddr;
						break;
					}
					else if (sRetIP.compare("") == 0)
					{
						sRetIP = sTempAddr;
					}
					else if (IsInternetIP((*pAddr)->s_addr))
					{
						sRetIP = sTempAddr;
					}
					pAddr += 1;
				}
			}
		}
	}
	return sRetIP;
}

/************************Start Of _TSendBufferLinkedList**************************************************/
void _TSendBufferLinkedList::DoInitial(const int iSize)
{
	m_pFirst = nullptr;
	m_pLast = nullptr;
	m_iNodeCacheSize = iSize;
}

void _TSendBufferLinkedList::DoFinalize()
{
	PSendBufferNode pNode;
	while (m_pFirst != nullptr)
	{
		pNode = m_pFirst;
		m_pFirst = pNode->Next;
		free(pNode->szBuf);
		delete(pNode);
	}
	m_pLast = nullptr;
}

/*
*  ����β�ڵ㣬�ȳ�����β�ڵ���ӡ����β�ڵ㲻�ܷ��·��͵����ݣ��򴴽��½ڵ�
*  ע��:�����ʵ�ֱ�֤ÿ��bufferֻ��һ�����ͽڵ��У�һ���ڵ���Զ�Ӧ�������buffer
*       ����Ľڵ㲢û�б�֤ά��m_iNodeCacheSize��Ϊ���buffsize���ǿ��ܳ����
*/
void _TSendBufferLinkedList::AddBufferToList(const char* pBuf, int iCount)
{
	if ((m_pLast != nullptr) && (m_pLast->nBufLen + iCount <= m_iNodeCacheSize))
	{
		memcpy(&(m_pLast->szBuf[m_pLast->nBufLen]), pBuf, iCount);
		m_pLast->nBufLen += iCount;
	}
	else
	{
		int iAllocLen = 0;
		PSendBufferNode pNode = new TSendBufferNode;
		pNode->Next = nullptr;
		pNode->nStart = 0;
		if (iCount < m_iNodeCacheSize)
			iAllocLen = m_iNodeCacheSize;
		else
			iAllocLen = iCount;
		pNode->szBuf = (char*)malloc(iAllocLen);
		memcpy(pNode->szBuf, pBuf, iCount);
		pNode->nBufLen = iCount;

		if (m_pLast != nullptr)
			m_pLast->Next = pNode;
		else
			m_pFirst = pNode;
		m_pLast = pNode;
	}
}

int _TSendBufferLinkedList::GetBufferFromList(char* pDesBuf, int iBufMaxSize, int iBufUntreatedBytes)
{
	int iRemainLen = 0;
	//�Ӷ�����ȡ�ȴ����͵�����
	while (m_pFirst != nullptr)
	{
		PSendBufferNode pNode = m_pFirst;
		//��ǰ���ڷ��͵�m_SendBlob��buffer��ʣ��ĳ���
		iRemainLen = iBufMaxSize - iBufUntreatedBytes;
		//�ý����Ҫ���͵����ݳ���
		int iDataLen = pNode->nBufLen - pNode->nStart;
		if (iDataLen > iRemainLen)
		{
			//���ݲ���һ�η������
			memcpy(&pDesBuf[iBufUntreatedBytes], &pNode->szBuf[pNode->nStart], iRemainLen);
			iBufUntreatedBytes = iBufMaxSize;
			pNode->nStart += iRemainLen;
			break;
		}
		else
		{
			memcpy(&pDesBuf[iBufUntreatedBytes], &pNode->szBuf[pNode->nStart], iDataLen);
			iBufUntreatedBytes += iDataLen;
			m_pFirst = pNode->Next;
			if (nullptr == m_pFirst)
				m_pLast = nullptr;
			free(pNode->szBuf);
			delete(pNode);
		}
	}
	return iBufUntreatedBytes;
}

/************************End Of _TSendBufferLinkedList****************************************************/



/************************Start Of CExecutableBase*********************************************************/
CExecutableBase::CExecutableBase() :m_bTerminated(false), m_pThread(nullptr), m_bExecuteOver(false), m_sThreadName("NoName")
{
	m_Event = CreateEvent(nullptr, false, false, nullptr);
}

CExecutableBase::~CExecutableBase()
{
	CloseHandle(m_Event);
}

void CExecutableBase::InitialWorkThread()
{
	if (nullptr == m_pThread)
	{
		m_pThread = new std::thread(&CExecutableBase::Execute, this);
		SendDebugString(m_sThreadName + ":new thread");
	}
}

void CExecutableBase::WaitThreadExecuteOver()
{
	if (!IsTerminated())
	{
		Terminate();
		if (!m_bExecuteOver)
		{
			SendDebugString(m_sThreadName + ":before wait");
			WaitForSingleObject(m_Event, INFINITE);
			SendDebugString(m_sThreadName + ":after wait");
		}
		else
		{
			SendDebugString(m_sThreadName + ":no join");
		}
		m_pThread = nullptr;
	}
}

void CExecutableBase::Execute()
{
	DoExecute();
	m_bExecuteOver = true;
	SetEvent(m_Event);
	SendDebugString(m_sThreadName + ":Execute Over");
}

void CExecutableBase::Terminate()
{ 
	m_bTerminated = true; 
	SendDebugString(m_sThreadName + ":Terminate");
}

bool CExecutableBase::IsTerminated()
{ 
	return m_bTerminated; 
}
/************************End Of CExecutableBase***********************************************************/