#pragma once
#include <Windows.h>
#include "COMMON_DEF.h"

#define N_PLC_FAULT_BUF				18
#define N_ALL_FAULT_BUF				N_PLC_FAULT_BUF
#define N_ALL_FAULTS				(N_PLC_FAULT_BUF)*16

//故障種別ビット
#define FAULT_CLEAR					BIT0
#define FAULT_HEAVY1				BIT1
#define FAULT_HEAVY2				BIT2
#define FAULT_HEAVY3				BIT3
#define FAULT_LIGHT					BIT4
#define FAULT_INTERLOCK				BIT5


//動作制限ビット
#define FAULT_COMMON_HEVY			BIT0
#define FAULT_AXIS_MH_HEVY			BIT1
#define FAULT_AXIS_BH_HEVY			BIT2
#define FAULT_AXIS_SL_HEVY			BIT3
#define FAULT_AXIS_GT_HEVY			BIT4
#define FAULT_AXIS_AH_HEVY			BIT5

#define FAULT_COMMON_LIGT			BIT8
#define FAULT_AXIS_MH_LIGT			BIT9
#define FAULT_AXIS_BH_LIGT			BIT10
#define FAULT_AXIS_SL_LIGT			BIT11
#define FAULT_AXIS_GT_LIGT			BIT12
#define FAULT_AXIS_AH_LIGT			BIT13




enum FAULT_TYPE {
	WORK = 0,	//作業用
	HEVY1,		//重故障1
	HEVY2,		//重故障2
	HEVY3,		//重故障3
	LIGHT,		//軽故障
	IL,			//渋滞
	RMT,		//リモート
	AUTO,		//自動
	ALL			//全体
};

#define PLC_IF_N_ITEM_CHAR			48
#define PLC_IF_N_MSG_CHAR			48

typedef struct _ST_FAULT_ITEM {
	INT16	type;						//種別　
	INT16	limit;						//制限される軸	0－8bit：即　
	WCHAR	item[PLC_IF_N_MSG_CHAR];	//表示テキスト(項目名)
	WCHAR	comment[PLC_IF_N_MSG_CHAR];	//表示テキスト(コメント)
}ST_FAULT_ITEM, * LPST_FAULT_ITEM;

typedef struct _ST_FAULT_LIST {
	ST_FAULT_ITEM faults[N_ALL_FAULTS];	//登録フォルト数
	INT16 fault_mask[FAULT_TYPE::ALL][N_ALL_FAULT_BUF];	//登録フォルト数
}ST_FAULT_LIST, * LPST_FAULT_LIST;


class CFaults
{
private:
	int crane_id;

public:
	INT16* prbuf;		//PLCからの読み取りバッファアドレス
	INT16* pwbuf;		//PLCへの書き込みバッファアドレス(遠隔,自動用Faultセット用）
	INT16* prfltbuf;	//PLCからの故障ビットバッファアドレス
	INT16* pwfltbuf;	//PLCへの故障ビットバッファアドレス(遠隔,自動用Faultセット用）

	CFaults(int _crane_id,INT16* _prbuf,INT16* _pwbuf) {
		crane_id = _crane_id;
		prbuf = _prbuf;
		pwbuf = _pwbuf;
		setup(crane_id);
	}
	virtual ~CFaults() {
		;
	}

	ST_FAULT_LIST flt_list;									//フォルト定義構造体
	

	int get_id() { return crane_id; };
	int setup(int crane_id); 

	UINT16 faults_hold[N_PLC_FAULT_BUF];			//フォルトビット列保持(前回値）
	UINT16 faults_disp[N_PLC_FAULT_BUF];			//表示用フォルトビット列
	UINT16 faults_trig_on[N_PLC_FAULT_BUF];			//発生フォルトビット列   
	UINT16 faults_trig_off[N_PLC_FAULT_BUF];		//解消フォルトビット列
	UINT16 faults_work[N_PLC_FAULT_BUF];			//作業用フォルトビット列
	UINT16 faults_chkmask[N_PLC_FAULT_BUF];			//チェック用マスクビット列

	int chk_flt_trig();//発生/解消フォルトビット列セット
	void set_flt_mask(int code);//フォルトチェックマスクセット
	int set_disp_buf(int code);//表示用フォルトビット列セット

};

