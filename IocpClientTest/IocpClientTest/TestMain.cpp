/**************************************************************************************
@author: �²�
@content: iocp��Ĳ�������---�ͻ���
**************************************************************************************/

#include "CIocpSampleClient.h"
#include <tchar.h>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	if (DoInitialWinSocket())
	{
		CSampleClientManager sampleServer;
		sampleServer.ConnectToServer("127.0.0.1", 7001);
		sampleServer.InitialWorkThread();

		HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
		while (true)
		{
			WaitForSingleObject(m_Event, 100);
		}
	}
	else
	{
		std::cout << "DoInitialWinSocket Fail!" << std::endl;
	}
	DoFinalizeWinSocket();
	return 0;
}