#include "CMCProtocol.h"
#include "COteCS.h"
#include "resource.h"
#include "CGamePad.h"
#include "CCrane.h"

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;
extern ST_DEVICE_CODE g_my_code; //端末コード

extern CCrane* pCrane;


//ソケット
static CMCProtocol* pMCSock;				//MCプロトコルオブジェクトポインタ

//クラススタティックメンバ
ST_OTE_CS_MON1 COteCS::st_mon1;
ST_OTE_CS_MON2 COteCS::st_mon2;

ST_OTE_CS_INF COteCS::st_work;
ST_OTE_CS_OBJ COteCS::st_obj;

//共有メモリ参照用定義

//共有メモリ
static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUi;
static LPST_OTE_CC_IF	pOteCCInf;
static LPST_OTE_CS_INF	pOteCsInf;

static CGamePad* pPad = NULL;

static LONG rcv_count_plc_r = 0, snd_count_plc_r = 0, rcv_errcount_plc_r = 0;
static LONG rcv_count_plc_w = 0, snd_count_plc_w = 0, rcv_errcount_plc_w = 0;
static LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;  //システムカウント
static LARGE_INTEGER frequency;				//システム周波数
static LONGLONG res_delay_max_w, res_delay_max_r;	//PLC応答時間

static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

COteCS::COteCS() {
	st_obj.remote_pb.set(0); st_obj.remote_mode.set(0); st_obj.game_pad_pb.set(0);
}
COteCS::~COteCS() {
}

HRESULT COteCS::initialize(LPVOID lpParam) {

	HRESULT hr = S_OK;

	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_CS_INF);
	if (OK_SHMEM != pOteCsInfObj->create_smem(SMEM_OTE_CS_INF_NAME, out_size, MUTEX_OTE_CS_INF_NAME)) {
		err |= SMEM_NG_OTE_CS_INF;
	}
	set_outbuf(pOteCsInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pOteCCInf = (LPST_OTE_CC_IF)pOteCcInfObj->get_pMap();
	pOteEnvInf = (LPST_OTE_ENV_INF)(pOteEnvInfObj->get_pMap());
	pOteCsInf = (LPST_OTE_CS_INF)(pOteCsInfObj->get_pMap());
	pOteUi = (LPST_OTE_UI)pOteUiObj->get_pMap();

	if ((pOteEnvInf == NULL) || (pOteCsInf == NULL) || (pOteUi == NULL) || (pOteCCInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IFウィンドウ  MON2 OPEN PLC通信
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	Sleep(100);
	wos.str(L"");//初期化
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Initialize : MON NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		pMCSock = new CMCProtocol(ID_SOCK_MC_OTE_CS);
		if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_OTE) != S_OK) {
			wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
			wos << L"Err :" << pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		else {
			wos << L"MCProtocol Init OK"; msg2listview(wos.str());
		}
	}
	//### GamePadインスタンス
	pPad = new CGamePad();
	pPad->set_id(0);

	//###  オペレーションパネル設定
	//Function mode RADIO1
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);
	//モニタウィンドウテキスト	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"MKCC IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ２ CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	st_obj.pad_mh=new CPadNotch(pCrane->get_base_mh(), ID_HOIST);
	st_obj.pad_bh=new CPadNotch(pCrane->get_base_bh(), ID_BOOM_H);
	st_obj.pad_sl=new CPadNotch(pCrane->get_base_sl(), ID_SLEW);
	st_obj.pad_gt=new CPadNotch(pCrane->get_base_gt(), ID_GANTRY);
	st_obj.pad_ah=new CPadNotch(pCrane->get_base_ah(), ID_AHOIST);

	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	return hr;
}

HRESULT COteCS::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

