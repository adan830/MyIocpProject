/**************************************************************************************
@author: �²�
@content: DispatchGate�ĳ����ͱ��������� 
**************************************************************************************/
#ifndef __CC_DISPATCH_GATE_GLOBAL_H__
#define __CC_DISPATCH_GATE_GLOBAL_H__

#include "stdafx.h"

using namespace std;

enum TIpType
{
	itUnKnow,
	itAllow,
	itDeny,
	itMaster
};

typedef struct _TIpRuleNode
{
	std::string sMatchIP;
	TIpType ipType;
}TIpRuleNode, *PIpRuleNode;


//��־�ȼ�---ȫ�ֳ�����.h�ļ���������
const int lmtMessage = 0;
const int lmtWarning = 1;
const int lmtError = 2;
const int lmtException = 3;

const int MAX_CENTER_SERVER_COUNT = 3;    //����������������
const int MAX_RESSERVER_COUNT = 40;       //��Դ�������������

//�����ļ���---ȫ�ֱ���ʹ��extern��.h�ļ���������.cpp�ļ�����
extern string G_CurrentExePath;                   //��ǰ�������ڵ�Ŀ¼·�� 
extern TServerConnectInfo G_ResServerInfos[];  //��Դ��������������Ϣ

//������ȫ�ֶ���


#endif //__CC_DISPATCH_GATE_GLOBAL_H__