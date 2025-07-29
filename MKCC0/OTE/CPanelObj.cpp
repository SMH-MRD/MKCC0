//#include "pch.h"
#include "framework.h"
#include "CPanelObj.h"
#include "CSHAREDMEM.H"

using namespace Gdiplus;

extern ST_DRAWING_BASE		drawing_items;

#define N_MAIN_PNL_OBJ		64

static ST_OBJ_PROPERTY main_props[N_MAIN_PNL_OBJ] = {
		{ID_MAIN_PNL_OBJ_STR_MESSAGE		,Point(920,0)	,Size(1000,40)	,L"メッセージなし"	},
		{ID_MAIN_PNL_OBJ_PB_REMOTE			,Point(20,50)	,Size(100,100)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_LMP_REMOTE			,Point(20,50)	,Size(100,100)	,L"遠隔"			},
		{ID_MAIN_PNL_OBJ_TXT_UID			,Point(20,290)	,Size(100,30)	,L"UID"				},
		{ID_MAIN_PNL_OBJ_PB_AUTH			,Point(20,320)	,Size(100,40)	,L"認証"			},
		{ID_MAIN_PNL_OBJ_TXT_PC_COM_STAT	,Point(10,905)	,Size(120,30)	,L"PC   R     S"	},
		{ID_MAIN_PNL_OBJ_TXT_PLC_COM_STAT	,Point(10,935)	,Size(120,30)	,L"PLC  R     S"	},
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

#define N_SUB_PNL_OBJ		64
static ST_OBJ_PROPERTY sub_set_props[N_SUB_PNL_OBJ] = {
	//設定サブウィンドウ
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0 ,Point(85,110)	,Size(30,30)	,L"7"			},//i=0
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1	,Point(155,80)	,Size(30,30)	,L"14"			},
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2	,Point(225,110)	,Size(30,30)	,L"21"			},
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD	,Point(20,110)	,Size(30,30)	,L"主巻モード"	},
	{ID_SUB_PNL_SET_OBJ_LMP_MHSPD	,Point(130,120)	,Size(80,80)	,L"主巻モード"	},

	{ID_SUB_PNL_SET_OBJ_RDO_BHR_0	,Point(85,280)	,Size(30,30)	,L"57"			},//i=5
	{ID_SUB_PNL_SET_OBJ_RDO_BHR_1	,Point(155,250)	,Size(30,30)	,L"62"			},
	{ID_SUB_PNL_SET_OBJ_RDO_BHR_2	,Point(225,280)	,Size(35,30)	,L"ﾚｽﾄ"			},
	{ID_SUB_PNL_SET_OBJ_RDO_BHR		,Point(20,280)	,Size(30,30)	,L"引込モード"	},
	{ID_SUB_PNL_SET_OBJ_LMP_BHR		,Point(130,290)	,Size(80,80)	,L"引込モード"	},

	//状態サブウィンドウ
	{ID_SUB_PNL_STAT_OBJ_PB_NEXT	,Point(230,420)	,Size(50,30)	,L"NEXT"		},//i=10
	{ID_SUB_PNL_STAT_OBJ_PB_BACK	,Point(285,420)	,Size(50,30)	,L"BACK"		},

	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_DIR		,Point(35, 70 )	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_DIR		,Point(35, 105)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_SL_DIR		,Point(35, 140)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_GT_DIR		,Point(35, 175)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_TG_V		,Point(95,70 )	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_TG_V		,Point(95,105)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_SL_TG_V		,Point(95,140)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_GT_TG_V		,Point(95,175)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_V	,Point(155,70 )	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_V	,Point(155,105)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_SL_REF_V	,Point(155,140)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_GT_REF_V	,Point(155,175)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_V		,Point(215,70 )	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_V		,Point(215,105)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_V		,Point(215,140)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_GT_FB_V		,Point(215,175)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_TRQ	,Point(275,70 )	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_TRQ	,Point(275,105 ),Size(50,30)	,L"-"	},

	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_PG	,Point(335,70)	,Size(100,30)	,L"MH PG CNT"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_PG	,Point(335,105)	,Size(100,30)	,L"BH PG CNT"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_PG	,Point(335,140)	,Size(100,30)	,L"SL PG CNT"	},
	{ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_ABS	,Point(445,70)	,Size(100,30)	,L"MH ABS CNT"	},

	//故障表示サブウィンドウ
	{ID_SUB_PNL_FLT_OBJ_IMG_BK		,Point(0,0)		,Size(640,500)	,L"背景"		},	//i=30	CSwitchImg* img_flt_bk;
	{ID_SUB_PNL_FLT_OBJ_PB_NEXT		,Point(270,420)	,Size(30,30)	,L"次"			},	//		CPbCtrl* pb_stat_next;
	{ID_SUB_PNL_FLT_OBJ_CB_HISTORY	,Point(235,420)	,Size(40,30)	,L"履歴"		},	//		CCbCtrl* cb_stat_back;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY1	,Point(5,420)	,Size(40,30)	,L"重1"			},	//		CCbCtrl* cb_disp_flt_heavy1;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY2	,Point(50,420)	,Size(40,30)	,L"重2"			},	//		CCbCtrl* cb_disp_flt_heavy2;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY3	,Point(95,420)	,Size(40,30)	,L"重3"			},	//		CCbCtrl* cb_disp_flt_heavy3;
	{ID_SUB_PNL_FLT_OBJ_CB_BYPASS	,Point(280,10)	,Size(60,30)	,L"BYPASS"		},	//		CCbCtrl* cb_flt_bypass;
	{ID_SUB_PNL_OBJ_STR_FLT_MESSAGE	,Point(10,10)	,Size(300,30)	,L"故障表示"	},	//		CStringGdi* str_flt_message
	{ID_SUB_PNL_FLT_OBJ_CB_LITE		,Point(140,420)	,Size(40,30)	,L"軽"			},	//		CCbCtrl* cb_disp_flt_light;
	{ID_SUB_PNL_FLT_OBJ_CB_IL		,Point(185,420)	,Size(35,30)	,L"IL"			},	//		CCbCtrl* cb_disp_flt_il;
	{ID_SUB_PNL_FLT_OBJ_PB_PLCMAP	,Point(305,420)	,Size(35,30)	,L"MAP"			},	//		CCbCtrl* cb_disp_plcmap;
	{ID_SUB_PNL_FLT_OBJ_LV_FAULTS	,Point(5,45)	,Size(620,365)	,L"LIST VIEW"	},	//		CCbCtrl* cb_disp_flt_heavy;
};

