
#include "CAuxAgent.h"
#include "resource.h"
#include "CSHAREDMEM.H"
#include "SmemAux.H"
#include "CMCProtocol.h"

LPST_AUXEQ CAuxAgent::pst_work;
ST_AUXAG_MON1 CAuxAgent::st_mon1;
ST_AUXAG_MON2 CAuxAgent::st_mon2;

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pCsInfObj;

//ソケット
static CMCProtocol* pMCSock;				//MCプロトコルオブジェクトポインタ

//共有メモリ
static LPST_AUX_ENV_INF		pEnv_Inf = NULL;
static LPST_AUX_CS_INF		pCS_Inf = NULL;
static LPST_AUX_AGENT_INF	pAgent_Inf = NULL;

static PINT16				pOteCtrl = NULL;	//OTE操作入力信号ポインタ

static LONG rcv_count_plc_r = 0, snd_count_plc_r = 0, rcv_errcount_plc_r = 0;
static LONG rcv_count_plc_w = 0, snd_count_plc_w = 0, rcv_errcount_plc_w = 0;
static LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;  //システムカウント
static LARGE_INTEGER frequency;				//システム周波数
static LONGLONG res_delay_max_w, res_delay_max_r;	//PLC応答時間

HRESULT CAuxAgent::initialize(LPVOID lpParam){

	HRESULT hr = S_OK;
	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_AGENT_INF);
	set_outbuf(pAgentInfObj->get_pMap());

	//### 入力用共有メモリ取得
	pAgent_Inf = (LPST_AUX_AGENT_INF)pAgentInfObj->get_pMap();
	pEnv_Inf = (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pCS_Inf = (LPST_AUX_CS_INF)pCsInfObj->get_pMap();

	//### IFウィンドウOPEN
	if (st_mon2.hwnd_mon == NULL) {
		WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
		LPARAM lp = BC_ID_MON2;
		SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
		Sleep(1000);
	}
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}

	//### 初期化
	wos.str(L"");//初期化
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Initialize : MON NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		pMCSock = new CMCProtocol(ID_SOCK_MC_AUX_BRK);
		if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_SLBRK) != S_OK) {
			wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
			wos << L"Err :" << pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		else {
			wos << L"MCProtocol Init OK"; msg2listview(wos.str());
		}
	}

	//モニタウィンドウテキスト	
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"SL BRK");
	
	inf.panel_func_id = IDC_TASK_FUNC_RADIO1;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
	for (int i = 1; i < 6; i++)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
	//モード設定0
	inf.mode_id = BC_ID_MODE0;
	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0), BM_SETCHECK, BST_CHECKED, 0L);

	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

#if 0

	CAuxAgent* pTiltObj = (CAuxAgent*)lpParam;
	pst_work = &(pTiltObj->st_work);

	int code = 0;

	wos.str(L"");
	if ((code = LALanioInit()) == 0) {
		wos<<L"LANIO Init NG : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return S_FALSE;
	}
	else{
		wos <<L"LANIO Init OK : Code "<< code;
	}
	msg2listview(wos.str());

	wos.str(L"");

	if ((st_work.laniocount = LALanioSearch(st_work.timeout)) < 0) {
		wos << L"LANIO Search ERROR : Code "<< LALanioGetLastError();
		msg2listview(wos.str());
		return S_FALSE;
	}
	else if(st_work.laniocount == 0){
	//	LALanioConnectDirect(L"192,168,100,30", 1000);
		wos << L"LANIO Search CANNOT FIND";
		msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		//IPアドレス確認
		wos << L"LANIO Search OK : Count " << st_work.laniocount;
		msg2listview(wos.str());

		for (int i = 0; i < st_work.laniocount; i++) {

			//ID,モデル確認
			int error = LALanioGetId(0, &st_work.lanio_id[i]);
			error = LALanioGetModel(i, &st_work.lanio_model[i]);
			wos.str(L"");
			if ((0 < st_work.lanio_model[i]) && (LANIO_N_MODEL > st_work.lanio_model[i]))
				wos << L"ID : " << st_work.lanio_id[i] << L"    MODEL : " << st_work.model_text[st_work.lanio_model[i]];
			else
				wos << L"ID : " << st_work.lanio_id[i] << L"    MODEL : ??";

			msg2listview(wos.str());

			char ipaddress[16];
			error = LALanioGetIpAddress(i, ipaddress);
			wos.str(L"");
			wos << L"IP : " << ipaddress;
			msg2listview(wos.str());

			st_work.hlanio[i] = LALanioConnect(i);
			wos.str(L"");
			if (st_work.hlanio[i] == -1) {
				wos << L"  Connect ERROR Code " << LALanioGetLastError();
				msg2listview(wos.str());
				return S_FALSE;;
			}
			else {
				wos << L" Connect OK : Handle " << st_work.hlanio;
				msg2listview(wos.str());
			}

			if (st_work.lanio_model[i] == LANIO_MODEL_LA_5AI) {

				wos.str(L"");
				wos << st_work.model_text[st_work.lanio_model[i]] << L" Set Range Code |";
				for (int j = 0; j < LANIO_N_CH_LA_5AI; j++) {
					if (LALanioSetAIRange(st_work.hlanio[i], j,st_work.lanio_ai_range[j])) {
						wos << st_work.lanio_ai_range[j] << L"|";
					}
					else {
						wos << L"NG|";
					}
				}
				msg2listview(wos.str());

				wos.str(L"");
				if( LALanioSetADCsps(st_work.hlanio[i], st_work.lanio_sps[LANIO_MODEL_LA_5AI])){
					wos << st_work.model_text[st_work.lanio_model[i]] << L" SPS set OK CODE:" << st_work.lanio_sps[LANIO_MODEL_LA_5AI];
				}
				else {
					wos << st_work.model_text[st_work.lanio_model[i]] << L" SPS set NG";
				}
				msg2listview(wos.str());

			}
		}
	}

