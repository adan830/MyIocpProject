/**************************************************************************************
@author: �²�
@content: �Լ�ʹ�õ�һ�����ýṹ������
**************************************************************************************/
#ifndef __CC_UTILS_H__
#define __CC_UTILS_H__

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
	PPHashPortItem m_ItemBuckets;							//���ڴ洢�ͻ��˶���ļ���hash
	int m_iBucketSize;                                      //�̶������鳤��
	int m_iHashItemCount;  				     			    //��ǰ�����еĿͻ��˾������
}TSimpleHash, *PSimpleHash;


#endif //__CC_UTILS_H__