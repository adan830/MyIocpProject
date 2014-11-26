/**************************************************************************************
@author: �²�
@content: ͨ����־��
**************************************************************************************/
#ifndef __CC_LOG_SOCKET_H__
#define __CC_LOG_SOCKET_H__

#include <string>
#include <list>
#include "CCTcpSocketCommon.h"
#include "CCTcpClientSocket.h"

namespace CC_UTILS{

	const int LOG_SEGMENTATION_SIGN = 0xFFDDEEAA;             // �ָ���ʶ
	const int SMM_PING = 1000;                                // ����
	const int SMM_REGSITER = 1001;                            // �Ǽ�
	const int SMM_DEBUG_MESSAGE = 1002;                       // ��־��Ϣ
	const int SMM_ADD_LABEL = 1003;                           // ����һ���µ�Label (����Service)
	const int SMM_UPDATE_LABEL = 1004;                        // ����Label��Ϣ  (����Service)&(���͵���ش���)
	const int SMM_APP_CLOSE = 1005;                           // ����ر�
	const int SMM_REGSITER_EXT = 1006;                        // �Ǽ�
	const int SMM_ADD_LISTVIEW = 1007;                        // ���listview
	const int SMM_UPDATE_LISTVIEW = 1008;                     // ����listview
	const int SMM_TRACE_DATA = 1009;                          // ���ݸ���

	const int SERVICE_NAME_LENGTH = 16;						  // ����������
	const int MAX_LISTVIEW_COUNT = 10;						  // �����listview������
	const int LABEL_CAPTION_LENGTH = 31;                      // �����label�ؼ��ı�����󳤶�
	const std::string DEFAULT_SERVICE_IP = "127.0.0.1";		  // Ĭ�ϵķ���ip
	const int DEFAULT_SERVICE_PORT = 7822;                    // Service  -> Monitor Center   ?????????????????????????
	const int DEFAULT_MONITOR_PORT = 7823;                    // Viewer  -> Monitor Center    ?????????????????????????

	//��־�ȼ�
	const int LOG_TYPE_MESSAGE = 0;  
	const int LOG_TYPE_WARNING = 1;
	const int LOG_TYPE_ERROR = 2;
	const int LOG_TYPE_EXCEPTION = 3;
	const int LOG_TYPE_DEBUG = 255;

	//��־��Ϣ�����ͷ�ṹ
	typedef struct _TLogSocketHead
	{
		unsigned long ulSign;            // �ָ���ʶ
		unsigned short usIdent;          // Э���
		unsigned short usBehindLen;      // ��������ĳ���
	}TLogSocketHead, *PLogSocketHead;

	//SMM_ADD_LISTVIEW
	typedef char TShortValue[SERVICE_NAME_LENGTH];	
	typedef TShortValue TListViewInfo[MAX_LISTVIEW_COUNT];
	typedef TListViewInfo* PListViewInfo;

	//SMM_UPDATE_LISTVIEW
	typedef struct _TUpdateViewInfo
	{
		unsigned short usRow;
		unsigned short usCol;
		TShortValue value;
	}TUpdateViewInfo, *PUpdateViewInfo;

	//SMM_ADD_LABEL  (Service -> Monitor IN) ����Service
	typedef struct _TLogLabelInfo
	{
		int iLeft;
		int iTop;
		int iTag;
		char szCaption[LABEL_CAPTION_LENGTH+1];
	}TLogLabelInfo, *PLogLabelInfo;

	//SMM_UPDATE_LABEL(Service -> Monitor IN) ����Service
	typedef struct _TUpdateLabelInfo
	{
		int iTag;								// SMM_ADD_LABEL ָ����tagid
		char szValue[LABEL_CAPTION_LENGTH + 1];
	}TUpdateLabelInfo, *PUpdateLabelInfo;

	typedef struct _TWaitBufferNode
	{
		char* pBuf;
		unsigned short usBufLen;
		_TWaitBufferNode* pNext;
	}TWaitBufferNode, *PWaitBufferNode;

	/**
	*
	* ͨ����־��
	* �����ö���󣬾Ϳ���ֱ�ӵ��ã������õ�������־������������־
	*
	*/
	class CLogSocket : public CExecutableBase
	{
	public:
		CLogSocket(std::string &sName, bool bListView);
		virtual ~CLogSocket();        
		virtual void DoExecute();    

		void AddLabel(const std::string &sDesc, int iLeft, int iTop, int iTag = 0);
		void UpdateLabel(const std::string &sDesc, int iTag);
		void AddListView(PListViewInfo pInfo);
		void SetListViewColumns(PListViewInfo pInfo);
		void UpdateListView(const std::string &sDesc, unsigned short usRow, unsigned short usCol);
		void SendLogMsg(const std::string &sMsg, int iType = 0);
		void SendToServer(unsigned short usIdent, int iParam, const char* pBuf, unsigned short usBufLen);
		void SendTracerData(const std::string &sRoleName, const char* pBuf, unsigned short usBufLen);		
		void SetServiceName(const std::string &sName);
		std::string GetServiceName();
	protected:
	private:
		void ClearWaitBuffers();
		void SendHeartBeat();
		void LoadConfig();
		void RegisterServer();
		void RegisterServerEx();
		void SendWaitMsg();
		void OnSocketConnect(void* Sender);
		void OnSocketDisconnect(void* Sender);
		void OnSocketRead(void* Sender, const char* pBuf, unsigned short usBufLen);
	private:
		std::string m_sServiceName;
		HANDLE m_DelayEvent;
		CNetworkEventClientSocketManager m_ClientSocket;
		int m_iPingCount;
		std::list<PWaitBufferNode> m_WaitSendList;
		std::mutex m_WaitMsgCS;
		bool m_bListView;
		PListViewInfo m_pListViewInfo;
	};
}

#endif //__CC_LOG_SOCKET_H__