#endif
	return S_OK;
}

HRESULT CAuxAgent::routine_work(void* pObj){
	input();
	parse();
	output();
	return S_OK;
}

int CAuxAgent::input() {
		
	//for (int i = 0; i < st_work.laniocount; i++) {
	//	switch (st_work.lanio_model[i]) {
	//	case LANIO_MODEL_LA_5AI: {
	//		pst_work->lanio_stat[i] = LALanioAIAll(st_work.hlanio[i], st_work.lanio_ai_data);
	//	}break;
	//	default:break;
	//	}
	//}

	return S_OK;
}

static INT16 pc_healthy = 0;

int CAuxAgent::parse() {           //メイン処理

	pc_healthy++;//PCヘルシーカウンタ
	//PCヘルシー信号
	//if (pc_healthy & 0x0002)	pAgent_Inf->slbrk_wbuf[0] |= 0x8000;
	//else						pAgent_Inf->slbrk_wbuf[0] &= 0x7FFF;

	//pAgent_Inf->slbrk_wbuf[0]++;

	return STAT_OK;
}
int CAuxAgent::output() {          //出力処理

	pCS_Inf->fb_slbrk.brk_fb_level = pAgent_Inf->slbrk_rbuf[0] & 0x000F;	//旋回ブレーキフィードバックレベル
	pCS_Inf->fb_slbrk.brk_fb_hw_brk = pAgent_Inf->slbrk_rbuf[0] & 0x0010;	//旋回ブレーキフィードバックHW

	pAgent_Inf->slbrk_wbuf[0] = pCS_Inf->com_slbrk.pc_com_autosel | pCS_Inf->com_slbrk.pc_com_hw_brk |pCS_Inf->com_slbrk.pc_com_brk_level;
	

	return STAT_OK;
}
int CAuxAgent::close() {
	int error = LALanioEnd();
	return 0;
}

static wostringstream monwos;

