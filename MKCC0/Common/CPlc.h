#pragma once
#include "PLC_DEF.h"
#include "CFaults.h"

//######!!!!!!!! 通信バッファのアライメントはWORD単位にする#######################
//######!!!!!!!! 構造体のポインタで参照したときにずれる 	#######################
#pragma pack(push,2)

#define MASK_BIT_MH					0x00000001
#define MASK_BIT_BH					0x00000002
#define MASK_BIT_SL					0x00000004
#define MASK_BIT_GT					0x00000008
#define MASK_BIT_AH					0x00000010
#define MASK_BIT_PC_CTRL_ACTIVE		0x00008000
#define MASK_BIT_PC_SIM_MODE		0x00004000
#define MASK_BIT_PC_OPEPNL_ACTIVE   0x00002000
#define MASK_BIT_PC_AUTO_ACTIVE		0x00001000
#define MASK_BIT_RMT_ESTP_ACTIVE	0x00000800

#define ID_PLC_HCOUNT_MH			0
#define ID_PLC_HCOUNT_AH			1
#define ID_PLC_HCOUNT_BH			2
#define ID_PLC_HCOUNT_SL			3
#define ID_PLC_ABSO_MH				0
#define ID_PLC_ABSO_AH				1
#define ID_PLC_ABSO_GT				2

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
	INT16   helthy;						// D10300:PLCヘルシーカウンタ
	INT16   plc_ctrl;					// D10301: PLC運転モード
	UINT16  cab_ai[4];					// D10302:運転室PLC→電気室PLC W出力
	INT16   cab_bi[4];					// D10306:運転室PLC→電気室PLC b出力
	INT16   cab_xi[4];					// D10310:運転室PLC→電気室PLC b出力
	INT16   cab_bo;						// D10314:b2b0-f = Y70-F
	INT16   cab_yo;						// D10315:b160-f
	INT16   erm_900;					// D10316:
	INT16   erm_bo[8];					// D10317:電気室PLC b出力
	INT32	mh_z;						// D10325:主巻揚程　D1010
	INT32	r_01m;						// D10326:引込半径　D2772(W202)
	INT16	spar0[4];					// D10327:予備
	INT16	cv_tg[4];					// D10333:目標速度％
	INT16	spar1[2];					// D10337:予備
	INT16   plc_fault[N_PLC_FAULT_BUF]; // D10339:故障信号
	INT16   erm_y[5];					// D10357:電気室PLC Y出力
	INT16   erm_x[7];					// D10362:電気室PLC X入力
	INT16   spar2;						//
	INT16   inv_vref[4];				// D10370:インバータ速度指令
	INT16   inv_vfb[4];					// D10375:インバータ速度FB
	INT16   inv_trq[2];					// D10378:インバータトルク指令
	INT16   spar3[2];					// D10369:インバータPLC DO指令	
	INT32   hcount_fb[4];				// D10380:高速カウンタユニット FB値
	INT32   absocoder_fb[3];			// D10388:アブソコーダ FB値
	INT16   spar4[4];					// D10396:予備

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

typedef struct _ST_PLC_AXIS_SET {
	float	pos_fb;									// 位置
	INT16   notch_ref;								// ノッチ入力
	INT16   v_ref_tg;								// 目標速度
	INT16   v_ref;									// 速度指令
	INT16   v_fb;									// 速度FB
	INT16   trq_ref;								// トルク指令
	INT16   drum_layer;								// ドラム層
	INT16	brake;									// ブレーキ状態
	INT16	mode;									// モード
	INT16   limit;									// 制限
	INT16   fault;									// 故障
	INT32   absocoder;								// アブソコーダ
	INT32	pg_count;								// パルスジェネレータカウント
}ST_PLC_AXIS_SET, * LPST_PLC_AXIS_SET;

#define N_OTE_OPE_PLC_FAULT_BUF  20
typedef struct _ST_PLC_WBUF_HHGG38 {
	INT16   pc_healthy;								// D10650　PCヘルシー出力信号
	INT16   pc_status;								// D10651　PC Status
	INT16   crane_id;								// D10652　接続クレーンID
	INT16  rmt_status;								// D10653　遠隔ステータス
	INT16	spare0[6];								// D10654　予備
	INT16   lamp1;									// D10660　ランプ出力(Y80-8F）
	INT16   lamp2;									// D10661　ランプ出力(タッチパネル）
	float   mh_hight;								// D10662　主巻揚程
	float   ah_hight;								// D10664　補巻揚程
	float   mh_load;								// D10666　主巻荷重
	float   ah_load;								// D10668　補巻荷重
	float   r;										// D10670　半径
	float   wind_spd;								// D10672　風速
	INT16   sl_brk_fb1;								// D10674　旋回ブレーキFB（ブレーキレベル,ステータス）
	INT16   sl_brk_fb2;								// D10675　旋回ブレーキFB（エラー）
	INT16   sl_brk_fb3;								// D10676　旋回ブレーキFB（エラーコード）
	INT16   sl_brk_fb4;								// D10677　予備
	INT16   spare1[1];								// D10677　予備
	INT16   fault_group;							// D10679　故障コードグループ
	INT16   fault_code[N_OTE_OPE_PLC_FAULT_BUF];	// D10731　故障コード
	ST_PLC_AXIS_SET mh_set;							// D10700　主巻
	ST_PLC_AXIS_SET bh_set;							// D10716　引込
	ST_PLC_AXIS_SET sl_set;							// D10732　旋回
	ST_PLC_AXIS_SET gt_set;							// D10748　走行
	ST_PLC_AXIS_SET ah_set;							// D10764　補巻
	INT16   spare2[20];								// D10672　予備
}ST_PLC_WBUF_HHGG38, * LPST_PLC_WBUF_HHGG38;

