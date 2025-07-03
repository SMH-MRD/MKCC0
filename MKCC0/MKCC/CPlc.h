#pragma once
#include "PLC_DEF.h"


#define MASK_BIT_MH             0x00000001
#define MASK_BIT_BH             0x00000002
#define MASK_BIT_SL             0x00000004
#define MASK_BIT_GT             0x00000008
#define MASK_BIT_AH             0x00000010
#define MASK_BIT_PC_CTRL_ACTIVE  0x00008000
#define MASK_BIT_PC_SIM_MODE    0x00004000


/*** PLC IF�o�b�t�@�\���̒�` ***/
//�f�t�H���g�i�݂炢�j�̒�`
typedef struct _ST_PLC_WBUF {//����PC��PLC
	INT16   helthy;               //PC�w���V�[�o�͐M��
	INT16   ctrl_mode;            //��������t���O
	UINT16  cab_ai[8];            //�^�]��PLC AI�M���y�����p�z
	INT16   cab_bi[6];            //�^�]��PLC���d�C��PLC b�o��
	INT16   spare0[8];            //�^�]��PLC���d�C��PLC b�o��
	UINT32  hcounter[4];          //�����J�E���^���j�b�g 
	UINT32  absocoder[3];         //�A�u�\�R�[�_ 
	INT16   spare1[2];            //�\��
	INT16   pc_fault[2];          //PC���o�ُ�}�b�v
	INT16   spare2[16];           //�\��
	INT16   erm_unitx[4];         //�d�C�����j�b�gX 020-03F�F�J�E���^���j�b�g�@040-04F�@120-12F�FABS���j�b�g
	INT16   erm_x[8];             //�d�C��X 060-0CF
	INT16   inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare3[40];
}ST_PLC_WBUF, * LPST_PLC_WBUF;
typedef struct _ST_PLC_RBUF {
	INT16   helthy;               //PLC�w���V�[�J�E���^
	INT16   plc_ctrl;             // PLC�^�]���[�h
	UINT16  cab_ai[6];            //�^�]��PLC���d�C��PLC W�o��
	INT16   cab_bi[5];            //�^�]��PLC���d�C��PLC b�o��
	INT16   erm_900;
	INT16   erm_bo[6];            //�d�C��PLC b�o��
	INT32   pos[5];               //�e���ʒu�M��
	INT16   spd_tg[6];            //�e���w�ߑ��x�ڕW
	INT16   plc_fault[18];        //�e�����x�M��
	INT16   erm_y[4];             //�d�C��PLC Y�o��
	INT16   erm_x[8];             //�d�C��PLC X����
	INT16   inv_cc_y[6];          //�C���o�[�^PLC DO�w��
	INT16   inv_cc_Ww1[6];        //�C���o�[�^PLC ���x�w�߁@rpm
	INT16   inv_cc_x[6];          //�C���o�[�^FB�������ݒl�@���f�o�C�X
	INT16   inv_cc_Wr1[6];        //�C���o�[�^FB�������ݒl�@rpm
	INT16   inv_cc_Wr2[6];        //�C���o�[�^FB�������ݒl�@�g���N0.1%
	INT16   spare1[4];            //�\��
}ST_PLC_RBUF, * LPST_PLC_RBUF;

