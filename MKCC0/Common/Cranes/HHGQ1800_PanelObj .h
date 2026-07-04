static ST_OBJ_PROPERTY main_props_hhgq18[N_MAIN_PNL_OBJ] = {
		{ID_MAIN_PNL_OBJ_STR_MESSAGE		,Point(920,0)	,Size(1000,40)	,L"-"	},
		{ID_MAIN_PNL_OBJ_STR_CRANE_TXT		,Point(750,1040),Size(1000,80)	,L"不明"},
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
		{ID_MAIN_PNL_OBJ_PB_CRANE_RELEASE	,Point(1780,850),Size(100,100)	,L"接続解除"		},

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
static ST_OBJ_PROPERTY sub_set_props_hhgq18[N_SUB_PNL_OBJ] = {
	//設定サブウィンドウ
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_0 ,Point(85,80)	,Size(30,30)	,L"-"			},//i=0
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_1	,Point(85,110)	,Size(30,30)	,L"7"			},
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_2	,Point(155,80)	,Size(30,30)	,L"14"			},
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD_3	,Point(225,110)	,Size(30,30)	,L"21"			},
	{ID_SUB_PNL_SET_OBJ_RDO_MHSPD	,Point(20,110)	,Size(30,30)	,L"主巻モード"	},
	{ID_SUB_PNL_SET_OBJ_LMP_MHSPD	,Point(130,120)	,Size(80,80)	,L"主巻モード"	},

	{ID_SUB_PNL_SET_OBJ_RDO_BHR_0	,Point(85,280)	,Size(30,30)	,L"-"			},//i=6
	{ID_SUB_PNL_SET_OBJ_RDO_BHR_1	,Point(85,280)	,Size(30,30)	,L"57"			},
	{ID_SUB_PNL_SET_OBJ_RDO_BHR_2	,Point(155,250)	,Size(30,30)	,L"62"			},
	{ID_SUB_PNL_SET_OBJ_RDO_BHR_3	,Point(225,280)	,Size(35,30)	,L"ﾚｽﾄ"			},
	{ID_SUB_PNL_SET_OBJ_RDO_BHR		,Point(20,280)	,Size(30,30)	,L"引込モード"	},
	{ID_SUB_PNL_SET_OBJ_LMP_BHR		,Point(130,290)	,Size(80,80)	,L"引込モード"	},

	{ID_SUB_PNL_SET_OBJ_CB_VDLY_DEVICE		,Point(380,50)	,Size(100,30)	,L"DeviceON"	},//i=12
	{ID_SUB_PNL_SET_OBJ_PB_VDLY_AUTO_PRM	,Point(380,100)	,Size(100,30)	,L"AutoPrm"		},
	{ID_SUB_PNL_SET_OBJ_PB_VDLY_PRM_SAVE	,Point(380,160)	,Size(100,30)	,L"SavePrm"		},
	{ID_SUB_PNL_SET_OBJ_PB_VDLY_PRM_LOAD	,Point(380,200)	,Size(100,30)	,L"LoadPrm"		},
	{ID_SUB_PNL_SET_OBJ_ST_VDLY_AUTO_PRM	,Point(500,105)	,Size(150,30)	,L"STANDBY"		},
	{ID_SUB_PNL_SET_OBJ_ST_VDLY_PRM_SAVE	,Point(500,180)	,Size(150,60)	,L"要求待ち"	},
	
	//状態サブウィンドウ
	{ID_SUB_PNL_STAT_OBJ_PB_NEXT	,Point(230,420)	,Size(50,30)	,L"NEXT"		},//i=17
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
	{ID_SUB_PNL_STAT_OBJ_STATIC_GT_FB_ABS	,Point(445,175)	,Size(100,30)	,L"GT ABS CNT"	},

	//故障表示サブウィンドウ
	{ID_SUB_PNL_FLT_OBJ_IMG_BK		,Point(0,0)		,Size(640,500)	,L"背景"		},	//i=35	CSwitchImg* img_flt_bk;
	{ID_SUB_PNL_FLT_OBJ_PB_NEXT		,Point(315,440)	,Size(30,30)	,L"次"			},	//		CPbCtrl* pb_stat_next;
	{ID_SUB_PNL_FLT_OBJ_CB_HISTORY	,Point(280,440)	,Size(40,30)	,L"履歴"		},	//		CCbCtrl* cb_stat_back;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY1	,Point(5,440)	,Size(40,30)	,L"重1"			},	//		CCbCtrl* cb_disp_flt_heavy1;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY2	,Point(50,440)	,Size(40,30)	,L"重2"			},	//		CCbCtrl* cb_disp_flt_heavy2;
	{ID_SUB_PNL_FLT_OBJ_CB_HEAVY3	,Point(95,440)	,Size(40,30)	,L"重3"			},	//		CCbCtrl* cb_disp_flt_heavy3;
	{ID_SUB_PNL_FLT_OBJ_CB_BYPASS	,Point(560,30)	,Size(60,30)	,L"BYPASS"		},	//		CCbCtrl* cb_flt_bypass;
	{ID_SUB_PNL_OBJ_STR_FLT_MESSAGE	,Point(10,30)	,Size(300,30)	,L"故障表示"	},	//		CStringGdi* str_flt_message
	{ID_SUB_PNL_FLT_OBJ_CB_LITE		,Point(140,440)	,Size(40,30)	,L"軽"			},	//		CCbCtrl* cb_disp_flt_light;
	{ID_SUB_PNL_FLT_OBJ_CB_IL		,Point(185,440)	,Size(35,30)	,L"IL"			},	//		CCbCtrl* cb_disp_flt_il;
	{ID_SUB_PNL_FLT_OBJ_CB_PCFLT	,Point(230,440)	,Size(35,30)	,L"PC"			},  //		CCbCtrl* cb_disp_flt_pc;
	{ID_SUB_PNL_FLT_OBJ_PB_PLCMAP	,Point(330,440)	,Size(35,30)	,L"MAP"			},	//		CCbCtrl* cb_disp_plcmap;
	{ID_SUB_PNL_FLT_OBJ_LV_FAULTS	,Point(5,65)	,Size(620,365)	,L"LIST VIEW"	},	//		CCbCtrl* cb_disp_flt_heavy;

	//通信サブウィンドウ
	{ID_SUB_PNL_COM_OBJ_RDO_IF_LINE	,Point(50,40)	,Size(60,30)	,L"有線"		},
	{ID_SUB_PNL_COM_OBJ_RDO_IF_WIFI	,Point(150,40)	,Size(60,30)	,L"WiFi"		},
	{ID_SUB_PNL_COM_OBJ_RDO_IF_MODE	,Point(50,40)	,Size(200,30)	,L"IFモード"	},

	{ID_SUB_PNL_COM_OBJ_SND_CYCLE	,Point(35, 90)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_DELEY_CYCLE	,Point(180, 90)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_LOST_CYCLE	,Point(350, 90)	,Size(50,30)	,L"-"	},

	{ID_SUB_PNL_COM_OBJ_DELAY_MAX	,Point(45, 155)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_DELAY_MIN	,Point(135,155)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_DELAY_AVE	,Point(225,155)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_N_LOST		,Point(315,155)	,Size(50,30)	,L"-"	},

	{ID_SUB_PNL_COM_UPDATE_SETTING	,Point(550,190)	,Size(30,30)	,L"SET"	},	//		CPbCtrl* pb_stat_next;

	{ID_SUB_PNL_COM_OBJ_SND_SEQ_NO	,Point(45, 210)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_RCV_SEQ_NO	,Point(135,210)	,Size(50,30)	,L"-"	},
	{ID_SUB_PNL_COM_OBJ_DEFF_SEQ_NO	,Point(225,210)	,Size(50,30)	,L"-"	},

	{ID_SUB_PNL_COM_OBJ_V_DELAY		,Point(45,270)	,Size(50,30)	,L"-"	},
};
static ST_OBJ_PROPERTY gwin_set_props_hhgq18[N_GWIN_OBJ] = {
	//グラフィックMAINウィンドウ
	{ID_GWIN_MAIN_OBJ_IMG_BK		,Point(0,0)			,Size(PRM_GWIN_SIZE_W,PRM_GWIN_SIZE_H),L"背景"		},//i=0
	{ID_GWIN_MAIN_OBJ_IMG_BOOM_XY	,Point(500,550)		,Size(70,500)	,L"ブーム上面"	},
	{ID_GWIN_MAIN_OBJ_IMG_GT_BASE	,Point(445,470)		,Size(120,62)	,L"走行装置"	},
	{ID_GWIN_MAIN_OBJ_IMG_POTAL		,Point(490,450)		,Size(50,100)	,L"ポータル"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_MH	,Point(20,20)		,Size(400,40)	,L"主巻位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_BH	,Point(20,60)		,Size(400,40)	,L"引込位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_SL	,Point(20,100)		,Size(400,40)	,L"旋回位置"	},
	{ID_GWIN_MAIN_OBJ_STR_POS_GT	,Point(20,140)		,Size(400,40)	,L"走行位置"	},//i=6

	{ID_GWIN_MAIN_OBJ_STR_POS_MOUSE	,Point(900,960)		,Size(100,20)	,L"マウス位置"	},//i=7

	{ID_GWIN_MAIN_OBJ_STR_SLBK_MODE,Point(700,830)		,Size(400,20)	,L"SLBK MODE"	},//i=8
	{ID_GWIN_MAIN_OBJ_STR_SLBK_LEVEL,Point(700,860)		,Size(400,20)	,L"SLBK LEVEL"	},//i=9

	{ID_GWIN_MAIN_OBJ_STR_WIND_SPD,Point(20,180)		,Size(400,40)	,L"風速　　"	},//i=10
	{ID_GWIN_MAIN_OBJ_STR_WIND_DIR,Point(20,220)		,Size(400,40)	,L"風向　　"	},//i=11

};
static ST_OBJ_PROPERTY gsubwin_set_props_hhgq18[N_GSUBWIN_OBJ] = {
	//グラフィックMAINウィンドウ
	{ID_GWIN_SUB_OBJ_IMG_BK			,Point(0,0)			,Size(645,510)	,L"背景"		},//i=0
	{ID_GWIN_SUB_OBJ_IMG_BOOM_YZ	,Point(130,300)		,Size(450,60)	,L"ブーム側面"	},
	{ID_GWIN_SUB_OBJ_IMG_HOOK_MH	,Point(500,400)		,Size(45,60)    ,L"フック"		},

	{ID_GWIN_SUB_OBJ_STR_POS_MH		,Point(20,20)		,Size(300,40)	,L"主巻位置"	},
	{ID_GWIN_SUB_OBJ_STR_ANGLE_BH	,Point(20,60)		,Size(300,40)	,L"引込位置"	},

	{ID_GWIN_SUB_OBJ_STR_POS_MOUSE	,Point(550,460)		,Size(100,20)	,L"マウス位置"	},//i=7
};
