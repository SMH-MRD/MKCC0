#include "CMCProtocol.h"
#include "CCcAgent.h"
#include "resource.h"
#include "CCrane.h"
#include "CPlc.h"
#include "CSpec.h"
#include "CHelper.h"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;

extern CCrane* pCrane;

//ソケット
static CMCProtocol* pMCSock;				//MCプロトコルオブジェクトポインタ

//クラススタティックメンバ
ST_AGENT_MON1 CAgent::st_mon1;
ST_AGENT_MON2 CAgent::st_mon2;

ST_CC_AGENT_INF CAgent::st_work;
ST_CC_PLC_IO CAgent::st_work_plcio;

//共有メモリ
static LPST_CC_ENV_INF		pEnv_Inf	= NULL;
static LPST_CC_CS_INF		pCS_Inf		= NULL;
static LPST_CC_PLC_IO		pPLC_IO		= NULL;
static LPST_CC_AGENT_INF	pAgent_Inf	= NULL;
static LPST_CC_OTE_INF		pOTE_Inf	= NULL;

static PINT16				pOteCtrl	= NULL;	//OTE操作入力信号ポインタ
static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

static LONG rcv_count_plc_r = 0, snd_count_plc_r = 0, rcv_errcount_plc_r = 0;
static LONG rcv_count_plc_w = 0, snd_count_plc_w = 0, rcv_errcount_plc_w = 0;
static LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;  //システムカウント
static LARGE_INTEGER frequency;				//システム周波数
static LONGLONG res_delay_max_w,res_delay_max_r;	//PLC応答時間


CAgent::CAgent() {
	// 共有メモリオブジェクトのインスタンス化
}
CAgent::~CAgent() {

}

HRESULT CAgent::initialize(LPVOID lpParam) {

	HRESULT hr = S_OK;
	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	//### 出力用共有メモリ取得
	out_size = sizeof(ST_CC_AGENT_INF);
	set_outbuf(pAgInfObj->get_pMap());

	//### 入力用共有メモリ取得

	pAgent_Inf	= (LPST_CC_AGENT_INF)pAgInfObj->get_pMap();
	pEnv_Inf	= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPLC_IO		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pCS_Inf		= (LPST_CC_CS_INF)pCsInfObj->get_pMap();
	pOTE_Inf	= (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	
	if ((pEnv_Inf == NULL) || (pPLC_IO == NULL) || (pCS_Inf == NULL) || (pAgent_Inf == NULL) || (pOTE_Inf == NULL)){
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		//OTE操作入力信号ポインタセット
		pOteCtrl = pOTE_Inf->st_msg_ote_u_rcv.body.st.ctrl_ope;
	}

	//### クレーンオブジェクト取得
	if (pCrane == NULL) {
		wos.str(L""); wos << L"Initialize : CraneObject NULL"; msg2listview(wos.str());
		return S_FALSE;
	}else{
		pPlcWIf = &(pCrane->pPlc->plc_io_wif);
		pPlcRIf = &(pCrane->pPlc->plc_io_rif);
	}

	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	Sleep(1000);
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
		pMCSock = new CMCProtocol(ID_SOCK_MC_CC_AGENT);
		if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_CC) != S_OK) {
			wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
			wos << L"Err :"<<pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
			return S_FALSE;
		}
		else {
			wos << L"MCProtocol Init OK"; msg2listview(wos.str());
		}
	}
	
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"PLC IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ２ CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	set_func_pb_txt();
	set_item_chk_txt();
	set_panel_tip_txt();

	return hr;
}

