#include "CSockLib.H"
#include "CCcEnv.h"
#include "resource.h"
#include "framework.h"
#include "AUXEQ_DEF.H"
#include "CCrane.H"
#include "CFaults.h"
#include "phisics.h"
#include "CComm.h"
#include "SmemAux.H"
#include "SWYSENSOR_DEF.H"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;
extern CSharedMem* pAuxCsInfObj;

extern INT32 aux_slbrk_status;						    //旋回ブレーキ	組み込み状況
extern INT32 aux_lanio_status;						    //LANIO			組み込み状況
extern INT32 aux_sway_status;						    //振れセンサ	組み込み状況
extern INT32 aux_gt_pos_sys_status;						//走行位置検出	組み込み状況

//ソケット
static CSockUDP* pUSockCcEnv;	//ユニキャストOTE通信受信用
extern ST_DEVICE_CODE g_my_code;
extern ST_APP_COMMON_PARAM g_app_common_param;
extern CCrane* pCrane;

ST_ENV_MON1 CCcEnv::st_mon1;
ST_ENV_MON2 CCcEnv::st_mon2;

CSpec* CCcEnv::pspec;

ST_CC_ENV_INF CCcEnv::st_work;

//共有メモリ
static LPST_CC_ENV_INF		pEnvInf;
static LPST_CC_PLC_IO		pPlcIo;
static LPST_JOB_IO			pJobIo;
static LPST_CC_POL_INF		pPolInf;
static LPST_CC_AGENT_INF	pAgentInf;
static LPST_CC_CS_INF		pCsInf;
static LPST_CC_SIM_INF		pSimInf;
static LPST_CC_OTE_INF		pOteInf;

static LPST_AUX_CS_INF		pAUX_CS_Inf = NULL;

static LONG rcv_count_u = 0, snd_count_u = 0;

static LPST_CRANE_STAT pCrStat = NULL;
static double dt = 0.02;	//スキャン周期20ms
/****************************************************************************/
/*   デフォルト関数											                */
/****************************************************************************/

CCcEnv::CCcEnv() {
}
CCcEnv::~CCcEnv() {
}

HRESULT CCcEnv::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;

	wos.str(L"初期化中…"); msg2host(wos.str());

	dt = (double)inf.cycle_ms / 1000.0;

	pEnvInf		= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPlcIo		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pJobIo		= (LPST_JOB_IO)(pJobIoObj->get_pMap());
	pPolInf		= (LPST_CC_POL_INF)(pPolInfObj->get_pMap());
	pAgentInf	= (LPST_CC_AGENT_INF)(pAgInfObj->get_pMap());
	pCsInf		= (LPST_CC_CS_INF)(pCsInfObj->get_pMap());
	pSimInf		= (LPST_CC_SIM_INF)(pSimuStatObj->get_pMap());
	pOteInf		= (LPST_CC_OTE_INF)(pOteInfObj->get_pMap());

	pAUX_CS_Inf = (LPST_AUX_CS_INF)pAuxCsInfObj->get_pMap();

	pCrStat = &(pEnvInf->crane_stat);

	if ((pEnvInf == NULL) || (pPlcIo == NULL) || (pJobIo == NULL) || (pPolInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL) || (pSimInf == NULL) || (pOteInf == NULL) || (pAUX_CS_Inf == NULL)) {
		hr = S_FALSE;
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	}

	if(pCrane == NULL) {
		wos.str(L""); wos << L"Initialize : Crane Object NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	pspec = pCrane->pSpec;

	//### パラメータセット
	{
		inf.mode_id = BC_ID_MODE0;//モード設定0

		CCcEnv* pEnvObj = (CCcEnv*)lpParam;
		pEnvInf->app_common_param = st_work.app_common_param = g_app_common_param;
		pEnvInf->device_code = st_work.device_code = g_my_code;
		inf.mode_id = st_work.app_common_param.app_mode;

		//### ドラムパラメータ初期化
		//set_drum_param(pCrane->st_crane_inf.crane_id);

		switch (pCrane->st_crane_inf.crane_type) {
		case CRANE_TYPE_ID_JC: {
			set_param_JC(pCrane->st_crane_inf.crane_id);
			fp_set_stat = set_stat_JC;
		} break;
		case CRANE_TYPE_ID_GC: {
			set_param_GC(pCrane->st_crane_inf.crane_id);
			fp_set_stat = set_stat_GC;
		}break;
		case CRANE_TYPE_ID_OHC: {
			set_param_OHC(pCrane->st_crane_inf.crane_id);
			fp_set_stat = set_stat_OHC;
		}break;
		default: 
			set_param_JC(pCrane->st_crane_inf.crane_id); 
			fp_set_stat = set_stat_JC;
		break;
		}
	
		//振れ周期補正用パラメータ
		pEnvInf->g_ratio_x = 1.0;
		pEnvInf->g_ratio_y = 1.0;

		//### 対象クレーン用関数ポインタセット
	//	fp_set_drum_stat = set_drum_stat;
		//計算用パラメータ設定
		pCrStat->abs_preset_cnt[ID_GANTRY] = (INT32)(pCrane->pSpec->axis_spec[ID_GANTRY].PosPreset / pCrane->pSpec->axis_spec[ID_GANTRY].Ddrm0 / PI180 * pCrane->pSpec->axis_spec[ID_GANTRY].CntAbsR);
	}
	
	//### IFウィンドウOPEN
	{
		WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
		LPARAM lp = BC_ID_MON2;
		SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

		if (st_mon2.hwnd_mon == NULL) {
			wos << L"Err(MON2 NULL Handle!!):";
			msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		Sleep(1000);
		//モニタ2選択CB状態セット
		{
			if (st_mon2.hwnd_mon != NULL)
				SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
			else
				SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
		}
	}

	//###  オペレーションパネル設定
	{
		inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
		for (int i = 1; i < 6; i++)	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);

		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
		if (pEnvInf->app_common_param.app_mode == MODE_ENV_APP_EMURATOR) {
			CheckRadioButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, IDC_TASK_MODE_RADIO2, IDC_TASK_MODE_RADIO1);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, BST_UNCHECKED);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, BST_UNCHECKED);
		}
		else if (pEnvInf->app_common_param.app_mode == MODE_ENV_APP_SIMURATION) {
			CheckRadioButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, IDC_TASK_MODE_RADIO2, IDC_TASK_MODE_RADIO2);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, BST_UNCHECKED);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, BST_UNCHECKED);
		}
		else {
			CheckRadioButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, IDC_TASK_MODE_RADIO2, IDC_TASK_MODE_RADIO0);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, BST_UNCHECKED);
			CheckDlgButton(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, BST_UNCHECKED);
		}
		//モニタウィンドウテキスト	
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"AUX IF");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, L"Product");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, L"Emulator");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, L"Simulator");
		set_func_pb_txt();
		set_item_chk_txt();
		set_panel_tip_txt();
	}
	//### 変数初期化
	plc_enable_hold = 0;

	return S_OK;
}

