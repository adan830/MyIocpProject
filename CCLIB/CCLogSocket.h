/**************************************************************************************
@author: �²�
@content: ͨ����־��
**************************************************************************************/
#ifndef __CC_LOG_SOCKET_H__
#define __CC_LOG_SOCKET_H__

#include <string>
#include "CCTcpSocketCommon.h"
#include "CCTcpClientSocket.h"

namespace CC_UTILS{

	const int SERVICE_NAME_LENGTH = 16;						  // ����������
	const int MAX_LISTVIEW_COUNT = 10;						  // �����listview������
	const int LABEL_CAPTION_LENGTH = 31;                      // �����label�ؼ��ı�����󳤶�
	const std::string DEFAULT_SERVICE_IP = "127.0.0.1";		  // Ĭ�ϵķ���ip
	const int DEFAULT_SERVICE_PORT = 7822;                    // Service  -> Monitor Center   ?????????????????????????
	const int DEFAULT_MONITOR_PORT = 7823;                    // Viewer  -> Monitor Center    ?????????????????????????


	//SMM_ADD_LISTVIEW
	typedef char TShortValue[SERVICE_NAME_LENGTH];	
	typedef TShortValue TListViewInfo[MAX_LISTVIEW_COUNT];
	typedef TListViewInfo* PListViewInfo;


	/**
	*
	* ͨ����־��
	* �����ö���󣬾Ϳ���ֱ�ӵ��ã������õ�������־������������־
	*
	*/
	class CLogSocket : public CExecutableBase
	{
	public:
		CLogSocket(std::string& sName, bool bListView);
		//---�������̳�,Ҳ�Ͳ����麯���ˣ���������
		~CLogSocket();
		void DoExecute();

		void AddLabel(const std::string& sDesc, int iLeft, int iTop, int iTag = 0);
		void UpdateLabel(const std::string& sDesc, int iTag);
		void AddListView(PListViewInfo pInfo);
		void SetListViewColumns(PListViewInfo pInfo);
		void UpdateListView(const std::string& sDesc, unsigned short usRow, unsigned short usCol);
		void SendLogMsg(const std::string& sMsg, int iType = 0);
		void SendImportantLog(unsigned short usIdent, const char* pData, unsigned short usDataLen);
		void SendToServer(unsigned short usIdent, int iParam, const char* pBuf, unsigned short usBufLen);
		void SendTracerData(const std::string sRoleName, const char* pBuf, unsigned short usBufLen);
	protected:
	private:
		//noncopy����
		CLogSocket();
		CLogSocket(CLogSocket& rhs);
		CLogSocket& operator=(const CLogSocket& worker);

		void ClearWaitBuffers();
		void SendHeartBeat();
		void LoadConfig();
		void RegisterServer();
		void RegisterServerEx();
		void SendWaitMsg();
		void OnSocketConnect(void* Sender);
		void OnSocketDisConnect(void* Sender);
		void OnSocketRead(void* Sender, const char* pBuf, unsigned short usBufLen);
		void SetServiceName(std::string sValue);
	private:
		std::string m_sServiceName;

		/*
    m_Servicename: ansistring;
    m_DelayEvent: THandle;
    m_Socket: TTcpClientSocket;
    m_PingCount: integer;
    m_OnConnectEvent: TNotifyEvent;
    m_OnDisConnectEvent: TNotifyEvent;
    m_ReceiveBuffer: PAnsiChar;
    m_ReceiveBufferLen: integer;
    m_WaitSendList: TList;
    m_WaitMsgCS: TRTLCriticalSection;
    m_BoListView: Boolean;
    m_ListViewInfo: PListViewInfo;
		*/

	};
	/*
  TLogSocket = class(TThread)
  private


  public

    property OnConnect: TNotifyEvent read m_OnConnectEvent write m_OnConnectEvent;
    property OnDisConnect: TNotifyEvent read m_OnDisConnectEvent write m_OnDisConnectEvent;
    property ServiceName: AnsiString read m_Servicename write SetServiceName;
  end;
	*/
}

#endif //__CC_LOG_SOCKET_H__