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

//デフォルト（みらい）の定義
typedef struct _ST_PLC_WRITE {//制御PC→PLC
    INT16   helthy;               //PCヘルシー出力信号
    INT16   ctrl_mode;            //自動制御フラグ
    UINT16  cab_ai[8];            //運転室PLC AI信号【将来用】
    INT16   cab_di[6];            //運転室PLC→電気室PLC b出力
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
}ST_PLC_WRITE, * LPST_PLC_WRITE;
typedef struct _ST_PLC_READ {
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
}ST_PLC_READ, * LPST_PLC_READ;

//西多度津70tJC102号の定義
typedef struct _ST_PLC_WRITE_HHGH29 {//制御PC→PLC
    INT16   helthy;               //PCヘルシー出力信号
    INT16   ctrl_mode;            //自動制御フラグ
    UINT16  cab_ai[8];            //運転室PLC AI信号【将来用】
    INT16   cab_di[6];            //運転室PLC→電気室PLC b出力
    INT16   spare0[8];            //運転室PLC→電気室PLC b出力
    UINT32  hcounter[4];          //高速カウンタユニット 
    UINT32  absocoder[3];         //アブソコーダ 
    INT16   spare1[2];            //予備
    INT16   pc_fault[2];          //PC検出異常マップ
    INT16   spare2[16];           //予備
    INT16   erm_unitx[6];         //電気室ユニットX 020-03F：カウンタユニット　040-04F：ABSユニット
    INT16   erm_x[7];             //電気室X 080-0EF
    INT16   spare3[9];
    INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
    INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
    INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
    INT16   spare4[30];
}ST_PLC_WRITE_HHGH29, * LPST_PLC_WRITE_HHGH29;
typedef struct _ST_PLC_READ_HHGH29 {
    INT16   helthy;               //PLCヘルシーカウンタ
    INT16   plc_ctrl;             // PLC運転モード
    UINT16  cab_ai[6];            //運転室PLC→電気室PLC W出力
    INT16   cab_bi[5];            //運転室PLC→電気室PLC b出力
    INT16   erm_900;
    INT16   erm_bo[8];            //電気室PLC b出力
    INT32   pos[5];               //各軸位置信号
    INT16   spd_tg[6];            //各軸指令速度目標
    INT16   plc_fault[17];        //各軸速度信号
    INT16   erm_y[5];             //電気室PLC Y出力
    INT16   erm_x[7];             //電気室PLC X入力
    INT16   inv_cc_y[6];          //インバータPLC DO指令
    INT16   inv_cc_x[6];          //インバータFB書き込み値　ｘデバイス
    INT16   inv_cc_Ww1[6];        //インバータPLC 速度指令　rpm
    INT16   inv_cc_Wr1[6];        //インバータFB書き込み値　rpm
    INT16   inv_cc_Wr2[6];        //インバータFB書き込み値　トルク0.1%
    INT16   spare1[3];            //予備
}ST_PLC_READ_HHGH29, * LPST_PLC_READ_HHGH29;



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


//PLC IOマッピング
 
#define ID_MC_ERM_BO_000            0
#define ID_MC_ERM_BO_010            1
#define ID_MC_ERM_BO_140            2
#define ID_MC_ERM_BO_150            3
#define ID_MC_ERM_BO_160            4
#define ID_MC_ERM_BO_170            5

#define MC_SETBIT_LAMP_CONTROL_SOURCE 0x0002

//PLC IOマッピング　point.x WORD INDEX, point.y BIT INDEX