HRESULT CCcEnv::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << inf.dt;
		//wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		if (inf.mode_id == MODE_ENV_APP_SIMURATION)		wos  << L" MODE>>SIMULATOR";
		else if (inf.mode_id == MODE_ENV_APP_EMURATOR)	wos  << L" MODE>>EMULATOR";
		else											wos  << L" MODE>>PRODUCT";
		wos << L"  AUX_SWAY:" << aux_sway_status;

		msg2host(wos.str());
	}
	
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int CCcEnv::input() {
	return S_OK;
}

int CCcEnv::parse() {

	//### 状態セット ###
	//fp_set_drum_stat(pCrane->st_crane_inf.crane_id);
	fp_set_stat(pCrane->st_crane_inf.crane_id);
	
	//###  故障情報セット ###
	if (pPlcIo->plc_enable) {
		if(plc_enable_hold) {	//PLC有効状態変化無し（前回も有効）
			set_faults_info();
		}
		else {					//PLC有効トリガ状態（前回無効）
			refresh_faults_info();
		}
	}
	plc_enable_hold = pPlcIo->plc_enable;	//PLC有効状態保持

	return STAT_OK; 
}
int CCcEnv::output() {          //出力処理
	//共有メモリに直接書き込みにする　memcpy_s(pEnvInf, sizeof(ST_CC_ENV_INF), &st_work, sizeof(ST_CC_ENV_INF));
	return STAT_OK;
}

int CCcEnv::close() {
	return 0;
}

/****************************************************************************/
/*   クレーン状態														    */
/****************************************************************************/
/// <summary>
/// 
/// </summary>
/// <param name="id"></param>
void CCcEnv::set_param_JC(int id) {

	//### ドラムの周長(層ドラムパラメータ初期化(インデックス＝層数）
	for (int i = 0; i < N_DRUM_LAYER; i++) {
		if (i == 0) {
			pEnvInf->Cdrm[ID_HOIST][i]	= 0.0;
			pEnvInf->Cdrm[ID_BOOM_H][i] = 0.0;
			pEnvInf->Cdrm[ID_SLEW][i]	= 0.0;
			pEnvInf->Cdrm[ID_GANTRY][i] = 0.0;
			pEnvInf->Cdrm[ID_AHOIST][i] = 0.0;

			pEnvInf->Cdrm[ID_BH_HST][i] = 0.0;
		}
		else {//ドラム層の円周長計算(層負荷直径×π)
			pEnvInf->Cdrm[ID_HOIST][i]	= (pspec->axis_spec[ID_HOIST].Ddrm0		+ (double)(i - 1) * pspec->axis_spec[ID_HOIST].dDdrm) * PI180;
			pEnvInf->Cdrm[ID_BOOM_H][i]	= (pspec->axis_spec[ID_BOOM_H].Ddrm0	+ (double)(i - 1) * pspec->axis_spec[ID_BOOM_H].dDdrm) * PI180;
			pEnvInf->Cdrm[ID_SLEW][i]	= (pspec->axis_spec[ID_SLEW].Ddrm0		+ (double)(i - 1) * pspec->axis_spec[ID_SLEW].dDdrm) * PI180;
			pEnvInf->Cdrm[ID_GANTRY][i] = (pspec->axis_spec[ID_GANTRY].Ddrm0	+ (double)(i - 1) * pspec->axis_spec[ID_GANTRY].dDdrm) * PI180;
			pEnvInf->Cdrm[ID_AHOIST][i] = (pspec->axis_spec[ID_AHOIST].Ddrm0	+ (double)(i - 1) * pspec->axis_spec[ID_AHOIST].dDdrm) * PI180;

			//引込主巻ドラム 層負荷直径はBHを使用
			pEnvInf->Cdrm[ID_BH_HST][i] = (pspec->axis_spec[ID_BOOM_H].Ddrm1 + (double)(i-1) * pspec->axis_spec[ID_HOIST].dDdrm) * PI180;//ドラム追加半径は主巻ドラムの数値を使う
		}
		
		//層巻取り量(ドラム周長×ドラム溝数)の積算値を計算)
		if (i == 0) {//ドラム層巻取り量の全ロープ長
			pEnvInf->Ldrm[ID_HOIST][i]	= 0.0;
			pEnvInf->Ldrm[ID_BOOM_H][i] = 0.0;
			pEnvInf->Ldrm[ID_SLEW][i]	= 0.0;
			pEnvInf->Ldrm[ID_GANTRY][i] = 0.0;
			pEnvInf->Ldrm[ID_AHOIST][i] = 0.0;
			pEnvInf->Ldrm[ID_BH_HST][i] = 0.0;//引込主巻ドラムは主巻ドラムの全ロープ長を使用
		}
		else {
			pEnvInf->Ldrm[ID_HOIST][i]	= pEnvInf->Ldrm[ID_HOIST][i - 1]	+ pEnvInf->Cdrm[ID_HOIST][i]	* pspec->axis_spec[ID_HOIST].Ndmizo0;
			pEnvInf->Ldrm[ID_BOOM_H][i] = pEnvInf->Ldrm[ID_BOOM_H][i - 1]	+ pEnvInf->Cdrm[ID_BOOM_H][i]	* pspec->axis_spec[ID_BOOM_H].Ndmizo0;
			pEnvInf->Ldrm[ID_SLEW][i]	= pEnvInf->Ldrm[ID_SLEW][i - 1]		+ pEnvInf->Cdrm[ID_SLEW][i]		* pspec->axis_spec[ID_SLEW].Ndmizo0;
			pEnvInf->Ldrm[ID_GANTRY][i] = pEnvInf->Ldrm[ID_GANTRY][i - 1]	+ pEnvInf->Cdrm[ID_GANTRY][i]	* pspec->axis_spec[ID_GANTRY].Ndmizo0;
			pEnvInf->Ldrm[ID_AHOIST][i] = pEnvInf->Ldrm[ID_AHOIST][i - 1]	+ pEnvInf->Cdrm[ID_AHOIST][i]	* pspec->axis_spec[ID_AHOIST].Ndmizo0;
			pEnvInf->Ldrm[ID_BH_HST][i] = pEnvInf->Ldrm[ID_BH_HST][i - 1]	+ pEnvInf->Cdrm[ID_BH_HST][i]	* pspec->axis_spec[ID_BOOM_H].Ndmizo1;
		}
	}
	
	//### ドラム全巻取り量(ドラム層巻取り量の最終層値)
	pEnvInf->Lspan[ID_HOIST]	= pspec->axis_spec[ID_HOIST].Lfull;
	pEnvInf->Lspan[ID_BOOM_H]	= pspec->axis_spec[ID_BOOM_H].Lfull;
	pEnvInf->Lspan[ID_SLEW]		= pspec->axis_spec[ID_SLEW].Lfull;
	pEnvInf->Lspan[ID_GANTRY]	= pspec->axis_spec[ID_GANTRY].Lfull;
	pEnvInf->Lspan[ID_AHOIST]	= pspec->axis_spec[ID_AHOIST].Lfull;

	switch (id) {
	case CRANE_ID_H6R602:
	{
		;
	}break;
	case CRANE_ID_HHGQ18:
	{
		;
	}break;
	case CRANE_ID_HHGH29:
	default:
	{
		;
	}break;
	};
	return;
};
void CCcEnv::set_param_GC(int id) {
	switch (id) {
	case CRANE_ID_HHFM08:
	default:
	{
		;
	}break;
	};
	return;
};
void CCcEnv::set_param_OHC(int id) {
	switch (id) {
	case CRANE_ID_XXXXXX:
	default:
	{
		;
	}break;
	};
	return;
};

