#include "pch.h"
#include "framework.h"
#include "CPanelObj.h"
#include "CSHAREDMEM.H"

using namespace Gdiplus;

extern ST_DRAWING_BASE		drawing_items;

#define N_MAIN_PNL_OBJ		64

static ST_OBJ_PROPERTY main_props[N_MAIN_PNL_OBJ] = {
		{ID_MAIN_PNL_OBJ_STR_MESSAGE		,Point(0,0)		,Size(500,30)	,L"メッセージなし"	},
		{ID_MAIN_PNL_OBJ_PB_REMOTE			,Point(0,30)	,Size(100,30)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_LMP_REMOTE			,Point(100,30)	,Size(100,100)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_TXT_UID			,Point(0,60)	,Size(100,30)	,L"UID"				},
		{ID_MAIN_PNL_OBJ_PB_AUTH			,Point(20,90)	,Size(100,40)	,L"認証"			},
		{ID_MAIN_PNL_OBJ_LAB_PC_COM_STAT	,Point(0,120)	,Size(100,30)	,L"PC通信"			},
		{ID_MAIN_PNL_OBJ_LAB_PLC_COM_STAT	,Point(0,120)	,Size(100,30)	,L"PLC通信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCR			,Point(100,120)	,Size(100,30)	,L"PC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCS			,Point(100,120)	,Size(100,30)	,L"PC送信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCR			,Point(100,120)	,Size(100,30)	,L"PLC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCS			,Point(100,120)	,Size(100,30)	,L"PLC送信"			},
		{ID_MAIN_PNL_OBJ_CB_ESTOP			,Point(1780,30)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_LMP_ESTOP			,Point(1780,30)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_PB_CSOURCE			,Point(1780,160),Size(100,100)	,L"主 幹"			},
		{ID_MAIN_PNL_OBJ_LMP_CSOURCE		,Point(1780,160),Size(100,100)	,L"主 幹"			},
		{ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		,Point(0,120)	,Size(100,30)	,L"操作器"			},
		{ID_MAIN_PNL_OBJ_PB_PAD_MODE		,Point(0,120)	,Size(100,30)	,L"PAD MODE"		},
		{ID_MAIN_PNL_OBJ_LMP_PAD_MODE		,Point(0,120)	,Size(100,30)	,L"PAD MODE"		},
		{ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		,Point(0,120)	,Size(100,30)	,L"アシスト"		},
		{ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		,Point(0,120)	,Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND	,Point(0,120)	,Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		,Point(0,120)	,Size(100,30)	,L"未接続"			},
		{ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	,Point(0,120)	,Size(100,30)	,L"クレーン選択"	},

		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		,Point(20,150)	,Size(100,30)	,L"MODE1"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		,Point(20,195)	,Size(100,30)	,L"MODE2"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE		,Point(20,150)	,Size(100,30)	,L"表示モード"		},
		
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT	,Point(20,500)	,Size(100,40)	,L"故障"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET	,Point(20,545)	,Size(100,40)	,L"設定"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM	,Point(20,590)	,Size(100,40)	,L"通信"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM	,Point(20,635)	,Size(100,40)	,L"カメラ"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	,Point(20,680)	,Size(100,40)	,L"状態"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR	,Point(20,725)	,Size(100,40)	,L"クリア"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND		,Point(20,500)	,Size(100,40)	,L"オプション"		},
};

HRESULT CMainPanelObj::setup_obj() { 
	
	int i = 0;
	Image* pimg[32];//画像ポインタ配列
	//0 メッセージ
		str_message			= new CStringGdi(ID_MAIN_PNL_OBJ_STR_MESSAGE, &main_props[i].pt, &main_props[i].sz,  main_props[i].txt,pgraph_bk, Rect(main_props[i].pt.X, main_props[i].pt.Y, main_props[i].sz.Width, main_props[i].sz.Height));
	
	//1-2 リモートPBL
	i++;pb_remote			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	static Image img0(L"../Img/HHGH29/sw80_of_o.png"), img1(L"../Img/HHGH29/sw80_of_o.png");pimg[0] = &img0; pimg[1] = &img1;
	i++; lmp_remote			= new CSwitchImg(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, pimg, 2, 3);
	
	//3-4 認証表示TXT/PB
	i++;txt_uid				;
	i++;pb_auth				;

	//5-10 通信状態ラベル/ランプ
	i++;lab_pc_com_stat		;
	i++;lab_plc_com_stat	;

	static Image img2(L"../Img/HHGH29/lmp1_b_of.png"), img3(L"../Img/HHGH29/lmp1_b_of.png"), img4(L"../Img/HHGH29/lmp1_g_on.png"), img5(L"../Img/HHGH29/lmp1_r_on.png"), img6(L"../Img/HHGH29/lmp1_b_on.png"), img7(L"../Img/HHGH29/lmp1_y_on.png");
	pimg[2] = &img2, pimg[3] = &img3, pimg[4] = &img4, pimg[5] = &img5, pimg[6] = &img6, pimg[7] = &img7;
	i++;lmp_pcr				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[2], 5, 3);
	i++;lmp_pcs				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[2], 5, 3);
	i++;lmp_plcr			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[2], 5, 3);
	i++;lmp_plcs			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[2], 5, 3);

	//11-12 緊急停止PBL
	i++;cb_estop			= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	static Image img8(L"../Img/HHGH29/estop_of.png"), img9(L"../Img/HHGH29/estop_on.png");
	pimg[8] = &img8, pimg[9] = &img9;
	i++;lmp_estop = new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[8], 2, 3);
	lmp_estop->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20],drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_GRAY]);

	//13-14 主幹PBL
	i++;pb_csource			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_CSOURCE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	
	static Image img10(L"../Img/HHGH29/sw80_of_b.png"), img11(L"../Img/HHGH29/sw80_on_g.png"), img12(L"../Img/HHGH29/sw80_on_r.png");
	pimg[10] = &img10, pimg[11] = &img11, pimg[12] = &img12;
	i++;lmp_csource = new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_CSOURCE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraph_mem, &pimg[10], 3, 3);
	lmp_csource->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_GRAY]);

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
	delete lmp_estop;		
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