#define ID_OPEPLC_B220              0
#define ID_OPEPLC_B230              1
#define ID_OPEPLC_B240              2
#define ID_OPEPLC_B250              3
#define ID_OPEPLC_B260              4
#define ID_OPEPLC_M900              0
struct CABPLC_BOUT_MAP {
    //B220-B26F
    POINT ctrl_on = { ID_OPEPLC_B220,0x0001 };
    POINT ctrl_off = { ID_OPEPLC_B220,0x0002 };
    POINT mh_spd_low = { ID_OPEPLC_B220,0x0004 };
    POINT mh_spd_middle = { ID_OPEPLC_B220,0x0008 };
    POINT bh_normal = { ID_OPEPLC_B220,0x0010 };
    POINT bh_down = { ID_OPEPLC_B220,0x0020 };
    POINT bh_rest = { ID_OPEPLC_B220,0x0040 };
    POINT mh_high_spd = { ID_OPEPLC_B220,0x0080 };
    POINT ah_multi = { ID_OPEPLC_B220,0x0100 };
    POINT ah_single = { ID_OPEPLC_B220,0x0200 };
    POINT notch_ah = { ID_OPEPLC_B220,10 };     //ノッチのYはビットシフト量
    POINT notch_mh = { ID_OPEPLC_B230,0 };       //ノッチのYはビットシフト量
    POINT load_swayless = { ID_OPEPLC_B230,0x0040 };
    POINT gt_spd_30 = { ID_OPEPLC_B230,0x0080 };
    POINT gt_spd_35 = { ID_OPEPLC_B230,0x0100 };
    POINT notch_gt = { ID_OPEPLC_B230,9 };      //ノッチのYはビットシフト量
    POINT cab_estp = { ID_OPEPLC_B230,0x8000 };
    POINT whip_5t_ng = { ID_OPEPLC_B240,0x0001 };
    POINT mlim_acc_low = { ID_OPEPLC_B240,0x0002 };
    POINT mlim_3times = { ID_OPEPLC_B240,0x0004 };
    POINT mlim_load_90 = { ID_OPEPLC_B240,0x0008 };
    POINT mlim_load_100 = { ID_OPEPLC_B240,0x0010 };
    POINT mlim_high_load = { ID_OPEPLC_B240,0x0020 };
    POINT mlim_normal = { ID_OPEPLC_B240,0x0040 };
    POINT wind_spd16 = { ID_OPEPLC_B240,0x0080 };
    POINT bz_stop = { ID_OPEPLC_B240,0x0100 };
    POINT fault_reset = { ID_OPEPLC_B240,0x0200 };
    POINT il_bypass = { ID_OPEPLC_B240,0x0400 };
    POINT mhbrk_opn_normal = { ID_OPEPLC_B240,0x0800 };
    POINT mhbrk_opn_abnormal = { ID_OPEPLC_B240,0x1000 };
    POINT mh_emr_open = { ID_OPEPLC_B240,0x2000 };
    POINT ah_emr_lower = { ID_OPEPLC_B240,0x4000 };
    POINT trace = { ID_OPEPLC_B240,0x8000 };
    POINT notch_bh = { ID_OPEPLC_B250,0 };//ノッチのYはビットシフト量
    POINT ah_sel_low_spd = { ID_OPEPLC_B250,0x0040 };
    POINT ah_sel_high_spd = { ID_OPEPLC_B250,0x0080 };
    POINT foot_brk = { ID_OPEPLC_B250,0x0100 };
    POINT notch_sl = { ID_OPEPLC_B250,9 };//ノッチのYはビットシフト量
    POINT siren = { ID_OPEPLC_B260,0x0001 };
    POINT camera_ghange = { ID_OPEPLC_B260,0x0002 };
    POINT slew_fix = { ID_OPEPLC_B260,0x0004 };
    POINT camera_sel_east = { ID_OPEPLC_B260,0x0008 };
    POINT camera_sel_conj = { ID_OPEPLC_B260,0x0010 };
    POINT camera_sel_west = { ID_OPEPLC_B260,0x0020 };
    POINT high_pow_ON = { ID_OPEPLC_B260,0x0040 };
    POINT high_pow_OFF = { ID_OPEPLC_B260,0x0080 };
    POINT slew_notch5 = { ID_OPEPLC_B260,0x0100 };
    POINT jib_approch_wide = { ID_OPEPLC_B260,0x0200 };
    POINT jib_approch_narrow = { ID_OPEPLC_B260,0x0400 };
    POINT spare0 = { ID_OPEPLC_B260,0x0800 };
    POINT jib_approch_stop = { ID_OPEPLC_B260,0x1000 };
    POINT gt_warm_shock_on = { ID_OPEPLC_B260,0x2000 };
    POINT gt_warm_shock_off = { ID_OPEPLC_B260,0x4000 };
    POINT ah_high_pos_load = { ID_OPEPLC_B260,0x8000 };
};
struct CABPLC_BIN_MAP {
    //B220-B26F
    POINT ctrl_on = { ID_OPEPLC_B220,0x0001 };
    POINT ctrl_off = { ID_OPEPLC_B220,0x0002 };
    POINT mh_spd_low = { ID_OPEPLC_B220,0x0004 };
    POINT mh_spd_middle = { ID_OPEPLC_B220,0x0008 };
    POINT bh_normal = { ID_OPEPLC_B220,0x0010 };
    POINT bh_down = { ID_OPEPLC_B220,0x0020 };
    POINT bh_rest = { ID_OPEPLC_B220,0x0040 };
    POINT mh_high_spd = { ID_OPEPLC_B220,0x0080 };
    POINT ah_multi = { ID_OPEPLC_B220,0x0100 };
    POINT ah_single = { ID_OPEPLC_B220,0x0200 };
    POINT notch_ah = { ID_OPEPLC_B220,10 };     //ノッチのYはビットシフト量
    POINT notch_mh = { ID_OPEPLC_B230,0 };       //ノッチのYはビットシフト量
    POINT load_swayless = { ID_OPEPLC_B230,0x0040 };
    POINT gt_spd_30 = { ID_OPEPLC_B230,0x0080 };
    POINT gt_spd_35 = { ID_OPEPLC_B230,0x0100 };
    POINT notch_gt = { ID_OPEPLC_B230,9 };      //ノッチのYはビットシフト量
    POINT cab_estp = { ID_OPEPLC_B230,0x8000 };
    POINT whip_5t_ng = { ID_OPEPLC_B240,0x0001 };
    POINT mlim_acc_low = { ID_OPEPLC_B240,0x0002 };
    POINT mlim_3times = { ID_OPEPLC_B240,0x0004 };
    POINT mlim_load_90 = { ID_OPEPLC_B240,0x0008 };
    POINT mlim_load_100 = { ID_OPEPLC_B240,0x0010 };
    POINT mlim_high_load = { ID_OPEPLC_B240,0x0020 };
    POINT mlim_normal = { ID_OPEPLC_B240,0x0040 };
    POINT wind_spd16 = { ID_OPEPLC_B240,0x0080 };
    POINT bz_stop = { ID_OPEPLC_B240,0x0100 };
    POINT fault_reset = { ID_OPEPLC_B240,0x0200 };
    POINT il_bypass = { ID_OPEPLC_B240,0x0400 };
    POINT mhbrk_opn_normal = { ID_OPEPLC_B240,0x0800 };
    POINT mhbrk_opn_abnormal = { ID_OPEPLC_B240,0x1000 };
    POINT mh_emr_open = { ID_OPEPLC_B240,0x2000 };
    POINT ah_emr_lower = { ID_OPEPLC_B240,0x4000 };
    POINT trace = { ID_OPEPLC_B240,0x8000 };
    POINT notch_bh = { ID_OPEPLC_B250,0 };//ノッチのYはビットシフト量
    POINT ah_sel_low_spd = { ID_OPEPLC_B250,0x0040 };
    POINT ah_sel_high_spd = { ID_OPEPLC_B250,0x0080 };
    POINT foot_brk = { ID_OPEPLC_B250,0x0100 };
    POINT notch_sl = { ID_OPEPLC_B250,9 };//ノッチのYはビットシフト量
    POINT siren = { ID_OPEPLC_B260,0x0001 };
    POINT camera_ghange = { ID_OPEPLC_B260,0x0002 };
    POINT slew_fix = { ID_OPEPLC_B260,0x0004 };
    POINT camera_sel_east = { ID_OPEPLC_B260,0x0008 };
    POINT camera_sel_conj = { ID_OPEPLC_B260,0x0010 };
    POINT camera_sel_west = { ID_OPEPLC_B260,0x0020 };
    POINT high_pow_ON = { ID_OPEPLC_B260,0x0040 };
    POINT high_pow_OFF = { ID_OPEPLC_B260,0x0080 };
    POINT slew_notch5 = { ID_OPEPLC_B260,0x0100 };
    POINT jib_approch_wide = { ID_OPEPLC_B260,0x0200 };
    POINT jib_approch_narrow = { ID_OPEPLC_B260,0x0400 };
    POINT spare0 = { ID_OPEPLC_B260,0x0800 };
    POINT jib_approch_stop = { ID_OPEPLC_B260,0x1000 };
    POINT gt_warm_shock_on = { ID_OPEPLC_B260,0x2000 };
    POINT gt_warm_shock_off = { ID_OPEPLC_B260,0x4000 };
    POINT ah_high_pos_load = { ID_OPEPLC_B260,0x8000 };
};
struct ERMPLC_M900_MAP {
    //SIM用追加信号
    POINT ctrl_source_cut = { ID_OPEPLC_M900,0x0001 };
    POINT mh1_source_com = { ID_OPEPLC_M900,0x0010 };
    POINT mh2_source_com = { ID_OPEPLC_M900,0x0020 };
    POINT ah_source_com = { ID_OPEPLC_M900,0x0040 };
    POINT gt_source_com = { ID_OPEPLC_M900,0x0080 };
    POINT sl_source_com = { ID_OPEPLC_M900,0x0100 };
    POINT bh_source_com = { ID_OPEPLC_M900,0x0200 };
    POINT ctrl_source_comple = { ID_OPEPLC_M900,0x0400 };
    POINT ctrl_source_pls = { ID_OPEPLC_M900,0x0800 };
    POINT common_source_com = { ID_OPEPLC_M900,0x1000 };
    POINT each_source_com = { ID_OPEPLC_M900,0x2000 };
};