/// <summary>
/// 
/// </summary>
/// <param name="id"></param>
/// <returns></returns>


HRESULT CCcEnv::set_stat_JC(int id) {
	LPUN_PLC_RBUF pPlcRbuf = (LPUN_PLC_RBUF)pPlcIo->buf_io_read;
	double hcount_sl = 0.0;
	switch (id) {
	case CRANE_ID_H6R602:
	{
		hcount_sl = pPlcRbuf->st_h6r602.hcount_fb[ID_PLC_HCOUNT_SL];//旋回角度計算用高速カウンタ値取り込み
	}break;
	case CRANE_ID_HHGQ18:
	{
		hcount_sl = pPlcRbuf->st_hhgq18.hcount_fb[ID_PLC_HCOUNT_SL];//旋回角度計算用高速カウンタ値取り込み
	}break;
	case CRANE_ID_HHGH29:
	default:
	{
		hcount_sl = pPlcRbuf->st_hhgh29.hcount_fb[ID_PLC_HCOUNT_SL];//旋回角度計算用高速カウンタ値取り込み
	}break;
	};

	//###　ドラム回転数セット
	//主巻ドラム回転　(abs fb - プリセットカウント）/ドラム1回転abs cnt + プリセットドラム回転数
	pCrStat->nd[ID_HOIST].p = (pPlcIo->stat_axis[ID_HOIST].absocoder - pspec->axis_spec[ID_HOIST].CntAbsSet0) / pspec->axis_spec[ID_HOIST].CntAbsR		+ pspec->axis_spec[ID_HOIST].NdrmAbsSet0;

	//起伏(起伏）ドラム回転　　(pg fb - プリセットカウント）/ドラム1回転pg cnt + プリセットドラム回転数
	pCrStat->nd[ID_BOOM_H].p = - (pPlcIo->stat_axis[ID_BOOM_H].pg_count - pspec->axis_spec[ID_BOOM_H].CntPgSet0) / pspec->axis_spec[ID_BOOM_H].CntPgDrumR	+ pspec->axis_spec[ID_BOOM_H].NdrmPgSet0;

	//起伏(主巻）ドラム回転　　(pg fb - プリセットカウント）/ドラム1回転pg cnt + プリセットドラム回転数
	pCrStat->nd[ID_BH_HST].p = pspec->st_struct.Nttl_bh - pCrStat->nd[ID_BOOM_H].p;

	//旋回ドラム回転　　(pg fb - プリセットカウント）/ドラム1回転pg cnt + プリセットドラム回転数
	pCrStat->nd[ID_SLEW].p = (pPlcIo->stat_axis[ID_SLEW].pg_count - pspec->axis_spec[ID_SLEW].CntPgSet0) / pspec->axis_spec[ID_SLEW].CntPgDrumR + pspec->axis_spec[ID_SLEW].NdrmPgSet0;

	//走行ドラム回転　(abs fb - プリセットカウント）/ドラム1回転abs cnt + プリセットドラム回転数
	pCrStat->nd[ID_GANTRY].p = (pPlcIo->stat_axis[ID_GANTRY].absocoder - pspec->axis_spec[ID_GANTRY].CntAbsSet0) / pspec->axis_spec[ID_GANTRY].CntAbsR + pspec->axis_spec[ID_GANTRY].NdrmAbsSet0;

	//###  ドラム回転加速度，速度セット
	//###  回転速度セット ±rpm単位 →rps単位に変換
	double v_fb = (double)pPlcIo->stat_axis[ID_HOIST].v_fb / 60.0 / pspec->axis_spec[ID_HOIST].Gear_ratio;		//主巻 RPS	
	pCrStat->nd[ID_HOIST].a = (v_fb - pCrStat->nd[ID_HOIST].v) / dt;
	pCrStat->nd[ID_HOIST].v	= v_fb;

	v_fb =(double)pPlcIo->stat_axis[ID_BOOM_H].v_fb / 60.0 / pspec->axis_spec[ID_BOOM_H].Gear_ratio;			//起伏(起伏） RPS
	pCrStat->nd[ID_BOOM_H].a = (v_fb - pCrStat->nd[ID_BOOM_H].v) / dt;
	pCrStat->nd[ID_BOOM_H].v = v_fb;

	v_fb = -pCrStat->nd[ID_BOOM_H].v;																			//起伏（主巻） RPS
	pCrStat->nd[ID_BH_HST].a = (v_fb - pCrStat->nd[ID_BH_HST].v) / dt;
	pCrStat->nd[ID_BH_HST].v = v_fb;

	v_fb = (double)pPlcIo->stat_axis[ID_SLEW].v_fb / 60.0 / pspec->axis_spec[ID_SLEW].Gear_ratio;				//旋回 RPS
	pCrStat->nd[ID_SLEW].a = (v_fb - pCrStat->nd[ID_SLEW].v) / dt;
	pCrStat->nd[ID_SLEW].v	= v_fb;

	v_fb = (double)pPlcIo->stat_axis[ID_GANTRY].v_fb / 60.0 / pspec->axis_spec[ID_GANTRY].Gear_ratio;			//走行 RPS
	pCrStat->nd[ID_GANTRY].a = (v_fb - pCrStat->nd[ID_GANTRY].v) / dt;
	pCrStat->nd[ID_GANTRY].v = v_fb;

	//###  巻取量セット
	//ドラム層数計算（ドラム回転数/ドラム1層巻取り回転数）+1
	pCrStat->i_layer[ID_HOIST]	= (INT32)(pCrStat->nd[ID_HOIST].p / pspec->axis_spec[ID_HOIST].Ndmizo0)		+ 1;
	pCrStat->i_layer[ID_BOOM_H]	= (INT32)(pCrStat->nd[ID_BOOM_H].p / pspec->axis_spec[ID_BOOM_H].Ndmizo0)	+ 1;
	pCrStat->i_layer[ID_BH_HST] = (INT32)(pCrStat->nd[ID_BH_HST].p / pspec->axis_spec[ID_BH_HST].Ndmizo1)	+ 1;
	pCrStat->i_layer[ID_SLEW]	= 1;
	pCrStat->i_layer[ID_GANTRY]	= 1;

	//範囲外チェック
	if (!(pCrStat->i_layer[ID_HOIST]	> 0 && pCrStat->i_layer[ID_HOIST]	< N_DRUM_LAYER))	pCrStat->i_layer[ID_HOIST]	= 0;
	if (!(pCrStat->i_layer[ID_BOOM_H]	> 0 && pCrStat->i_layer[ID_BOOM_H]	< N_DRUM_LAYER))	pCrStat->i_layer[ID_BOOM_H]	= 0;
	if (!(pCrStat->i_layer[ID_BH_HST]	> 0 && pCrStat->i_layer[ID_BH_HST]	< N_DRUM_LAYER))	pCrStat->i_layer[ID_BH_HST]	= 0;

	//ドラム層巻取数計算
	pCrStat->n_layer[ID_HOIST]	= pCrStat->nd[ID_HOIST].p	- (double)(pCrStat->i_layer[ID_HOIST]  - 1 ) * pspec->axis_spec[ID_HOIST].Ndmizo0;
	pCrStat->n_layer[ID_BOOM_H] = pCrStat->nd[ID_BOOM_H].p	- (double)(pCrStat->i_layer[ID_BOOM_H] - 1 ) * pspec->axis_spec[ID_BOOM_H].Ndmizo0;
	pCrStat->n_layer[ID_BH_HST] = pCrStat->nd[ID_BH_HST].p	- (double)(pCrStat->i_layer[ID_BH_HST] - 1 ) * pspec->axis_spec[ID_BH_HST].Ndmizo1;
	pCrStat->n_layer[ID_SLEW]	= pCrStat->nd[ID_SLEW].p;
	pCrStat->n_layer[ID_GANTRY] = pCrStat->nd[ID_GANTRY].p;

	//ドラム巻取量
	pCrStat->ld[ID_HOIST].p		= pEnvInf->Ldrm[ID_HOIST][pCrStat->i_layer[ID_HOIST] - 1]	+ pCrStat->n_layer[ID_HOIST]  * pEnvInf->Cdrm[ID_HOIST][pCrStat->i_layer[ID_HOIST]];
	pCrStat->ld[ID_BOOM_H].p	= pEnvInf->Ldrm[ID_BOOM_H][pCrStat->i_layer[ID_BOOM_H] - 1] + pCrStat->n_layer[ID_BOOM_H] * pEnvInf->Cdrm[ID_BOOM_H][pCrStat->i_layer[ID_BOOM_H]];
	pCrStat->ld[ID_BH_HST].p	= pEnvInf->Ldrm[ID_BH_HST][pCrStat->i_layer[ID_BH_HST] - 1] + pCrStat->n_layer[ID_BH_HST] * pEnvInf->Cdrm[ID_BH_HST][pCrStat->i_layer[ID_BH_HST]];
	pCrStat->ld[ID_SLEW].p		= pEnvInf->Ldrm[ID_SLEW][pCrStat->i_layer[ID_SLEW] - 1]		+ pCrStat->n_layer[ID_SLEW]   * pEnvInf->Cdrm[ID_SLEW][pCrStat->i_layer[ID_SLEW]];;
	pCrStat->ld[ID_GANTRY].p	= pEnvInf->Ldrm[ID_GANTRY][pCrStat->i_layer[ID_GANTRY] - 1] + pCrStat->n_layer[ID_GANTRY] * pEnvInf->Cdrm[ID_GANTRY][pCrStat->i_layer[ID_GANTRY]];
	
	//巻取速度（ドラム回転速度×ドラム周長：1秒間の回転量×1回転の長さ）
	pCrStat->ld[ID_HOIST].v		= pCrStat->nd[ID_HOIST].v	* pEnvInf->Cdrm[ID_HOIST][pCrStat->i_layer[ID_HOIST]];
	pCrStat->ld[ID_BOOM_H].v	= pCrStat->nd[ID_BOOM_H].v	* pEnvInf->Cdrm[ID_BOOM_H][pCrStat->i_layer[ID_BOOM_H]];
	pCrStat->ld[ID_BH_HST].v	= pCrStat->nd[ID_BH_HST].v	* pEnvInf->Cdrm[ID_BH_HST][pCrStat->i_layer[ID_BH_HST]];
	pCrStat->ld[ID_SLEW].v		= pCrStat->nd[ID_SLEW].v	* pEnvInf->Cdrm[ID_SLEW][pCrStat->i_layer[ID_SLEW]];
	pCrStat->ld[ID_GANTRY].v	= pCrStat->nd[ID_GANTRY].v	* pEnvInf->Cdrm[ID_GANTRY][pCrStat->i_layer[ID_GANTRY]];

	//###d 
	//引込入限 = 繰り出し量0　→　ドラム巻取り量=繰り出し量
	//現在のd値　繰出量/ロープ本数 + 引込入限d値（d0)
	pCrStat->d.p	= pspec->st_struct.d0 + (pspec->axis_spec[ID_BOOM_H].Lfull- pCrStat->ld[ID_BOOM_H].p) / pspec->axis_spec[ID_BOOM_H].Nwire0;
	v_fb			= pCrStat->ld[ID_BOOM_H].v / pspec->axis_spec[ID_BOOM_H].Nwire0;	//現在のd値速度 ロープ巻き取り速度/ロープ本数
	pCrStat->d.a = (v_fb - pCrStat->d.v)/dt;
	pCrStat->d.v = v_fb;

	//###dh 
	double Lb = pspec->st_struct.Lb, Ha = pspec->st_struct.Ha, Hp = pspec->st_struct.Hp;
	double d2 = pCrStat->d.p * pCrStat->d.p;
	pCrStat->dh.p = (Lb * Lb - d2 - Ha * Ha) / (2.0 * Ha);
	v_fb = -pCrStat->d.p * pCrStat->d.v / Ha;
	pCrStat->dh.a = (v_fb - pCrStat->dh.v) / dt;
	pCrStat->dh.v = v_fb;

	//### 旋回半径
	pCrStat->r.p = sqrt(d2 - pCrStat->dh.p * pCrStat->dh.p);	//旋回半径
	v_fb = (pCrStat->d.p * pCrStat->d.v - pCrStat->dh.p * pCrStat->dh.v) / pCrStat->r.p;
	pCrStat->r.a = (v_fb - pCrStat->r.v) / dt;
	pCrStat->r.v = v_fb;

	//### 主巻ロープ長
	//主巻ロープ長 = ( ロープ全長- 主巻ドラム巻取量 - 起伏(主巻）ドラム巻取量 - (d値×ジブ部ロープ本数))/吊部ロープ本数
	pCrStat->mhl.p = (pspec->axis_spec[ID_HOIST].Lfull - pCrStat->ld[ID_HOIST].p - pCrStat->ld[ID_BH_HST].p - pCrStat->d.p * pspec->axis_spec[ID_HOIST].Nwire0)/ pspec->axis_spec[ID_HOIST].Nwire1;
	//主巻ロープ長速度 = 主巻ロープ長の微分
	v_fb = -(pCrStat->ld[ID_HOIST].v + pCrStat->ld[ID_BH_HST].v + pCrStat->d.v * pspec->axis_spec[ID_HOIST].Nwire0 ) / pspec->axis_spec[ID_HOIST].Nwire1;
	pCrStat->mhl.a = (v_fb - pCrStat->mhl.v) / dt;
	pCrStat->mhl.v = v_fb;

	//起伏角
	pCrStat->bh_th.p = acos(pCrStat->r.p / Lb);
	if (pCrStat->dh.p < 0.0) pCrStat->bh_th.p *= -1.0;
	pCrStat->bh_th.v = -pCrStat->r.v / (Lb * sin(pCrStat->bh_th.p));

	//旋回角度(rad)
	LONG count_sl = (LONG)(hcount_sl - pspec->axis_spec[ID_SLEW].CntPgSet0) % (LONG)(pspec->axis_spec[ID_SLEW].Kp * 360);//180度カウント数/180
	pCrStat->sl_ph.p = count_sl / pspec->axis_spec[ID_SLEW].Kp * RAD1DEG;
	if (pCrStat->sl_ph.p > PI180) pCrStat->sl_ph.p -= PI360;
	
	// 360°回転数　= （TTB円周/旋回ドラム円周）＝ TTB径/ピニオン径 →　ピニオン1回転あたりの旋回角度 = 360°/360°回転数
	// rad/s　=　RPS　×　360°/（TTB径/ピニオン径）×　π　/　180°
	// rad/s　=　RPS　×　2　×　π　×　ピニオン径　/　TTB径　
	v_fb = pCrStat->nd[ID_SLEW].v * PI360 * pspec->axis_spec[ID_SLEW].Ddrm0 / pspec->axis_spec[ID_SLEW].Ddrm1;
	pCrStat->sl_ph.a = (v_fb - pCrStat->sl_ph.v) / dt;
	pCrStat->sl_ph.v = v_fb;

	//吊点高さ
	pCrStat->hpz.p = pspec->st_struct.Hp + pspec->st_struct.Ha + pCrStat->dh.p;
	pCrStat->hpz.v = pCrStat->dh.v;
	pCrStat->hpz.a = pCrStat->dh.a;

	//揚程 Simulation側の揚程値を使用する
	pCrStat->ldz.p = pCrStat->hpz.p - pCrStat->mhl.p;
	pCrStat->ldz.v = - pCrStat->mhl.v;
	pCrStat->ldz.a = -pCrStat->mhl.a;

	//### 荷重・位置状態セット ###
	//荷重
	pCrStat->m.p = pPlcIo->weight;

	//走行位置
	double dL = (double)(pPlcIo->stat_axis[ID_GANTRY].absocoder - pCrStat->abs_preset_cnt[ID_GANTRY]) / pCrane->pSpec->axis_spec[ID_GANTRY].CntAbsR;//ドラム回転数
	dL *= PI180 * pCrane->pSpec->axis_spec[ID_GANTRY].Ddrm0;
	pCrStat->gt.p = pCrane->pSpec->axis_spec[ID_GANTRY].PosPreset + dL;


	//ロープ長（PLC側の揚程値を使用）
	pEnvInf->l_mh = Hp + sqrt(Lb * Lb - pPlcIo->r * pPlcIo->r) - pPlcIo->h_mh;

	//振れ周期,各周波数
	if(pEnvInf->g_ratio_x <= 0.0) pEnvInf->g_ratio_x = 1.0;
	if(pEnvInf->g_ratio_y <= 0.0) pEnvInf->g_ratio_y = 1.0;
	pEnvInf->wx		= sqrt(GA * pEnvInf->g_ratio_x / pEnvInf->l_mh);
	pEnvInf->wy		= sqrt(GA * pEnvInf->g_ratio_y / pEnvInf->l_mh);
	//pEnvInf->w2x	= pEnvInf->wx * pEnvInf->wx;
	//pEnvInf->w2y	= pEnvInf->wy * pEnvInf->wy;
	pEnvInf->Tx		= PI360 / pEnvInf->wx;
	pEnvInf->Ty		= PI360 / pEnvInf->wy;

	return S_OK;
};

