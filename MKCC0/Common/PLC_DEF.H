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
//#define SLBRK_MC_ADDR_W_READ			10460
#define SLBRK_MC_SIZE_W_READ			16
#define SLBRK_MC_ADDR_W_WRITE			96
//#define SLBRK_MC_ADDR_W_WRITE			10450
#define SLBRK_MC_SIZE_W_WRITE			16

#define PLC_IF_REMOTE_OFF_MODE		    0x00000000		//機上運転モード（PCからの出力無効,端末にて状態のモニタのみ）
#define PLC_IF_RMT_MODE_0			    0x00000001		//遠隔運転モード（PCからの出力有効　通常運用モード）
#define PLC_IF_SIM_MODE_1			    0x00000003		//操作端末からの操作入力有効通常運用モード
#define PLC_IF_SIM_MODE_2			    0x00000007		//PLC切り離しモード（PLC IFで機器への指定（ブレーキ,MC等）をシミュレート）

#define PLC_IF_PC_CTRL_ACTIVE		    0x00000001
#define PLC_IF_PC_CTRL_DEACTIVE		    0x00000000

// PLC_状態信号構造体（機上センサ信号)
#define PLC_IF_BRK_OPEN				    true
#define PLC_IF_BRK_CLOSE			    false

#define PLC_IF_INV_DIO_FWD			    0x0001
#define PLC_IF_INV_DIO_REV			    0x0002

//極限タイプコード
#define PLC_IF_LIMIT_COM_FWD_NORM		0x0000001
#define PLC_IF_LIMIT_COM_REV_NORM		0x0000002
#define PLC_IF_LIMIT_COM_FWD_EMR		0x0000004
#define PLC_IF_LIMIT_COM_REV_EMR		0x0000008
#define PLC_IF_LIMIT_COM_FWD_SLOW		0x0000010
#define PLC_IF_LIMIT_COM_REV_SLOW		0x0000020
#define PLC_IF_LIMIT_COM_FWD_SLOW2		0x0000040
#define PLC_IF_LIMIT_COM_REV_SLOW2		0x0000080
#define PLC_IF_LIMIT_COM_FWD_NORM_EX1	0x0000100
#define PLC_IF_LIMIT_COM_REV_NORM_EX1	0x0000200
#define PLC_IF_LIMIT_COM_FWD_EMR_EX1	0x0000400
#define PLC_IF_LIMIT_COM_REV_EMR_EX1	0x0000800
#define PLC_IF_LIMIT_COM_FWD_NORM_EX2	0x0001000
#define PLC_IF_LIMIT_COM_REV_NORM_EX2	0x0002000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX2	0x0004000
#define PLC_IF_LIMIT_COM_REV_EMR_EX2	0x0008000
#define PLC_IF_LIMIT_COM_FWD_NORM_EX3	0x0010000
#define PLC_IF_LIMIT_COM_REV_NORM_EX3	0x0020000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX3	0x0040000
#define PLC_IF_LIMIT_COM_REV_EMR_EX3	0x0080000

#define PLC_IF_ID_MH7               0
#define PLC_IF_ID_MH14              1
#define PLC_IF_ID_MH21              2

#define PLC_IF_ID_AH12              0
#define PLC_IF_ID_AH24              1

#define PLC_IF_ID_BH57              0
#define PLC_IF_ID_BH62              1
#define PLC_IF_ID_BH_REST           2

#define ID_MC_INV_GNT               0
#define ID_MC_INV_AH                1
#define ID_MC_INV_SLW               2
#define ID_MC_INV_BH                3
#define ID_MC_INV_MH1               4
#define ID_MC_INV_MH2               5

//INVERTOR CC LINK
#define ID_CC_LINK_INV_GT           0
#define ID_CC_LINK_INV_AH           1
#define ID_CC_LINK_INV_SL           2
#define ID_CC_LINK_INV_BH           3
#define ID_CC_LINK_INV_MH1          4
#define ID_CC_LINK_INV_MH2          5

#define ID_CC_LINK_X                0
#define ID_CC_LINK_Y                0
#define ID_CC_LINK_W_COMMON         0
#define ID_CC_LINK_W_SPDREF         1
#define ID_CC_LINK_W_COMCODE        2
#define ID_CC_LINK_W_COMITEM        3
#define ID_CC_LINK_R_RESMON         0
#define ID_CC_LINK_R_SPDREF         1
#define ID_CC_LINK_R_RESCOM         2
#define ID_CC_LINK_R_RESITEM        3

//CC LINK
#define CC_PRM_RPM100_MH            6666
#define CC_PRM_RPM100_AH            10000
#define CC_PRM_RPM100_BH            20000
#define CC_PRM_RPM100_SLW           20000
#define CC_PRM_RPM100_GT            20000

#define PLC_IF_CAB_AI_FOOT_BRK      0
#define PLC_IF_CAB_AI_BH_R          1
#define PLC_IF_CAB_AI_MH_LOAD       2
#define PLC_IF_CAB_AI_AH_LOAD       3
#define PLC_IF_CAB_AI_BH_ANGLE      4

#define PLC_IF_HCOUNT_MH            0
#define PLC_IF_HCOUNT_AH            1
#define PLC_IF_HCOUNT_BH            2
#define PLC_IF_HCOUNT_SL            3

#define PLC_IF_ABS_MH               0
#define PLC_IF_ABS_AH               1

#define PLC_IF_CCID_GT              0
#define PLC_IF_CCID_AH              1
#define PLC_IF_CCID_SL              2
#define PLC_IF_CCID_BH              3
#define PLC_IF_CCID_MH1             4
#define PLC_IF_CCID_MH2             5

#define PLC_IF_RINDEX_RMTSW_HHGH29 65		//遠隔スイッチ情報の読み出し位置
#define PLC_IF_RMASK_RMTSW_HHGH29  0x0800	//遠隔スイッチ情報のマスク

