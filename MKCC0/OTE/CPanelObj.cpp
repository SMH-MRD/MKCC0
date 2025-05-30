//#include "pch.h"
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
		{ID_MAIN_PNL_OBJ_STR_PC_COM_STAT	,Point(10,900)	,Size(200,30)	,L"PC   R       S"	},
		{ID_MAIN_PNL_OBJ_STR_PLC_COM_STAT	,Point(10,930)	,Size(200,30)	,L"PLC  R       S"	},
		{ID_MAIN_PNL_OBJ_LMP_PCR			,Point(62,905)	,Size(16,16)	,L"PC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PCS			,Point(100,905)	,Size(16,16)	,L"PC送信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCR			,Point(62,935)	,Size(16,16)	,L"PLC受信"			},
		{ID_MAIN_PNL_OBJ_LMP_PLCS			,Point(100,935)	,Size(16,16)	,L"PLC送信"			},
		{ID_MAIN_PNL_OBJ_CB_ESTOP			,Point(1780,50)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_LMP_ESTOP			,Point(1780,50)	,Size(100,100)	,L"緊急停止"		},
		{ID_MAIN_PNL_OBJ_PB_SYUKAN_ON		,Point(1780,180),Size(100,50)	,L"主幹入"			},
		{ID_MAIN_PNL_OBJ_LMP_SYUKAN_ON		,Point(1780,180),Size(100,50)	,L"主幹入"			},
		{ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF		,Point(1780,235),Size(100,100)	,L"主幹切"			},
		{ID_MAIN_PNL_OBJ_LMP_SYUKAN_OFF		,Point(1780,235),Size(100,100)	,L"主幹切"			},
		{ID_MAIN_PNL_OBJ_CB_PNL_NOTCH		,Point(1780,360),Size(100,40)	,L"操作器"			},
		{ID_MAIN_PNL_OBJ_PB_PAD_MODE		,Point(1780,405),Size(100,40)	,L"PAD"				},
		{ID_MAIN_PNL_OBJ_LMP_PAD_MODE		,Point(1780,405),Size(100,40)	,L"PAD"				},
		{ID_MAIN_PNL_OBJ_PB_ASSIST_FUNC		,Point(1780,450),Size(100,40)	,L"アシスト"		},
		{ID_MAIN_PNL_OBJ_TXT_OPE_TYPE		,Point(1780,735),Size(100,30)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_PB_OTE_TYPE_WND	,Point(1780,760),Size(100,40)	,L"端末モード"		},
		{ID_MAIN_PNL_OBJ_TXT_LINK_CRANE		,Point(1780,820),Size(100,30)	,L"未接続"			},
		{ID_MAIN_PNL_OBJ_PB_CRANE_SEL_WND	,Point(1780,850),Size(100,40)	,L"接続選択"		},

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

		{ID_MAIN_PNL_OBJ_PB_FRESET			,Point(1800,560),Size(60,60)	,L"故障リセット"	},
		{ID_MAIN_PNL_OBJ_LMP_FRESET			,Point(1800,560),Size(60,60)	,L"故障リセット"	},
		{ID_MAIN_PNL_OBJ_TXT_FRESET			,Point(1780,530),Size(100,30)	,L"故障リセット"	},

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
	//画像ポインタ配列

	static Image img_o80_of(L"../Img/HHGH29/sw80_of_o.png"), img_o80_on(L"../Img/HHGH29/sw80_on_o.png");
	static Image img_b1_of(L"../Img/HHGH29/lmp1_b_of.png"), img_b1_on(L"../Img/HHGH29/lmp1_b_on.png"), img_y1_on(L"../Img/HHGH29/lmp1_y_on.png"), img_g1_on(L"../Img/HHGH29/lmp1_g_on.png"), img_r1_on(L"../Img/HHGH29/lmp1_r_on.png");
	static Image img_estp_of(L"../Img/HHGH29/estop_of.png"), img_estp_on(L"../Img/HHGH29/estop_on.png"), img_estp_of2(L"../Img/HHGH29/estop_of2.png"), img_estp_on2(L"../Img/HHGH29/estop_on2.png");
	static Image img_w80_of(L"../Img/HHGH29/sw80_of_w.png"), img_b80_on(L"../Img/HHGH29/sw80_on_b.png"), img_g80_on(L"../Img/HHGH29/sw80_on_g.png"), img_r80_on(L"../Img/HHGH29/sw80_on_r.png");
	static Image img_freset_of(L"../Img/HHGH29/freset_off.png"), img_freset_on(L"../Img/HHGH29/freset_on.png");

	Image* pimg_remote[N_IMG_SWITCH_MAX] = { &img_o80_of, &img_o80_on , &img_b80_on, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of };
	Image* pimg_syukan_on[N_IMG_SWITCH_MAX] = { &img_w80_of, &img_g80_on , &img_r80_on, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of };
	Image* pimg_syukan_off[N_IMG_SWITCH_MAX] = { &img_w80_of, &img_r80_on , &img_g80_on, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of, &img_w80_of };
	Image* pimg_estop[N_IMG_SWITCH_MAX] = { &img_estp_of, &img_estp_on, &img_estp_of2, &img_estp_on2 , &img_b80_on, &img_w80_of, &img_w80_of, &img_b80_on };
	Image* pimg_signal[N_IMG_SWITCH_MAX] = { &img_b1_of, &img_b1_on , &img_y1_on, &img_r1_on, &img_g1_on, &img_b1_of, &img_b1_of, &img_b1_of };
	Image* pimg_freset[N_IMG_SWITCH_MAX] = { &img_freset_of, &img_freset_on, &img_w80_of, &img_w80_of , &img_b80_on, &img_w80_of, &img_w80_of, &img_b80_on };

