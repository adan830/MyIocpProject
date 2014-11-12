/**************************************************************************************
@author: �²�
@content: ��������ͨ�ŵĳ����ͽṹ����
**************************************************************************************/

#ifndef __CC_PROTOCOL_SERVER_H__
#define __CC_PROTOCOL_SERVER_H__

// Server��ͨѶ��Э��ͷ
typedef struct _TServerSocketHeader
{
	unsigned long ulSign;				// �ָ��� SEGMENTATION_SIGN
	int iParam;							// ��չ����
	unsigned short usIdent;				// Э���
	unsigned short usBehindLen;			// �������ݳ���
}TServerSocketHeader, *PServerSocketHeader;



const int SS_SEGMENTATION_SIGN = 0XFFEEDDCC;                        // Server֮��ͨ��Э����ʼ��־

// ��������Э��
const int SM_REGISTER = 0x1000;			                            // ע�������
const int SM_UNREGISTER = 0x1001;							        // ע��������
const int SM_PING = 0x1002;								            // �������

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