int COteCS::input() {
	//	ゲームパッド取り込み
	if ((pPad != NULL)&& st_work.st_body.game_pad_mode ){
		if (pPad->PollController(pPad->controllerId)) {//GamePad状態取り込み
			st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
		}
		pOteCsInf->gpad_in.syukan_on	= pPad->chk_on(st_obj.syukan_on.set(pPad->get_start()));
		pOteCsInf->gpad_in.syukan_off	= pPad->chk_on(st_obj.syukan_off.set(pPad->get_Y()));
		pOteCsInf->gpad_in.remote		= pPad->chk_on(st_obj.remote_pb.set(pPad->get_A()));
		pOteCsInf->gpad_in.estop		= pPad->chk_on(st_obj.estop.set(pPad->get_B()));
		pOteCsInf->gpad_in.f_reset		= pPad->chk_on(st_obj.f_reset.set(pPad->get_back()));
		pOteCsInf->gpad_in.bypass		= pPad->chk_on(st_obj.bypass.set(pPad->get_X()));
		pOteCsInf->gpad_in.kidou_r		= pPad->chk_on(st_obj.kidou_r.set(pPad->get_thumbr()));
		pOteCsInf->gpad_in.kidou_l		= pPad->chk_on(st_obj.kidou_l.set(pPad->get_thumbl()));
		pOteCsInf->gpad_in.pan_l		= pPad->chk_on(st_obj.pan_l.set(pPad->get_left()));
		pOteCsInf->gpad_in.pan_r		= pPad->chk_on(st_obj.pan_r.set(pPad->get_right()));
		pOteCsInf->gpad_in.tilt_u		= pPad->chk_on(st_obj.tilt_u.set(pPad->get_up()));
		pOteCsInf->gpad_in.tilt_d		= pPad->chk_on(st_obj.tilt_d.set(pPad->get_down()));
		pOteCsInf->gpad_in.zoom_f		= pPad->chk_on(st_obj.zoom_f.set(pPad->get_shoulderr()));
		pOteCsInf->gpad_in.zoom_n		= pPad->chk_on(st_obj.zoom_n.set(pPad->get_shoulderl()));

		pOteCsInf->gpad_in.trig_l		= st_obj.trig_l.set(pPad->get_trig_L());
		pOteCsInf->gpad_in.trig_r		= st_obj.trig_r.set(pPad->get_trig_R());

		//GamePadのアナログ値をValueオブジェクトにセット⇒ノッチ信号を共有メモリにセット
		st_obj.pad_mh->set(pPad->get_RY());pOteCsInf->gpad_in.pad_mh = st_obj.pad_mh->get_notch();
		st_obj.pad_bh->set(pPad->get_LY());pOteCsInf->gpad_in.pad_bh = st_obj.pad_bh->get_notch();
		st_obj.pad_sl->set(pPad->get_LX());pOteCsInf->gpad_in.pad_sl = st_obj.pad_sl->get_notch();
		st_obj.pad_gt->set(pPad->get_RX());pOteCsInf->gpad_in.pad_gt = st_obj.pad_gt->get_notch();
	}

	// 操作パネル入力取り込み
	st_work.st_body.ctrl_ope[OTE_PNL_CTRLS::syukan_on] //SCAD PBの状態 or GamePad入力(↑でsetメソッド実行している）をセット 
		= st_obj.syukan_on.set(pOteUi->ctrl_stat[OTE_PNL_CTRLS::syukan_on] | st_obj.syukan_on.get());
	st_work.st_body.ctrl_ope[OTE_PNL_CTRLS::syukan_off]
		= st_obj.syukan_off.set(pOteUi->ctrl_stat[OTE_PNL_CTRLS::syukan_off] | st_obj.syukan_off.get());
	st_work.st_body.ctrl_ope[OTE_PNL_CTRLS::estop]
		= st_obj.estop.set(pOteUi->ctrl_stat[OTE_PNL_CTRLS::estop] | st_obj.estop.get());
	st_work.st_body.ctrl_ope[OTE_PNL_CTRLS::fault_reset]
		= st_obj.f_reset.set(pOteUi->ctrl_stat[OTE_PNL_CTRLS::fault_reset] | st_obj.f_reset.get());
	
	
	;
	st_obj.bypass;
	st_obj.kidou_r;
	st_obj.kidou_l;
	st_obj.pan_l;
	st_obj.pan_r;
	st_obj.tilt_u;
	st_obj.tilt_d;
	st_obj.zoom_f;
	st_obj.zoom_n;

	return S_OK;
}
int COteCS::parse() {           //メイン処理

	//リモート操作有効化設定
	if (CODE_TRIG_ON == st_obj.remote_pb.chk_trig(pOteUi->ctrl_stat[OTE_PNL_CTRLS::remote])){//
		if (st_work.st_body.remote == CODE_PNL_COM_SELECTED) 
			st_work.st_body.remote = CODE_PNL_COM_OFF;
		else if (st_work.st_body.remote == CODE_PNL_COM_ACTIVE) 
			st_work.st_body.remote = CODE_PNL_COM_OFF;
		else													
			st_work.st_body.remote = CODE_PNL_COM_SELECTED;
	}
	if ((st_work.st_body.remote == CODE_PNL_COM_SELECTED) &&(pOteCCInf->cc_active_ote_id == g_my_code.serial_no))
		st_work.st_body.remote = CODE_PNL_COM_ACTIVE;

	//GamePadmoモード設定
	if (CODE_TRIG_ON == st_obj.game_pad_pb.chk_trig(pOteUi->ctrl_stat[OTE_PNL_CTRLS::game_pad])) {
		if (st_work.st_body.game_pad_mode == CODE_PNL_COM_ACTIVE)
			st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
		else
			st_work.st_body.game_pad_mode = CODE_PNL_COM_ACTIVE;
	}
	//操作卓タイプ変更
	if (st_obj.ote_type.chk_trig(pOteUi->ctrl_stat[OTE_PNL_CTRLS::ote_type])) {
		st_work.st_body.ote_type = st_obj.ote_type.get();
	}

	//ノッチ指令値　!!！250526　とりあえずGpad入力を出力（直接送信バッファにセット）
	st_work.st_body.axis[ID_AXIS::mh].notch_ref = st_obj.pad_mh->get_notch();
	st_work.st_body.axis[ID_AXIS::bh].notch_ref = st_obj.pad_bh->get_notch();
	st_work.st_body.axis[ID_AXIS::sl].notch_ref = st_obj.pad_sl->get_notch();
	st_work.st_body.axis[ID_AXIS::gt].notch_ref = st_obj.pad_gt->get_notch();
	st_work.st_body.axis[ID_AXIS::ah].notch_ref = st_obj.pad_ah->get_notch();

	return STAT_OK;
}

