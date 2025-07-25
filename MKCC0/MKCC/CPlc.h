#pragma once
#include "PLC_DEF.h"
#include "CFaults.h"


#define MASK_BIT_MH             0x00000001
#define MASK_BIT_BH             0x00000002
#define MASK_BIT_SL             0x00000004
#define MASK_BIT_GT             0x00000008
#define MASK_BIT_AH             0x00000010
#define MASK_BIT_PC_CTRL_ACTIVE  0x00008000
#define MASK_BIT_PC_SIM_MODE    0x00004000

#define ID_PLC_HCOUNT_MH      0
#define ID_PLC_HCOUNT_AH      1
#define ID_PLC_HCOUNT_BH      2
#define ID_PLC_HCOUNT_SL      3
#define ID_PLC_ABSO_MH      0
#define ID_PLC_ABSO_AH      1
#define ID_PLC_ABSO_GT      2

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
typedef struct _ST_PLC_WBUF_HHGH29 {	//制御PC→PLC
	INT16   helthy;						//D10200:PCヘルシー出力信号
	INT16   ctrl_mode;					//D10201:自動制御フラグ
	UINT16  cab_ai[4];					//D10202:運転室PLC AI信号【モーメントリミッタ】
	INT16   cab_bi[4];					//D10206:運転室PLC→電気室PLC b出力
	INT16   spare[2];					//D10210:予備
	INT16   mh_load;					//D10212:主巻荷重
	INT16   ah_load;					//D10213:補巻荷重
	INT16   wind_spd;					//D10214:風速
	INT16   spear[15];					//D10215:予備
	UINT32  hcounter[4];				//D10230:高速カウンタユニット 
	UINT32  absocoder[3];				//D10238　アブソコーダ 
	INT16   pc_fault[N_PLC_FAULT_BUF];	//D10244　PC　FAULT
	INT16   spare0[8];					//D10244　予備 
	INT16   spd_fb[4];					//D10270:速度FB（符号付き　3200が最高速度）
	INT16   trq_ref[2];					//D10274:トルク指令（符号付き　4000/200%）
	INT16   spear1[24];					//D10276:インバータ速度指令（符号付き　3200が最高速度）

}ST_PLC_WBUF_HHGH29, * LPST_PLC_WBUF_HHGH29;
typedef struct _ST_PLC_RBUF_HHGH29 {
	INT16   helthy;						// D10400:PLCヘルシーカウンタ
	INT16   plc_ctrl;					// D10401: PLC運転モード
	UINT16  cab_ai[4];					// D10402:運転室PLC→電気室PLC W出力
	INT16   cab_bi[4];					// D10406:運転室PLC→電気室PLC b出力
	INT16   cab_xi[4];					// D10410:運転室PLC→電気室PLC b出力
	INT16   cab_bo;						// D10414:b2b0-f = Y70-F
	INT16   cab_yo;						// D10415:b160-f
	INT16   erm_900;					// D10416:
	INT16   erm_bo[8];					// D10417:電気室PLC b出力
	INT16	mh_z;						// D10425:主巻揚程　D7100
	INT16	r_01m;						// D10426:引込半径　D2772(W202)
	INT16	spar0[6];					// D10427:予備
	INT16	cv_tg[4];					// D10433:目標速度％
	INT16	spar1[2];					// D10437:予備
	INT16   plc_fault[N_PLC_FAULT_BUF]; // D10439:故障信号
	INT16   erm_y[5];					// D10457:電気室PLC Y出力
	INT16   erm_x[7];					// D10462:電気室PLC X入力
	INT16   spar2;						//
	INT16   inv_vref[4];				// D10470:インバータ速度指令
	INT16   inv_vfb[4];					// D10475:インバータ速度FB
	INT16   inv_trq[2];					// D10478:インバータトルク指令
	INT16   spar3[2];					// D10469:インバータPLC DO指令	
	INT32   hcount_fb[4];				// D10480:高速カウンタユニット FB値
	INT32   absocoder_fb[3];			// D10488:アブソコーダ FB値
	INT16   spar4[4];					// D10496:予備

}ST_PLC_RBUF_HHGH29, * LPST_PLC_RBUF_HHGH29;