HRESULT CCcEnv::set_stat_GC(int id) {
	switch (id) {
	case CRANE_ID_HHGQ18:
	{
		;
	}break;
	case CRANE_ID_HHGH29:
	default:
	{
		;
	}break;
	};
	return S_OK;
};

HRESULT CCcEnv::set_stat_OHC(int id) {
	switch (id) {
	case CRANE_ID_HHGQ18:
	{
		;
	}break;
	case CRANE_ID_HHGH29:
	default:
	{
		;
	}break;
	};
	return S_OK;
};

/****************************************************************************/
/*   故障情報											                    */
/****************************************************************************/
void CCcEnv::set_faults_info() {
	PINT16 pflt_rbuf = pCrane->pFlt->prfltbuf;

	//####	PLCフォルト
	//###	毎周期更新
	//##	PLC故障検出情報現在値を取り込み
	for (int i = FAULT_TYPE::BASE; i <= FAULT_TYPE::IL; i++) {
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			pCrStat->fault_list.faults_detected_map[i][j] = pflt_rbuf[j]& pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::BASE][j];
		}
	}

	//### 1秒に1回トリガ検出更新
	if (inf.act_count % 50 == 0){	
		SYSTEMTIME systime; GetSystemTime(&systime);
		//トリガ検出ロジック　	(前回値　XOR　現在値）AND　現在値でトリガON検出
		//						(前回値　XOR　現在値）AND　前回値でトリガOFF検出
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			plc_falt_detected_trig_on[j] = (plc_falt_detected_hold[j]^ pCrStat->fault_list.faults_detected_map[FAULT_TYPE::BASE][j])& pCrStat->fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
			plc_falt_detected_trig_off[j] = plc_falt_detected_hold[j] ^ pCrStat->fault_list.faults_detected_map[FAULT_TYPE::BASE][j]& plc_falt_detected_hold[j];
	
			INT16 chk_bit;
			if (plc_falt_detected_trig_on[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//チェックビット
					if (plc_falt_detected_trig_on[j] & chk_bit) {
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].systime = systime;	//時間
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code = j*16+k;		//故障コード
						++pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code;				//++故障コードは１から開始
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].status = CODE_TRIG_ON;//種別
					
						pCrStat->fault_list.iw_history++;	//書き込みポインタ更新
						pCrStat->fault_list.history_count++;	//レコード数更新
						if (pCrStat->fault_list.iw_history >= N_FAULTS_HISTORY_BUF)pCrStat->fault_list.iw_history = 0;
						if (pCrStat->fault_list.history_count > N_FAULTS_HISTORY_BUF)pCrStat->fault_list.history_count = N_FAULTS_HISTORY_BUF;
					}
				}
			}
			if (plc_falt_detected_trig_off[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//チェックビット
					if (plc_falt_detected_trig_off[j] & chk_bit) {
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].systime = systime;	//時間
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code = j * 16 + k;	//故障コード
						++pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code;				//++故障コードは１から開始
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].status = CODE_TRIG_OFF;//種別

						pCrStat->fault_list.iw_history++;	//書き込みポインタ更新
						pCrStat->fault_list.history_count++;	//レコード数更新
						if (pCrStat->fault_list.iw_history >= N_FAULTS_HISTORY_BUF)pCrStat->fault_list.iw_history = 0;
						if (pCrStat->fault_list.history_count > N_FAULTS_HISTORY_BUF)pCrStat->fault_list.history_count = N_FAULTS_HISTORY_BUF;

					}
				}
			}
		}

		//####	PCフォルト
		for (int j = 0; j < N_PC_FAULT_BUF; j++) {
			pc_falt_detected_trig_on[j] = (pc_falt_detected_hold[j] ^ pPolInf->pc_fault_map[j]) & pPolInf->pc_fault_map[j];
			pc_falt_detected_trig_off[j] = pc_falt_detected_hold[j] ^ pPolInf->pc_fault_map[j] & pc_falt_detected_hold[j];

			INT16 chk_bit;
			if (pc_falt_detected_trig_on[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//チェックビット
					if (pc_falt_detected_trig_on[j] & chk_bit) {
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].systime = systime;			//時間
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code = j * 16 + k;			//故障コード
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code += N_PC_FLT_CODE_OFFSET;	//故障コードは550から開始
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].status = CODE_TRIG_ON;//種別

						pCrStat->fault_list.iw_history++;	//書き込みポインタ更新
						pCrStat->fault_list.history_count++;	//レコード数更新
						if (pCrStat->fault_list.iw_history >= N_FAULTS_HISTORY_BUF)	pCrStat->fault_list.iw_history = 0;
						if (pCrStat->fault_list.history_count > N_FAULTS_HISTORY_BUF)pCrStat->fault_list.history_count = N_FAULTS_HISTORY_BUF;
					}
				}
			}
			if (pc_falt_detected_trig_off[j]) {
				for (int k = 0; k < 16; k++) {
					chk_bit = 1 << k;	//チェックビット
					if (pc_falt_detected_trig_off[j] & chk_bit) {
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].systime = systime;			//時間
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code = j * 16 + k;			//故障コード
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].code + N_PC_FLT_CODE_OFFSET;	//故障コードは550から開始
						pCrStat->fault_list.history[pCrStat->fault_list.iw_history].status = CODE_TRIG_OFF;		//種別

						pCrStat->fault_list.iw_history++;	//書き込みポインタ更新
						pCrStat->fault_list.history_count++;	//レコード数更新
						if (pCrStat->fault_list.iw_history >= N_FAULTS_HISTORY_BUF)	pCrStat->fault_list.iw_history = 0;
						if (pCrStat->fault_list.history_count > N_FAULTS_HISTORY_BUF)pCrStat->fault_list.history_count = N_FAULTS_HISTORY_BUF;
					}
				}
			}
		}

		//トリガ検出前回値保持
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) plc_falt_detected_hold[j] = pCrStat->fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
		for (int j = 0; j < N_PC_FAULT_BUF; j++) pc_falt_detected_hold[j]	= pPolInf->pc_fault_map[j];
	}

	return;
}
void CCcEnv::refresh_faults_info() {
	PINT16 pflt_rbuf = pCrane->pFlt->prfltbuf;

	for (int i = FAULT_TYPE::BASE; i <= FAULT_TYPE::IL; i++) {
		for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
			pCrStat->fault_list.faults_detected_map[i][j] = pflt_rbuf[j] & pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::BASE][j];
		}
	}
	//前回値=今回値,トリガ検出無し
	for (int j = 0; j < N_PLC_FAULT_BUF; j++) {
		plc_falt_detected_hold[j] = pCrStat->fault_list.faults_detected_map[FAULT_TYPE::BASE][j];
		plc_falt_detected_trig_on[j] = 0;	//トリガON無し
		plc_falt_detected_trig_off[j] = 0;	//トリガOFF無し
	}

	for (int j = 0; j < N_PC_FAULT_BUF; j++) {
		pc_falt_detected_trig_on[j] = 0;	//トリガON無し
		pc_falt_detected_trig_off[j] = 0;	//トリガOFF無し
	}
	return;
}



