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

//CM_CLOSE_WINDOW��SCM_OPEN_WINDOW��SCM_CLOSE_WINDOW
typedef struct _TClientWindowRec
{
	TClientWindowType WinType;
	int Param;
	unsigned long TransID;          // ����ID
	//buf:��ʾ��Ϣ�ַ���
}TClientWindowRec, *PClientWindowRec;

const int CS_SEGMENTATION_CLIENTSIGN = 0XAABBCCDD;          // ��Ϸ�ͻ��˺ͷ�������ͨѶЭ����ʼ��־

//��Ϸ�ͻ��˷��͸�����������Ϣ

const int CM_CLOSE_WINDOW = 14;                             // �رմ���

//�������˷�����Ϸ�ͻ��˵���Ϣ

const int SCM_OPEN_WINDOW = 14;                             // ��������
const int SCM_CLOSE_WINDOW = 15;                            // ���������رմ���



#endif //__CC_PROTOCOL_CLIENT_H__