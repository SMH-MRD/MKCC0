#include "CCcSim.h"

#include "resource.h"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;

extern CCrane* pCrane;

//共有メモリ
static LPST_CC_ENV_INF		pEnv_Inf	= NULL;
static LPST_CC_CS_INF		pCS_Inf		= NULL;
static LPST_CC_PLC_IO		pPLC_IO		= NULL;
static LPST_CC_AGENT_INF	pAgent_Inf	= NULL;
static LPST_CC_OTE_INF		pOTE_Inf	= NULL;
static LPST_CC_SIM_INF		pSim_Inf	= NULL;

static PINT16				pOteCtrl = NULL;	//OTE操作入力信号ポインタ
static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

ST_SIM_MON1 CSim::st_mon1;
ST_SIM_MON2 CSim::st_mon2;

ST_CC_SIM_INF CSim::st_sim_inf;
ST_CC_SIM_WORK CSim::st_work; 

CSpec* CSim::pspec;

CSim::CSim() {

}
CSim::~CSim() {

}

HRESULT CSim::initialize(LPVOID lpParam) {

	//### 入出力用共有メモリ取得
	pAgent_Inf = (LPST_CC_AGENT_INF)pAgInfObj->get_pMap();
	pEnv_Inf = (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPLC_IO = (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pCS_Inf = (LPST_CC_CS_INF)pCsInfObj->get_pMap();
	pOTE_Inf = (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	pSim_Inf = (LPST_CC_SIM_INF)pSimuStatObj->get_pMap();

	pPlcWIf = &st_work.st_plc_w;						//PLC出力IF
	pPlcRIf = (ST_PLC_IO_RIF*)pPLC_IO->buf_io_read;		//PLC入力IF

	if (pCrane == NULL)return S_FALSE;
	pspec = pCrane->pSpec;

	CSim* pSimObj = (CSim*)lpParam;
	int code = 0;
	//計算パラメータセット
	//0.1%速度時のPG1秒間カウント値設定　100%rpm/60/1000 * モータ1回転のカウント数
	st_work.axis[ID_HOIST].PGcnt01v		= pCrane->pSpec->base_mh.Rpm_rated / 60000.0* pCrane->pSpec->base_mh.CntPgR;	//主巻
	st_work.axis[ID_BOOM_H].PGcnt01v	= pCrane->pSpec->base_bh.Rpm_rated / 60000.0 * pCrane->pSpec->base_bh.CntPgR;	//引込
	st_work.axis[ID_SLEW].PGcnt01v		= pCrane->pSpec->base_sl.Rpm_rated / 60000.0 * pCrane->pSpec->base_sl.CntPgR;	//旋回
	st_work.axis[ID_GANTRY].PGcnt01v	= pCrane->pSpec->base_gt.Rpm_rated / 60000.0 * pCrane->pSpec->base_gt.CntPgR;	//走行
	//0.1%速度時のアブソコーダカウント値設定 100%rpm/60/1000 * モータ1回転のカウント数
	st_work.axis[ID_HOIST].ABSOcnt01v = pCrane->pSpec->base_mh.Rpm_rated / 60000.0 * pCrane->pSpec->base_mh.CntAbsR / pCrane->pSpec->base_mh.Gear_ratio;	//主巻
	st_work.axis[ID_GANTRY].ABSOcnt01v = pCrane->pSpec->base_gt.Rpm_rated / 60000.0 * pCrane->pSpec->base_gt.CntAbsR / pCrane->pSpec->base_gt.Gear_ratio;	//走行
		
	//ドラム動作初期値
	init_drm_motion();
	//旋回360°回転PGカウント= ピニオン1回転カウント×TTB径/ピニオン径
	st_work.sl_cnt_pg360 = (INT32)(pspec->base_sl.CntPgR * pspec->base_sl.Ddrm1 / pspec->base_sl.Ddrm0);

	//operation panel　初期設定
	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);

	return S_OK;
}

HRESULT CSim::routine_work(void* pObj) {
	if (inf.total_act % 25 == 0) {
		wos.str(L""); wos << inf.status << L":" << inf.dt;
		//		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int CSim::input() {
	
	return S_OK;
}

int CSim::parse() {					//メイン処理
	set_sensor_fb();						// センサフィードバック設定
	calc_axis_motion();						// 軸動作計算
	calc_load_motion();						// 吊荷動作計算
	calc_plc_output();						// PLC出力計算
	return S_OK;
}
int CSim::output() {						//出力処理

	memcpy_s(pSim_Inf, sizeof(ST_CC_SIM_INF), &st_sim_inf, sizeof(ST_CC_SIM_INF));
	return S_OK;
}

HRESULT CSim::init_drm_motion() {	//ドラムパラメータ設定(巻取量,層数,速度,加速度）

	//ドラム回転数計算用パラメータ初期値設定
	st_sim_inf.hcount_mh		= 96450040;		//主巻PG　(R45,H70）
	st_sim_inf.hcount_bh		= 85167878;		//引込PG　(R45,H70）
	st_sim_inf.hcount_sl		= 15000000;		//旋回PG　0°
	st_sim_inf.absocoder_mh		= 50000;		//主巻アブソコーダ初期値3層開始位置(R45,H70）
	st_sim_inf.absocoder_gt		= 32595;		//走行アブソコーダ初期値50m
	return S_OK;
}  

HRESULT CSim::set_sensor_fb() {				//トルク指令,高速カウンタ,アブソコーダ,LS他
	//速度FB,トルク指令
	//主巻の速度FB,トルク指令設定
	if (pPLC_IO->stat_mh.inv_ref_dir != CODE_DIR_STP) {
		if( pPLC_IO->stat_mh.brk_fb == 0) {//ブレーキ閉
			st_sim_inf.trq_ref_mh = 600;	//30%トルク指令
			st_sim_inf.vfb_mh = 0;			//速度FBは0
		}
		else{//ブレーキ開
			st_sim_inf.trq_ref_mh = 2000;	//100%トルク指令
			//inv_ref(ベース100%で0.1%単位表現)
			st_sim_inf.vfb_mh = (INT16)pPLC_IO->stat_mh.inv_ref_v;
		}
	}
	else{
		st_sim_inf.trq_ref_mh = 0;	//停止時は速度0
		st_sim_inf.vfb_mh = 0;			//速度FBは0
	}
	
	//引込の速度FB,トルク指令設定
	if (pPLC_IO->stat_bh.inv_ref_dir != CODE_DIR_STP) {
		if (pPLC_IO->stat_bh.brk_fb == 0) {//ブレーキ閉
			st_sim_inf.trq_ref_bh = 600;	//30%トルク指令
			st_sim_inf.vfb_bh = 0;			//速度FBは0
		}
		else {//ブレーキ開
			st_sim_inf.trq_ref_bh = 2000;	//100%トルク指令
			//inv_ref(ベース100%で0.1%単位表現)
			st_sim_inf.vfb_bh = (INT16)pPLC_IO->stat_bh.inv_ref_v;
		}
	}
	else {
		st_sim_inf.trq_ref_bh	= 0;	//停止時は速度0
		st_sim_inf.vfb_bh		= 0;	//速度FBは0
	}
	
	//旋回の速度FB
	if (pPLC_IO->stat_sl.inv_ref_dir != CODE_DIR_STP) {
		if (pPLC_IO->stat_sl.brk_fb) //!!!旋回ブレーキは信号ONで閉
			st_sim_inf.vfb_sl = 0;			//速度FBは0
		else //ブレーキ開
			st_sim_inf.vfb_sl = (INT16)pPLC_IO->stat_sl.inv_ref_v;
	}
	else st_sim_inf.vfb_sl = 0;			//速度FBは0
	
	//走行の速度FB,トルク指令設定
	if (pPLC_IO->stat_gt.inv_ref_dir != CODE_DIR_STP) {
		if (pPLC_IO->stat_gt.brk_fb==0) 
			st_sim_inf.vfb_gt = 0;			//速度FBは0
		else //ブレーキ開
			st_sim_inf.vfb_gt = (INT16)pPLC_IO->stat_gt.inv_ref_v;
	}
	else st_sim_inf.vfb_gt = 0;			//速度FBは0

	//高速カウンタ,アブソコーダフィードバック設定
	//crane_stat.nd[].vは±0.1%単位の速度値
	if (pPLC_IO->stat_mh.brk_fb) {
		st_sim_inf.hcount_mh	+= (INT32)(pEnv_Inf->crane_stat.nd[ID_HOIST].v	* inf.dt * st_work.axis[ID_HOIST].PGcnt01v ) ;	//主巻PGフィードバック
		st_sim_inf.absocoder_mh	+= (INT32)(pEnv_Inf->crane_stat.nd[ID_HOIST].v	* inf.dt * st_work.axis[ID_HOIST].ABSOcnt01v);
	}
	if(pPLC_IO->stat_bh.brk_fb)//!!!引込は正転でPGはマイナスカウント
		st_sim_inf.hcount_bh	-= (INT32)(pEnv_Inf->crane_stat.nd[ID_BOOM_H].v * inf.dt * st_work.axis[ID_BOOM_H].PGcnt01v);
	
	//!!!旋回ブレーキは信号OFFで開
	if((!pPLC_IO->stat_sl.brk_fb)&&((pEnv_Inf->crane_stat.nd[ID_SLEW].v<-5)||(pEnv_Inf->crane_stat.nd[ID_SLEW].v>5)))
		st_sim_inf.hcount_sl	+= (INT32)(pEnv_Inf->crane_stat.nd[ID_SLEW].v	* inf.dt * st_work.axis[ID_SLEW].PGcnt01v);
	//プリセット
	if (st_sim_inf.hcount_sl > pspec->base_sl.CntPgSet0 + st_work.sl_cnt_pg360) 
		st_sim_inf.hcount_sl = (INT32)pspec->base_sl.CntPgSet0;
	if (st_sim_inf.hcount_sl < pspec->base_sl.CntPgSet0 - st_work.sl_cnt_pg360) 
		st_sim_inf.hcount_sl = (INT32)pspec->base_sl.CntPgSet0;

	
	if(pPLC_IO->stat_gt.brk_fb)
		st_sim_inf.absocoder_gt += (INT32)(pEnv_Inf->crane_stat.nd[ID_GANTRY].v * inf.dt * st_work.axis[ID_GANTRY].ABSOcnt01v);	//他は未使用なので0

	
	//荷重
	st_work.weight_mh=pspec->st_struct.Whook;								//フック質量
	st_sim_inf.mlim_weight_AI = (INT16)(st_work.weight_mh/80000.0 * 1600);	//フック質量AI入力計算値(kgf→AD変換値 80t->1600(2V))
	
	st_sim_inf.mlim_r_AI = (INT16)(pEnv_Inf->crane_stat.r.p *10.0);	//モーメントリミッタ半径AI入力計算値(0(30)-50(80)m→AD変換値 0-1600(2V))
	return S_OK;
}            

HRESULT CSim::calc_axis_motion() {			//軸荷動作計算
	return S_OK;
}         
HRESULT CSim::calc_load_motion() {			//吊荷動作計算
	return S_OK;
}          
HRESULT CSim::calc_plc_output() {			//軸状態出力（PLC IO用）

	return S_OK;
}        

int CSim::close() {

	return 0;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CSim::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[SIM_ID_MON1_STATIC_INF0] = CreateWindowW(TEXT("STATIC"), st_mon1.text[SIM_ID_MON1_STATIC_INF0], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[SIM_ID_MON1_STATIC_INF0].x, st_mon1.pt[SIM_ID_MON1_STATIC_INF0].y,
			st_mon1.sz[SIM_ID_MON1_STATIC_INF0].cx, st_mon1.sz[SIM_ID_MON1_STATIC_INF0].cy,
			hWnd, (HMENU)(SIM_ID_MON1_CTRL_BASE + SIM_ID_MON1_STATIC_INF0), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, SIM_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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

		st_mon1.wo.str(L""); 

		st_mon1.wo	<< L"[INV FWD/RWD] mh:" << pPLC_IO->stat_mh.inv_ref_dir
			<< L" bh:" << pPLC_IO->stat_bh.inv_ref_dir
			<< L" sl:" << pPLC_IO->stat_sl.inv_ref_dir
			<< L" gt:" << pPLC_IO->stat_gt.inv_ref_dir
			<< L" \n";
		st_mon1.wo << L"[INV Ref V] mh:" << pPLC_IO->stat_mh.inv_ref_v
			<< L" bh:" << pPLC_IO->stat_bh.inv_ref_v
			<< L" sl:" << pPLC_IO->stat_sl.inv_ref_v
			<< L" gt:" << pPLC_IO->stat_gt.inv_ref_v
			<< L" \n";

		SetWindowText(st_mon1.hctrl[SIM_ID_MON1_STATIC_INF0], st_mon1.wo.str().c_str());

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, SIM_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CSim::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加

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

HWND CSim::open_monitor_wnd(HWND h_parent_wnd, int id) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	ATOM fb = RegisterClassExW(&wcex);

	if ((id == BC_ID_MON1) &&(st_mon1.hwnd_mon == NULL)){
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("SIM_MON1");
		wcex.lpszClassName = TEXT("SIM_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);
		st_mon1.hwnd_mon = CreateWindowW(TEXT("SIM_MON1"), TEXT("SIM_MON1"), WS_OVERLAPPEDWINDOW,
			SIM_MON1_WND_X, SIM_MON1_WND_Y, SIM_MON1_WND_W, SIM_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
	}
	else if ((id == BC_ID_MON2) && (st_mon2.hwnd_mon == NULL)) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("SIM_MON2");
		wcex.lpszClassName = TEXT("SIM_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("SIM_MON2"), TEXT("SIM_MON2"), WS_OVERLAPPEDWINDOW,
			SIM_MON2_WND_X, SIM_MON2_WND_Y, SIM_MON2_WND_W, SIM_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CSim::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1) {
		DestroyWindow(st_mon1.hwnd_mon);
		st_mon1.hwnd_mon = NULL;
	}
	else if (id == BC_ID_MON2) {
		DestroyWindow(st_mon2.hwnd_mon);
		st_mon2.hwnd_mon = NULL;
	}
	else;
	return;
}
void CSim::show_monitor_wnd(int id) {
	if (id == BC_ID_MON1) {
		ShowWindow(st_mon1.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon1.hwnd_mon);
	}
	else if (id == BC_ID_MON2) {
		ShowWindow(st_mon2.hwnd_mon, SW_SHOW);
		UpdateWindow(st_mon2.hwnd_mon);
	}
	else;
	return;
}
void CSim::hide_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		ShowWindow(st_mon1.hwnd_mon, SW_HIDE);
	else if (id == BC_ID_MON2)
		ShowWindow(st_mon2.hwnd_mon, SW_HIDE);
	else;
	return;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CSim::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}

		}break;
		case IDC_TASK_ITEM_CHECK2:
		case IDC_TASK_ITEM_CHECK3:
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
			inf.mode_id = BC_ID_MODE0;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = BC_ID_MODE1;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = BC_ID_MODE2;
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
				open_monitor_wnd(inf.hwnd_parent, BC_ID_MON2);
			}
			else {
				close_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CSim::msg2listview(wstring wstr) {

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
void CSim::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CSim::set_panel_tip_txt() {
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
void CSim::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CSim::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO4: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
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