/****************************************************************************/
/*   自動用ヘルパー関数											                    */
/****************************************************************************/

//位置によるモータ加減速度計算
double CCcEnv::cal_acc(int motion, double pos) {
	return 0.0;
}
 //位置に応じた加速度を計算(旋回、引込用）
double CCcEnv::cal_acc_hp(int motion, double R, double pos) {
	return 0.0;
}     
//加減速振れ振角計算rad
double CCcEnv::get_arad_acc(int motion, double R, double pos) {
	return 0.0;
} 
//振れ角振幅計算rad
double CCcEnv::get_arad_sway(int motion) {
	return 0.0;
}
//振れ角位相計算rad
double CCcEnv::get_phase_sway(int motion) {
	return 0.0;
}                       

double CCcEnv::cal_sway_amp2(int motion) {
	return 0.0;
}
double CCcEnv::cal_sway_amp(int motion) {
	return 0.0;
}
//停止距離計算
double CCcEnv::cal_dist4stop(int motion, bool is_abs_answer) {
	return 0.0;
} 
//目標位置までの距離
double CCcEnv::cal_dist4target(int motion, bool is_abs_answer) {
	return 0.0;
} 
// 0速チェック
bool CCcEnv::is_speed_0(int motion) {
	return 0.0;
}                            
 // 位置に応じた速度,加速度の比率　起伏のみ
