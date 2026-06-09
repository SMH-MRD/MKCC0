#pragma once

#include "COMMON_DEF.h"
/******************************************************************************************/
/// <summary>
/// PLCとのMCプロトコル通信バッファの内容定義
/// </summary>
/******************************************************************************************/

#define CC_MC_ADDR_W_READ				10300
#define CC_MC_SIZE_W_READ				128
#define CC_MC_ADDR_W_WRITE				10200
#define CC_MC_SIZE_W_WRITE				100

#define OTE_MC_ADDR_W_READ				10600
#define OTE_MC_SIZE_W_READ				50
#define OTE_MC_ADDR_W_WRITE				10650
#define OTE_MC_SIZE_W_WRITE				150

#define SLBRK_MC_ADDR_W_READ			16
#define SLBRK_MC_SIZE_W_READ			16
#define SLBRK_MC_ADDR_W_WRITE			96
#define SLBRK_MC_SIZE_W_WRITE			16

#define PLC_IF_RINDEX_RMTSW_HHGH29		65		//遠隔スイッチ情報の読み出し位置
#define PLC_IF_RMASK_RMTSW_HHGH29		0x0800	//遠隔スイッチ情報のマスク

//PLC制御ビットマスク(PLCへの出力信号のビット位置とマスク)
#define MASK_BIT_PC_CTRL_ACTIVE		0x8000
#define MASK_BIT_PC_SIM_MODE		0x4000
#define MASK_BIT_PC_OPEPNL_ACTIVE   0x2000
#define MASK_BIT_PC_AUTO_ACTIVE		0x1000
#define MASK_BIT_RMT_ESTP_ACTIVE	0x0800

//PG,ABSOデータのインデックス
#define ID_PLC_HCOUNT_MH			0
#define ID_PLC_HCOUNT_AH			1
#define ID_PLC_HCOUNT_BH			2
#define ID_PLC_HCOUNT_SL			3
#define ID_PLC_ABSO_MH				0
#define ID_PLC_ABSO_AH				1
#define ID_PLC_ABSO_GT				2

// ######## OTE PLC関連定義 ##########################

#define N_OTE_OPE_PLC_FAULT_BUF					20
#define OTE_CODE_OPEPLC_INFO_SHEAT_IL			0x0001	//着座IL条件
#define OTE_CODE_OPEPLC_INFO_NOTCH_ALL0			0x0002	//全ノッチ0
#define OTE_CODE_OPEPLC_SET_SHEAT_IL_BYPASS		0x0001	//着座ILBypass
#define OTE_CODE_OPEPLC_SET_TOUCH_PB_ACTIVE		0x0002	//タッチPB表示
#define OTE_CODE_OPEPLC_SET_CTRL_IL_BYPASS		0x0004	//制御遅延ILBypass
#define OTE_CODE_OPEPLC_SET_VDELAY_IL_BYPASS	0x0008	//映像遅延ILBypass

/*** PLC IO構造体定義 ***/

#define N_ST_PLC_IO_DEF					256

#define CODE_PLCIO_WORD					0
#define CODE_PLCIO_BIT					1
#define CODE_PLCIO_DWORD				2
#define CODE_PLCIO_FLOAT				3 
#define CODE_PLCIO_DOUBLE				4 
#define CODE_PLCIO_BITS					5 //NOTCH CS等
#define CODE_PLCIO_BIT_NC				6 //ノーマルクローズ

#define CODE_PLC_CTRL_PLC_REMOTE		0x8000