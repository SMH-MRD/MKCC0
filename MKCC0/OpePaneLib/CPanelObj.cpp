#include "pch.h"
#include "framework.h"
#include "CPanelObj.h"
#include "CSHAREDMEM.H"

using namespace Gdiplus;

extern ST_DRAWING_BASE		drawing_items;

#define N_MAIN_PNL_OBJ		64

static ST_OBJ_PROPERTY main_props[N_MAIN_PNL_OBJ] = {
		{ID_MAIN_PNL_OBJ_STR_MESSAGE		,Point(0,0)	,Size(1920,40)	,L"メッセージなし"	},
		{ID_MAIN_PNL_OBJ_PB_REMOTE			,Point(20,50)	,Size(100,100)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_LMP_REMOTE			,Point(20,50)	,Size(100,100)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_TXT_UID			,Point(20,290)	,Size(100,30)	,L"UID"				},
		{ID_MAIN_PNL_OBJ_PB_AUTH			,Point(20,320)	,Size(100,40)	,L"認証"			},
		{ID_MAIN_PNL_OBJ_STR_PC_COM_STAT	,Point(20,900)	,Size(200,30)	,L"PC   R   S"		},
		{ID_MAIN_PNL_OBJ_STR_PLC_COM_STAT	,Point(20,930)	,Size(200,30)	,L"PLC  R   S"		},
		{ID_MAIN_PNL_OBJ_LMP_PCR			,Point(50,900)	,Size(16,16)	,L"PC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCS			,Point(80,900)	,Size(16,16)	,L"PC送信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCR			,Point(50,930)	,Size(16,16)	,L"PLC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCS			,Point(80,930)	,Size(16,16)	,L"PLC送信"			},
		{ID_MAIN_PNL_OBJ_CB_ESTOP			,Point(1780,50)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_LMP_ESTOP			,Point(1780,50)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_PB_CSOURCE			,Point(1780,180),Size(100,100)	,L"主 幹"			},
		{ID_MAIN_PNL_OBJ_LMP_CSOURCE		,Point(1780,180),Size(100,100)	,L"主 幹"			},
		{ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		,Point(1780,300),Size(100,40)	,L"操作器"			},
		{ID_MAIN_PNL_OBJ_PB_PAD_MODE		,Point(1780,345),Size(100,40)	,L"PAD"				},
		{ID_MAIN_PNL_OBJ_LMP_PAD_MODE		,Point(1780,345),Size(100,40)	,L"PAD"				},
		{ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		,Point(1780,390),Size(100,40)	,L"アシスト"		},
		{ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		,Point(1780,615),Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND	,Point(1780,645),Size(100,40)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		,Point(1780,715),Size(100,30)	,L"未接続"			},
		{ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	,Point(1780,755),Size(100,40)	,L"接続選択"		},

		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE1		,Point(20,170)	,Size(100,40)	,L"MODE1"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE2		,Point(20,215)	,Size(100,40)	,L"MODE2"			},
		{ID_MAIN_PNL_OBJ_RDO_DISP_MODE		,Point(20,170)	,Size(100,40)	,L"表示モード"		},
		
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT	,Point(20,500)	,Size(100,40)	,L"故障"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET	,Point(20,545)	,Size(100,40)	,L"設定"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM	,Point(20,590)	,Size(100,40)	,L"通信"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM	,Point(20,635)	,Size(100,40)	,L"カメラ"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT	,Point(20,680)	,Size(100,40)	,L"状態"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR	,Point(20,725)	,Size(100,40)	,L"クリア"			},
		{ID_MAIN_PNL_OBJ_RDO_OPT_WND		,Point(20,500)	,Size(100,40)	,L"オプション"		},
};


CPanelObjBase::CPanelObjBase(HWND _hwnd)
{
	hWnd = _hwnd;

	RECT rect;
	GetClientRect(hWnd, &rect);
	rc_panel.X = rect.left; rc_panel.Y = rect.top; rc_panel.Width = rect.right - rect.left; rc_panel.Height = rect.bottom - rect.top;
	hdc = GetDC(hWnd);

	hBmp_mem = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
	hBmp_bk = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
	hBmp_inf = CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
	if (hBmp_mem == NULL || hBmp_bk == NULL || hBmp_inf == NULL) {
		hr = E_FAIL;
		return;
	}

	hdc_mem = CreateCompatibleDC(hdc);
	hdc_bk = CreateCompatibleDC(hdc);
	hdc_inf = CreateCompatibleDC(hdc);
	if (hdc_mem == NULL || hdc_bk == NULL || hdc_inf == NULL) {
		hr = E_FAIL;
		return;
	}
	SelectObject(hdc_mem, hBmp_mem);
	SelectObject(hdc_bk, hBmp_bk);
	SelectObject(hdc_inf, hBmp_inf);

	pgraphic = new Graphics(hdc);
	pgraphic_mem = new Graphics(hdc_mem);
	pgraphic_bk = new Graphics(hdc_bk);
	pgraphic_inf = new Graphics(hdc_inf);
	if (pgraphic_mem == NULL || pgraphic_bk == NULL || pgraphic_inf == NULL) {
		hr = E_FAIL;
		return;
	}
	pgraphic_mem->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_bk->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_inf->SetSmoothingMode(SmoothingModeAntiAlias);

	pBrushBk = drawing_items.pbrush[ID_PANEL_COLOR_DGRAY];

}
CPanelObjBase::~CPanelObjBase() {
	delete pgraphic;
	delete pgraphic_mem;
	delete pgraphic_bk;
	delete pgraphic_inf;
	DeleteDC(hdc_mem);
	DeleteDC(hdc_bk);
	DeleteDC(hdc_inf);
	DeleteObject(hBmp_mem);
	DeleteObject(hBmp_bk);
	DeleteObject(hBmp_inf);
	ReleaseDC(hWnd, hdc);
}

