/**************************************************************************************
@author: �²�
@content: �Լ�ʹ�õ�Hash��
		  ֧�ֿ������ѭ��������������First��ʱ���Ǳ�����ǰ�������Ľ��״̬
**************************************************************************************/
#ifndef __CC_HASH_CLASS_H__
#define __CC_HASH_CLASS_H__

#include <functional>

namespace CC_UTILS{

	typedef std::function<bool(void* pointer, unsigned long ulParam, int &iResult)> TTouchFunc;


	//������Ϊkey��hash
	typedef std::function<void(void* pValue, int iKey)> TRemoveIntValueEvent;
	typedef struct _TIntHashItem
	{
		_TIntHashItem* BPrev;   //����bucket�����е�ǰ�ý��
		_TIntHashItem* BNext;   //����bucket�����еĺ��ý��
		_TIntHashItem* LPrev;   //�������е�ǰ�ýڵ�
		_TIntHashItem* LNext;   //�������еĺ��ýڵ�
		int Key;
		void* Value;
	}TIntHashItem, *PIntHashItem, **PPIntHashItem;

	class CIntegerHash
	{
	public:		
		CIntegerHash(unsigned long ulSize = 1023);
		virtual ~CIntegerHash();
		bool Add(const int iKey, void* pValue);
		void Clear();
		bool Remove(const int iKey);
		void* ValueOf(const int iKey);
		int Touch(TTouchFunc func, unsigned long ulParam);
		int GetNext(void* ptr);
		void First();
		bool Eof();
		void* GetNextNode();         //�ú�������������һ��㣬����m_pCurrentQueueNodeָ����һ�������������
		int GetCount(){ return m_iTotalCount; };
	public:
		TRemoveIntValueEvent m_RemoveEvent;
	private:
		unsigned long HashOf(const int iKey);
		PPIntHashItem Find(const int iKey);
	private:
		int m_iTotalCount;
		unsigned long m_ulBucketSize;
		PIntHashItem m_pFirstListNode;
		PIntHashItem m_pLastListNode;
		PIntHashItem m_pCurrentQueueNode;
		PPIntHashItem m_TopBuckets;
	};




}

#endif //__CC_HASH_CLASS_H__