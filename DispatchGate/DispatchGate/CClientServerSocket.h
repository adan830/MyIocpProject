/**************************************************************************************
@author: �²�
@content: Dispatch�Կͻ������ӵļ���socket����
**************************************************************************************/
#ifndef __CC_CLIENT_SERVER_SOCKET_H__
#define __CC_CLIENT_SERVER_SOCKET_H__

#include "stdafx.h"

/**
*
* DispatchGate�����ĵ����ͻ������Ӷ���
*
*/
class CDGClient : public CClientConnector
{
public:
	CDGClient();
	~CDGClient();
protected:
	virtual void Execute(unsigned long ulTick);
	virtual void SocketRead(const char* pBuf, int iCount);
private:
	unsigned long m_ulLastConnectTick;
	unsigned long m_ulForceCloseTick;
	unsigned short m_usSelectMaskServerID;       //���ѡ��ķ������������Եı��
	int m_iEncodeIdx;                            //���ܱ��
	int m_iSelectRealServerID;                   //���ѡ��ķ�������ʵ���
	bool m_bIsGMIP;								 //�ж��Ƿ�ΪgmIP��
	int m_iClientType;							 //��ͬ�Ŀͻ�������
	unsigned char m_ucNetType;                   //�ͻ��˵���������---ͨ���ͻ��˶Թ̶���ַ�Ľ��������жϲ�ͬ����������
};

/*
  TClient = class(TCustomClient)
  private
    FEnCodeIdx: integer;
    FConnectTick: Cardinal;
    FSelectServer: Word;
    FBoMasterIP: Boolean;
    FClientType: integer;
    FForceCloseTick: Cardinal;
    FSelServerID: Integer;
    FNetType: Byte;
    procedure cmSelectServer(Buf: Pansichar; BufLen: Word);
    procedure cmCloseWindow(Buf: Pansichar; BufLen: Word);
    procedure SendToClient(wIdent: Word; Data: PAnsiChar; wDataLen: Word);
  protected
    procedure Execute(Tick: cardinal); override;
    procedure SocketRead(const Buf; Count: integer); override;
    procedure OnCreate; override;
  public
    procedure ForceClose;
    procedure OpenWindow(wtype: TClientWindowType; nParam: integer; const msg: ansistring = '');
    property SelectServerID: Word read FSelectServer;
    property SelServerID: Integer read FSelServerID;
    property BoMasterIP: Boolean read FBoMasterIP;
    property EnCodeIdx: Integer read FEnCodeIdx;
    property ClientType: Integer read FClientType;
    property netType: Byte read FNetType;
  end;
*/

/**
*
* DispatchGate�Կͻ��˵ļ���������
*
*/
class CClientServerSocket : public CIOCPServerSocketManager
{
public:
	CClientServerSocket();
	virtual ~CClientServerSocket();
protected:
	virtual void DoActive();
private:
	bool OnCheckIPAddress(const std::string& sIP);
	CDGClient* OnCreateClientSocket(const std::string& sIP);
	void OnSocketError(void* Sender, int& iErrorCode);
	void OnClientConnect(void* Sender);
	void OnClientDisconnect(void* Sender);
};

#endif //__CC_CLIENT_SERVER_SOCKET_H__