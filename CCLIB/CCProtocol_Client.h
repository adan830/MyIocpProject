/**************************************************************************************
@author: �²�
@content: ��Ϸ�ͻ������������ͨ�ŵĳ����ͽṹ����
**************************************************************************************/

#ifndef __CC_PROTOCOL_CLIENT_H__
#define __CC_PROTOCOL_CLIENT_H__

// Client����������ͨѶͷ
typedef struct _TClientSocketHead
{
	unsigned long ulSign;				// �ָ��� CS_SEGMENTATION_SIGN
	unsigned short usPackageLen;		// ����ܳ���
	unsigned short usIdent;				// Ident or $8000 = ѹ��   [Ident>=$8000 ����ͷ��2�ֽڱ�ʾѹ��ǰ���ݳ��ȣ���������Ϊѹ����������Ident<$8000 ����ͷ��Ϊ������]
	unsigned long ulIdx;			    // �����,�������ݿ��ܻᱻѹ��
}TClientSocketHead, *PClientSocketHead;

//�ͻ��˵Ĵ�������
enum TClientWindowType{
	cwInValid = 0,                                          // ��Ч����
	cwAuthenWindow = 1,                                     // ��¼��֤����
	cwWarRule,                                              // ͬ���սЭ�鴰��
	cwQueue,                                                // �ŶӴ���
	cwCreateRole,                                           // ������ɫ����
	cwMessageBox,                                           // ��������
	cwDialog,
	cwNotice,                                               // ������Լ
	cwDealRequest,                                          // ����������ʾ��
	cwDealDialog,                                           // ���׿�
	cwVerifyCode,                                           // ��֤�봰��
	cwReLive,                                               // �����
	cwTransDialog,                                          // ���񴰿ڣ������͵Ĵ���ȷ�Ϸ���ʱ���뽫TransIDԭ������
	/*
		cwTransDialog���ʹ��ھ���˵����
		SCM_OPEN_WINDOW��Param 0:ȷ�Ͽ� 1������� 2 : 2�������
						 TransID������ID��TClientWindowRec�ṹ��Ϊ��ʾ��Ϣ
		CM_CLOSE_WINDOW��Param 0 : ȡ��   1��ȷ�� TransID������ID�����������TClientWindowRec�ṹ��Ϊ��������
	*/

	cwScriptDialog,                                         // �ű��Ի���
	cwNpcItemOper,                                          // Npc�Ե�����Ʒ�Ĳ�������
	/*
		cwNpcItemOper���ʹ��ھ���˵����
		Param : 1 ��Ʒ������
	*/

	cwNpcShop,                                              // Npc�̵괰��
	cwIMTransDialog,                                        // IMServer�����Ի���
	cwStorage,                                              // �ֿⴰ��
	cwMonCry,                                               // ����˵��
	/*
		cwMonCry���ʹ��ھ���˵����
		Param: ͷ������
		TransID : ��Ч����
		buf : ˵������
	*/

	cwCopyHint,                                             // ������ʾ
	/*
		cwCopyHint���ʹ��ھ���˵����
		Param: ����ID
	    TransID : 0 : ���� 1 : ͨ�� 2��ʧ��
	    buf : ��ʾ��Ϣ
	*/

	cwColdTime,                                             // ����ʱ
	/*
		cwColdTime���ʹ��ھ���˵����
		Param:����ʱʱ��
		TransID : ����ʱ����ID
		buf : ����ʱ��ʾ
	*/

	cwWeaponUpgrade,                                        // ��������
	cwGuildSet,                                             // �л����ý���
	cwYBDeal,                                               // Ԫ�����׿�
	cwGridData,                                             // �������ݴ���
	/*
		cwGridData���ʹ��ھ���˵����
		Param:TGridType
		buf : ���ڶ�������
	*/

	cwSafeCard,                                             //�ܱ�����֤
	/*
	    cwSafeCard���ʹ��ھ���˵����
		SCM_OPEN_WINDOW
		param : Ϊ3������ֵ
		CM_CLOSE_WINDOW : 
		BUF : Ϊ3�������Ӧ����ֵ�ַ���
	*/

