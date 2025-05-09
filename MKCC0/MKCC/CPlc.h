#pragma once
#include "PLC_DEF.h"

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
	UINT16  cab_ai[8];            //D10202:運転室PLC AI信号【モーメントリミッタ】
	INT16   cab_bi[4];            //D10210:運転室PLC→電気室PLC b出力
	UINT32  hcounter[4];          //D10214:高速カウンタユニット 
	UINT32  absocoder[3];         //D10222アブソコーダ 
	INT16   pc_fault[2];          //PC検出異常マップ
	INT16   erm_unitx[6];         //電気室ユニットX 020-03F：カウンタユニット　040-04F：ABSユニット
	INT16   erm_x[7];             //電気室X 080-0EF
	INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
	INT16   spare4[30];
}ST_PLC_WBUF_HHGH29, * LPST_PLC_WBUF_HHGH29;
typedef struct _ST_PLC_RBUF_HHGH29 {
	INT16   helthy;             // D10400:PLCヘルシーカウンタ
	INT16   plc_ctrl;           // D10401: PLC運転モード
	UINT16  cab_ai[4];          // D10402:運転室PLC→電気室PLC W出力
	INT16   cab_bi[4];          // D10406:運転室PLC→電気室PLC b出力
	INT16   cab_bxi[4];         // D10410:運転室PLC→電気室PLC b出力
	INT16   erm_900;			// D10414:
	INT16   erm_bo[8];          // D10415:電気室PLC b出力
	INT32   pos[4];             // D10423:各軸位置信号
	INT16   spd_tg[6];          // D10431:各軸指令速度目標
	INT16   plc_fault[17];      // D10437:故障信号
	INT16   erm_y[5];           // D10454:電気室PLC Y出力
	INT16   erm_x[7];           // D10459:電気室PLC X入力
	INT16   inv_cc_y[6];        //インバータPLC DO指令
	INT16   inv_cc_x[6];        //インバータFB書き込み値　ｘデバイス
	INT16   inv_cc_Ww1[6];      //インバータPLC 速度指令　rpm
	INT16   inv_cc_Wr1[6];      //インバータFB書き込み値　rpm
	INT16   inv_cc_Wr2[6];      //インバータFB書き込み値　トルク0.1%
	INT16   spare1[3];          //予備
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

//ノッチビットパターン
#define PTN_NOTCH_0			0x0001
#define PTN_NOTCH_F1		0x0002
#define PTN_NOTCH_F2		0x000A
#define PTN_NOTCH_F3		0x001A
#define PTN_NOTCH_F4		0x003A
#define PTN_NOTCH_R1		0x0004
#define PTN_NOTCH_R2		0x000C
#define PTN_NOTCH_R3		0x001C
#define PTN_NOTCH_R4		0x003C

/*** PLC IO構造体定義 ***/
#define CODE_PLCIO_WORD		0
#define CODE_PLCIO_BIT		1
#define CODE_PLCIO_DWORD	2
#define CODE_PLCIO_UNIT		3 //NOTCH CS等
#define CODE_PLCIO_FAULT	4 //故障

typedef struct _ST_PLC_IO_DEF {
	INT16   i;		//情報が含まれているバッファのインデックス
	INT16	mask;
	INT16	type;
	INT16	prm;	//ノッチ,CS：ビットシフト数,　Fault：バッファサイズ
}ST_PLC_IO_DEF, * LPST_PLC_IO_DEF;

typedef struct _ST_PLC_IO_RDEF {
	ST_PLC_IO_DEF estop;
	ST_PLC_IO_DEF csource;
	ST_PLC_IO_DEF faults;
}ST_PLC_IO_RDEF, * LPST_PLC_IO_RDEF;

typedef struct _ST_PLC_IO_WDEF {
	ST_PLC_IO_DEF estop;
	ST_PLC_IO_DEF csource;
	ST_PLC_IO_DEF faults;
}ST_PLC_IO_WDEF, * LPST_PLC_IO_WDEF;

class CPlc
{
	private:
	int crane_id;

public:
	CPlc(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	};
	virtual ~CPlc() {};

	static ST_PLC_IO_RDEF plc_io_rdef;
	static ST_PLC_IO_WDEF plc_io_wdef;

	int setup(int cran_id);

};