HRESULT CAgent::routine_work(void* pObj) {
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

/// <summary>
/// 
/// </summary>
/// <returns></returns>
int CAgent::input() {
	return S_OK;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
/// 


static INT16 pc_healthy=0;
static INT16 plc_healthy_chk_count = 0;
static INT16 plc_healthy = 0;

int CAgent::parse() {//メイン処理
	pc_healthy++;//PCヘルシーカウンタ更新


	if (plc_healthy == pPLC_IO->buf_io_read[0]) {				//PLCヘルシー状態が変化していない場合
		plc_healthy_chk_count--;								//PLCヘルシー状態が変化している⇒チェックカウントダウン
		if (plc_healthy_chk_count > 0)plc_healthy_chk_count--;
		else plc_healthy_chk_count = 0;
	}
	else plc_healthy_chk_count = PRM_CC_PLC_CHK_COUNT;			//PLCヘルシー状態が変化している⇒チェックカウントリミットセット
	plc_healthy = pPLC_IO->buf_io_read[0];						//PLCヘルシー前回値保持

	//PCヘルシー書込セット
	pCrane->pPlc->wval(pPlcWIf->pc_healthy, pc_healthy);	
	//PCコントロール信号セット

//	INT16 mask = 0xc000;	//PLC通信有効、デバッグモード
	INT16 mask = MASK_BIT_PC_COM_ACTIVE; mask |= MASK_BIT_PC_DBG_MODE;	//PLC通信有効、デバッグモード
	if (plc_healthy) {
		pAgent_Inf->pc_ctrl_mode2plc |= mask;
	}
	else {
		pAgent_Inf->pc_ctrl_mode2plc &= ~mask;
	}
	pCrane->pPlc->wval(pPlcWIf->pc_ctrl_mode, pAgent_Inf->pc_ctrl_mode2plc);

	//### OTE操作信号書込セット
	//PB,スイッチ類
	pCrane->pPlc->wval(pPlcWIf->syukan_on, pOteCtrl[OTE_PNL_CTRLS::syukan_on]);		//主幹ON
	pCrane->pPlc->wval(pPlcWIf->syukan_off, pOteCtrl[OTE_PNL_CTRLS::syukan_off]);	//主幹OFF
	pCrane->pPlc->wval(pPlcWIf->estop, pOteCtrl[OTE_PNL_CTRLS::estop]);				//非常停止
	//Notch信号
	pCrane->pPlc->wval(pPlcWIf->mh_notch, CNotchHelper::get_code4_by_notch(pOteCtrl[OTE_PNL_CTRLS::notch_mh], 0));
	pCrane->pPlc->wval(pPlcWIf->bh_notch, CNotchHelper::get_code4_by_notch(pOteCtrl[OTE_PNL_CTRLS::notch_bh], 0));
	pCrane->pPlc->wval(pPlcWIf->sl_notch, CNotchHelper::get_code4_by_notch(pOteCtrl[OTE_PNL_CTRLS::notch_sl], 0));
	pCrane->pPlc->wval(pPlcWIf->gt_notch, CNotchHelper::get_code4_by_notch(pOteCtrl[OTE_PNL_CTRLS::notch_gt], 0));
	//### OTE操作信号書込セット

	return S_OK;
}

/// <summary>
/// 出力関数
/// </summary>
/// <returns></returns>
static INT16 healthy_count = 0;
int CAgent::output() {
	//ヘルシー出力
	pPLC_IO->plc_enable = plc_healthy_chk_count;

	//制御指令出力
	memcpy_s(pAgent_Inf, sizeof(ST_CC_AGENT_INF), &st_work, sizeof(ST_CC_AGENT_INF));
	//PLC IO送信データ出力
	//送信は 共有メモリに設定後、送信バッファにコピー（受信は直接共有メモリに読み込む）
	memcpy_s(&st_work_plcio.buf_io_write,sizeof(pPLC_IO->buf_io_write),pPLC_IO->buf_io_write, sizeof(pPLC_IO->buf_io_write));

	return S_OK;
}

int CAgent::close() {

	return 0;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

/// <summary>
/// MON1 コールバック関数
/// </summary>
LRESULT CALLBACK CAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[AGENT_ID_MON1_STATIC_MSG] = CreateWindowW(TEXT("STATIC"), st_mon1.text[AGENT_ID_MON1_STATIC_MSG], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[AGENT_ID_MON1_STATIC_MSG].x, st_mon1.pt[AGENT_ID_MON1_STATIC_MSG].y,
			st_mon1.sz[AGENT_ID_MON1_STATIC_MSG].cx, st_mon1.sz[AGENT_ID_MON1_STATIC_MSG].cy,
			hWnd, (HMENU)(AGENT_ID_MON1_CTRL_BASE + AGENT_ID_MON1_STATIC_MSG), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, AGENT_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, AGENT_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

/// <summary>
/// Mon2 コールバック関数
/// </summary>
static bool is_write_req_turn = false;//書き込み要求送信の順番でtrue
static int n_page_w = 5, i_page_w = 0, i_ref_w = 0;
static int n_page_r = 5, i_page_r = 0, i_ref_r = 0;
LRESULT CALLBACK CAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = AGENT_ID_MON2_STATIC_MSG; i <= AGENT_ID_MON2_STATIC_RES_W; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AGENT_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//PB
		for (int i = AGENT_ID_MON2_PB_R_BLOCK_SEL; i <= AGENT_ID_MON2_PB_DISP_DEC_SEL; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(AGENT_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
	
		UINT rtn = SetTimer(hWnd, AGENT_ID_MON2_TIMER, AGENT_PRM_MON2_TIMER_MS, NULL);

		//IFデータ表示ページ数計算
		n_page_w = CC_MC_SIZE_W_WRITE / (AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN);
		if (CC_MC_SIZE_W_WRITE % (AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_w++;
		n_page_r = CC_MC_SIZE_W_READ / (AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN);
		if (CC_MC_SIZE_W_READ % (AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_r++;
		
		monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
		SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());


		monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
		SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());

		monwos.str(L""); monwos << L"表示中" ;
		SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
		st_mon2.msg_disp_mode = AGENT_MON2_MSG_DISP_HEX;
		monwos.str(L""); monwos << L"16進" ;
		SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
	}break;
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		if (is_write_req_turn) {//書き込み要求送信
			st_mon2.wo_req_w.str(L"");
			//3Eフォーマット Dデバイス書き込み要求送信
			if (pMCSock->send_write_req_D_3E(pPLC_IO->buf_io_write) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			//電文内容表示出力
			if ((st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				//ヘッダ部分
				st_mon2.wo_req_w << L"Sw>>"
					<< L"#sub:"		<< std::hex << pMCSock->mc_req_msg_w.subcode
					<< L"#serial:"	<< pMCSock->mc_req_msg_w.serial
					<< L"#NW:"		<< pMCSock->mc_req_msg_w.nNW
					<< L"#PC:"		<< pMCSock->mc_req_msg_w.nPC
					<< L"#UIO:"		<< pMCSock->mc_req_msg_w.nUIO
					<< L"#Ucd:"		<< pMCSock->mc_req_msg_w.nUcode
					<< L"#len:"		<< pMCSock->mc_req_msg_w.len
					<< L"#tm:"		<< pMCSock->mc_req_msg_w.timer
					<< L"#com:"		<< pMCSock->mc_req_msg_w.com
					<< L"#scom:"	<< pMCSock->mc_req_msg_w.scom<< L"\n"
					<< L"#d_no:"	<< pMCSock->mc_req_msg_w.d_no
					<< L"#d_code:"	<< pMCSock->mc_req_msg_w.d_code
					<< L"#n_dev:"	<< pMCSock->mc_req_msg_w.n_device << L"\n";
				//データ部分1ページ10WORD　4行で切替表示
				for (int i = 0; i < AGENT_MON2_MSG_DISP_N__DATAROW; i++) {
					int no = CC_MC_ADDR_W_WRITE + i_page_w * AGENT_MON2_MSG_DISP_N_DATA_COLUMN * AGENT_MON2_MSG_DISP_N__DATAROW + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN;
					st_mon2.wo_req_w << L"D"<< dec << no << L" |";
					if (st_mon2.msg_disp_mode == AGENT_MON2_MSG_DISP_HEX)	st_mon2.wo_req_w << hex;
					for (int j = 0; j < AGENT_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
						if (st_mon2.msg_disp_mode == AGENT_MON2_MSG_DISP_HEX)
							st_mon2.wo_req_w << std::setw(4) << std::setfill(L'0') << pPLC_IO->buf_io_write[i_ref_w + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						else
							st_mon2.wo_req_w << std::setw(6) << std::setfill(L' ') << pPLC_IO->buf_io_write[i_ref_w + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
					}
					st_mon2.wo_req_w << L"\n";
				}
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_REQ_W], st_mon2.wo_req_w.str().c_str());
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

			if ((st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				st_mon2.wo_req_r << L"Sr>>"
					<< L"#sub:"		<< std::hex << pMCSock->mc_req_msg_r.subcode
					<< L"#serial:"	<< pMCSock->mc_req_msg_r.serial
					<< L"#NW:"		<< pMCSock->mc_req_msg_r.nNW
					<< L"#PC:"		<< pMCSock->mc_req_msg_r.nPC
					<< L"#UIO:"		<< pMCSock->mc_req_msg_r.nUIO
					<< L"#Ucd:"		<< pMCSock->mc_req_msg_r.nUcode
					<< L"#len:"		<< pMCSock->mc_req_msg_r.len
					<< L"#tm:"		<< pMCSock->mc_req_msg_r.timer
					<< L"#com:"		<< pMCSock->mc_req_msg_r.com
					<< L"#scom:"	<< pMCSock->mc_req_msg_r.scom << L"\n"
					<< L"#d_no:"	<< pMCSock->mc_req_msg_r.d_no
					<< L"#d_code:"	<< pMCSock->mc_req_msg_r.d_code
					<< L"#n_dev:"	<< pMCSock->mc_req_msg_r.n_device ;
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_REQ_R], st_mon2.wo_req_r.str().c_str());
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

			monwos.str(L""); monwos << L" PLC healthy:" << pPLC_IO->plc_enable;
			SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_MSG], monwos.str().c_str());

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
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_INF], monwos.str().c_str());
			}
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId- AGENT_ID_MON2_CTRL_BASE)
		{
		case AGENT_ID_MON2_PB_R_BLOCK_SEL: { 
			if (++i_page_r >= n_page_r)i_page_r = 0;
			monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
			SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());
			i_ref_r = i_page_r * AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN;
		}break;
		case AGENT_ID_MON2_PB_W_BLOCK_SEL: { 
			if (++i_page_w >= n_page_w)i_page_w = 0;
			monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
			SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());
			i_ref_w = i_page_w * AGENT_MON2_MSG_DISP_N__DATAROW * AGENT_MON2_MSG_DISP_N_DATA_COLUMN;
		}break;
		case AGENT_ID_MON2_PB_MSG_DISP_SEL: {
			if (st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) {
				st_mon2.msg_disp_mode = AGENT_MON2_MSG_DISP_OFF;
				monwos.str(L""); monwos << L"非表示";
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"-:";
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
			else {
				st_mon2.msg_disp_mode = AGENT_MON2_MSG_DISP_HEX;
				monwos.str(L""); monwos << L"表示中";
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
				monwos.str(L""); monwos << L"16進";
				SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
			}
		}break;
		case AGENT_ID_MON2_PB_DISP_DEC_SEL: {
			if (st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) {
				if (st_mon2.msg_disp_mode == AGENT_MON2_MSG_DISP_HEX) {
					st_mon2.msg_disp_mode = AGENT_MON2_MSG_DISP_DEC;
					monwos.str(L""); monwos << L"10進";
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
				else {
					st_mon2.msg_disp_mode = AGENT_MON2_MSG_DISP_HEX;
					monwos.str(L""); monwos << L"16進";
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
				}
			}
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case ID_SOCK_MC_CC_AGENT://MCソケット受信イベント
	{
		if (pMCSock == NULL)break;
		int nEvent = WSAGETSELECTEVENT(lp);
		st_mon2.wo_res_r.str(L"");
		st_mon2.wo_res_w.str(L"");
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = pMCSock->rcv_msg_3E(pPLC_IO->buf_io_read);//読み出し応答の時はデータ部のみ指定バッファにコピー
			if (nRtn == MC_RES_READ) {//読み出し応答
				rcv_count_plc_r++;
				
				//モニタWindow表示中ならば、モニタ表示出力処理
				if ((st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_r << L"Rr>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_r.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_r.serial
						<< L"#NW:" << pMCSock->mc_res_msg_r.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_r.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_r.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_r.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_r.len
						<< L"#end:" << pMCSock->mc_res_msg_r.endcode << L"\n";

						for (int i = 0; i < AGENT_MON2_MSG_DISP_N__DATAROW; i++) {
						int no = CC_MC_ADDR_W_READ + i_page_r * AGENT_MON2_MSG_DISP_N_DATA_COLUMN * AGENT_MON2_MSG_DISP_N__DATAROW + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN;
						st_mon2.wo_res_r << L"D" << dec << no << L" |";
						if (st_mon2.msg_disp_mode == AGENT_MON2_MSG_DISP_HEX)	st_mon2.wo_res_r << hex;
						for (int j = 0; j < AGENT_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
							if (st_mon2.msg_disp_mode == AGENT_MON2_MSG_DISP_HEX)
							st_mon2.wo_res_r << std::setw(4) << std::setfill(L'0') << pPLC_IO->buf_io_read[i_ref_r + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						else
							st_mon2.wo_res_r << std::setw(6) << std::setfill(L' ') << pPLC_IO->buf_io_read[i_ref_r + i * AGENT_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						}
						st_mon2.wo_res_r << L"\n";
					}
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
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
				if ((st_mon2.msg_disp_mode != AGENT_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
					st_mon2.wo_res_w << L"Rw>>"
						<< L"#sub:" << std::hex << pMCSock->mc_res_msg_w.subcode
						<< L"#serial:" << pMCSock->mc_res_msg_w.serial
						<< L"#NW:" << pMCSock->mc_res_msg_w.nNW
						<< L"#PC:" << pMCSock->mc_res_msg_w.nPC
						<< L"#UIO:" << pMCSock->mc_res_msg_w.nUIO
						<< L"#Ucd:" << pMCSock->mc_res_msg_w.nUcode
						<< L"#len:" << pMCSock->mc_res_msg_w.len
						<< L"#end:" << pMCSock->mc_res_msg_w.endcode;
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
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
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_RES_R], st_mon2.wo_res_r.str().c_str());
				}
				else {
					st_mon2.wo_res_w << L"PLC WRITE RES_ERR  CODE:err_code" << err_code;
					rcv_errcount_plc_w++;
					SetWindowText(st_mon2.hctrl[AGENT_ID_MON2_STATIC_RES_W], st_mon2.wo_res_w.str().c_str());
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

/// <summary>
/// モニタ関連関数
/// </summary>
HWND CAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
			AGENT_MON1_WND_X, AGENT_MON1_WND_Y, AGENT_MON1_WND_W, AGENT_MON1_WND_H,
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
			AGENT_MON2_WND_X, AGENT_MON2_WND_Y, AGENT_MON2_WND_W, AGENT_MON2_WND_H,
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
void CAgent::close_monitor_wnd(int id) {
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
void CAgent::show_monitor_wnd(int id) {
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
void CAgent::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			case IDC_TASK_FUNC_RADIO1: {
				WPARAM wp = inf.index;
				LPARAM lp = BC_BK_COLOR_LIGHT_GREEN;
				SendMessage(inf.hwnd_parent, WM_USER_SET_BK_COLOR_REQ, wp, lp);
			}break;
			case IDC_TASK_FUNC_RADIO4: {
				set_item_chk_txt();
			}break;
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
void CAgent::msg2listview(wstring wstr) {

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
void CAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CAgent::set_panel_tip_txt() {
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
void CAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAgent::set_item_chk_txt() {
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