int COteCS::output() {          
		//送信バッファ内容を共有メモリにコピー
		memcpy_s(&pOteCsInf->st_body, sizeof(ST_OTE_U_BODY), &st_work.st_body, sizeof(ST_OTE_U_BODY));
	return STAT_OK;
}

int COteCS::close() {
	delete pPad;
	delete st_obj.pad_mh;
	delete st_obj.pad_bh;
	delete st_obj.pad_sl;
	delete st_obj.pad_gt;
	delete st_obj.pad_ah;
	return 0;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;


LRESULT CALLBACK COteCS::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[OTE_CS_ID_MON1_STATIC_MSG] = CreateWindowW(TEXT("STATIC"), st_mon1.text[OTE_CS_ID_MON1_STATIC_MSG], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[OTE_CS_ID_MON1_STATIC_MSG].x, st_mon1.pt[OTE_CS_ID_MON1_STATIC_MSG].y,
			st_mon1.sz[OTE_CS_ID_MON1_STATIC_MSG].cx, st_mon1.sz[OTE_CS_ID_MON1_STATIC_MSG].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_STATIC_MSG), hInst, NULL);

		st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT] = CreateWindowW(TEXT("BUTTON"), st_mon1.text[OTE_CS_ID_MON1_CB_VIB_ACT], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
			st_mon1.pt[OTE_CS_ID_MON1_CB_VIB_ACT].x, st_mon1.pt[OTE_CS_ID_MON1_CB_VIB_ACT].y, st_mon1.sz[OTE_CS_ID_MON1_CB_VIB_ACT].cx, st_mon1.sz[OTE_CS_ID_MON1_CB_VIB_ACT].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_CB_VIB_ACT), hInst, NULL);
			
		// スライダー作成（左モーター）
		st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L] = CreateWindowEx(0, TRACKBAR_CLASS, st_mon1.text[OTE_CS_ID_MON1_SLIDER_VIB_L],
			WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
			st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_L].x, st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_L].y,
			st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_L].cx, st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_L].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_SLIDER_VIB_L), hInst, nullptr);
		
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_SETPOS, TRUE, 0);

		// スライダー作成（右モーター）
		st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R] = CreateWindowEx(0, TRACKBAR_CLASS, st_mon1.text[OTE_CS_ID_MON1_SLIDER_VIB_R],
			WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
			st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_R].x, st_mon1.pt[OTE_CS_ID_MON1_SLIDER_VIB_R].y,
			st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_R].cx, st_mon1.sz[OTE_CS_ID_MON1_SLIDER_VIB_R].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_SLIDER_VIB_R), hInst, nullptr);

		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_SETPOS, TRUE, 0);
		
		//表示更新用タイマー
		SetTimer(hWnd, OTE_CS_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case OTE_CS_ID_MON1_CTRL_BASE + OTE_CS_ID_MON1_CB_VIB_ACT: {
			if (pPad == NULL)break;
			if(BST_UNCHECKED== SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT], BM_GETCHECK, 0, 0))
				pPad->VibrateController(pPad->controllerId, 0, 0);
			break;
		}
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {

		if (pPad != NULL) {
			monwos.str(L"");
			if(pPad->error_code)
			monwos << "Controller " << pPad->controllerId << " is  Error:" << pPad->error_code <<"\n" ;
			else
				monwos << "Controller " << pPad->controllerId << " is connected\n";
			monwos << "  Buttons: " << std::hex << pPad->state.Gamepad.wButtons << std::dec << "\n";
			monwos << "  Left Thumb: (" << pPad->state.Gamepad.sThumbLX << ", " << pPad->state.Gamepad.sThumbLY << ")\n";
			monwos << "  Right Thumb: (" << pPad->state.Gamepad.sThumbRX << ", " << pPad->state.Gamepad.sThumbRY << ")\n";
			monwos << "  Triggers: L=" << (int)pPad->state.Gamepad.bLeftTrigger << ", R=" << (int)pPad->state.Gamepad.bRightTrigger << "\n\n";

			monwos << "PAD Notch MH:" << st_obj.pad_mh->get_notch() << " BH:" << st_obj.pad_bh->get_notch();
			monwos << " SL:" << st_obj.pad_sl->get_notch() << " GT:" << st_obj.pad_gt->get_notch();

		}
		SetWindowText(st_mon1.hctrl[OTE_CS_ID_MON1_STATIC_MSG], monwos.str().c_str());
#if 0
			// ボタン A で振動開始（中くらい）
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				VibrateController(controllerId, 30000, 30000);
			}

			// ボタン B で振動停止
			if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
				VibrateController(controllerId, 0, 0);
			}
		}

