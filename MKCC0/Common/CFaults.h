#pragma once
#include <Windows.h>
#include "COMMON_DEF.h"

#define N_PLC_FAULT_BUF				18
#define N_PC_FAULT_BUF				4
#define N_ALL_FAULTS				(N_PLC_FAULT_BUF + N_PC_FAULT_BUF)*16
#define N_PLC_FAULT_ITEM			N_PLC_FAULT_BUF * 16
#define N_PC_FAULT_ITEM				N_PC_FAULT_BUF * 16

#define N_RPC_FLT_CODE_OFFSET		850
#define N_PC_FLT_CODE_OFFSET		550
#define N_FLT_DISP_CODE_OFFSET		300

//故障種別ビット
#define FAULT_CLEAR					BIT0
#define FAULT_HEAVY1				BIT1
#define FAULT_HEAVY2				BIT2
#define FAULT_HEAVY3				BIT3
#define FAULT_LIGHT					BIT4
#define FAULT_INTERLOCK				BIT5
#define FAULT_PC_CTRL				BIT6
#define FAULT_HISTORY				BIT7//履歴


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
	BASE = 0,	//作業用
	HEVY1,		//重故障1
	HEVY2,		//重故障2
	HEVY3,		//重故障3
	LIGHT,		//軽故障
	IL,			//渋滞
	PC,			//PC
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
	ST_FAULT_ITEM plc_faults[N_PLC_FAULT_ITEM];				//登録フォルト数
	INT16 plc_fault_mask[FAULT_TYPE::ALL][N_PLC_FAULT_BUF];	//登録フォルト数
	ST_FAULT_ITEM pc_faults[N_PC_FAULT_ITEM];				//登録フォルト数
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
	CFaults(int _crane_id) {
		crane_id = _crane_id;
		setup(crane_id);
	}
	virtual ~CFaults() {
		;
	}

	ST_FAULT_LIST flt_list;									//フォルト定義構造体
	

	int get_id() { return crane_id; };
	int setup(int crane_id); 

	UINT16 faults_disp[N_PLC_FAULT_BUF];			//表示用フォルトビット列
	UINT16 faults_work[N_PLC_FAULT_BUF];			//作業用フォルトビット列
	UINT16 faults_chkmask[N_PLC_FAULT_BUF];			//チェック用マスクビット列

	void set_flt_mask(int code);//フォルトチェックマスクセット
	int set_disp_buf(int code);//表示用フォルトビット列セット

};

//## 制御PC検出フォルトマップ定義
#define FLTS_ID_ERR_CPC_PLC_COMM		0		//制御PC-機上PLC通信異常
#define FLTS_MASK_ERR_CPC_PLC_COMM		0x0001
#define FLTS_ID_ERR_CPC_RPC_COMM		0		//制御PC-遠隔PC通信異常
#define FLTS_MASK_ERR_CPC_RPC_COMM		0x0002
#define FLTS_ID_ERR_CPC_SLBRK_COMM		0		//制御PC-旋回ブレーキPLC通信異常
#define FLTS_MASK_ERR_CPC_SLBRK_COMM	0x0004
#define FLTS_ID_RMTSW_OFF				0		//機上遠隔モードOFF
#define FLTS_MASK_RMTSW_OFF				0x0008
#define FLTS_ID_ERR_RPC__ESTP			0		//遠隔操作卓非常停止
#define FLTS_MASK_ERR_RPC_ESTP			0x0010
#define FLTS_ID_ERR_RPC_RPLC_COMM		0		//遠隔PC-遠隔PLC通信異常
#define FLTS_MASK_ERR_RPC_RPLC_COMM		0x0020
#define FLTS_ID_RMT_OPE_DEACTIVE		0		//遠隔操作権未取得
#define FLTS_MASK_RMT_OPE_DEACTIVE		0x0040
#define FLTS_ID_ERR_SLBRK_ESTOP			0		//旋回ブレーキ非常停止
#define FLTS_MASK_ERR_SLBRK_ESTOP		0x0400
#define FLTS_ID_ERR_SLBRK_TMOV			0		//旋回ブレーキタイムオーバー
#define FLTS_MASK_ERR_SLBRK_TMOV		0x0800
#define FLTS_ID_ERR_SLBRK_PLC_ERR		0		//旋回ブレーキPLC異常
#define FLTS_MASK_ERR_SLBRK_PLC_ERR		0x1000
#define FLTS_ID_ERR_SLBRK_HW_ERR		0		//旋回ブレーキハード異常
#define FLTS_MASK_ERR_SLBRK_HW_ERR		0x2000
#define FLTS_ID_ERR_SLBRK_LOCAL_MODE	0		//旋回ブレーキ遠隔解除
#define FLTS_MASK_ERR_SLBRK_LOCAL_MODE	0x4000
#define FLTS_ID_ERR_SLBRK_SYS_ERR		0		//旋回ブレーキ通信異常
#define FLTS_MASK_ERR_SLBRK_SYS_ERR		0x8000

#define FLTS_ID_ERR_SLBRK_KARABURI		1		//空振り検出
#define FLTS_MASK_ERR_SLBRK_KARABURI	0x0001
#define FLTS_ID_ERR_SLBRK_CHK_NG		1		//機能チェックNG
#define FLTS_MASK_ERR_SLBRK_CHK_NG		0x0002
#define FLTS_ID_ERR_OTE_ESTP			1		//操作端末非常停止
#define FLTS_MASK_ERR_OTE_ESTP			0x0004
#define FLTS_ID_ERR_OTE_SOURCE_OFF		1		//操作端末主幹断
#define FLTS_MASK_ERR_OTE_SOURCE_OFF	0x0008