#define OTE_CODE_OPEPLC_REMOTE_PB		0x0001
#define OTE_CODE_OPEPLC_AUTOMODE_PB		0x0002
#define OTE_CODE_OPEPLC_AUTO_SEL_MH_PB	0x0004
#define OTE_CODE_OPEPLC_AUTO_SEL_BH_PB	0x0008
#define OTE_CODE_OPEPLC_AUTO_SEL_SL_PB	0x0010
#define OTE_CODE_OPEPLC_AUTO_SEL_GT_PB	0x0020
#define OTE_CODE_OPEPLC_AUTO_SEL_AH_PB	0x0040

typedef struct _ST_PLC_RBUF_HHGG38 {
	INT16   plc_healthy;							// D10600: PLCヘルシーカウンタ
	INT16   plc_status;								// D10601: PLC運転モード
	INT16   auto_sw;								// D10602: 運転室PLC→電気室PLC W出力
	INT16   got_command;							// D10603: GOTモード
	INT16   got_crane_selected;						// D10604: GOT Crane
	INT16  spare0[5];								// D10605:
	INT16	ai_sl_foot;								// D10610:
	INT16	ai_spare[3];							// D10611:
	INT16	xin[10];								// D10614-23:
	INT16   spare1[3];								// D10624: 
	INT16   mh_mode_cs;								// D10627: 
	INT16   bh_mode_cs;								// D10628: 
	INT16   lamp_sw;								// D10629
	INT16	notch_RY0;								// D10630
	INT16	notch_RX0;								// D10631
	INT16	notch_LY0;								// D10632
	INT16	notch_LX0;								// D10633
	INT16	notch_R1;								// D10634
	INT16	notch_L1;								// D10635
	INT16   spare2[14];								// D10636: 
}ST_PLC_RBUF_HHGG38, * LPST_PLC_RBUF_HHGG38;

union UN_OPE_PLC_RBUF {
	INT16 rbuf[OTE_MC_SIZE_W_READ];
	ST_PLC_RBUF_HHGG38	st_hhgg38;
};

union UN_OPE_PLC_WBUF {
	INT16 wbuf[OTE_MC_SIZE_W_WRITE];
	ST_PLC_WBUF_HHGG38	st_hhgh38;
};


#pragma pack(pop)

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

	//B2B0
	ST_PLC_IO_DEF fault_bz;			//故障ブザー	
	ST_PLC_IO_DEF auto_kyusi;		//旋回自動給脂装置ランプ	

	//B160
	ST_PLC_IO_DEF syukan_comp_bz;	//主幹投入完了  Bz
	ST_PLC_IO_DEF syukan_mc_comp;	//主幹MC投入完了
	ST_PLC_IO_DEF fault_pl;			//故障ランプ
	ST_PLC_IO_DEF syukairo_comp;    //主回路準備完了
	ST_PLC_IO_DEF takamaki_mode;	//高巻モード
	ST_PLC_IO_DEF bh_rest_mode;		//引込レストモード
	ST_PLC_IO_DEF mercury_lamp_sw1;	//水銀灯スイッチ1
	ST_PLC_IO_DEF mercury_lamp_sw2;	//水銀灯スイッチ2
	ST_PLC_IO_DEF mercury_lamp_sw3;	//水銀灯スイッチ3
	ST_PLC_IO_DEF douryoku_ok;		//動力電源確立ランプ
	ST_PLC_IO_DEF siren_sw;			//モータサイレンスイッチ
	
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

	ST_PLC_IO_DEF target_v_mh;		//主巻目標速度
	ST_PLC_IO_DEF target_v_bh;		//引込目標速度
	ST_PLC_IO_DEF target_v_sl;		//旋回目標速度
	ST_PLC_IO_DEF target_v_gt;		//走行目標速度

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

	//揚程、旋回半径
	ST_PLC_IO_DEF h_mh_mm;
	ST_PLC_IO_DEF r_bh_mm;

}ST_PLC_IO_RIF, * LPST_PLC_IO_RIF;

typedef struct _ST_PLC_IO_WIF {
	//PLC制御
	ST_PLC_IO_DEF pc_healthy;
	ST_PLC_IO_DEF pc_ctrl_mode;
	//運転室操作台
	//B160　遠隔追加スイッチ
	ST_PLC_IO_DEF mercury_lamp_sw1;	//水銀灯スイッチ1
	ST_PLC_IO_DEF mercury_lamp_sw2;	//水銀灯スイッチ2
	ST_PLC_IO_DEF mercury_lamp_sw3;	//水銀灯スイッチ3
	ST_PLC_IO_DEF siren_sw;			//モータサイレンスイッチ
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

	CPlc(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	};

	virtual ~CPlc() {};

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