#endif


	}break;
	case WM_HSCROLL: {
		//ゲームパッドのバイブチェック
		if (pPad == NULL)break;
		if (BST_CHECKED == SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_CB_VIB_ACT], BM_GETCHECK, 0, 0)) {
			if ((HWND)lp == st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L] || (HWND)lp == st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R]) {
				int left = SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_L], TBM_GETPOS, 0, 0);
				int right = SendMessage(st_mon1.hctrl[OTE_CS_ID_MON1_SLIDER_VIB_R], TBM_GETPOS, 0, 0);
			
				left *= 65535 / 100; right *= 65535 / 100;
				
				pPad->VibrateController(pPad->controllerId, (WORD)left, (WORD)right);
			}
		}
		else {
			pPad->VibrateController(pPad->controllerId, 0, 0);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_CS_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static bool is_write_req_turn = false;//書き込み要求送信の順番でtrue

LRESULT CALLBACK COteCS::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = OTE_CS_ID_MON2_STATIC_MSG; i <= OTE_CS_ID_MON2_STATIC_RES_W; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//PB
		for (int i = OTE_CS_ID_MON2_PB_R_BLOCK_SEL; i <= OTE_CS_ID_MON2_PB_DISP_DEC_SEL; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_CS_ID_MON2_TIMER, OTE_CS_PRM_MON2_TIMER_MS, NULL);

		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());

		st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
		monwos.str(L""); monwos << L"表示中";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
		monwos.str(L""); monwos << L"16進";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
	}break;
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		if (is_write_req_turn) {//書き込み要求送信
			st_mon2.wo_req_w.str(L"");
			//3Eフォーマット Dデバイス書き込み要求送信
			if (pMCSock->send_write_req_D_3E(pOteCsInf->buf_io_write) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {

				st_mon2.wo_req_w << L"Sw>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_w.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_w.serial
					<< L"#NW:" << pMCSock->mc_req_msg_w.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_w.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_w.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_w.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_w.len
					<< L"#tm:" << pMCSock->mc_req_msg_w.timer
					<< L"#com:" << pMCSock->mc_req_msg_w.com
					<< L"#scom:" << pMCSock->mc_req_msg_w.scom << L"\n"
					<< L"#d_no:" << pMCSock->mc_req_msg_w.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_w.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_w.n_device;

					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_REQ_W], st_mon2.wo_req_w.str().c_str());
			}

			QueryPerformanceCounter(&start_count_w);  // 書き込み要求送信時カウント値取り込み
			is_write_req_turn = false;
		}
		else {
			st_mon2.wo_req_r.str(L"");
			//読み出し要求送信
			if (pMCSock->send_read_req_D_3E() != S_OK) {
				st_mon2.wo_req_r << L"ERROR : send_read_req_D_3E()";
			}
			else snd_count_plc_r++;

			if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				st_mon2.wo_req_r << L"Sr>>"
					<< L"#sub:" << std::hex << pMCSock->mc_req_msg_r.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_r.serial
					<< L"#NW:" << pMCSock->mc_req_msg_r.nNW
					<< L"#PC:" << pMCSock->mc_req_msg_r.nPC
					<< L"#UIO:" << pMCSock->mc_req_msg_r.nUIO
					<< L"#Ucd:" << pMCSock->mc_req_msg_r.nUcode
					<< L"#len:" << pMCSock->mc_req_msg_r.len
					<< L"#tm:" << pMCSock->mc_req_msg_r.timer
					<< L"#com:" << pMCSock->mc_req_msg_r.com
					<< L"#scom:" << pMCSock->mc_req_msg_r.scom
					<< L"#d_no:" << pMCSock->mc_req_msg_r.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_r.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_r.n_device;
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_REQ_R], st_mon2.wo_req_r.str().c_str());
			}

			QueryPerformanceCounter(&start_count_r);  // 書き込み要求送信時カウント値取り込み
			is_write_req_turn = true;
		}

		//共通表示
		if (st_mon2.is_monitor_active) {
			//カウンタ表示
			monwos.str(L""); monwos << L"RCV:R " << rcv_count_plc_r
				<< L"  W " << rcv_count_plc_w
				<< L"    SND:R " << snd_count_plc_r
				<< L"  W " << snd_count_plc_w
				<< L"    ERR:R " << rcv_errcount_plc_r
				<< L"  W " << rcv_errcount_plc_w
				<< L"    遅延μs:R " << res_delay_max_r
				<< L"  W " << res_delay_max_w;

			SetWindowText(hWnd, monwos.str().c_str());

			SOCKADDR_IN	addr;
			if (pMCSock != NULL) {
				addr = pMCSock->get_addrin_rcv(); monwos.str(L"");
				monwos << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_snd();
				monwos << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMCSock->get_addrin_from();
				monwos << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_INF], monwos.str().c_str());
			}
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - OTE_CS_ID_MON2_CTRL_BASE)
		{
		case OTE_CS_ID_MON2_PB_R_BLOCK_SEL: {

			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());
		}break;
		case OTE_CS_ID_MON2_PB_W_BLOCK_SEL: {

			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());
		}break;
		case OTE_CS_ID_MON2_PB_MSG_DISP_SEL: {
			if (st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) {
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_OFF;
				monwos.str(L""); monwos << L"非表示";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"-:";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
			else {
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
				monwos.str(L""); monwos << L"表示中";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"16進";
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
		}break;
		case OTE_CS_ID_MON2_PB_DISP_DEC_SEL: {
			if (st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) {
				if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX) {
					st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_DEC;
					monwos.str(L""); monwos << L"10進";
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
				else {
					st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
					monwos.str(L""); monwos << L"16進";
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
			}
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case ID_SOCK_MC_OTE_CS://MCソケット受信イベント
	{
		if (pMCSock == NULL)break;
		int nEvent = WSAGETSELECTEVENT(lp);
		st_mon2.wo_res_r.str(L"");
		st_mon2.wo_res_w.str(L"");
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = pMCSock->rcv_msg_3E(pOteCsInf->buf_io_read);
			if (nRtn == MC_RES_READ) {//読み出し応答
				rcv_count_plc_r++;
				if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_r << L"Rr>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_r.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_r.serial
						<< L"#NW:" << pMCSock->mc_res_msg_r.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_r.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_r.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_r.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_r.len
						<< L"#end:" << pMCSock->mc_res_msg_r.endcode << L"\n";


					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());

					QueryPerformanceCounter(&end_count_r);    // 現在のカウント数
					LONGLONG lspan = (end_count_r.QuadPart - start_count_r.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
					if (res_delay_max_r < lspan) res_delay_max_r = lspan;
					if (rcv_count_plc_r % 40 == 0) {
						res_delay_max_r = 0;
					}
				}
			}
			else if (nRtn == MC_RES_WRITE) {

				rcv_count_plc_w++;
				if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {

					st_mon2.wo_res_w << L"Rw>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_w.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_w.serial
						<< L"#NW:" << pMCSock->mc_res_msg_w.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_w.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_w.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_w.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_w.len
						<< L"#end:" << pMCSock->mc_res_msg_w.endcode;
				}
				SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());

				QueryPerformanceCounter(&end_count_w);    // 現在のカウント数
				LONGLONG lspan = (end_count_w.QuadPart - start_count_w.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
				if (res_delay_max_w < lspan) res_delay_max_w = lspan;
				if (rcv_count_plc_w % 40 == 0) {
					res_delay_max_w = 0;
				}
			}
			else {
				int err_code = WSAGetLastError();
				if (is_write_req_turn) {
					st_mon2.wo_res_r << L"PLC READ RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_r++;
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}
				else {
					st_mon2.wo_res_w << L"PLC WRITE RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_w++;
					SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
				}
			}
		}break;
		default: break;
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

HWND COteCS::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("CC_AG_MON1");
		wcex.lpszClassName = TEXT("CC_AG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("CC_AG_MON1"), TEXT("CC_AG_MON1"), WS_OVERLAPPEDWINDOW,
			OTE_CS_MON1_WND_X, OTE_CS_MON1_WND_Y, OTE_CS_MON1_WND_W, OTE_CS_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);

		monwos.str(L"");
		if (st_mon1.hwnd_mon != NULL) monwos << L"Succeed : MON1 open";
		else                          monwos << L"!! Failed : MON1 open";
		msg2listview(monwos.str());

		return st_mon1.hwnd_mon;
	}
	else if (id == BC_ID_MON2) {//通信用ウィンドウ
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon2Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("CC_AG_MON2");
		wcex.lpszClassName = TEXT("CC_AG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("CC_AG_MON2"), TEXT("CC_AG_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_CS_MON2_WND_X, OTE_CS_MON2_WND_Y, OTE_CS_MON2_WND_W, OTE_CS_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

		//show_monitor_wnd(id);MON2はオープン時表示しない
		wos.str(L"");
		if (st_mon2.hwnd_mon != NULL) wos << L"Succeed : MON2 open";
		else                          wos << L"!! Failed : MON2 open";
		msg2listview(wos.str());

		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void COteCS::close_monitor_wnd(int id) {
	wos.str(L"");
	if (id == BC_ID_MON1) {
		DestroyWindow(st_mon1.hwnd_mon);
		wos << L"MON1 closed";
	}
	else if (id == BC_ID_MON2) {
		DestroyWindow(st_mon2.hwnd_mon);
		wos << L"MON2 closed";
	}
	else;
	msg2listview(wos.str());
	return;
}
void COteCS::show_monitor_wnd(int id) {
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
}
void COteCS::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK COteCS::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;

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
				show_monitor_wnd(BC_ID_MON2);
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void COteCS::msg2listview(wstring wstr) {

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
void COteCS::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void COteCS::set_panel_tip_txt() {
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
void COteCS::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteCS::set_item_chk_txt() {
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









