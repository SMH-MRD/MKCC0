#include "pch.h"
#include "framework.h"
#include "CPanelObj.h"
#include "CSHAREDMEM.H"

using namespace Gdiplus;

#define N_MAIN_PNL_OBJ		64

static ST_OBJ_PROPERTY main_props[N_MAIN_PNL_OBJ] = {
		{ID_MAIN_PNL_OBJ_STR_MESSAGE		,Point(0,0)		,Size(500,30)	,L"メッセージなし"	},
		{ID_MAIN_PNL_OBJ_PB_REMOTE			,Point(0,30)	,Size(100,30)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_LMP_REMOTE			,Point(100,30)	,Size(30,30)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_TXT_UID			,Point(0,60)	,Size(100,30)	,L"UID"				},
		{ID_MAIN_PNL_OBJ_PB_AUTH			,Point(0,90)	,Size(100,30)	,L"認証"			},
		{ID_MAIN_PNL_OBJ_LAB_PC_COM_STAT	,Point(0,120)	,Size(100,30)	,L"PC通信"			},
		{ID_MAIN_PNL_OBJ_LAB_PLC_COM_STAT	,Point(0,120)	,Size(100,30)	,L"PLC通信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCR			,Point(0,120)	,Size(100,30)	,L"PC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCS			,Point(0,120)	,Size(100,30)	,L"PC送信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCR			,Point(0,120)	,Size(100,30)	,L"PLC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCS			,Point(0,120)	,Size(100,30)	,L"PLC送信"			},
		{ID_MAIN_PNL_OBJ_CB_ESTOP			,Point(0,120)	,Size(100,30)	,L"緊停止"		},
		{ID_MAIN_PNL_OBJ_LMP_ESTOP			,Point(0,120)	,Size(100,30)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_PB_CSOURCE			,Point(0,120)	,Size(100,30)	,L"主幹"			},
		{ID_MAIN_PNL_OBJ_LMP_CSOURCE		,Point(0,120)	,Size(100,30)	,L"主幹"			},
		{ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		,Point(0,120)	,Size(100,30)	,L"操作器"			},
		{ID_MAIN_PNL_OBJ_PB_PAD_MODE		,Point(0,120)	,Size(100,30)	,L"PAD MODE"		},
		{ID_MAIN_PNL_OBJ_LMP_PAD_MODE		,Point(0,120)	,Size(100,30)	,L"PAD MODE"		},
		{ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		,Point(0,120)	,Size(100,30)	,L"アシスト"		},
		{ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		,Point(0,120)	,Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND	,Point(0,120)	,Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		,Point(0,120)	,Size(100,30)	,L"未接続"			},
		{ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	,Point(0,120)	,Size(100,30)	,L"クレーン選択"	},

		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		,Point(0,120)	,Size(100,30)	,L"MODE1"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		,Point(0,120)	,Size(100,30)	,L"MODE2"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE		,Point(0,120)	,Size(100,30)	,L"表示モード"		},
		
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT	,Point(0,120)	,Size(100,30)	,L"故障"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET	,Point(0,120)	,Size(100,30)	,L"設定"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM	,Point(0,120)	,Size(100,30)	,L"通信"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM	,Point(0,120)	,Size(100,30)	,L"カメラ"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	,Point(0,120)	,Size(100,30)	,L"状態"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR	,Point(0,120)	,Size(100,30)	,L"クリア"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND		,Point(0,120)	,Size(100,30)	,L"オプション"		},
};

HRESULT CMainPanelObj::setup_obj() { 
	
	int i = 0;
	Image* pimg[N_IMG_SWITCH_MAX];
	//0 メッセージ
		str_message			= new CStringGdi(ID_MAIN_PNL_OBJ_STR_MESSAGE, &main_props[i].pt, &main_props[i].sz,  main_props[i].txt,pgraph_bk, Rect(main_props[i].pt.X, main_props[i].pt.Y, main_props[i].sz.Width, main_props[i].sz.Height));
	
	//1-2 リモートPBL
	i++;pb_remote			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	static Image img0(L"..\Img\HHGH29\sw_of_o.png"), img1(L"..\Img\HHGH29\sw_of_o.png");pimg[0] = &img0; pimg[1] = &img1;
	i++; lmp_remote			= new CSwitchImg(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, pimg, 2, 3);
	
	//3-4 認証表示TXT/PB
	i++;txt_uid				;
	i++;pb_auth				;

	//5-10 通信状態ラベル/ランプ
	i++;lab_pc_com_stat		;
	i++;lab_plc_com_stat	;
	i++;lmp_pcr				;
	i++;lmp_pcs				;
	i++;lmp_plcr			;
	i++;lmp_plcs			;

	//11-12 緊急停止PBL
	i++;cb_estop			= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++;lamp_estop			;

	//13-14 主幹PBL
	i++;pb_csource			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_CSOURCE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++;lmp_csource			;

	//15-18 操作条件設定PB類
	i++;cb_pnl_notch		;
	i++;pb_pad_mode			;
	i++;lmp_pad_mode		;
	i++;pb_assist_func		;

	//19-20 端末モード設定TXT/PB
	i++;txt_ote_type		;
	i++;pb_ote_type_wnd		;

	//21-22 接続クレーン設定TXT/PB
	i++;txt_link_crane		;
	i++;pb_crane_sel_wnd	;

	//23-25モード設定ラジオボタン
	i++; cb_disp_mode1		;
	i++; cb_disp_mode2		;
	i++; rdo_disp_mode		;

	//26-33オプションウィンドウ設定ラジオボタン
	i++; cb_opt_flt			;
	i++; cb_opt_set			;
	i++; cb_opt_com			;
	i++; cb_opt_cam			;
	i++; cb_opt_stat		;
	i++; cb_opt_clr			;
	i++; rdo_opt_wnd		;

	return S_OK;
}
void CMainPanelObj::delete_obj() {
	delete str_message;
	delete pb_remote;
	delete lmp_remote;
	delete rdo_disp_mode;	
	delete txt_uid;
	delete pb_auth;			
	delete rdo_opt_wnd;	
	delete lab_pc_com_stat;	
	delete lab_plc_com_stat;
	delete lmp_pcr;			
	delete lmp_pcs;			
	delete lmp_plcr;		
	delete lmp_plcs;		
	delete cb_estop;		
	delete lamp_estop;		
	delete pb_csource;		
	delete lmp_csource;		
	delete cb_pnl_notch;	
	delete pb_pad_mode;		
	delete lmp_pad_mode;	
	delete pb_assist_func;	
	delete txt_ote_type;	
	delete pb_ote_type_wnd;	
	delete txt_link_crane;	
	delete pb_crane_sel_wnd;
}