//0 メッセージ
		str_message			= new CStringGdi(ID_MAIN_PNL_OBJ_STR_MESSAGE, &main_props[i].pt, &main_props[i].sz,  main_props[i].txt,
											 pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_20]);
	
//1-2 リモートPBL
	i++;pb_remote			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,pgraphic,drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lmp_remote			= new CLampCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_remote, 3, 3);
	lmp_remote->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//3-4 認証表示TXT/PB
	i++; txt_uid			= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_UID, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; pb_auth			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_AUTH, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

//5-10 通信状態ラベル/ランプ
	i++;str_pc_com_stat		= new CStringGdi(ID_MAIN_PNL_OBJ_STR_PC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, 
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_14]);
	i++;str_plc_com_stat	= new CStringGdi(ID_MAIN_PNL_OBJ_STR_PLC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, 
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_WHITE], drawing_items.pfont[ID_PANEL_FONT_14]);
	i++;lmp_pcr				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_signal, 6, 3, pgraphic);
	i++;lmp_pcs				= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_signal, 6, 3, pgraphic);
	i++;lmp_plcr			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,pimg_signal, 6, 3, pgraphic);
	i++;lmp_plcs			= new CSwitchImg(ID_MAIN_PNL_OBJ_LMP_PLCS, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,pimg_signal, 6, 3, pgraphic);

//11-12 緊急停止PBL
	i++;cb_estop			= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,NULL, drawing_items.ppen[ID_PANEL_COLOR_RED], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++;lmp_estop			= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_ESTOP, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_estop, 4, 3);
	lmp_estop->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20],drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//13-14 主幹入PBL
	i++;pb_syukan_on			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_SYUKAN_ON, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++;lmp_syukan_on			= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_SYUKAN_ON, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_syukan_on, 3, 3);
	lmp_syukan_on->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);
//13-14 主幹切PBL
	i++; pb_syukan_off = new CPbCtrl(ID_MAIN_PNL_OBJ_PB_SYUKAN_OFF, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lmp_syukan_off = new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_SYUKAN_OFF, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_syukan_off, 3, 3);
	lmp_syukan_off->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//15-18 操作条件設定PB類
//操作器表示
	i++;cb_pnl_notch		= new CCbCtrl(ID_MAIN_PNL_OBJ_CB_PNL_NOTCH, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
//PAD MODE
	i++;pb_pad_mode			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_PAD_MODE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lmp_pad_mode		= new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_PAD_MODE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_remote, 3, 3);
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
	i++; cb_disp_mode1		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_DISP_MODE1, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_mode2		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_DISP_MODE2, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	CCbCtrl* pcb_mode[2]	= { cb_disp_mode1 ,cb_disp_mode2 };
	i++; rdo_disp_mode		= new CRadioCtrl(2,pcb_mode);

	//26-33オプションウィンドウ設定ラジオボタン
	i++; cb_opt_flt			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_FLT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_opt_set			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_SET, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_opt_com			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_COM, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_opt_cam			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_CAM, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_opt_stat		= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_opt_clr			= new CCbCtrl(ID_MAIN_PNL_OBJ_RDO_OPT_WND_CLR, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	CCbCtrl* pcb_opt[8]		= { cb_opt_flt,cb_opt_set,cb_opt_com,cb_opt_cam,	cb_opt_stat,cb_opt_clr };
	i++; rdo_opt_wnd		= new CRadioCtrl(6, pcb_opt);

	//故障リセット
	i++; pb_freset = new CPbCtrl(ID_MAIN_PNL_OBJ_PB_FRESET, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_DGRAY], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lmp_freset = new CLampCtrl(ID_MAIN_PNL_OBJ_LMP_FRESET, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_freset, 2, 4);
	lmp_pad_mode->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_14], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);
	i++; txt_freset = new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_FRESET, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);

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
	delete pb_syukan_on;		
	delete lmp_syukan_on;	
	delete pb_syukan_off;
	delete lmp_syukan_off;
	delete cb_pnl_notch;	
	delete pb_pad_mode;		
	delete lmp_pad_mode;	
	delete pb_assist_func;	
	delete txt_ote_type;	
	delete pb_ote_type_wnd;	
	delete pb_freset;
	delete lmp_freset;
	delete txt_link_crane;	
	delete pb_crane_sel_wnd;

}