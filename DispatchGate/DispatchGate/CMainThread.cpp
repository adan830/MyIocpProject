/**************************************************************************************
@author: �²�
@content: ���̵߳�Ԫ
**************************************************************************************/
#include "stdafx.h"
#include "CMainThread.h"

/************************Start Of CMainThread**************************************************/
CMainThread::CMainThread(const std::string &sServerName) : m_ulSlowRunTick(0), m_ulCheckConfigTick(0), m_iConfigFileAge(0)
{
	/*
	m_LogSocket := TLogSocket.Create(ServerName);
	G_DBSocket := TDBServerSocket.Create(ServerName);
	G_GateSocket := TServerSocket.Create;
	G_CenterSocket := TCenterSocket.Create;
	G_Echo := TUDPEchoServer.Create(53);
	G_PigSocket := TPigClient.Create;
	*/
}

CMainThread::~CMainThread()
{
	WaitThreadExecuteOver();
	/*
	G_PigSocket.Free;
	G_CenterSocket.Free;
	G_GateSocket.Free;
	G_DBSocket.Free;
	G_Echo.Free;
	m_LogSocket.Free;
	*/
}

void CMainThread::CheckConfig(const unsigned long ulTick)
{	
	if ((0 == m_ulCheckConfigTick) || (ulTick - m_ulCheckConfigTick >= 30 * 1000))
	{
		m_ulCheckConfigTick = ulTick;	
		int iAge = CC_UTILS::GetFileAge(G_ConfigFileName);
		if (iAge != m_iConfigFileAge)
		{
			if (m_iConfigFileAge > 0)
				Log("Reload Config File...", lmtMessage);

			m_iConfigFileAge = iAge;
			CWgtIniFile* pIniFileParser = new CWgtIniFile();
			pIniFileParser->loadFromFile(G_ConfigFileName);
			try
			{
				/*
				G_DBSocket.LoadConfig(IniFile);
				G_GateSocket.LoadConfig(IniFile);
				G_PigSocket.LoadConfig(IniFile);
				*/
				delete pIniFileParser;
			}
			catch (...)
			{
				delete pIniFileParser;
			}
		}
	}

}

void CMainThread::DoExecute()
{
	Log("DispatchGate ����.");
	unsigned long ulTick;
	while (!IsTerminated())
	{
		int iErrorCode = 1;
		try
		{
			ulTick = GetTickCount();
			if (ulTick - m_ulSlowRunTick >= 1000)
			{
				m_ulSlowRunTick = ulTick;
				CheckConfig(ulTick);
				/*
				G_CenterSocket.DoHeartbeat;
				G_PigSocket.DoHeartbest;
				*/
			}
		}
		catch (...)
		{

		}
		WaitForSingleObject(m_Event, 10);
	}
	/*
	G_PigSocket.Close;
	G_GateSocket.Close;
	G_DBSocket.Close;
	*/
}
/************************End Of CMainThread****************************************************/


void Log(const char* str, byte loglv)
{
	SendDebugString(str);
}