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