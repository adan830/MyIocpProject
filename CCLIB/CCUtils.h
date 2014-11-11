/**************************************************************************************
@author: �²�
@content: �Լ�ʹ�õ�һ�����ýṹ������
**************************************************************************************/
#ifndef __CC_UTILS_H__
#define __CC_UTILS_H__

#include <string>

namespace CC_UTILS{
	//���׵�Hash���
	typedef struct _THashPortItem
	{
		int iHandle;
		void* pItem;
		_THashPortItem* Next;
	}THashPortItem, *PHashPortItem, **PPHashPortItem;

	/**
	* һ�����׵�hash�������ͻ������
	*/
	typedef struct _TSimpleHash
	{
	public:
		void DoInitial(int iSize);
		void AddPortItem(const int iKey, void* pClient);
		void RemovePortItem(const int iKey);
		void ClearAllPortItems();
		PPHashPortItem FindPortItemPointer(const int iKey);     //������Ҫ���ص���PortItem��ָ��
		int GetItemCount();
	private:
		PPHashPortItem m_ppItemBuckets;							//���ڴ洢�ͻ��˶���ļ���hash
		int m_iBucketSize;                                      //�̶������鳤��
		int m_iHashItemCount;  				     			    //��ǰ�����еĿͻ��˾������
	}TSimpleHash, *PSimpleHash;

	/**
	* һ�����׵�buffer��д����������Ҫ����һ������ά���Ľ���buffer����
	*/
	typedef struct _TBufferStream
	{
	public:
		void Initialize();
		void Finalize();
		bool Write(char* pBuf, const int iCount);
		bool Reset(int iUsedLength);
		void* GetMemPoint();
		int GetPosition();
	private:
		void* m_pMemory;
		int m_iMemorySize;
		int m_iMemoryPosition;
	}TBufferStream, *PBufferStream;

	//�����ļ�������޸�ʱ�����İ汾��
	int GetFileAge(const std::string &sFileName);
}

#endif //__CC_UTILS_H__