	cwPayPwd,                                               // ֧������
	cwWeaponMelting,                                        // ��������
	cwRoleReName,                                           // ��ɫ������
	cwHopePool,                                             // ��Ը��
	cwEmail,                                                // �ʼ���
	cwPlayerShop,                                           // �������
	cwBag,                                                  // ����
	cwYearCeremony                                          // ���ʢ��
};

// CM_SELECT_SERVER
typedef struct _TCMSelectServer
{
	int iMaskServerID;				 // ѡ��Ķ���ķ�������
	int iCheckIP;                    // �������� �̶������� ��ַ���������ֲ�ͬ����������
}TCMSelectServer, *PCMSelectServer;

// CM_CLOSE_WINDOW��SCM_OPEN_WINDOW��SCM_CLOSE_WINDOW
typedef struct _TClientWindowRec
{
	TClientWindowType WinType;
	int Param;
	unsigned long TransID;          // ����ID
	//buf:��ʾ��Ϣ�ַ���
}TClientWindowRec, *PClientWindowRec;

// SCM_SELECT_AREA
typedef struct _TNextGateInfo
{
	int iSessionID;
	int iGateAddr;
	int iGatePort;
}TNextGateInfo, *PNextGateInfo;



const int CS_SEGMENTATION_CLIENTSIGN = 0XAABBCCDD;          // ��Ϸ�ͻ��˺ͷ�������ͨѶЭ����ʼ��־

//��Ϸ�ͻ��˷��͸�����������Ϣ
const int CM_PING = 10;										 // ����
const int CM_SELECT_SERVER_OLD = 11;						 // �ɰ�ѡ��Э��
const int CM_GAME_CONNECT = 12;								 // ���ӵ�GameGate
const int CM_CLOSE_WINDOW = 14;                              // �رմ���
const int CM_GAME_QUEUE = 15;                                // ��ѯ����
const int CM_REGIST_REQUEST = 17;                            // �½��ʺ�
const int CM_VERIFY_CODE = 18;                               // �ύ��֤��
const int CM_NEXT_VERIFY_CODE = 19;                          // ��һ����֤��
const int CM_LOGON_OK = 21;                                  // ������Ϸok
const int CM_RUSH = 99;										 // ǰ��
const int CM_QUIT = 100;									 // �����˳�

//��Ϸ�ͻ��˲�����Ҫ��Ϣ�����±��
const int CM_SELECT_SERVER = 3011;                           // ѡ��Э��
const int CM_AUTHEN_REQUEST = 3013;                          // ��֤����
const int CM_CREATE_ROLE = 3016;                             // ������ɫ
const int CM_ENTER_GAMESCENE = 3020;                         // ������Ϸ
const int CM_GPS_CHECK_RESPONSE = 4100;                      // ����ң����ݰ�����
const int CM_WALK = 4101;                                    //��
const int CM_RUN = 4102;                                     //��
const int CM_PHYHIT = 4103;                                  //������
const int CM_SPELL = 4104;                                   //ħ������
const int CM_SAY = 4107;                                     //�ͻ��˷���������Ϣ
const int CM_PICKUP = 4108;                                  //ʰȡ��Ʒ
const int CM_CLICK_NPC = 4115;                               //���NPC
const int CM_CLICK_NPC_OPTION = 4116;                        //���NPC�Ի����е�ѡ��
const int CM_USE_BAG_ITEM = 4117;                            //�ͻ���˫��ʹ�ñ�����Ʒ
const int CM_QUESTION_DETAIL_REQ = 4118;                     //�ͻ������������Ϣ
const int CM_QUESTION_ANSWER = 4119;                         //�ش�����
const int CM_PWDPROTECT_CHG = 4120;                          //�������뱣��״̬�л�

//�������˷�����Ϸ�ͻ��˵���Ϣ

const int SCM_OPEN_WINDOW = 14;                             // ��������
const int SCM_CLOSE_WINDOW = 15;                            // ���������رմ���
const int SCM_RESSERVER_INFO = 18;                          // ��Դ��������IP



#endif //__CC_PROTOCOL_CLIENT_H__