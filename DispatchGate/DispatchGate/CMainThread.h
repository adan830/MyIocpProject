/**************************************************************************************
@author: �²�
@content: ���̵߳�Ԫ
**************************************************************************************/
#ifndef __CC_DISPATCH_GATE_MAIN_THREAD_H__
#define __CC_DISPATCH_GATE_MAIN_THREAD_H__

#include "stdafx.h"

/**
*
* CExecutableBase������----���߳�(������̳У�DoExecute����Ϊ�麯��)
*
*/
class CMainThread : public CExecutableBase
{
public:
	CMainThread(const std::string &sServerName);
	virtual ~CMainThread();
	void DoExecute();
private:
	void CheckConfig(const unsigned long ulTick);
private:
	unsigned long m_ulSlowRunTick;       //����ִ��tick
	unsigned long m_ulCheckConfigTick;   //config�ļ����
	int m_iConfigFileAge;                //��¼config�ļ��İ汾��
};

void Log(std::string &str, byte loglv = 0);

#endif //__CC_DISPATCH_GATE_MAIN_THREAD_H__