#define ID_ERMPLC_B0                0
#define ID_ERMPLC_B1                1
#define ID_ERMPLC_B14               2
#define ID_ERMPLC_B15               3
#define ID_ERMPLC_B16               4
struct ERMPLC_BOUT_MAP {
    //B0-B1F
    POINT cycle_bit                     = { 0,0x0001 };
    POINT cycle_bit_remote              = { 0,0x0002 };
    POINT fault_bz                      = { 0,0x0080 };
    POINT concurrent_limit_area         = { 0,0x0100 };
    POINT il_bypass                     = { 0,0x0200 };
    POINT inv_fault_time_on             = { 0,0x0800 };
    POINT fault_item_detect             = { 1,0x0001 };
    POINT mh_edl_start                  = { 1,0x0020 };
    POINT ah_edl_start                  = { 1,0x0040 };
    //B0-B1F
    POINT got_useful_func               = { 2,0x0001 };
    POINT hook_pos_in_hlimit            = { 2,0x0002 };
    POINT hook_pos_in_high_pos          = { 2,0x0004 };
    POINT hook_pos_in_under_hlimit      = { 2,0x0008 };
    POINT hook_pos_bh_down_over         = { 2,0x0010 };
    POINT hook_pos_bh_down_under        = { 2,0x0020 };
    POINT hook_pos_bh_down_normal       = { 2,0x0040 };
    POINT mh_high1_con_up_slow_area     = { 2,0x0080 };
    POINT mh_high1_con_up_stop_area     = { 2,0x0100 };
    POINT mh_normal_up_slow_area        = { 2,0x0200 };
    POINT mh_normal_con_up_slow1_area   = { 2,0x0400 };
    POINT mh_normal_con_up_slow2_area   = { 2,0x0800 };
    POINT mh_normal_con_up_stop_area    = { 2,0x1000 };
    POINT ctrl_source_ok_bz             = { 4,0x0001 };
    POINT ctrl_source_mc_ok             = { 4,0x0002 };//主幹ON
    POINT fault_pl                      = { 4,0x0004 };
    POINT ctrl_standby                  = { 4,0x0008 };//主幹条件成立
    POINT load_disp_ok                  = { 4,0x0010 };
    POINT high_pos_operation_mode       = { 4,0x0100 };
    POINT bh_rest_mode                  = { 4,0x0200 };
    POINT bh_fixed_pl                   = { 4,0x0400 };
    POINT ah_active_jc                  = { 4,0x1000 };
    POINT bh_fixed_pl2                  = { 4,0x2000 };
    POINT motive_power_ok               = { 4,0x4000 };
 };

