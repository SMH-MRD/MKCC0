#pragma once
#include <ws2tcpip.h>

#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "NET_DEF.H"

#include <iostream>
#include <iomanip>
#include <sstream>

#define ID_SOCK_MC_CLIENT   PORT_MC_CLIENT
#define ID_SOCK_MC_SERVER   PORT_MC_SERVER

#define ID_SOCK_MC_CC_AGENT		PORT_MC_CLIENT
#define ID_SOCK_MC_OTE_AGENT    PORT_MC_CLIENT
#define ID_SOCK_MC_OTE_CS		PORT_MC_CLIENT

#define CC_MC_ADDR_W_READ	10300
#define CC_MC_SIZE_W_READ	100
#define CC_MC_ADDR_W_WRITE	10200
#define CC_MC_SIZE_W_WRITE	100

#define OTE_MC_ADDR_W_READ	10300
#define OTE_MC_SIZE_W_READ	100
#define OTE_MC_ADDR_W_WRITE	10200
#define OTE_MC_SIZE_W_WRITE	100

#define CODE_3E_FORMAT		0x50
#define CODE_4E_FORMAT		0x54
#define CODE_3E_NULL		0x00
#define CODE_NW				0x00
#define CODE_PC				0xFF

#define CODE_UIO			0x3FF
#define CODE_UIO_L			0xFF
#define CODE_UIO_H			0x03
#define CODE_UNODE			0x00

#define CODE_MON_TIMER		0x0001
#define CODE_DEVICE_D		0xA8

#define CODE_COMMAND_READ	0x0401
#define CODE_SUBCOM_READ	0x00	//WORD�P�ʓǂݏo��

#define CODE_COMMAND_WRITE	0x1401
#define CODE_SUBCOM_WRITE	0x00	//WORD�P�ʏ�������

#define MAX_MC_DATA			1024

#define MC_RES_WRITE		2		//������M�^�C�v�@��������
#define MC_RES_READ			3		//������M�^�C�v�@��������
#define MC_RES_ERR			0		//������M�^�C�v�@�G���[

//�v�����M�d���t�H�[�}�b�g
typedef struct _stXEMsgReq {
	UINT16	subcode;				//�T�u�w�b�_
	UINT16  serial;					//�V���A��
	UINT16	blank;					//��
	UINT8	nNW;					//�l�b�g���[�N�ԍ�
	UINT8	nPC;					//PC�ԍ�
	UINT16	nUIO;					//�v�����j�b�gIO�ԍ�
	UINT8	nUcode;					//�v�����j�b�g�ǔ�
	UINT16	len;					//�f�[�^��
	UINT16	timer;					//�Ď��^�C�}
	UINT16	com;					//�R�}���h
	UINT16	scom;					//�T�u�R�}���h
	UINT16	d_no;					//�f�o�C�X�ԍ�(3byte�\����2byte�̂ݗ��p���鎖�ɂ���j
	UINT8	d_no0;					//�f�o�C�X�ԍ�(3byte�\����2byte�̂ݗ��p���鎖�ɂ���j
	UINT8	d_code;					//�f�o�C�X�R�[�h
	UINT16	n_device;				//�ǂ�/�����f�o�C�X�_��
	UINT16	req_data[MAX_MC_DATA];	//�������݃f�[�^
}ST_XE_REQ, * LPST_XE_REQ;
//��M�d���t�H�[�}�b�g
typedef struct _stXEMsgRes {
	UINT16	subcode;				//�T�u�w�b�_
	UINT16  serial;					//�V���A��
	UINT16	blank;					//��
	UINT8	nNW;					//�l�b�g���[�N�ԍ�
	UINT8	nPC;					//PC�ԍ�
	UINT16	nUIO;					//�v�����j�b�gIO�ԍ�
	UINT8	nUcode;					//�v�����j�b�g�ǔ�
	UINT16	len;					//�f�[�^��
	UINT16	endcode;				//�I���R�[�h
	UINT16	res_data[MAX_MC_DATA];	//�ǂݍ��݃f�[�^
}ST_XE_RES, * LPST_XE_RES;

class CMCProtocol
{
public:
	CMCProtocol(INT32 _eventID);
	~CMCProtocol();	
	
	//�ʐM�d������M���e�\�z�p�o�b�t�@ 
	UINT8 read_req_snd_buf[MAX_MC_DATA * 2];	//�Ǎ��ݗv�����M�p�o�b�t�@
	UINT8 write_req_snd_buf[MAX_MC_DATA * 2];	//�����ݗv�����M�p�o�b�t�@

	UINT8 rcv_buf[MAX_MC_DATA * 2];
	UINT8 snd_buf[MAX_MC_DATA * 2];

	std::wostringstream msg_wos;

	//�ʐM���b�Z�[�W�o�b�t�@
	ST_XE_REQ mc_req_msg_r, mc_req_msg_w;
	ST_XE_RES mc_res_msg_r, mc_res_msg_w, mc_res_msg_err;
	
	//�ǂݏ����f�o�C�X�A�h���X,�o�b�t�@�T�C�Y�Z�b�g/�Q�b�g
	UINT16 D_no_r, D_no_w, n_D_read, n_D_write;
	void set_access_D_w(UINT16 no, UINT16 num) { D_no_w = no; n_D_write = num; return; };
	void set_access_D_r(UINT16 no, UINT16 num) { D_no_r = no; n_D_read = num; return; };

	//������/�I��
	HRESULT Initialize(HWND hwnd,int com_id);
	HRESULT close();

	HRESULT set_sndbuf_read_D_3E();				//3E�@D�f�o�C�X�ǂݍ��ݗv�����M�t�H�[�}�b�g�Z�b�g
	HRESULT send_read_req_D_3E();				//3E�t�H�[�}�b�g W�f�o�C�X�ǂݏo���v�����M

	HRESULT set_sndbuf_write_D_3E();			//3E�@D�f�o�C�X�������ݗv�����M�t�H�[�}�b�g�Z�b�g
	HRESULT send_write_req_D_3E(void* p_data);	//3E�t�H�[�}�b�g W�f�o�C�X�������ݗv�����M
		
	UINT rcv_msg_3E();
	
	//�f�o�b�O,�V�~�����[�^�p
	UINT16 snd_responce(ST_XE_REQ st_com, UINT16* pdata);
	HRESULT parse_snd_buf(UINT8* p8,LPST_XE_REQ pbuf);

private:
	INT32 eventID;

};