#define N_GWIN_OBJ			32
static ST_OBJ_PROPERTY gwin_set_props[N_GWIN_OBJ] = {
	//グラフィックMAINウィンドウ
	{ID_GWIN_MAIN_OBJ_IMG_BK		,Point(0,0)			,Size(1000,1000)	,L"背景"		},//i=0
	//{ID_GWIN_MAIN_OBJ_IMG_BOOM_XY	,Point(500,550)		,Size(28,400)	,L"ブーム上面"	},
	{ID_GWIN_MAIN_OBJ_IMG_BOOM_XY	,Point(500,550)		,Size(70,500)	,L"ブーム上面"	},
	{ID_GWIN_MAIN_OBJ_IMG_GT_BASE	,Point(445,522)		,Size(120,62)	,L"走行装置"	},
	{ID_GWIN_MAIN_OBJ_IMG_POTAL		,Point(490,500)		,Size(50,100)	,L"ポータル"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_MH	,Point(20,20)		,Size(400,40)	,L"主巻位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_BH	,Point(20,60)		,Size(400,40)	,L"引込位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_SL	,Point(20,100)		,Size(400,40)	,L"旋回位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_GT	,Point(20,140)		,Size(400,40)	,L"走行位置"	},//i=6

	{ID_GWIN_MAIN_OBJ_STR_POS_MOUSE	,Point(900,960)		,Size(100,20)	,L"マウス位置"	},//i=7
};

#define N_GSUBWIN_OBJ			32
static ST_OBJ_PROPERTY gsubwin_set_props[N_GSUBWIN_OBJ] = {
	//グラフィックMAINウィンドウ
	{ID_GWIN_SUB_OBJ_IMG_BK			,Point(0,0)			,Size(645,510)	,L"背景"		},//i=0
	{ID_GWIN_SUB_OBJ_IMG_BOOM_YZ	,Point(130,300)		,Size(450,60)	,L"ブーム側面"	},
	{ID_GWIN_SUB_OBJ_IMG_HOOK_MH	,Point(500,400)		,Size(45,60)    ,L"フック"		},

	{ID_GWIN_SUB_OBJ_STR_POS_MH		,Point(20,20)		,Size(300,40)	,L"主巻位置"	},
	{ID_GWIN_SUB_OBJ_STR_ANGLE_BH	,Point(20,60)		,Size(300,40)	,L"引込位置"	},

	{ID_GWIN_SUB_OBJ_STR_POS_MOUSE	,Point(550,460)		,Size(100,20)	,L"マウス位置"	},//i=7
};

CPanelObjBase::CPanelObjBase(HWND _hwnd)
{
	hPnlWnd = _hwnd;
	setup_graphics(hPnlWnd);
}
CPanelObjBase::~CPanelObjBase() {
	clear_graghics();
}

HRESULT CPanelObjBase::setup_graphics(HWND hwnd) {
	clear_graghics();

	RECT rect;
	GetClientRect(hwnd, &rect);
	rc_panel.X = rect.left; rc_panel.Y = rect.top; rc_panel.Width = rect.right - rect.left; rc_panel.Height = rect.bottom - rect.top;
	
	hdc		= GetDC(hwnd);
	hdc_img = CreateCompatibleDC(hdc);
	hdc_bk	= CreateCompatibleDC(hdc);
	hdc_inf = CreateCompatibleDC(hdc);
	if (hdc_img == NULL || hdc_bk == NULL || hdc_inf == NULL) return E_FAIL;

	
	//グラフィックオブジェクト作成
	pgraphic		= new Gdiplus::Graphics(hdc);//バッファリング（描画まとめ）用オブジェクト

	pbmp_inf		= new Bitmap(rc_panel.Width, rc_panel.Height, pgraphic);	//情報用メモリビットマップ
	pgraphic_inf	= new Gdiplus::Graphics(pbmp_inf);							//情報書き込みオブジェクト
	
	pbmp_img		= new Bitmap(rc_panel.Width, rc_panel.Height, pgraphic);			//画像書き込みオブジェクト
	pgraphic_img	= new Gdiplus::Graphics(pbmp_img);							//画像用メモリビットマップ

	pbmp_bk			= new Bitmap(rc_panel.Width, rc_panel.Height, pgraphic);	//背景用メモリビットマップ
	pgraphic_bk		= new Gdiplus::Graphics(pbmp_bk);								//背景用オブジェクト

	if (pgraphic_img == NULL || pgraphic_bk == NULL || pgraphic_inf == NULL) return E_FAIL;
	if (pbmp_img == NULL || pbmp_bk == NULL || pbmp_inf == NULL)return E_FAIL;
	
	pgraphic_img->SetSmoothingMode(SmoothingModeHighQuality);
	pgraphic_bk	->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_inf->SetSmoothingMode(SmoothingModeAntiAlias);
	pgraphic_img->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	pgraphic_bk->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	pgraphic_inf->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	pgraphic_img->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	pgraphic_bk->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	pgraphic_inf->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	pBrushBk = drawing_items.pbrush[ID_PANEL_COLOR_DGRAY];

#if 0
//ビットマップの作成
hBmp0		= CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
hBmp_img	= CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
hBmp_bk		= CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
hBmp_inf	= CreateCompatibleBitmap(hdc, rc_panel.Width, rc_panel.Height);
if (hBmp0 == NULL || hBmp_img == NULL || hBmp_bk == NULL || hBmp_inf == NULL)return E_FAIL;

SelectObject(hdc, hBmp0);
SelectObject(hdc_img, hBmp_img);
SelectObject(hdc_bk, hBmp_bk);
SelectObject(hdc_inf, hBmp_inf);
#endif

	return S_OK;
}
void CPanelObjBase::clear_graghics() {
	if(pgraphic != NULL)		delete pgraphic;
	if(pgraphic_img != NULL)	delete pgraphic_img;
	if (pgraphic_bk != NULL)	delete pgraphic_bk;
	if (pgraphic_inf != NULL)	delete pgraphic_inf;
	DeleteDC(hdc);
	DeleteDC(hdc_img);
	DeleteDC(hdc_bk);
	DeleteDC(hdc_inf);
	DeleteObject(hBmp0);
	DeleteObject(hBmp_img);
	DeleteObject(hBmp_bk);
	DeleteObject(hBmp_inf);
	ReleaseDC(hPnlWnd, hdc);
	return;
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
											 pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_WHITE],drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pfont[ID_PANEL_FONT_20]);
	