#define ID_ERMPLC_YD0               0
#define ID_ERMPLC_YE0               1
#define ID_ERMPLC_YF0               2
struct ERMPLC_YOUT_MAP {
    //D0-F0
    POINT il_0notch                         = { 0,0x0001 };
    POINT mh_brk_mc                         = { 0,0x0002 };
    POINT ah_brk_mc                         = { 0,0x0004 };
    POINT bh_brk_mc                         = { 0,0x0008 };
    POINT gt_brk_mc                         = { 0,0x0010 };
    POINT multi_trans_reset                 = { 0,0x0020 };
    POINT inv_reel_preset                   = { 0,0x0800 };
    POINT inv_reel_power_on                 = { 0,0x1000 };
    POINT inv_reel_gt_start                 = { 0,0x2000 };
    POINT inv_reel_fault_reset              = { 0,0x4000 };
    POINT mh_fan_mc                         = { 2,0x0001 };
    POINT bh_fan_mc                         = { 2,0x0002 };
    POINT db_convertor_fan_mc               = { 2,0x0004 };
    POINT ah_fan_mc                         = { 2,0x0008 };
    POINT gt_alarm                          = { 2,0x0010 };
    POINT initial_charge                    = { 2,0x0020 };
};

#define ID_ERMPLC_X20               0
#define ID_ERMPLC_X30               1
#define ID_ERMPLC_X40               2
#define ID_ERMPLC_X120              3
struct ERMPLC_UXIN_MAP {
    //X20
    POINT hcnt_mh_ah_ready = { ID_ERMPLC_X20,0x0001 };
    //X30
    POINT hcnt_bh_sl_ready = { ID_ERMPLC_X30,0x0001 };
    //X40
    POINT abs_mh_ready = { ID_ERMPLC_X40,0x0002 };
    //X120
    POINT abs_ah_ready = { ID_ERMPLC_X120,0x0002 };
};