LRESULT CALLBACK CAuxAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[AUXAG_ID_MON1_STATIC_INF] = CreateWindowW(TEXT("STATIC"), st_mon1.text[AUXAG_ID_MON1_STATIC_INF], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[AUXAG_ID_MON1_STATIC_INF].x, st_mon1.pt[AUXAG_ID_MON1_STATIC_INF].y, 
			st_mon1.sz[AUXAG_ID_MON1_STATIC_INF].cx, st_mon1.sz[AUXAG_ID_MON1_STATIC_INF].cy, 
			hWnd, (HMENU)(AUXAG_ID_MON1_CTRL_BASE + AUXAG_ID_MON1_STATIC_INF), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, AUXAG_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		monwos.str(L"");

		//for (int i = 0; i < pst_work->laniocount; i++) {
		//	if (pst_work->lanio_model[i] == LANIO_MODEL_LA_5AI) {
		//		monwos << L"AI DATA  STATUS[" << pst_work->lanio_stat[i] << L"]\n";
		//		for (int j = 0; j < LANIO_N_CH_LA_5AI; j++) {
		//			monwos << L"Ch" << j + 1 << L": " << pst_work->lanio_ai_data[j] << L"\n";
		//		}
		//	}
		//}

		SetWindowText(st_mon1.hctrl[AUXAG_ID_MON1_STATIC_INF], monwos.str().c_str());
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon= NULL;	
		st_mon1.is_monitor_active = false;
		KillTimer(hWnd, AUXAG_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static bool is_write_req_turn = false;//書き込み要求送信の順番でtrue

LRESULT CALLBACK CAuxAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = AUXAG_ID_MON2_STATIC_MSG; i <= AUXAG_ID_MON2_STATIC_MAIN_INF; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXAG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//CB
		for (int i = AUXAG_ID_MON2_CB_COM_LEVEL_BIT0; i <= AUXAG_ID_MON2_CB_COM_AUTOSEL; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AUXAG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		//タイマー起動
		UINT rtn = SetTimer(hWnd, AUXAG_ID_MON2_TIMER, AUXAG_PRM_MON2_TIMER_MS, NULL);
		break;

	}
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		if (is_write_req_turn) {//書き込み要求送信
			st_mon2.wo_req_w.str(L"");
			//3Eフォーマット Dデバイス書き込み要求送信
			if (pMCSock->send_write_req_D_3E(pAgent_Inf->slbrk_wbuf) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			//電文内容表示出力
			if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				//ヘッダ部分
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
					<< L"#n_dev:" << pMCSock->mc_req_msg_w.n_device << L"\n";
	
					st_mon2.wo_req_w << L"D" << dec << L" |";
					if (st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX)	st_mon2.wo_req_w << hex;
					st_mon2.wo_req_w << std::setw(4) << std::setfill(L'0') << pAgent_Inf->slbrk_wbuf[0] << L"|";
					st_mon2.wo_req_w << L"\n";

				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_REQ_W], st_mon2.wo_req_w.str().c_str());
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

			if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
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
					<< L"#scom:" << pMCSock->mc_req_msg_r.scom << L"\n"
					<< L"#d_no:" << pMCSock->mc_req_msg_r.d_no
					<< L"#d_code:" << pMCSock->mc_req_msg_r.d_code
					<< L"#n_dev:" << pMCSock->mc_req_msg_r.n_device;
				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_REQ_R], st_mon2.wo_req_r.str().c_str());
			}

			QueryPerformanceCounter(&start_count_r);  // 書き込み要求送信時カウント値取り込み
			is_write_req_turn = true;
		}
		//共通表示 (送受信カウント,遅延時間,IP情報）
		if (st_mon2.is_monitor_active) {
			monwos.str(L""); monwos << L"RCV:R " << rcv_count_plc_r
				<< L"  W " << rcv_count_plc_w
				<< L"    SND:R " << snd_count_plc_r
				<< L"  W " << snd_count_plc_w
				<< L"    ERR:R " << rcv_errcount_plc_r
				<< L"  W " << rcv_errcount_plc_w
				<< L"    遅延μs:R " << res_delay_max_r
				<< L"  W " << res_delay_max_w;

			SetWindowText(hWnd, monwos.str().c_str());

			monwos.str(L""); 
			if(st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX)monwos << hex << std::setw(4) << std::setfill(L'0');

			monwos << L" BRK D96:" << pAgent_Inf->slbrk_wbuf[0] << L"  D16:" << pAgent_Inf->slbrk_rbuf[0] << L" 17:" << pAgent_Inf->slbrk_rbuf[1] << L" 18:" << pAgent_Inf->slbrk_rbuf[2];
			SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_MSG], monwos.str().c_str());

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
				SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_INF], monwos.str().c_str());
			}
			
			monwos.str(L"");
			monwos << L" MAIN CS >> LEVEL:" << pCS_Inf->com_slbrk.pc_com_brk_level << L"  HW:" << pCS_Inf->com_slbrk.pc_com_hw_brk << L" RST:" << pCS_Inf ->com_slbrk.pc_com_reset << L" EMG:" << pCS_Inf->com_slbrk.pc_com_emg << L" AUTO:" << pCS_Inf->com_slbrk.pc_com_autosel;
			SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_MAIN_INF], monwos.str().c_str());

		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - AUXAG_ID_MON2_CTRL_BASE)
		{
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT0: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT0], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFE;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0001;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT1: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT1], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFD;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0002;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT2: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT2], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFFB;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0004;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_LEVEL_BIT3: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_LEVEL_BIT3], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFF7;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0008;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_HW_BRK: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_HW_BRK], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFEF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0010;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_RST: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_RST], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFDF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0020;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_EMG: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_EMG], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFFBF;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0040;
			}
		}break;
		case AUXAG_ID_MON2_CB_COM_AUTOSEL: {
			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[AUXAG_ID_MON2_CB_COM_AUTOSEL], BM_GETCHECK, 0, 0)) {
				pAgent_Inf->slbrk_wbuf[0] &= 0xFF7F;
			}
			else {
				pAgent_Inf->slbrk_wbuf[0] |= 0x0080;
			}
		}break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case ID_SOCK_MC_AUX_BRK://MCソケット受信イベント
	{
		if (pMCSock == NULL)break;
		int nEvent = WSAGETSELECTEVENT(lp);
		st_mon2.wo_res_r.str(L"");
		st_mon2.wo_res_w.str(L"");
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = pMCSock->rcv_msg_3E(pAgent_Inf->slbrk_rbuf);//読み出し応答の時はデータ部のみ指定バッファにコピー
			if (nRtn == MC_RES_READ) {//読み出し応答
				rcv_count_plc_r++;

				//モニタWindow表示中ならば、モニタ表示出力処理
				if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_r << L"Rr>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_r.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_r.serial
						<< L"#NW:" << pMCSock->mc_res_msg_r.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_r.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_r.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_r.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_r.len
						<< L"#end:" << pMCSock->mc_res_msg_r.endcode << L"\n";
					if (st_mon2.msg_disp_mode == AUXAG_MON2_MSG_DISP_HEX) st_mon2.wo_res_r << hex << std::setw(4)<< std::setfill(L'0') ;
					else st_mon2.wo_res_r <<dec << std::setw(6)<< std::setfill(L' ');
	
					st_mon2.wo_res_r << L"D" << L" |";
							st_mon2.wo_res_r << hex << std::setw(4) << pAgent_Inf->slbrk_rbuf[0] << L"|" << pAgent_Inf->slbrk_rbuf[1] << L"|" << pAgent_Inf->slbrk_rbuf[2] << L"|";
					st_mon2.wo_res_r << L"\n";

					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}

				/**************** 読み込み応答時間計測(400回の最大値）*************************************/
				QueryPerformanceCounter(&end_count_r);    // 現在のカウント数
				LONGLONG lspan = (end_count_r.QuadPart - start_count_r.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
				if (res_delay_max_r < lspan) res_delay_max_r = lspan;
				if (rcv_count_plc_r % 400 == 0) res_delay_max_r = 0;
				/******************************************************************************************/
			}
			else if (nRtn == MC_RES_WRITE) {
				rcv_count_plc_w++;
				if ((st_mon2.msg_disp_mode != AUXAG_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_w << L"Rw>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_w.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_w.serial
						<< L"#NW:" << pMCSock->mc_res_msg_w.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_w.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_w.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_w.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_w.len
						<< L"#end:" << pMCSock->mc_res_msg_w.endcode;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
				}

				QueryPerformanceCounter(&end_count_w);    // 現在のカウント数
				LONGLONG lspan = (end_count_w.QuadPart - start_count_w.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
				if (res_delay_max_w < lspan) res_delay_max_w = lspan;
				if (rcv_count_plc_w % 400 == 0) {
					res_delay_max_w = 0;
				}
			}
			else {
				int err_code = WSAGetLastError();
				if (is_write_req_turn) {
					st_mon2.wo_res_r << L"PLC READ RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_r++;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}
				else {
					st_mon2.wo_res_w << L"PLC WRITE RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_w++;
					SetWindowText(st_mon2.hctrl[AUXAG_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
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
		st_mon2.is_monitor_active = false;
		KillTimer(hWnd, AUXAG_ID_MON2_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CAuxAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {
	
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
		wcex.lpszMenuName = TEXT("AUXAG_MON1");
		wcex.lpszClassName = TEXT("AUXAG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("AUXAG_MON1"), TEXT("AUXAG_MON1"), WS_OVERLAPPEDWINDOW,
			AUXAG_MON1_WND_X, AUXAG_MON1_WND_Y, AUXAG_MON1_WND_W, AUXAG_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
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
		wcex.lpszMenuName = TEXT("AUXAG_MON2");
		wcex.lpszClassName = TEXT("AUXAG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("AUXAG_MON2"), TEXT("SLBRK IF"), WS_OVERLAPPEDWINDOW,
			AUXAG_MON2_WND_X, AUXAG_MON2_WND_Y, AUXAG_MON2_WND_W, AUXAG_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_OFF;
//		show_monitor_wnd(id);
		return st_mon2.hwnd_mon;
	}
	else
	{
		return NULL;
	};

	return NULL;
}
void CAuxAgent::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CAuxAgent::show_monitor_wnd(int id) { 
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
void CAuxAgent::hide_monitor_wnd(int id) { 
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
LRESULT CALLBACK CAuxAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
		{
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt();
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			for (int i = 0; i < BC_N_ACT_ITEM; i++) {
				if (inf.panel_act_chk[wp - IDC_TASK_FUNC_RADIO1][i])
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_CHECKED, 0L);
				else
					SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_CHECK1 + i), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}break;

		case IDC_TASK_ITEM_CHECK1:
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
				st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_DEC;
			}
			else {
				hide_monitor_wnd(BC_ID_MON2);
				st_mon2.msg_disp_mode = AUXAG_MON2_MSG_DISP_OFF;
			}
		}break;
		}
	}break;

	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			wos.str(L"");
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) 	close_monitor_wnd(lp);
		else;
	}break;

	default:break;
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CAuxAgent::msg2listview(wstring wstr) {

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

void CAuxAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}

//タブパネルのStaticテキストを設定
void CAuxAgent::set_panel_tip_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
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
void CAuxAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxAgent::set_item_chk_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	return;
}

