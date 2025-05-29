#pragma once
#include "PLC_DEF.h"


#define MASK_BIT_MH             0x00000001
#define MASK_BIT_BH             0x00000002
#define MASK_BIT_SL             0x00000004
#define MASK_BIT_GT             0x00000008
#define MASK_BIT_AH             0x00000010
#define MASK_BIT_PC_COM_ACTIVE  0x00008000
#define MASK_BIT_PC_DBG_MODE    0x00004000


/*** PLC IFバッファ構造体定義 ***/
//デフォルト（みらい）の定義
typedef struct _ST_PLC_WBUF {//制御PC→PLC
	INT16   helthy;               //PCヘルシー出力信号
	INT16   ctrl_mode;            //自動制御フラグ
	UINT16  cab_ai[8];            //運転室PLC AI信号【将来用】
	INT16   cab_bi[6];            //運転室PLC→電気室PLC b出力
	INT16   spare0[8];            //運転室PLC→電気室PLC b出力
	UINT32  hcounter[4];          //高速カウンタユニット 
	UINT32  absocoder[3];         //アブソコーダ 
	INT16   spare1[2];            //予備
	INT16   pc_fault[2];          //PC検出異常マップ
	INT16   spare2[16];           //予備
	INT16   erm_unitx[4];         //電気室ユニットX 020-03F：カウンタユニット　040-04F　120-12F：ABSユニット
	INT16   erm_x[8];             //電気室X 060-0CF
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare3[40];
}ST_PLC_WBUF, * LPST_PLC_WBUF;
typedef struct _ST_PLC_RBUF {
	INT16   helthy;               //PLCヘルシーカウンタ
	INT16   plc_ctrl;             // PLC運転モード
	UINT16  cab_ai[6];            //運転室PLC→電気室PLC W出力
	INT16   cab_bi[5];            //運転室PLC→電気室PLC b出力
	INT16   erm_900;
	INT16   erm_bo[6];            //電気室PLC b出力
	INT32   pos[5];               //各軸位置信号
	INT16   spd_tg[6];            //各軸指令速度目標
	INT16   plc_fault[18];        //各軸速度信号
	INT16   erm_y[4];             //電気室PLC Y出力
	INT16   erm_x[8];             //電気室PLC X入力
	INT16   inv_cc_y[6];          //インバータPLC DO指令
	INT16   inv_cc_Ww1[6];        //インバータPLC 速度指令　rpm
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare1[4];            //予備
}ST_PLC_RBUF, * LPST_PLC_RBUF;