#define ID_ERMPLC_X60               0
#define ID_ERMPLC_X70               1
#define ID_ERMPLC_X80               2
#define ID_ERMPLC_X90               3
#define ID_ERMPLC_XA0               4
#define ID_ERMPLC_XB0               5
#define ID_ERMPLC_XC0               6
struct ERMPLC_XIN_MAP {
    //X60-C0
    POINT mh_high_area_emr_up_lim           = { ID_ERMPLC_X60,0x0001 };
    POINT spare0                            = { ID_ERMPLC_X60,0x0002 };
    POINT spare1                            = { ID_ERMPLC_X60,0x0004 };
    POINT mh_normal_area_emr_up_lim         = { ID_ERMPLC_X60,0x0008 };
    POINT bh_down_area_emr_up_lim           = { ID_ERMPLC_X60,0x0010 };
    POINT mh_emr_lower_lim                  = { ID_ERMPLC_X60,0x0020 };
    POINT bh_emr_in_lim                     = { ID_ERMPLC_X60,0x0040 };
    POINT bh_normal_in_lim                  = { ID_ERMPLC_X60,0x0080 };
    POINT slw_spd_change                    = { ID_ERMPLC_X60,0x0100 };
    POINT spare2                            = { ID_ERMPLC_X60,0x0200 };
    POINT spare3                            = { ID_ERMPLC_X60,0x0400 };
    POINT high_pos_emr_out_lim              = { ID_ERMPLC_X60,0x0800 };
    POINT bh_emr_out_lim                    = { ID_ERMPLC_X60,0x1000 };
    POINT bh_down_area_out_lim              = { ID_ERMPLC_X60,0x2000 };
    POINT bh_rest_slow                      = { ID_ERMPLC_X60,0x4000 };
    POINT bh_rest_lim                       = { ID_ERMPLC_X60,0x8000 };
    //X70
    POINT muliti_transmit_ok                = { ID_ERMPLC_X70,0x0001 };
    POINT spare4                            = { ID_ERMPLC_X70,0x0002 };
    POINT gt_anticol_stop                   = { ID_ERMPLC_X70,0x0004 };
    POINT gt_anticol_alarm                  = { ID_ERMPLC_X70,0x0008 };
    POINT gt_anticol_fault                  = { ID_ERMPLC_X70,0x0010 };
    POINT gt_east_lim_slow                  = { ID_ERMPLC_X70,0x0020 };
    POINT gt_east_lim_stop                  = { ID_ERMPLC_X70,0x0040 };
    POINT gt_west_lim_slow                  = { ID_ERMPLC_X70,0x0080 };
    POINT gt_west_lim_stop                  = { ID_ERMPLC_X70,0x0100 };
    POINT gt_fix_open                       = { ID_ERMPLC_X70,0x0200 };
    POINT gt_motor_thermal_trip             = { ID_ERMPLC_X70,0x0400 };
    POINT spare5                            = { ID_ERMPLC_X70,0x0800 };
    POINT mh_motor_fan_mc                   = { ID_ERMPLC_X70,0x1000 };
    POINT ah_motor_fan_mc                   = { ID_ERMPLC_X70,0x2000 };
    POINT bh_motor_fan_mc                   = { ID_ERMPLC_X70,0x4000 };
    POINT DB_conv_fan_mc                    = { ID_ERMPLC_X70,0x8000 };
    //X80
    POINT DB_over_heat                      = { ID_ERMPLC_X80,0x0001 };
    POINT mh_pos_preset                     = { ID_ERMPLC_X80,0x0002 };
    POINT mh_data_preset                    = { ID_ERMPLC_X80,0x0004 };
    POINT slw_panel_high_temp               = { ID_ERMPLC_X80,0x0008 };
    POINT leg_emr_stop                      = { ID_ERMPLC_X80,0x0010 };
    POINT slew_pos_0                        = { ID_ERMPLC_X80,0x0020 };
    POINT slew_pos_180                      = { ID_ERMPLC_X80,0x0040 };
    POINT slew_grease_active                = { ID_ERMPLC_X80,0x0080 };
    POINT slew_grease_fault                 = { ID_ERMPLC_X80,0x0100 };
    POINT ah_camlim_high_area_emr_up        = { ID_ERMPLC_X80,0x0200 };
    POINT ah_camlim_normal_area1_emr_up     = { ID_ERMPLC_X80,0x0400 };
    POINT ah_camlim_normal_area2_emr_up     = { ID_ERMPLC_X80,0x0800 };
    POINT ah_camlim_bh_down_area_emr_up     = { ID_ERMPLC_X80,0x1000 };
    POINT ah_camlim_emr_low                 = { ID_ERMPLC_X80,0x2000 };
    POINT spare6                            = { ID_ERMPLC_X80,0x4000 };
    POINT spare7                            = { ID_ERMPLC_X80,0x8000 };
    //X90
    POINT conv_1_trip                       = { ID_ERMPLC_X90,0x0001 };
    POINT inv_ctrl_trip                     = { ID_ERMPLC_X90,0x0002 };
    POINT brk_control_source_trip           = { ID_ERMPLC_X90,0x0004 };
    POINT DB_fan_trip                       = { ID_ERMPLC_X90,0x0008 };
    POINT motor_fan_trip                    = { ID_ERMPLC_X90,0x0010 };
    POINT spare8                            = { ID_ERMPLC_X90,0x0020 };
    POINT slw_grease_trip                   = { ID_ERMPLC_X90,0x0040 };
    POINT spare9                            = { ID_ERMPLC_X90,0x0080 };
    POINT aero_panel_fault                  = { ID_ERMPLC_X90,0x0100 };
    POINT remote_trace                      = { ID_ERMPLC_X90,0x0200 };
    POINT inv_reel_battery_low              = { ID_ERMPLC_X90,0x0400 };
    POINT inv_reel_gt_ok                    = { ID_ERMPLC_X90,0x0800 };
    POINT inv_reel_standby                  = { ID_ERMPLC_X90,0x1000 };
    POINT inv_reel_inv_normal               = { ID_ERMPLC_X90,0x2000 };
    POINT spare10                           = { ID_ERMPLC_X90,0x4000 };
    POINT inv_reel_escape_enable            = { ID_ERMPLC_X90,0x8000 };
    //XA0
    POINT motive_power_ok                   = { ID_ERMPLC_XA0,0x0001 };
    POINT mc0                               = { ID_ERMPLC_XA0,0x0002 };
    POINT conv2_mc                          = { ID_ERMPLC_XA0,0x0004 };
    POINT ctrl_brk_mc                       = { ID_ERMPLC_XA0,0x0008 };
    POINT mh_brk_mc                         = { ID_ERMPLC_XA0,0x0010 };
    POINT bh_brk_mc                         = { ID_ERMPLC_XA0,0x0020 };
    POINT gt_brk_mc                         = { ID_ERMPLC_XA0,0x0040 };
    POINT ah_brk_mc                         = { ID_ERMPLC_XA0,0x0080 };
    POINT dbu_charge_mc                     = { ID_ERMPLC_XA0,0x0100 };
    POINT spare11                           = { ID_ERMPLC_XA0,0x0200 };
    POINT jib_approch_alarm_enable          = { ID_ERMPLC_XA0,0x0400 };
    POINT jib_approch_alarm_disable         = { ID_ERMPLC_XA0,0x0800 };
    POINT conv1_over_heat                   = { ID_ERMPLC_XA0,0x1000 };
    POINT conv2_fuse_down                   = { ID_ERMPLC_XA0,0x2000 };
    POINT conv2_over_heat                   = { ID_ERMPLC_XA0,0x4000 };
    POINT conv1_fuse_down                   = { ID_ERMPLC_XA0,0x8000 };
    //XB0
    POINT thermal_trip_ah_brk               = { ID_ERMPLC_XB0,0x0001 };
    POINT thermal_trip_bh_motor             = { ID_ERMPLC_XB0,0x0002 };
    POINT thermal_trip_mh_fan               = { ID_ERMPLC_XB0,0x0004 };
    POINT thermal_trip_bh_fan               = { ID_ERMPLC_XB0,0x0008 };
    POINT thermal_trip_DB_fan               = { ID_ERMPLC_XB0,0x0010 };
    POINT thermal_trip_mh_brk               = { ID_ERMPLC_XB0,0x0020 };
    POINT thermal_trip_bh_brk               = { ID_ERMPLC_XB0,0x0040 };
    POINT thermal_trip_ah_fan               = { ID_ERMPLC_XB0,0x0080 };
    POINT braking_unit1_fault               = { ID_ERMPLC_XB0,0x0100 };
    POINT braking_unit2_fault               = { ID_ERMPLC_XB0,0x0200 };
    POINT braking_unit3_fault               = { ID_ERMPLC_XB0,0x0400 };
    POINT braking_unit4_fault               = { ID_ERMPLC_XB0,0x0800 };
    POINT braking_unit5_fault               = { ID_ERMPLC_XB0,0x1000 };
    POINT braking_unit6_fault               = { ID_ERMPLC_XB0,0x2000 };
    POINT braking_unit7_fault               = { ID_ERMPLC_XB0,0x4000 };
    POINT braking_unit8_fault               = { ID_ERMPLC_XB0,0x8000 };