//�����x��70tJC102���̒�`
typedef struct _ST_PLC_WBUF_HHGH29 {//����PC��PLC
	INT16   helthy;               //D10200:PC�w���V�[�o�͐M��
	INT16   ctrl_mode;            //D10201:��������t���O
	UINT16  cab_ai[4];            //D10202:�^�]��PLC AI�M���y���[�����g���~�b�^�z
	INT16   cab_bi[4];            //D10206:�^�]��PLC���d�C��PLC b�o��
	INT16   pc_fault[2];          //D10210:����PC�̏�
	INT16   mh_load;              //D10212:�努�׏d
	INT16   ah_load;              //D10213:�⊪�׏d
	INT16   wind_spd;             //D10214:����
	INT16   spear[15];            //D10215:�\��
	UINT32  hcounter[4];          //D10230:�����J�E���^���j�b�g 
	UINT32  absocoder[3];         //D10238�@�A�u�\�R�[�_ 
	INT16   spare0[26];			  //D10244�@�\�� 
	INT16   spd_fb[4];			  //D10270:���xFB�i�����t���@3200���ō����x�j
	INT16   trq_ref[2];			  //D10274:�g���N�w�߁i�����t���@4000/200%�j
	INT16   spear1[24];		  //D10276:�C���o�[�^���x�w�߁i�����t���@3200���ō����x�j

}ST_PLC_WBUF_HHGH29, * LPST_PLC_WBUF_HHGH29;
typedef struct _ST_PLC_RBUF_HHGH29 {
	INT16   helthy;             // D10400:PLC�w���V�[�J�E���^
	INT16   plc_ctrl;           // D10401: PLC�^�]���[�h
	UINT16  cab_ai[4];          // D10402:�^�]��PLC���d�C��PLC W�o��
	INT16   cab_bi[4];          // D10406:�^�]��PLC���d�C��PLC b�o��
	INT16   cab_xi[4];          // D10410:�^�]��PLC���d�C��PLC b�o��
	INT16   cab_bo;				// D10414:b2b0-f = Y70-F
	INT16   cab_yo;				// D10415:b160-f
	INT16   erm_900;			// D10416:
	INT16   erm_bo[8];          // D10417:�d�C��PLC b�o��
	INT16	mh_z;				// D10425:�努�g���@D7100
	INT16	r_mm;				// D10426:�������a�@D2772(W202)
	INT16	spar0[6];			// D10427:�\��
	INT16	cv_tg[4];			// D10433:�ڕW���x��
	INT16	spar1[2];			// D10437:�\��
	INT16   plc_fault[18];      // D10439:�̏�M��
	INT16   erm_y[5];           // D10457:�d�C��PLC Y�o��
	INT16   erm_x[7];           // D10462:�d�C��PLC X����
	INT16   spar2;				//
	INT16   inv_vref[4];		// D10470:�C���o�[�^���x�w��
	INT16   inv_vfb[4];			// D10475:�C���o�[�^���x�w��
	INT16   inv_trq[2];			// D10478:�C���o�[�^�g���N�w��
	INT16   spar3[2];			// D10469:�C���o�[�^PLC DO�w��	
	INT32   hcount_fb[4];       // D10480:�����J�E���^���j�b�g FB�l
	INT32   absocoder_fb[3];    // D10488:�A�u�\�R�[�_ FB�l
	INT16   spar4[4];			// D10496:�\��

}ST_PLC_RBUF_HHGH29, * LPST_PLC_RBUF_HHGH29;

union UN_PLC_RBUF {
	INT16 rbuf[CC_MC_SIZE_W_READ];
	ST_PLC_RBUF			st;
	ST_PLC_RBUF_HHGH29	st_hhgh29;
};

union UN_PLC_WBUF {
	INT16 rbuf[CC_MC_SIZE_W_WRITE];
	ST_PLC_WBUF			st;
	ST_PLC_WBUF_HHGH29	st_hhgh29;
};


/*** PLC IO�\���̒�` ***/
#define CODE_PLCIO_WORD		0
#define CODE_PLCIO_BIT		1
#define CODE_PLCIO_DWORD	2
#define CODE_PLCIO_FLOAT	3 
#define CODE_PLCIO_DOUBLE	4 
#define CODE_PLCIO_BITS		5 //NOTCH CS��
#define CODE_PLCIO_BIT_NC	6 //�m�[�}���N���[�Y

union UN_IF_VALUE {
	INT16 i16;
	INT32 i32;
	double d;
	float f;
};

typedef struct _ST_PLC_IO_DEF {
	INT16*	pi16;	//�M���������Ă���o�b�t�@�̃A�h���X
	INT16	mask;	//�M�����o�p�}�X�N;
	INT16	type;	//
	INT16	shift;	//�r�b�g�V�t�g��
	INT16   size;	//�o�b�t�@�T�C�Y
}ST_PLC_IO_DEF, * LPST_PLC_IO_DEF;

