#pragma once

#include "COMMON_DEF.h"
/******************************************************************************************/
/// <summary>
/// PLCとのMCプロトコル通信バッファの内容定義
/// </summary>
/******************************************************************************************/

#define CC_MC_ADDR_W_READ	    10400
#define CC_MC_SIZE_W_READ	    128
#define CC_MC_ADDR_W_WRITE	    10200
#define CC_MC_SIZE_W_WRITE	    128

#define OTE_MC_ADDR_W_READ	    10350
#define OTE_MC_SIZE_W_READ	    50
#define OTE_MC_ADDR_W_WRITE	    10550
#define OTE_MC_SIZE_W_WRITE	    50

/******************************************************************************************/
#define PLC_DEF_N_WCHAR                 64
/// <summary>
/// モニタウインドウへの表示用
/// </summary>
/******************************************************************************************/
typedef struct _ST_PLCIO_BLK_PROPATY {
    WCHAR text[PLC_DEF_N_WCHAR];
    int index;             //データ配列インデックス
    int size;              //データ配列サイズ
    int type;              //10進/16進表示種別
} ST_PLCIO_BLK_PROPATY, * LPST_PLCIO_BLK_PROPATY;

#define N_PLCIO_DISP_WBLK       6
typedef struct _ST_PLCIO_WBLK {
    ST_PLCIO_BLK_PROPATY defDWBlock[N_PLCIO_DISP_WBLK] = {
        {L"INT16 helthy;ctrl_mode;cab_ai[8];cab_di[6];spare0[8];"   ,0,24,NUM_FORMAT_TYPE_HEX16},
        {L"UINT32 hcounter[4];absocoder[3];"                        ,24,7,NUM_FORMAT_TYPE_HEX32},
        {L"INT16 spare1[2]; pc_fault[2];spare2[16];"                ,38,20,NUM_FORMAT_TYPE_HEX16},
        {L"INT16 erm_unitx[4];erm_x[8];inv_cc_x[6];"                ,58,18,NUM_FORMAT_TYPE_HEX16},
        {L"INT16 inv_cc_Wr1[6];inv_cc_Wr2[6];"                      ,76,24,NUM_FORMAT_TYPE_DEC16},
        {L"INT16 spare3[12];"                                       ,100,12,NUM_FORMAT_TYPE_HEX16},
    };
}ST_PLCIO__WBLK, * LPST_PLCIO__WBLK;

#define N_PLCIO_DISP_RBLK     7
typedef struct _ST_PLCIO_RBLK {
    ST_PLCIO_BLK_PROPATY defDWBlock[N_PLCIO_DISP_RBLK] = {
        {L"INT16 helthy;plc_ctrl;cab_ai[6];"                ,0,8,NUM_FORMAT_TYPE_DEC16},
        {L"INT16 cab_bi[5];erm_900;erm_bo[6];"              ,19,11,NUM_FORMAT_TYPE_HEX16},
        {L"INT32 pos[5];spd_tg[6];"                         ,30,11,NUM_FORMAT_TYPE_DEC32},
        {L"INT16 plc_fault[18];"                            ,52,18,NUM_FORMAT_TYPE_HEX16},
        {L"INT16 erm_y[4];erm_x[8];inv_cc_y[6];inv_cc_x[6]" ,70,24,NUM_FORMAT_TYPE_HEX16},
        {L"INT16 inv_cc_Ww1[6];inv_cc_Wr1[6];inv_cc_Wr2[6];",94,18,NUM_FORMAT_TYPE_DEC32},
        {L"INT16 spare1[4];"                                ,130,4,NUM_FORMAT_TYPE_HEX16}
    };
}ST_PLCIO__RBLK, * LPST_PLCIO__RBLK;


/******************************************************************************************/
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