    POINT mh_preset                         = { ID_ERMPLC_XC0,0x0001 };
    POINT ah_preset                         = { ID_ERMPLC_XC0,0x0002 };
    POINT spare12                           = { ID_ERMPLC_XC0,0x0004 };
    POINT spare13                           = { ID_ERMPLC_XC0,0x0008 };
    POINT spare14                           = { ID_ERMPLC_XC0,0x0010 };
    POINT spare15                           = { ID_ERMPLC_XC0,0x0020 };
    POINT spare16                           = { ID_ERMPLC_XC0,0x0040 };
    POINT spare17                           = { ID_ERMPLC_XC0,0x0080 };
    POINT spare18                           = { ID_ERMPLC_XC0,0x0100 };
    POINT spare19                           = { ID_ERMPLC_XC0,0x0200 };
    POINT spare20                           = { ID_ERMPLC_XC0,0x0400 };
    POINT spare21                           = { ID_ERMPLC_XC0,0x0800 };
    POINT spare22                           = { ID_ERMPLC_XC0,0x1000 };
    POINT spare23                           = { ID_ERMPLC_XC0,0x2000 };
    POINT spare24                           = { ID_ERMPLC_XC0,0x4000 };
    POINT spare25                           = { ID_ERMPLC_XC0,0x8000 };
};
struct INV_IF_X_MAP {
    //D0-F0
    POINT fwd = { 0,0x0001 };
    POINT rev = { 0,0x0002 };
    POINT fan_stop = { 0,0x0004 };
    POINT charge_ok = { 0,0x0008 };
    POINT fault = { 0,0x0800 };
    POINT on_monitor = { 0,0x1000 };
    POINT spd_set_ok = { 0,0x2000 };
    POINT com_complete = { 0,0x8000 };
};
struct INV_IF_Y_MAP {
    //D0-F0
    POINT com_fwd = { 0,0x0001 };
    POINT com_rev = { 0,0x0002 };
    POINT change_acc = { 0,0x0040 };
    POINT flt_reset = { 0,0x0100 };
    POINT free_run = { 0,0x0200 };
};

typedef struct _ST_PLC_IO_MAP{
    CABPLC_BOUT_MAP     mp_cab_Bout; 
    CABPLC_BIN_MAP      mp_cab_Bin; 
    ERMPLC_M900_MAP     mp_M900; 
    ERMPLC_BOUT_MAP     mp_erm_Bout; 
    ERMPLC_YOUT_MAP     mp_emr_Yout;
	ERMPLC_UXIN_MAP     mp_emr_UXin;
	ERMPLC_XIN_MAP      mp_emr_Xin;
    INV_IF_X_MAP        mp_inv_X;
	INV_IF_Y_MAP 	    mp_inv_Y;
} ST_PLC_IO_MAP, * LPST_PLC_IO_MAP;


