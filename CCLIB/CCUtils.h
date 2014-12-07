/**************************************************************************************
@author: �²�
@content: �Լ�ʹ�õ�һ�����ýṹ������
**************************************************************************************/
#ifndef __CC_UTILS_H__
#define __CC_UTILS_H__

#include <string>
#include <vector>

namespace CC_UTILS{

	extern std::string G_CurrentExeFileName;       //��ǰ���������·��
	extern std::string G_CurrentExeDir;            //��ǰ�������ڵ�Ŀ¼

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
		bool Write(const char* pBuf, const int iCount);
		bool Reset(int iUsedLength);
		void* GetMemPoint();
		int GetPosition();
	private:
		void* m_pMemory;
		int m_iMemorySize;
		int m_iMemoryPosition;
	}TBufferStream, *PBufferStream;

	//�����ļ�������޸�ʱ��ת���ɵ����������Լ���ļ��Ƿ��޸�
	int GetFileAge(const std::string &sFileName);

	//�����ļ��İ汾��Ϣ  
	//ע�⣺��Ҫ�����м���version.lib
	std::string GetFileVersion(const std::string &sFileName);

	//�ַ����ָ�
	void SplitStr(const std::string& s, const std::string& delim, std::vector<std::string>* ret);

	//�ַ���ת��������Ĭ��ֵ�����׳��쳣
	int StrToIntDef(const std::string& sTemp, const int iDef);
}

#endif //__CC_UTILS_H__