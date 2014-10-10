/**************************************************************************************
@author: �²�
@content: iocp��Ĳ�������---������
**************************************************************************************/

#include "CIocpSampleServer.h"
#include <tchar.h>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])   
{    
	if (DoInitialWinSocket())
	{
		CSampleServerManager sampleServer;
		sampleServer.OnCreate();
		sampleServer.InitialWorkThread();
		if (!sampleServer.IsActive())
		{
			sampleServer.m_Address = "0.0.0.0";
			sampleServer.m_Port = 7001;
			sampleServer.Open();

			HANDLE m_Event = CreateEvent(nullptr, false, false, nullptr);
			while (true)
			{
				WaitForSingleObject(m_Event, 100);
			}
		}
	}
	else
	{
		std::cout << "DoInitialWinSocket Fail!" << std::endl;
	}
	DoFinalizeWinSocket();
	return 0;   
}