HRESULT CMainPanelObj::setup_obj() { 
	
	int i = 0;
	Image* pimg[32];//画像ポインタ配列
//0 メッセージ
		str_message			= new CStringGdi(ID_MAIN_PNL_OBJ_STR_MESSAGE, &main_props[i].pt, &main_props[i].sz,  main_props[i].txt,
											 pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_20]);
	
//1-2 リモートPBL
	i++;pb_remote			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,pgraphic,drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	static Image img0(L"../Img/HHGH29/sw80_of_o.png"), img1(L"../Img/HHGH29/sw80_on_o.png"); pimg[0] = &img0; pimg[1] = &img1;
	i++; lmp_remote			= new CLampCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg, 2, 3);
	lmp_remote->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//3-4 認証表示TXT/PB
	i++; txt_uid			= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_UID, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; pb_auth			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_AUTH, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

//5-10 通信状態ラベル/ランプ
	i++;str_pc_com_stat		= new CStringGdi(ID_MAIN_PNL_OBJ_STR_PC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, 
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_NEAR], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_14]);
	i++;str_plc_com_stat	= new CStringGdi(ID_MAIN_PNL_OBJ_STR_PLC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, 
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_NEAR], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_14]);

	static Image img2(L"../Img/HHGH29/lmp1_b_of.png"), img3(L"../Img/HHGH29/lmp1_b_of.png"), img4(L"../Img/HHGH29/lmp1_g_on.png"), img5(L"../Img/HHGH29/lmp1_r_on.png"), img6(L"../Img/HHGH29/lmp1_b_on.png"), img7(L"../Img/HHGH29/lmp1_y_on.png");
	pimg[2] = &img2, pimg[3] = &img3, pimg[4] = &img4, pimg[5] = &img5, pimg[6] = &img6, pimg[7] = &img7;
	i++;lmp_pcr				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, &pimg[2], 5, 3, pgraphic);
	i++;lmp_pcs				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, &pimg[2], 5, 3, pgraphic);
	i++;lmp_plcr			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,&pimg[2], 5, 3, pgraphic);
	i++;lmp_plcs			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,&pimg[2], 5, 3, pgraphic);

//11-12 緊急停止PBL
	i++;cb_estop			= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	static Image img8(L"../Img/HHGH29/estop_of.png"), img9(L"../Img/HHGH29/estop_on.png");
	pimg[8] = &img8, pimg[9] = &img9;
	i++;lmp_estop			= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, &pimg[8], 2, 3);
	lmp_estop->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20],drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//13-14 主幹PBL
	i++;pb_csource			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_CSOURCE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	static Image img10(L"../Img/HHGH29/sw80_of_w.png"), img11(L"../Img/HHGH29/sw80_on_g.png"), img12(L"../Img/HHGH29/sw80_on_r.png");
	pimg[10] = &img10, pimg[11] = &img11, pimg[12] = &img12;
	i++;lmp_csource			= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_CSOURCE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, &pimg[10], 3, 3);
	lmp_csource->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//15-18 操作条件設定PB類
//操作器表示
	i++;cb_pnl_notch		= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_PNL_NOTCH, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
//PAD MODE
	i++;pb_pad_mode			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_PAD_MODE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	static Image img13(L"../Img/HHGH29/sw80_of_w.png"), img14(L"../Img/HHGH29/sw80_on_o.png");
	pimg[13] = &img13, pimg[14] = &img14;
	i++; lmp_pad_mode		= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_PAD_MODE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, &pimg[10], 3, 3);
	lmp_pad_mode->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_14], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);
//ASSIST
	i++;pb_assist_func		= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);


//19-20 端末モード設定TXT/PB
//操作端末タイプ
	i++;txt_ote_type		= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_OPE_TYPE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++;pb_ote_type_wnd		= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

//21-22 接続クレーン設定TXT/PB
	i++;txt_link_crane		= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_LINK_CRANE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++;pb_crane_sel_wnd	= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

	//23-25モード設定ラジオボタン
	i++; cb_disp_mode1		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_DISP_MODE1, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_disp_mode2		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_DISP_MODE2, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	CCbCtrl* pcb_mode[2]	= { cb_disp_mode1 ,cb_disp_mode2 };
	i++; rdo_disp_mode		= new CRadioCtrl(2,pcb_mode);

	//26-33オプションウィンドウ設定ラジオボタン
	i++; cb_opt_flt			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_opt_set			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_opt_com			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_opt_cam			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_opt_stat		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; cb_opt_clr			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	CCbCtrl* pcb_opt[8]		= { cb_opt_flt,cb_opt_set,cb_opt_com,cb_opt_cam,	cb_opt_stat,cb_opt_clr };
	i++; rdo_opt_wnd		= new CRadioCtrl(6, pcb_opt);

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
	delete str_pc_com_stat;	
	delete str_plc_com_stat;
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