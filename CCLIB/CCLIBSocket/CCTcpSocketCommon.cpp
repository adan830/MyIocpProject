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