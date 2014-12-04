/**************************************************************************************
@author: �²�
@content: ��������ͨ�ŵĳ����ͽṹ����
**************************************************************************************/

#ifndef __CC_PROTOCOL_SERVER_H__
#define __CC_PROTOCOL_SERVER_H__

#include "CCGameCommon.h"

// Server��ͨѶ��Э��ͷ
typedef struct _TServerSocketHeader
{
	unsigned long ulSign;				// �ָ��� SS_SEGMENTATION_SIGN
	int iParam;							// ��չ����
	unsigned short usIdent;				// Э���
	unsigned short usBehindLen;			// �������ݳ���
}TServerSocketHeader, *PServerSocketHeader;

// Dispatch Gate -> DBServer ת�����ѡ����Ϣ
typedef struct _TClientSelectServerInfo
{
	int iSessionID;
	int iSelectServerID;
	int iEnCodeIdx;
	int iClientType;
	bool bMasterIP;
	unsigned char ucNetType;
}TClientSelectServerInfo, *PClientSelectServerInfo;

// ip��ַ���ַ�������
typedef char TIPAddress[IP_ADDRESS_MAX_LEN+1];
typedef TIPAddress* PIPAddress;

// ��������ַ����
typedef struct _TServerAddress
{
	TIPAddress IPAddress;
	int iPort;
	unsigned char ucNetType;
}TServerAddress, *PServerAddress;

// ��������ǰ��������Ϣ
typedef struct _TServerConnectInfo
{
	TServerAddress Addr;
	int iConnectCount;
}TServerConnectInfo, *PServerConnectInfo;

// Pig��ѯ������Ϣ
typedef struct _TPigQueryServerInfo
{
	int iServerID;
	char szServerIP[IP_ADDRESS_MAX_LEN+1];
	char szServerName[SERVER_NAME_MAX_LEN+1];
}TPigQueryServerInfo, *PPigQueryServerInfo;

typedef struct _TPigMsgData
{
	unsigned char ucMsgType;
	unsigned short usAreaLen;
	unsigned short usMsgLen;
}TPigMsgData, *PPigMsgData;


const int SS_SEGMENTATION_SIGN = 0XFFEEDDCC;                        // ������֮��ͨ��Э����ʼ��־

// ��������Э��
const int SM_REGISTER = 0x1000;			                            // ע�������
const int SM_UNREGISTER = 0x1001;							        // ע��������
const int SM_PING = 0x1002;								            // �������

const int SM_SERVER_CONFIG = 0x1005;                                // ������Ϣ

const int SM_SELECT_SERVER = 0x2001;                                // ѡ������
const int SM_PLAYER_CONNECT = 0x2002;                               // �������
const int SM_PLAYER_DISCONNECT = 0x2003;                            // ��Ҷ���
const int SM_PLAYER_MSG = 0x2004;                                   // ��ҵ���Ϣ
const int SM_MULPLAYER_MSG = 0x2005;                                // Ⱥ������Ϣ


//PigServer���Э��
const int SM_PIG_MSG = 0x3001;		     						    //��תPig��Ϣ
const int SM_PIG_QUERY_AREA = 0x3002;                               //��ѯ������Ϣ

// Ĭ�ϵ������˿�
const int DEFAULT_AuthenServer_PORT = 2300;                         // AuthenServer <- DB
const int DEFAULT_DispatchGate_DB_PORT = 3300;                      // DispatchGate <- DB
const int DEFAULT_DispatchGate_CLIENT_PORT = 4300;                  // DispatchGate <- CLIENT
const int DEFAULT_DBServer_GS_PORT = 5300;                          // DBServer     <- GS
const int DEFAULT_DBServer_GG_PORT = 6300;                          // DBServer     <- GG
const int DEFAULT_GameServer_PORT = 7300;                           // GameServer   <- GG
const int DEFAULT_GameGate_PORT = 8300;                             // GameGate     <- CLIENT
const int DEFAULT_IMServer_GS_PORT = 9300;                          // IMServer  <- GS
const int DEFAULT_IMServer_GG_PORT = 7310;                          // IMServer <- GG
const int DEFAULT_RESCENTER_PORT = 9900;                            // ResourceServer <- Client
const int DEFAULT_RESCENTERCENTER_CR_PORT = 2900;                   // ResourceCenter <- ResourceServer
const int DEFAULT_RESCENTERCENTER_CD_PORT = 3900;                   // ResourceCenter <- DispatchGate
const int DEFAULT_WEBINTERFACE_PORT = 8010;                         // WEB <- WebClient
const int DEFAULT_WEBSERVER_PORT = 8011;                            // WEBInterface <- AuthenServer
const int DEFAULT_WEBQUERYSRV_PORT = 8012;                          // WEBQuery <- WebClient
const int DEFAULT_ACTLOG_SERVER_PORT = 8500;                        // GS <- ACTLog
const int DEFAULT_PIG_SERVER_PORT = 8600;                           // DispatchGate <- PigServer
const int DEFAULT_CONTROL_SERVER_PORT = 9800;                       // EventLog <- ControlClient


#endif //__CC_PROTOCOL_SERVER_H__