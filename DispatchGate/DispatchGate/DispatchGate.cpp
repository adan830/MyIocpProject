/**************************************************************************************
@author: �²�
@content: DispatchGate��Main��Ԫ������CCWindowsService.dll���������߳�
**************************************************************************************/

#include "stdafx.h"

using namespace std;

char DEFAULT_SERVICE_NAME[] = "DispatchGate";
char DEFAULT_DESCRIPTION[] = "LongGet ��Ϸ��¼���ط���";
CMainThread* pMainThread = nullptr;

bool DoAppStart(void* Sender)
{
	pMainThread = new CMainThread(DEFAULT_SERVICE_NAME);
	pMainThread->InitialWorkThread();
	return true;
}

bool DoAppStop(void* Sender)
{
	delete pMainThread;
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hWindowsServiceDll;
	TServiceManagerFunc ServiceManagerFunc;
#ifdef DEBUG
	hWindowsServiceDll = LoadLibrary("CCWindowsService_Debug.dll");
#else
	hWindowsServiceDll = LoadLibrary("CCWindowsService_Debug.dll");
#endif
	if (hWindowsServiceDll != nullptr)
	{
		if (DoInitialWinSocket())
		{
			G_ConfigFileName = ".\\config.ini";

			ServiceManagerFunc = (TServiceManagerFunc)GetProcAddress(hWindowsServiceDll, "DoApplicationRun");
			if (ServiceManagerFunc != nullptr)
				ServiceManagerFunc(DEFAULT_SERVICE_NAME, DEFAULT_DESCRIPTION, DoAppStart, DoAppStop);
			else
				std::cout << "the calling is error!" << std::endl;
		}
		else
		{
			std::cout << "DoInitialWinSocket Fail!" << std::endl;
		}
		FreeLibrary(hWindowsServiceDll);
	}
	else
	{
		std::cout << "dll is missing!" << std::endl;
	}
	return 0;
}