typedef struct _ST_PLC_IO_RIF {
	//PLC����
	ST_PLC_IO_DEF plc_healthy;
	ST_PLC_IO_DEF plc_ctrl_fb;
	//�^�]�������
	//B220
	ST_PLC_IO_DEF syukan_on;
	ST_PLC_IO_DEF syukan_off;
	ST_PLC_IO_DEF mh_spd_cs;
	ST_PLC_IO_DEF bh_mode_cs;
	ST_PLC_IO_DEF ah_use_sel;
	ST_PLC_IO_DEF ah_notch; //�⊪�m�b�`
	//B230
	ST_PLC_IO_DEF mh_notch;
	ST_PLC_IO_DEF gt_spd_sel;
	ST_PLC_IO_DEF gt_notch;
	ST_PLC_IO_DEF estop;
	//B240
	ST_PLC_IO_DEF ah_under_limit;	//�⊪�����׏d�ȉ�
	ST_PLC_IO_DEF mlim_warn_1;		//Ӱ����Я���������ؑ։׏d�ȉ�
	ST_PLC_IO_DEF mlim_warn_2;		//Ӱ����Я�3�{�����׏d�ȉ�
	ST_PLC_IO_DEF mlim_90;			//Ӱ����Я�90%�׏d
	ST_PLC_IO_DEF mlim_100;			//Ӱ����Я�100%�׏d
	ST_PLC_IO_DEF mlim_under_hmh;	//Ӱ����Я������׏d�ȉ�
	ST_PLC_IO_DEF mlim_normal;		//Ӱ����Я�����
	ST_PLC_IO_DEF wind_over16;
	ST_PLC_IO_DEF alarm_stp_pb;
	ST_PLC_IO_DEF fault_reset_pb;
	ST_PLC_IO_DEF bypass_pb;
	ST_PLC_IO_DEF mhbk_normal_ss;
	ST_PLC_IO_DEF mhbk_emr_ss;
	ST_PLC_IO_DEF mhbk_opn_pb;
	ST_PLC_IO_DEF ahbk_emr_low_ss;
	//B250
	ST_PLC_IO_DEF bh_notch;
	ST_PLC_IO_DEF sl_brake;			//����u���[�L
	ST_PLC_IO_DEF sl_notch;
	ST_PLC_IO_DEF sl_hydr_press_sw;	//����������̓X�C�b�`
	//B160
	ST_PLC_IO_DEF syukan_comp_bz;	//�劲��������  Bz
	ST_PLC_IO_DEF syukan_mc_comp;	//�劲MC��������
	ST_PLC_IO_DEF fault_pl;			//�̏჉���v
	ST_PLC_IO_DEF syukairo_comp;    //���H��������
	ST_PLC_IO_DEF takamaki_mode;	//�������[�h
	ST_PLC_IO_DEF bh_rest_mode;		//�������X�g���[�h
	ST_PLC_IO_DEF sl_fix_pl;		//����Œ�PL
	ST_PLC_IO_DEF ah_ari_jc;		//�⊪�LJC
	ST_PLC_IO_DEF sl_fix_pl2;		//����Œ�PL
	ST_PLC_IO_DEF douryoku_ok;		//���͓d���m��
	//X0C0
	ST_PLC_IO_DEF brk_mc3_fb;		//�u���[�L�劲�A���T�[�o�b�N
	ST_PLC_IO_DEF mh_brk1_fb;		//�努�u���[�L�A���T�[�o�b�N
	ST_PLC_IO_DEF bh_brk_fb;		//�����u���[�L�A���T�[�o�b�N
	ST_PLC_IO_DEF gt_brk_fb;		//���s�u���[�L�A���T�[�o�b�N

	//�C���o�[�^�ւ̎w�ߏo�͓��e
	ST_PLC_IO_DEF inv_fwd_mh;		//�努�C���o�[�^�w��FWD
	ST_PLC_IO_DEF inv_rev_mh;		//�努�C���o�[�^�w��REV
	ST_PLC_IO_DEF inv_fwd_bh;		//�����C���o�[�^�w��FWD
	ST_PLC_IO_DEF inv_rev_bh;		//�����C���o�[�^�w��REV
	ST_PLC_IO_DEF inv_fwd_sl;		//����C���o�[�^�w��FWD
	ST_PLC_IO_DEF inv_rev_sl;		//����C���o�[�^�w��REV
	ST_PLC_IO_DEF inv_fwd_gt;		//���s�C���o�[�^�w��FWD
	ST_PLC_IO_DEF inv_rev_gt;		//���s�C���o�[�^�w��REV

	ST_PLC_IO_DEF inv_vref_mh;		//�努�C���o�[�^���x�w��
	ST_PLC_IO_DEF inv_vref_bh;		//�����C���o�[�^���x�w��
	ST_PLC_IO_DEF inv_vref_sl;		//����C���o�[�^���x�w��
	ST_PLC_IO_DEF inv_vref_gt;		//���s�C���o�[�^���x�w��

	ST_PLC_IO_DEF inv_vfb_mh;		//�努�C���o�[�^���xFB
	ST_PLC_IO_DEF inv_vfb_bh;		//�����C���o�[�^���xFB
	ST_PLC_IO_DEF inv_vfb_sl;		//����C���o�[�^���xFB
	ST_PLC_IO_DEF inv_vfb_gt;		//���s�C���o�[�^���xFB

	ST_PLC_IO_DEF inv_trqref_mh;	//�努�C���o�[�^�g���N�w��
	ST_PLC_IO_DEF inv_trqref_bh;	//�����C���o�[�^�g���N�w��

	//�����J�E���^���j�b�g
	ST_PLC_IO_DEF hcounter_mh;
	ST_PLC_IO_DEF hcounter_bh;
	ST_PLC_IO_DEF hcounter_sl;
	//�A�u�\�R�[�_
	ST_PLC_IO_DEF absocoder_mh;
	ST_PLC_IO_DEF absocoder_gt;
	

}ST_PLC_IO_RIF, * LPST_PLC_IO_RIF;