//1-2 リモートPBL
	i++;pb_remote			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt,pgraphic,drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lmp_remote			= new CLampCtrl(ID_MAIN_PNL_OBJ_PB_REMOTE, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pimg_remote, 3, 3);
	lmp_remote->set_txt_items(drawing_items.pfont[ID_PANEL_FONT_20], drawing_items.pstrformat[ID_STR_FORMAT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY]);

//3-4 認証表示TXT/PB
	i++; txt_uid			= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_UID, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++; pb_auth			= new CPbCtrl(ID_MAIN_PNL_OBJ_PB_AUTH, &main_props[i].pt, &main_props[i].sz, main_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

//5-10 通信状態ラベル/ランプ
	i++;str_pc_com_stat		= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_PC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
	i++;str_plc_com_stat	= new CStaticCtrl(ID_MAIN_PNL_OBJ_TXT_PLC_COM_STAT, &main_props[i].pt, &main_props[i].sz, main_props[i].txt);
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
void CMainPanelObj::refresh_obj_graphics() {

	str_message->refresh_graphics(pgraphic);
	pb_remote->refresh_graphics(pgraphic);
	lmp_remote->refresh_graphics(pgraphic);
	txt_uid->refresh_graphics(pgraphic);
	pb_auth->refresh_graphics(pgraphic);
	str_pc_com_stat->refresh_graphics(pgraphic);
	str_plc_com_stat->refresh_graphics(pgraphic);
	lmp_pcr->refresh_graphics(pgraphic);
	lmp_pcs->refresh_graphics(pgraphic);
	lmp_plcr->refresh_graphics(pgraphic);
	lmp_plcs->refresh_graphics(pgraphic);

	cb_estop->refresh_graphics(pgraphic);
	lmp_estop->refresh_graphics(pgraphic);
	pb_syukan_on->refresh_graphics(pgraphic);
	lmp_syukan_on->refresh_graphics(pgraphic);
	pb_syukan_off->refresh_graphics(pgraphic);
	lmp_syukan_off->refresh_graphics(pgraphic);
	cb_pnl_notch->refresh_graphics(pgraphic);
	pb_pad_mode->refresh_graphics(pgraphic);
	lmp_pad_mode->refresh_graphics(pgraphic);
	pb_assist_func->refresh_graphics(pgraphic);
	txt_ote_type->refresh_graphics(pgraphic);
	pb_ote_type_wnd->refresh_graphics(pgraphic);
	txt_link_crane->refresh_graphics(pgraphic);
	pb_crane_sel_wnd->refresh_graphics(pgraphic);

	cb_disp_mode1->refresh_graphics(pgraphic);
	cb_disp_mode2->refresh_graphics(pgraphic);
	rdo_disp_mode->refresh_graphics(pgraphic);

	cb_opt_flt->refresh_graphics(pgraphic);	//
	cb_opt_set->refresh_graphics(pgraphic);	//
	cb_opt_com->refresh_graphics(pgraphic);	//
	cb_opt_cam->refresh_graphics(pgraphic);	//
	cb_opt_stat->refresh_graphics(pgraphic);	//
	cb_opt_clr->refresh_graphics(pgraphic);	//
	rdo_opt_wnd->refresh_graphics(pgraphic);	//

	pb_freset->refresh_graphics(pgraphic);
	lmp_freset->refresh_graphics(pgraphic);
	txt_freset->refresh_graphics(pgraphic);	//
	return;
}

HRESULT CSubPanelObj::setup_obj() {
	static Image img_cs_mode0(L"../Img/HHGH29/cs_mode0.png"), img_cs_mode1(L"../Img/HHGH29/cs_mode1.png"), img_cs_mode2(L"../Img/HHGH29/cs_mode2.png");	
	static Image img_flt_bk_gr(L"../Img/HHGH29/bk_sub_gr.png"), img_flt_bk_rd(L"../Img/HHGH29/bk_sub_rd.png"), img_flt_bk_yl(L"../Img/HHGH29/bk_sub_yl.png"), img_flt_bk_bl(L"../Img/HHGH29/bk_sub_bl.png") ;
	//画像ポインタ配列
	Image* pimg_cs_mh_spd_mode[N_IMG_SWITCH_MAX] = { &img_cs_mode0, &img_cs_mode1 , &img_cs_mode2, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0 };
	Image* pimg_cs_bh_r_mode[N_IMG_SWITCH_MAX] = { &img_cs_mode0, &img_cs_mode1 , &img_cs_mode2, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0, &img_cs_mode0 };
	Image* pimg_flt_bk[N_IMG_SWITCH_MAX] = { &img_flt_bk_gr, &img_flt_bk_bl , &img_flt_bk_yl, &img_flt_bk_rd, &img_flt_bk_gr, &img_flt_bk_gr, &img_flt_bk_gr, &img_flt_bk_gr };

	//設定ウィンドウオブジェクト
	int i = 0;	cb_mh_spd_mode0 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_mh_spd_mode1 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_mh_spd_mode2 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	CCbCtrl* pcb_opt[8] = { cb_mh_spd_mode0,cb_mh_spd_mode1,cb_mh_spd_mode2, };
	i++; rdo_mh_spd_mode = new CRadioCtrl(3, pcb_opt);
	i++; lmp_mh_spd_mode = new CSwitchImg(ID_SUB_PNL_SET_OBJ_LMP_MHSPD, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pimg_cs_mh_spd_mode, 3, 3, pgraphic);

	i++; cb_bh_r_mode0 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_BHR_0, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_bh_r_mode1 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_BHR_1, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_bh_r_mode2 = new CCbCtrl(ID_SUB_PNL_SET_OBJ_RDO_BHR_2, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_YELLOW], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	CCbCtrl* pcb_opt2[8] = { cb_bh_r_mode0,cb_bh_r_mode1,cb_bh_r_mode2, };
	i++; rdo_bh_r_mode = new CRadioCtrl(3, pcb_opt2);
	i++; lmp_bh_r_mode = new CSwitchImg(ID_SUB_PNL_SET_OBJ_LMP_BHR, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pimg_cs_bh_r_mode, 3, 3, pgraphic);


	//# 状態表示ウィンドウオブジェクト
	i++; pb_stat_next		= new CPbCtrl(ID_SUB_PNL_STAT_OBJ_PB_NEXT, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; pb_stat_back		= new CPbCtrl(ID_SUB_PNL_STAT_OBJ_PB_BACK, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

	i++; st_mh_notch_dir		= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_DIR	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_notch_dir		= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_DIR		, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_sl_notch_dir		= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_SL_DIR		, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_gt_notch_dir		= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_GT_DIR		, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_mh_target_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_TG_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_target_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_TG_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_sl_target_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_SL_TG_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_gt_target_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_GT_TG_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_mh_ref_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_ref_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_sl_ref_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_SL_REF_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_gt_ref_v			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_GT_REF_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_mh_fb_v				= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_fb_v				= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_sl_fb_v				= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_gt_fb_v				= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_GT_FB_V	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_mh_ref_trq			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_REF_TRQ	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_ref_trq			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_REF_TRQ	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);

	i++; st_mh_fb_pg			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_PG, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_bh_fb_pg			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_BH_FB_PG, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_sl_fb_pg			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_SL_FB_PG, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);
	i++; st_mh_fb_abs			= new CStaticCtrl(ID_SUB_PNL_STAT_OBJ_STATIC_MH_FB_ABS, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);

	//# 故障表示ウィンドウオブジェクト
	
	//!!背景用のグラフィックオブジェクトを渡す
	i++; img_flt_bk			= new CSwitchImg(ID_SUB_PNL_FLT_OBJ_IMG_BK	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pimg_flt_bk, 5, 3, pgraphic_bk);

	i++; pb_flt_next		= new CPbCtrl(ID_SUB_PNL_FLT_OBJ_PB_NEXT	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_history	= new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_HISTORY	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_flt_heavy1 = new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_HEAVY1	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_flt_heavy2 = new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_HEAVY2	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_flt_heavy3	= new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_HEAVY3	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_flt_bypass		= new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_BYPASS	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);

	i++; str_flt_message	= new CStringGdi(ID_SUB_PNL_OBJ_STR_FLT_MESSAGE, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt,
		pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_BLUE], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);

	i++; cb_disp_flt_light = new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_LITE		, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; cb_disp_interlock = new CCbCtrl(ID_SUB_PNL_FLT_OBJ_CB_IL		, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; pb_disp_flt_plcmap = new CPbCtrl(ID_SUB_PNL_FLT_OBJ_PB_PLCMAP	, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt, pgraphic, drawing_items.ppen[ID_PANEL_COLOR_BLACK], drawing_items.ppen[ID_PANEL_COLOR_DGRAY]);
	i++; lv_flt_list		= new CListViewCtrl(ID_SUB_PNL_FLT_OBJ_LV_FAULTS, &sub_set_props[i].pt, &sub_set_props[i].sz, sub_set_props[i].txt);


	return S_OK;
}
void CSubPanelObj::delete_obj() {
	delete cb_mh_spd_mode0;
	delete cb_mh_spd_mode1;
	delete cb_mh_spd_mode2;
	delete rdo_mh_spd_mode;
	delete lmp_mh_spd_mode;
	delete cb_bh_r_mode0;
	delete cb_bh_r_mode1;
	delete cb_bh_r_mode2;
	delete rdo_bh_r_mode;
	delete lmp_bh_r_mode;
	delete pb_stat_next;
	delete pb_stat_back;
	delete img_flt_bk;
	delete pb_flt_next;
	delete cb_disp_history;
	delete cb_disp_interlock;
	delete cb_disp_flt_light;
	delete cb_disp_flt_heavy1;
	delete cb_flt_bypass;
	delete str_flt_message;
	delete cb_disp_flt_heavy2;
	delete cb_disp_flt_heavy3;
	delete pb_disp_flt_plcmap;
	delete lv_flt_list;
	return;

}
void CSubPanelObj::refresh_obj_graphics() {
	cb_mh_spd_mode0->refresh_graphics(pgraphic);
	cb_mh_spd_mode1->refresh_graphics(pgraphic);
	cb_mh_spd_mode2->refresh_graphics(pgraphic);

	rdo_mh_spd_mode->refresh_graphics(pgraphic);
	lmp_mh_spd_mode->refresh_graphics(pgraphic);

	cb_bh_r_mode0->refresh_graphics(pgraphic);
	cb_bh_r_mode1->refresh_graphics(pgraphic);
	cb_bh_r_mode2->refresh_graphics(pgraphic);
	rdo_bh_r_mode->refresh_graphics(pgraphic);
	lmp_bh_r_mode->refresh_graphics(pgraphic);

	img_flt_bk->refresh_graphics(pgraphic_bk);
	str_flt_message->refresh_graphics(pgraphic_inf);

	return;
}

HRESULT CGWindowObj::setup_obj() {
static Image img_gwin_bk(L"../Img/HHGH29/hhgh29_bk0.png");
static Image img_boom_xy(L"../Img/HHGH29/hhgh29_bm_top.png");
static Image img_crane_gt_base(L"../Img/HHGH29/hhgh29_potal_top.png");
static Image img_crane_potal(L"../Img/HHGH29/hhgh29_post_top.png");

Image* pimg_gwin_bk[N_IMG_SWITCH_MAX]	= { &img_gwin_bk, &img_gwin_bk,&img_gwin_bk,&img_gwin_bk,&img_gwin_bk,&img_gwin_bk,&img_gwin_bk,&img_gwin_bk };
Image* pimg_boom_xy[N_IMG_SWITCH_MAX]	= { &img_boom_xy, &img_boom_xy,&img_boom_xy,&img_boom_xy,&img_boom_xy,&img_boom_xy,&img_boom_xy,&img_boom_xy };
Image* pimg_gt_base[N_IMG_SWITCH_MAX]	= { &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base,  &img_crane_gt_base};
Image* pimg_post[N_IMG_SWITCH_MAX] = { &img_crane_potal,  &img_crane_potal,  &img_crane_potal,  &img_crane_potal,  &img_crane_potal,  &img_crane_potal,  &img_crane_potal,  &img_crane_potal };


	//設定ウィンドウオブジェクト
	int i = 0;	lmg_bk_gwindow	= new CSwitchImg(ID_GWIN_MAIN_OBJ_IMG_BK,		&gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt, pimg_gwin_bk, 3, 3, pgraphic);
		i++;	lmg_crane_bm_xy = new CSwitchImg(ID_GWIN_MAIN_OBJ_IMG_BOOM_XY,	&gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt, pimg_boom_xy, 3, 3, pgraphic);
		i++;	lmg_crane_gt_base = new CSwitchImg(ID_GWIN_MAIN_OBJ_IMG_GT_BASE,	&gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt, pimg_gt_base, 3, 3, pgraphic);
		i++;	lmg_crane_potal = new CSwitchImg(ID_GWIN_MAIN_OBJ_IMG_POTAL, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt, pimg_post, 3, 3, pgraphic);

		i++;	str_load_mh = new CStringGdi(ID_GWIN_MAIN_OBJ_STR_POS_MH, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt,
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
		i++;	str_pos_bh = new CStringGdi(ID_GWIN_MAIN_OBJ_STR_POS_BH, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt,
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
		i++;	str_pos_sl = new CStringGdi(ID_GWIN_MAIN_OBJ_STR_POS_SL, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt,
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
		i++;	str_pos_gt = new CStringGdi(ID_GWIN_MAIN_OBJ_STR_POS_GT, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt,
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
		i++;	str_pos_mouse = new CStringGdi(ID_GWIN_MAIN_OBJ_STR_POS_MOUSE, &gwin_set_props[i].pt, &gwin_set_props[i].sz, gwin_set_props[i].txt,
											pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_14]);

	return S_OK;
}
void CGWindowObj::delete_obj() {
	delete lmg_bk_gwindow;
	delete lmg_crane_bm_xy;
	delete lmg_crane_gt_base;
	delete str_load_mh;
	delete str_pos_bh;
	delete str_pos_sl;
	delete str_pos_gt;
	return;
}
void CGWindowObj::refresh_obj_graphics() {
	lmg_bk_gwindow->refresh_graphics(pgraphic_bk);
	lmg_crane_gt_base->refresh_graphics(pgraphic_bk);

	lmg_crane_bm_xy->refresh_graphics(pgraphic_img);
	lmg_crane_potal->refresh_graphics(pgraphic_img);
	

	str_load_mh->refresh_graphics(pgraphic_inf);
	str_pos_bh->refresh_graphics(pgraphic_inf);
	str_pos_sl->refresh_graphics(pgraphic_inf);
	str_pos_gt->refresh_graphics(pgraphic_inf);
	str_pos_mouse->refresh_graphics(pgraphic_inf);
	return;
}

HRESULT CGSubWindowObj::setup_obj() {
	static Image img_gsubwin_bk(L"../Img/HHGH29/hhgh29_bk1.png");
	static Image img_boom_yz(L"../Img/HHGH29/hhgh29_bm_side.png");
	static Image img_hook_mh(L"../Img/HHGH29/hhgh29_hook.png");
	static Image img_hook_load_mh(L"../Img/HHGH29/hhgh29_hook_load.png");

	Image* pimg_gsubwin_bk[N_IMG_SWITCH_MAX] = { &img_gsubwin_bk, &img_gsubwin_bk,&img_gsubwin_bk,&img_gsubwin_bk,&img_gsubwin_bk,&img_gsubwin_bk,&img_gsubwin_bk,&img_gsubwin_bk };
	Image* pimg_boom_yz[N_IMG_SWITCH_MAX] = { &img_boom_yz, &img_boom_yz,&img_boom_yz,&img_boom_yz,&img_boom_yz,&img_boom_yz,&img_boom_yz,&img_boom_yz };
	Image* pimg_hook_mh[N_IMG_SWITCH_MAX] = { &img_hook_mh, &img_hook_load_mh,&img_hook_mh,&img_hook_mh,&img_hook_mh,&img_hook_mh,&img_hook_mh,&img_hook_mh };

	//設定ウィンドウオブジェクト
	int i = 0;	lmg_bk_gsubwindow = new CSwitchImg(ID_GWIN_SUB_OBJ_IMG_BK, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt, pimg_gsubwin_bk, 3, 3, pgraphic);
	i++;		lmg_crane_bm_yz = new CSwitchImg(ID_GWIN_SUB_OBJ_IMG_BOOM_YZ, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt, pimg_boom_yz, 3, 3, pgraphic);
	i++;		lmg_crane_hook_mh = new CSwitchImg(ID_GWIN_SUB_OBJ_IMG_HOOK_MH, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt, pimg_hook_mh, 3, 3, pgraphic);


	i++;	str_pos_mh = new CStringGdi(ID_GWIN_SUB_OBJ_STR_POS_MH, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt,
		pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
	i++;	str_angle_bh = new CStringGdi(ID_GWIN_SUB_OBJ_STR_POS_MH, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt,
		pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_20]);
	i++;	str_pos_mouse = new CStringGdi(ID_GWIN_SUB_OBJ_STR_POS_MOUSE, &gsubwin_set_props[i].pt, &gsubwin_set_props[i].sz, gsubwin_set_props[i].txt,
		pgraphic, drawing_items.pstrformat[ID_STR_FORMAT_LEFT_CENTER], drawing_items.pbrush[ID_PANEL_COLOR_DGRAY], drawing_items.pbrush[ID_PANEL_COLOR_BLACK], drawing_items.pfont[ID_PANEL_FONT_14]);
		
	return S_OK;
}
void CGSubWindowObj::delete_obj() {
	delete lmg_bk_gsubwindow;
	delete lmg_crane_bm_yz;
	delete lmg_crane_hook_mh;

	delete str_pos_mh;
	delete str_angle_bh;

	return;
}
void CGSubWindowObj::refresh_obj_graphics() {
	lmg_bk_gsubwindow->refresh_graphics(pgraphic_bk);
	lmg_crane_bm_yz->refresh_graphics(pgraphic_img);
	lmg_crane_hook_mh->refresh_graphics(pgraphic_img);

	str_pos_mh->refresh_graphics(pgraphic_inf);
	str_angle_bh->refresh_graphics(pgraphic_inf);
	str_pos_mouse->refresh_graphics(pgraphic_inf);

	return;
}