//西多度津70tJC102号の定義
typedef struct _ST_PLC_WBUF_HHGH29 {//制御PC→PLC
	INT16   helthy;               //D10200:PCヘルシー出力信号
	INT16   ctrl_mode;            //D10201:自動制御フラグ
	UINT16  cab_ai[4];            //D10202:運転室PLC AI信号【モーメントリミッタ】
	INT16   cab_bi[4];            //D10206:運転室PLC→電気室PLC b出力
	UINT32  hcounter[4];          //D10210:高速カウンタユニット 
	UINT32  absocoder[3];         //D10218　アブソコーダ 
	INT16   pc_fault[2];          //D10224　PC検出異常マップ
	INT16   erm_unitx[6];         //電気室ユニットX 020-03F：カウンタユニット　040-04F：ABSユニット
	INT16   erm_x[7];             //電気室X 080-0EF
	INT16   spare[30];
}ST_PLC_WBUF_HHGH29, * LPST_PLC_WBUF_HHGH29;
typedef struct _ST_PLC_RBUF_HHGH29 {
	INT16   helthy;             // D10400:PLCヘルシーカウンタ
	INT16   plc_ctrl;           // D10401: PLC運転モード
	UINT16  cab_ai[4];          // D10402:運転室PLC→電気室PLC W出力
	INT16   cab_bi[4];          // D10406:運転室PLC→電気室PLC b出力
	INT16   cab_xi[4];          // D10410:運転室PLC→電気室PLC b出力
	INT16   cab_bo;				// D10414:運転室PLCランプ出力	
	INT16   cab_yo;				// D10415:運転室PLCランプ出力
	INT16   erm_900;			// D10416:
	INT16   erm_bo[8];          // D10417:電気室PLC b出力
	INT32   pos[4];             // D10425:各軸位置信号
	INT16   spd_tg[6];          // D10432:各軸指令速度目標
	INT16   plc_fault[18];      // D10437:故障信号
	INT16   erm_y[5];           // D10457:電気室PLC Y出力
	INT16   erm_x[7];           // D10462:電気室PLC X入力
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


/*** PLC IO構造体定義 ***/
#define CODE_PLCIO_WORD		0
#define CODE_PLCIO_BIT		1
#define CODE_PLCIO_DWORD	2
#define CODE_PLCIO_FLOAT	3 
#define CODE_PLCIO_DOUBLE	4 
#define CODE_PLCIO_BITS		5 //NOTCH CS等
#define CODE_PLCIO_BIT_NC	6 //ノーマルクローズ

union UN_IF_VALUE {
	INT16 i16;
	INT32 i32;
	double d;
	float f;
};

typedef struct _ST_PLC_IO_DEF {
	INT16*	pi16;	//信号が入っているバッファのアドレス
	INT16	mask;	//信号抽出用マスク;
	INT16	type;	//
	INT16	shift;	//ビットシフト数
	INT16   size;	//バッファサイズ
}ST_PLC_IO_DEF, * LPST_PLC_IO_DEF;

typedef struct _ST_PLC_IO_RIF {
	//PLC制御
	ST_PLC_IO_DEF plc_healthy;
	ST_PLC_IO_DEF plc_ctrl_fb;
	//運転室操作台
	//B220
	ST_PLC_IO_DEF syukan_on;
	ST_PLC_IO_DEF syukan_off;
	ST_PLC_IO_DEF mh_spd_cs;
	ST_PLC_IO_DEF bh_mode_cs;
	ST_PLC_IO_DEF ah_use_sel;
	ST_PLC_IO_DEF ah_notch; //補巻ノッチ
	//B230
	ST_PLC_IO_DEF mh_notch;
	ST_PLC_IO_DEF gt_spd_sel;
	ST_PLC_IO_DEF gt_notch;
	ST_PLC_IO_DEF estop;
	//B240
	ST_PLC_IO_DEF ah_under_limit;	//補巻制限荷重以下
	ST_PLC_IO_DEF mlim_warn_1;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下
	ST_PLC_IO_DEF mlim_warn_2;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下
	ST_PLC_IO_DEF mlim_90;			//ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重
	ST_PLC_IO_DEF mlim_100;			//ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重
	ST_PLC_IO_DEF mlim_under_hmh;	//ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下
	ST_PLC_IO_DEF mlim_normal;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ正常
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
	ST_PLC_IO_DEF sl_brake;			//旋回ブレーキ
	ST_PLC_IO_DEF sl_notch;
	ST_PLC_IO_DEF sl_hydr_press_sw;	//旋回油圧圧力スイッチ
	//B160
	ST_PLC_IO_DEF syukan_comp_bz;	//主幹投入完了  Bz
	ST_PLC_IO_DEF syukan_mc_comp;	//主幹MC投入完了
	ST_PLC_IO_DEF fault_pl;			//故障ランプ
	ST_PLC_IO_DEF syukairo_comp;    //主回路準備完了
	ST_PLC_IO_DEF takamaki_mode;	//高巻モード
	ST_PLC_IO_DEF bh_rest_mode;		//引込レストモード
	ST_PLC_IO_DEF sl_fix_pl;		//旋回固定PL
	ST_PLC_IO_DEF ah_ari_jc;		//補巻有JC
	ST_PLC_IO_DEF sl_fix_pl2;		//旋回固定PL
	ST_PLC_IO_DEF douryoku_ok;		//動力電源確立

}ST_PLC_IO_RIF, * LPST_PLC_IO_RIF;

typedef struct _ST_PLC_IO_WIF {
	//PLC制御
	ST_PLC_IO_DEF pc_healthy;
	ST_PLC_IO_DEF pc_ctrl_mode;
	//運転室操作台
	//B220
	ST_PLC_IO_DEF syukan_on;
	ST_PLC_IO_DEF syukan_off;
	ST_PLC_IO_DEF mh_spd_cs;
	ST_PLC_IO_DEF bh_mode_cs;
	ST_PLC_IO_DEF ah_use_sel;
	ST_PLC_IO_DEF ah_notch; //補巻ノッチ
	//B230
	ST_PLC_IO_DEF mh_notch;
	ST_PLC_IO_DEF gt_spd_sel;
	ST_PLC_IO_DEF gt_notch;
	ST_PLC_IO_DEF estop;
	//B240
	ST_PLC_IO_DEF ah_under_limit;	//補巻制限荷重以下
	ST_PLC_IO_DEF mlim_warn_1;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ旋回加速切替荷重以下
	ST_PLC_IO_DEF mlim_warn_2;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ3倍速許可荷重以下
	ST_PLC_IO_DEF mlim_90;			//ﾓｰﾒﾝﾄﾘﾐｯﾀ90%荷重
	ST_PLC_IO_DEF mlim_100;			//ﾓｰﾒﾝﾄﾘﾐｯﾀ100%荷重
	ST_PLC_IO_DEF mlim_under_hmh;	//ﾓｰﾒﾝﾄﾘﾐｯﾀ高巻荷重以下
	ST_PLC_IO_DEF mlim_normal;		//ﾓｰﾒﾝﾄﾘﾐｯﾀ正常
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
	ST_PLC_IO_DEF sl_brake;			//旋回ブレーキ
	ST_PLC_IO_DEF sl_notch;
	ST_PLC_IO_DEF sl_hydr_press_sw;	//旋回油圧圧力スイッチ

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