typedef struct _ST_PLC_IO_WIF {
	//PLC����
	ST_PLC_IO_DEF pc_healthy;
	ST_PLC_IO_DEF pc_ctrl_mode;
	//�^�]�������
	//B220
	ST_PLC_IO_DEF syukan_on;
	ST_PLC_IO_DEF syukan_off;
	ST_PLC_IO_DEF mh_spd_cs;
	ST_PLC_IO_DEF bh_mode_cs;
	ST_PLC_IO_DEF ah_use_sel;
	ST_PLC_IO_DEF ah_notch; //�⊪�m�b�`
	//B230
	ST_PLC_IO_DEF mh_notch;
	ST_PLC_IO_DEF gt_spd_sel;
	ST_PLC_IO_DEF gt_notch;
	ST_PLC_IO_DEF estop;
	//B240
	ST_PLC_IO_DEF ah_under_limit;	//�⊪�����׏d�ȉ�
	ST_PLC_IO_DEF mlim_warn_1;		//Ӱ����Я���������ؑ։׏d�ȉ�
	ST_PLC_IO_DEF mlim_warn_2;		//Ӱ����Я�3�{�����׏d�ȉ�
	ST_PLC_IO_DEF mlim_90;			//Ӱ����Я�90%�׏d
	ST_PLC_IO_DEF mlim_100;			//Ӱ����Я�100%�׏d
	ST_PLC_IO_DEF mlim_under_hmh;	//Ӱ����Я������׏d�ȉ�
	ST_PLC_IO_DEF mlim_normal;		//Ӱ����Я�����
	ST_PLC_IO_DEF wind_over16;
	ST_PLC_IO_DEF alarm_stp_pb;
	ST_PLC_IO_DEF fault_reset_pb;
	ST_PLC_IO_DEF bypass_pb;
	ST_PLC_IO_DEF mhbk_normal_ss;
	ST_PLC_IO_DEF mhbk_emr_ss;
	ST_PLC_IO_DEF mhbk_opn_pb;
	ST_PLC_IO_DEF ahbk_emr_low_ss;
	//B250
	ST_PLC_IO_DEF bh_notch;
	ST_PLC_IO_DEF sl_brake;			//����u���[�L
	ST_PLC_IO_DEF sl_notch;
	ST_PLC_IO_DEF sl_hydr_press_sw;	//����������̓X�C�b�`

	//�����J�E���^���j�b�g
	ST_PLC_IO_DEF hcounter_mh;
	ST_PLC_IO_DEF hcounter_bh;
	ST_PLC_IO_DEF hcounter_sl;
	//�A�u�\�R�[�_
	ST_PLC_IO_DEF absocoder_mh;
	ST_PLC_IO_DEF absocoder_gt;

	//INV�o��
	ST_PLC_IO_DEF vfb_mh;
	ST_PLC_IO_DEF vfb_bh;
	ST_PLC_IO_DEF vfb_sl;
	ST_PLC_IO_DEF vfb_gt;
	ST_PLC_IO_DEF trqref_mh;
	ST_PLC_IO_DEF trqref_bh;

}ST_PLC_IO_WIF, * LPST_PLC_IO_WIF;

class CPlc
{
private:
	int crane_id;
	INT16* pbuf_r;
	INT16* pbuf_w;

public:
	CPlc(int _crane_id, INT16* _pbuf_r, INT16* _pbuf_w) {
		crane_id = _crane_id;
		set_rbuf(_pbuf_r);
		set_wbuf(_pbuf_w);
		setup(crane_id);
	};
	virtual ~CPlc() {};

	static ST_PLC_IO_RIF plc_io_rif;
	static ST_PLC_IO_WIF plc_io_wif;

	void set_rbuf(INT16* _pbuf_r) {
		pbuf_r = _pbuf_r;
	};
	void set_wbuf(INT16* _pbuf_w) {
		pbuf_w = _pbuf_w;
	};

	int setup(int cran_id);

	UN_IF_VALUE rval(ST_PLC_IO_DEF st_r_def);
	HRESULT wval(ST_PLC_IO_DEF st_w_def, INT16 val);
	HRESULT wval(ST_PLC_IO_DEF st_w_def, double val);
	HRESULT wval(ST_PLC_IO_DEF st_w_def, float val);
	HRESULT wval(ST_PLC_IO_DEF st_w_def, INT32 val);

};