double CCcEnv::cal_motion_retio(int imotion, double pos) {
	return 0.0;
}      
//振れ周期計算　ロープ長　指定
double CCcEnv::cal_T(double pos_hst, double R, int motion_id) {
	return 0.0;
}  
//振れ角周波数計算　ロープ長指定
double CCcEnv::cal_w(double pos_hst, double R, int motion_id) {
	return 0.0;
}
 //振れ角周波数の2乗計算　ロープ長指定
double CCcEnv::cal_w2(double pos_hst, double R, int motion_id) {
	return 0.0;
}
//ロープ長計算　巻き位置指定
double CCcEnv::cal_mhl(double pos_hst, double r) {
	return 0.0;
}               
 //最大速度計算
double CCcEnv::get_vmax(int motion) {
	return 0.0;
}                           

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CCcEnv::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[ENV_ID_MON1_STATIC_GPAD] = CreateWindowW(TEXT("STATIC"), st_mon1.text[ENV_ID_MON1_STATIC_GPAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].x, st_mon1.pt[ENV_ID_MON1_STATIC_GPAD].y,
			st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cx, st_mon1.sz[ENV_ID_MON1_STATIC_GPAD].cy,
			hWnd, (HMENU)(ENV_ID_MON1_CTRL_BASE + ENV_ID_MON1_STATIC_GPAD), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, ENV_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, ENV_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CCcEnv::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = ENV_ID_MON2_LABEL_SOCK; i <= ENV_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = ENV_ID_MON2_RADIO_RCV; i <= ENV_ID_MON2_RADIO_INFO; i++) {
			if (i == ENV_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = ENV_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(ENV_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT_PTR rtn = SetTimer(hWnd, ENV_ID_MON2_TIMER, ENV_PRM_MON2_TIMER_MS, NULL);
		break;

	}

	case WM_TIMER: {
	
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		st_mon2.hwnd_mon = NULL;
	}break;

	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CCcEnv::open_monitor_wnd(HWND h_parent_wnd, int id) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	ATOM fb = RegisterClassExW(&wcex);

	if (id == BC_ID_MON1) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("ENV_MON1");
		wcex.lpszClassName = TEXT("ENV_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("ENV_MON1"), TEXT("ENV_MON1"), WS_OVERLAPPEDWINDOW,
			ENV_MON1_WND_X, ENV_MON1_WND_Y, ENV_MON1_WND_W, ENV_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("ENV_MON2");
		wcex.lpszClassName = TEXT("ENV_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("ENV_MON2"), TEXT("ENV_MON2"), WS_OVERLAPPEDWINDOW,
			ENV_MON2_WND_X, ENV_MON2_WND_Y, ENV_MON2_WND_W, ENV_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		//show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CCcEnv::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CCcEnv::show_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
		st_mon1.is_monitor_active = true;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
		st_mon2.is_monitor_active = true;
	}
	else;
	return;
	return;
}
void CCcEnv::hide_monitor_wnd(int id) {
	if ((id == BC_ID_MON1) && (st_mon1.hwnd_mon != NULL)) {
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
		st_mon1.is_monitor_active = false;
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon != NULL)) {
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
		st_mon2.is_monitor_active = false;
	}
	else;
	return;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CCcEnv::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp);
			set_panel_tip_txt();
			set_item_chk_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}break;

		case IDC_TASK_ITEM_CHECK1: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1:
				set_item_chk_txt();
				break;
			default:break;
			}
		}break;
		case IDC_TASK_ITEM_CHECK3: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1:
				if (aux_sway_status == L_ON) {
					aux_sway_status = L_OFF;
					SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3), BM_SETCHECK, BST_UNCHECKED, 0L);
				}
				else {
					aux_sway_status = L_ON;
					SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3), BM_SETCHECK, BST_CHECKED, 0L);
				}
				break;
			default:break;
			}
		}break;

		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		case IDC_TASK_ITEM_CHECK6:
		{
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;

		case IDSET:
		{
			wstring wstr, wstr_tmp;

			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			msg2listview(wstr);

		}break;
		case IDRESET:
		{
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		}break;

		case IDC_TASK_MODE_RADIO0:
		{
			inf.mode_id = pEnvInf->app_common_param.app_mode = MODE_ENV_APP_PRODUCT;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = pEnvInf->app_common_param.app_mode = MODE_ENV_APP_EMURATOR;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = pEnvInf->app_common_param.app_mode = MODE_ENV_APP_SIMURATION;
		}break;

		case IDC_TASK_MON_CHECK1:
		{
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK1) == BST_CHECKED) {
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON1);
			}
			else {
				close_monitor_wnd(BC_ID_MON1);
			}
		}break;

		case IDC_TASK_MON_CHECK2: {
			if (IsDlgButtonChecked(hDlg, IDC_TASK_MON_CHECK2) == BST_CHECKED) {
				show_monitor_wnd(BC_ID_MON2);
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CCcEnv::msg2listview(wstring wstr) {

	const wchar_t* pwc; pwc = wstr.c_str();

	inf.hwnd_msglist = GetDlgItem(inf.hwnd_opepane, IDC_LIST1);
	LVITEM item;

	item.mask = LVIF_TEXT;
	item.pszText = (wchar_t*)pwc;								// テキスト
	item.iItem = inf.panel_msglist_count % BC_LISTVIEW_ROW_MAX;	// 番号
	item.iSubItem = 1;											// サブアイテムの番号
	ListView_SetItem(inf.hwnd_msglist, &item);

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	item.pszText = tbuf;   // テキスト
	item.iSubItem = 0;											// サブアイテムの番号
	ListView_SetItem(inf.hwnd_msglist, &item);

	//InvalidateRect(inf.hWnd_msgList, NULL, TRUE);
	inf.panel_msglist_count++;
	return;
}
void CCcEnv::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CCcEnv::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
	{
		wstr = L"1:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
		wstr = L"3:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM5), wstr.c_str());
		wstr = L"4:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM6), wstr.c_str());
		wstr = L"5:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM7), wstr.c_str());
		wstr = L"6:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM8), wstr.c_str());
	}break;
	}
	return;
}
//タブパネルのFunctionボタンのStaticテキストを設定
void CCcEnv::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"Aux");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CCcEnv::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"SLBRK");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"LANIO");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"SWAY");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"GTPOS");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:	
	case IDC_TASK_FUNC_RADIO4:
	case IDC_TASK_FUNC_RADIO5:
	case IDC_TASK_FUNC_RADIO6:
	default:
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
		break;
	}
	return;
}