union UN_PLC_RBUF {
	INT16 rbuf[CC_MC_SIZE_W_READ];
	ST_PLC_RBUF			st;
	ST_PLC_RBUF_HHGH29	st_hhgh29;
};

union UN_PLC_WBUF {
	INT16 wbuf[CC_MC_SIZE_W_WRITE];
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
	//X0C0
	ST_PLC_IO_DEF brk_mc3_fb;		//ブレーキ主幹アンサーバック
	ST_PLC_IO_DEF mh_brk1_fb;		//主巻ブレーキアンサーバック
	ST_PLC_IO_DEF bh_brk_fb;		//引込ブレーキアンサーバック
	ST_PLC_IO_DEF gt_brk_fb;		//走行ブレーキアンサーバック

	//インバータへの指令出力内容
	ST_PLC_IO_DEF inv_fwd_mh;		//主巻インバータ指令FWD
	ST_PLC_IO_DEF inv_rev_mh;		//主巻インバータ指令REV
	ST_PLC_IO_DEF inv_fwd_bh;		//引込インバータ指令FWD
	ST_PLC_IO_DEF inv_rev_bh;		//引込インバータ指令REV
	ST_PLC_IO_DEF inv_fwd_sl;		//旋回インバータ指令FWD
	ST_PLC_IO_DEF inv_rev_sl;		//旋回インバータ指令REV
	ST_PLC_IO_DEF inv_fwd_gt;		//走行インバータ指令FWD
	ST_PLC_IO_DEF inv_rev_gt;		//走行インバータ指令REV

	ST_PLC_IO_DEF inv_vref_mh;		//主巻インバータ速度指令
	ST_PLC_IO_DEF inv_vref_bh;		//引込インバータ速度指令
	ST_PLC_IO_DEF inv_vref_sl;		//旋回インバータ速度指令
	ST_PLC_IO_DEF inv_vref_gt;		//走行インバータ速度指令

	ST_PLC_IO_DEF inv_vfb_mh;		//主巻インバータ速度FB
	ST_PLC_IO_DEF inv_vfb_bh;		//引込インバータ速度FB
	ST_PLC_IO_DEF inv_vfb_sl;		//旋回インバータ速度FB
	ST_PLC_IO_DEF inv_vfb_gt;		//走行インバータ速度FB

	ST_PLC_IO_DEF inv_trqref_mh;	//主巻インバータトルク指令
	ST_PLC_IO_DEF inv_trqref_bh;	//引込インバータトルク指令

	//高速カウンタユニット
	ST_PLC_IO_DEF hcounter_mh;
	ST_PLC_IO_DEF hcounter_bh;
	ST_PLC_IO_DEF hcounter_sl;
	//アブソコーダ
	ST_PLC_IO_DEF absocoder_mh;
	ST_PLC_IO_DEF absocoder_gt;
	
	//荷重
	ST_PLC_IO_DEF m;

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

	//高速カウンタユニット
	ST_PLC_IO_DEF hcounter_mh;
	ST_PLC_IO_DEF hcounter_bh;
	ST_PLC_IO_DEF hcounter_sl;
	//アブソコーダ
	ST_PLC_IO_DEF absocoder_mh;
	ST_PLC_IO_DEF absocoder_gt;

	//INV出力
	ST_PLC_IO_DEF vfb_mh;
	ST_PLC_IO_DEF vfb_bh;
	ST_PLC_IO_DEF vfb_sl;
	ST_PLC_IO_DEF vfb_gt;
	ST_PLC_IO_DEF trqref_mh;
	ST_PLC_IO_DEF trqref_bh;

	//モーメントリミッタ
	ST_PLC_IO_DEF mlim_weight_ai;	//モーメントリミッタ荷重AI
	ST_PLC_IO_DEF mlim_r_ai;		//モーメントリミッタ旋回半径AI

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

	//static ST_PLC_IO_RIF plc_io_rif;
	//static ST_PLC_IO_WIF plc_io_wif;
	ST_PLC_IO_RIF plc_io_rif;
	ST_PLC_IO_WIF plc_io_wif;

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

