#include "CSockLib.H"
#include "COteAgent.h"
#include "resource.h"
#include "framework.h"
#include "OTE_DEF.H"
#include "CPanelObj.h"

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;

extern ST_DRAWING_BASE		drawing_items;
extern ST_DEVICE_CODE g_my_code; //端末コード

//ソケット
static CSockUDP* pUSockPC;					//ユニキャストPC通信受信用
static CSockUDP* pMSockOte;					//マルチキャストOTE通信受信用
static CSockUDP* pMSockPC;					//マルチキャストPC通信受信用

static SOCKADDR_IN addrin_ote_m2pc_snd;		//OTE->PC OTEマルチキャスト送信先アドレス（PC受信用)
static SOCKADDR_IN addrin_ote_m2ote_snd;	//PC->OTE OTEマルチキャスト送信先アドレス（OTE受信用)

ST_OTE_AG_MON1 COteAgent::st_mon1;
ST_OTE_AG_MON2 COteAgent::st_mon2;

ST_OTE_CC_IF COteAgent::st_work;

//共有メモリ

static LPST_OTE_ENV_INF	pOteEnvInf;
static LPST_OTE_UI		pOteUI;
static LPST_OTE_CC_IF	pOteCCIf;
static LPST_OTE_CS_INF	pOteCsInf;

static LONG rcv_count_pc_u = 0,	 snd_count_ote_u = 0;
static LONG rcv_count_pc_m = 0,  snd_count_m2pc = 0;
static LONG rcv_count_ote_m = 0, snd_count_m2ote = 0;

static LARGE_INTEGER start_count_s, end_count_r;			//システムカウント
static LARGE_INTEGER frequency;								//システム周波数
static LONGLONG res_delay_max, res_delay_min = 10000000;	//C応答時間

static wostringstream wos_msg;
/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
COteAgent::COteAgent() {
}
COteAgent::~COteAgent() {
}

int COteAgent::setup_crane_if(int crane_id) {

	HRESULT hr = S_OK;

	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index, WM_USER_WPH_OPEN_IF_WND);	//HWORD:コマンドコード, LWORD:送信元タスクインデックス
	LPARAM lp = BC_ID_MON2;										//LWORD:オープン対象のウィンドウのコード
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);

	//### 通信ソケットアドレスセット
	// //##インスタンス生成
	if (pUSockPC != NULL)pUSockPC->Close();
	pUSockPC = new CSockUDP(ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI);
	if (pMSockPC != NULL)pMSockPC->Close();
	pMSockPC = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MUL);
	if (pMSockOte != NULL)pMSockOte->Close();
	pMSockOte = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MUL);

	switch (crane_id) {
	case CARNE_ID_HHGH29: {
		//受信アドレス ！！【仮】受信アドレスはアダプタから読み取り設定
		if(pOteEnvInf->app_mode == OTE_ENV_APP_DEBUG_TYPE1) {
			pUSockPC->set_sock_addr(&pUSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_OTE);
			pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_PC2OTE);//受信アドレス
			pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_OTE2OTE);//受信アドレス
		}
		else
		{
			pUSockPC->set_sock_addr(&pUSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE_HHGG3801, OTE_IF_UNI_PORT_OTE);
			pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE_HHGG3801, OTE_IF_MULTI_PORT_PC2OTE);//受信アドレス
			pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_OTE_HHGG3801, OTE_IF_MULTI_PORT_OTE2OTE);//受信アドレス
		}
		//送信先アドレス
		pUSockPC->set_sock_addr(&(pUSockPC->addr_in_dst), OTE_IF_CRANE_IP_HHGH29, OTE_IF_UNI_PORT_PC);//送信先アドレス
		pMSockPC->set_sock_addr(&addrin_ote_m2pc_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2PC); //送信先アドレ???
		pMSockPC->set_sock_addr(&addrin_ote_m2ote_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2OTE);//送信先アドレス}
	}break;
	case CRANE_ID_H6R602: 
	{
		//受信アドレス ！！【仮】受信アドレスはアダプタから読み取り設定
		pUSockPC->set_sock_addr(&pUSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_OTE);
		pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_PC2OTE);//受信アドレス
		pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_OTE0, OTE_IF_MULTI_PORT_OTE2OTE);//受信アドレス

		//送信先アドレス
		pUSockPC->set_sock_addr(&(pUSockPC->addr_in_dst), OTE_IF_CRANE_IP_H6R602, OTE_IF_UNI_PORT_PC);//送信先アドレス
		pMSockPC->set_sock_addr(&addrin_ote_m2pc_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2PC); //送信先アドレス
		pMSockPC->set_sock_addr(&addrin_ote_m2ote_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2OTE);//送信先アドレス
	}break;
	case CARNE_ID_PC0: 
	case CARNE_ID_HHGQ18: 
	case CARNE_ID_HHFR22: 
	default: {
		//受信アドレス ！！【仮】受信アドレスはアダプタから読み取り設定
		pUSockPC->set_sock_addr(&pUSockPC->addr_in_rcv, OTE_IF_CRANE_IP_PC0, OTE_IF_UNI_PORT_OTE);
		pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv, OTE_IF_CRANE_IP_PC0, OTE_IF_MULTI_PORT_PC2OTE);//受信アドレス
		pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_CRANE_IP_PC0, OTE_IF_MULTI_PORT_OTE2OTE);//受信アドレス

		//送信先アドレス
		pUSockPC->set_sock_addr(&(pUSockPC->addr_in_dst), OTE_IF_CRANE_IP_PC0, OTE_IF_UNI_PORT_PC);//送信先アドレス
		pMSockPC->set_sock_addr(&addrin_ote_m2pc_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2PC); //送信先アドレス
		pMSockPC->set_sock_addr(&addrin_ote_m2ote_snd, OTE_IF_MULTI_IP_OTE, OTE_IF_MULTI_PORT_OTE2OTE);//送信先アドレス
	}break;
	}

	//### WSA初期化
	wos.str(L"");//初期化
	if (pUSockPC->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockPC->err_msg.str();  err |= SOCK_NG_UNICAST; hr = S_FALSE; }
	if (pMSockPC->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pMSockPC->err_msg.str();  err |= SOCK_NG_MULTICAST; hr = S_FALSE; }
	if (pMSockOte->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pMSockOte->err_msg.str(); err |= SOCK_NG_MULTICAST; hr = S_FALSE; }
	if (hr == S_FALSE)msg2listview(wos.str()); wos.str(L"");

	Sleep(1000);
	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return S_FALSE;
	}

	//##ソケットソケット生成・設定
	//##ユニキャスト
	if (pUSockPC->init_sock(st_mon2.hwnd_mon, pUSockPC->addr_in_rcv) != S_OK) {//init_sock():bind()→非同期化まで実施
		wos << L"OTE U SockErr:" << pUSockPC->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"OTE U Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//##マルチキャスト
	SOCKADDR_IN addr_buf;
	pMSockPC->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_PC, NULL);//PCマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockPC->init_sock(st_mon2.hwnd_mon, pMSockPC->addr_in_rcv, addr_buf) != S_OK) {//init_sock_m():bind()まで実施 + マルチキャストグループへ登録
		wos << L"PC M SockErr:" << pMSockPC->err_msg.str(); hr = S_FALSE;
	}
	else wos << L"PC M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	pMSockOte->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_OTE, NULL);//OTEマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockOte->init_sock(st_mon2.hwnd_mon, pMSockOte->addr_in_rcv, addr_buf) != S_OK) {
		wos << L"OTE M SockErr:" << pMSockOte->err_msg.str(); hr = S_FALSE;;
	}
	else  wos << L"OTE M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	st_work.st_msg_ote_u_snd.head.addr = pUSockPC->addr_in_rcv;
	st_work.st_msg_ote_m_snd.head.addr = pMSockOte->addr_in_rcv;

	if (hr == S_FALSE) {
		close_crane_if();
		close_monitor_wnd(BC_ID_MON2);	//通信モニタクローズ
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
		return S_FALSE;
	};
	return hr;
}

int COteAgent::close_crane_if() {
	
	if (pUSockPC != NULL)	pUSockPC->Close();	//ソケットクローズ
	if (pMSockPC != NULL)	pMSockPC->Close();				//ソケットクローズ
	if (pMSockOte!= NULL)	pMSockOte->Close();				//ソケットクローズ
	return STAT_OK;
}

HRESULT COteAgent::initialize(LPVOID lpParam) {

	//システム周波数読み込み
	QueryPerformanceFrequency(&frequency);

	HRESULT hr = S_OK;	
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_OTE_CC_IF);
	if (OK_SHMEM != pOteCcInfObj->create_smem(SMEM_OTE_CC_IF_NAME, out_size, MUTEX_OTE_CC_IF_NAME)) {
		err |= SMEM_NG_CS_INF; hr = S_FALSE;
	}
	set_outbuf(pOteCcInfObj->get_pMap());

	//### 共有メモリ取得
	pOteCCIf = (LPST_OTE_CC_IF)(pOteCcInfObj->get_pMap());
	pOteCsInf = (LPST_OTE_CS_INF)(pOteCsInfObj->get_pMap());
	pOteUI = (LPST_OTE_UI)pOteUiObj->get_pMap();
	pOteEnvInf = (LPST_OTE_ENV_INF)pOteEnvInfObj->get_pMap();

	if ((pOteCCIf == NULL) || (pOteCsInf == NULL) || (pOteUI == NULL) || (pOteEnvInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

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

	//モニタ2CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT COteAgent::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

int COteAgent::input() {

	return S_OK;
}

int COteAgent::parse() {

	//CC受信データ解析
	st_work.cc_active_ote_id = pOteCCIf->st_msg_pc_u_rcv.head.tgid;

	//送信データ解析
	//操作ボタン類（SCADA共有メモリ部）

	//##################### 送信バッファセット　############################################

	INT16* pctrl = st_work.st_msg_ote_u_snd.body.st.pnl_ctrl;//送信バッファのOTE操作信号情報部のポインタ
	memcpy_s(pctrl, sizeof(INT16) * OTE_PNL_CTRLS::MAX, pOteCsInf->pnl_ctrl, sizeof(INT16)* OTE_PNL_CTRLS::MAX);

	//故障信号要求コード
	st_work.st_msg_ote_u_snd.body.st.faults_disp_req = pOteUI->flt_req_code;	//故障信号要求コード

	//##################### クレーンとの通信チェック　############################################
	//CC通信状態ステータスセット（モニタ用）
	pOteCCIf->cc_com_stat_r = st_work.cc_com_stat_r; pOteCCIf->cc_com_stat_s = st_work.cc_com_stat_s;

	//カウンタは、受信時0値セット カウンタ値0より大でカウントアップ
	if(st_work.cc_comm_chk_cnt) st_work.cc_comm_chk_cnt++;

	if (st_work.cc_comm_chk_cnt > PRM_OTE_PC_COM_TMOV_CNT) 
		st_work.cc_comm_chk_cnt = PRM_OTE_PC_COM_TMOV_CNT;

	if (st_work.cc_comm_chk_cnt == PRM_OTE_PC_COM_TMOV_CNT){//通信異常
		//クレーン操作有効端末idクリア
		pOteCCIf->cc_active_ote_id	= CRANE_ID_NULL;
		pOteCCIf->id_conected_crane = CRANE_ID_NULL;
	}
	else {
		//クレーン操作有効端末id
		pOteCCIf->cc_active_ote_id = st_work.cc_active_ote_id;
		pOteCCIf->id_conected_crane = st_work.id_conected_crane;
	}

	return S_OK;
}

int COteAgent::close() {

	return 0;
}

int COteAgent::output() {          //出力処理
	//共有メモリ出力処理
	//受信データは直接共有メモリで受け取るのでコピー無し
	//送信データの内容を共有バッファへコピー
	memcpy_s(&(pOteCCIf->st_msg_ote_u_snd), sizeof(ST_OTE_U_MSG), &(st_work.st_msg_ote_u_snd), sizeof(ST_OTE_U_MSG));
	memcpy_s(&(pOteCCIf->st_msg_ote_m_snd), sizeof(ST_OTE_M_MSG), &(st_work.st_msg_ote_m_snd), sizeof(ST_OTE_M_MSG));
	
	//CC通信状態ステータスセット（CSモニタ用）
	pOteCCIf->cc_comm_chk_cnt = st_work.cc_comm_chk_cnt;
	
	if (pOteUI->hWnd_crane_ope_panel == NULL)
		pOteCCIf->id_conected_crane = NULL;

	return STAT_OK;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK COteAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
/****************************************************************************/
/*   通信関数											                    */
/****************************************************************************/
/// <summary>
/// OTEユニキャスト電文受信処理
/// </summary>
HRESULT COteAgent::rcv_uni_ote(LPST_PC_U_MSG pbuf) {
	int nRtn = pUSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_pc_u++;
	//CC通信チェックカウンタリセット
	//ユニキャスト送信時に1をセットしておき、定周期でカウントアップ
	st_work.cc_comm_chk_cnt = 0; 
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PCマルチキャスト電文受信処理 
/// </summary>
HRESULT COteAgent::rcv_mul_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pMSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR rcv:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_pc_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTEマルチキャスト電文受信処理 
/// </summary>
HRESULT COteAgent::rcv_mul_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pMSockOte->rcv_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR rcv:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}
		return S_FALSE;
	}
	rcv_count_ote_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTEユニキャスト電文送信処理 
/// </summary>
LPST_OTE_U_MSG COteAgent::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
		
	st_work.st_msg_ote_u_snd.head.myid = pOteEnvInf->device_code;
	st_work.st_msg_ote_u_snd.head.addr = pUSockPC->addr_in_rcv;
	st_work.st_msg_ote_u_snd.head.code = code;
	st_work.st_msg_ote_u_snd.head.status = stat;
	st_work.st_msg_ote_u_snd.head.tgid = 0;

	return &st_work.st_msg_ote_u_snd;
}
HRESULT COteAgent::snd_uni2pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pUSockPC == NULL) return S_FALSE;
	if (pUSockPC->snd_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTEマルチキャスト電文送信処理 
/// </summary>
//マルチキャストメッセージセット
LPST_OTE_M_MSG COteAgent::set_msg_m() {
	return &st_work.st_msg_ote_m_snd;
}
//PCへ送信
HRESULT COteAgent::snd_mul2pc(LPST_OTE_M_MSG pbuf) {
	if (pMSockPC == NULL) return S_FALSE;
	if (pMSockPC->snd_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m2pc_snd) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR snd:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2pc++;
	return S_OK;
}
//OTEへ送信
HRESULT COteAgent::snd_mul2ote(LPST_OTE_M_MSG pbuf) {
	if (pMSockOte == NULL) return S_FALSE;
	if (pMSockOte->snd_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m2ote_snd) == SOCKET_ERROR) {
		st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR snd:" << pMSockOte->err_msg.str();
		SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		wos_msg.str(L""); wos_msg << pMSockPC->err_msg.str();
		return S_FALSE;
	}
	snd_count_m2ote++;
	return S_OK;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static ULONG_PTR gdiToken;

LRESULT CALLBACK COteAgent::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiToken, &gdiplusStartupInput, nullptr);


		Point pt = Point(st_mon1.pt[OTE_AG_ID_MON1_GDI_STR_1].x, st_mon1.pt[OTE_AG_ID_MON1_GDI_STR_1].y);
		Size sz = Size(st_mon1.sz[OTE_AG_ID_MON1_GDI_STR_1].cx, st_mon1.sz[OTE_AG_ID_MON1_GDI_STR_1].cy);
		st_mon1.str_message = new CStringGdi(
			(OTE_AG_ID_MON1_CTRL_BASE+OTE_AG_ID_MON1_GDI_STR_1),
			&pt,&sz,st_mon1.text[OTE_AG_ID_MON1_GDI_STR_1],st_mon1.pgraphic, 
			drawing_items.pstrformat[ID_STR_FORMAT_LEFT_TOP], 
			drawing_items.pbrush[ID_PANEL_COLOR_BLACK], 
			drawing_items.pbrush[ID_PANEL_COLOR_DGRAY],
			drawing_items.pfont[ID_PANEL_FONT_10]
		);

		//ウィンドウにコントロール追加
		//st_mon1.hctrl[OTE_AG_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[OTE_AG_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
		//	st_mon1.pt[OTE_AG_ID_MON1_STATIC_1].x, st_mon1.pt[OTE_AG_ID_MON1_STATIC_1].y,
		//	st_mon1.sz[OTE_AG_ID_MON1_STATIC_1].cx, st_mon1.sz[OTE_AG_ID_MON1_STATIC_1].cy,
		//	hWnd, (HMENU)(OTE_AG_ID_MON1_CTRL_BASE + OTE_AG_ID_MON1_STATIC_1), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, OTE_AG_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		if (st_mon1.pgraphic == NULL) {
			st_mon1.hdc = GetDC(st_mon1.hwnd_mon);
			st_mon1.pgraphic = new Graphics(st_mon1.hdc);
		}
		st_mon1.str_message->update(st_mon1.pgraphic,L"表示サンプルチェック");

	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon1.hwnd_mon = NULL;
		KillTimer(hWnd, OTE_AG_ID_MON1_TIMER);
		delete st_mon1.pgraphic;
		st_mon1.pgraphic = NULL;
		GdiplusShutdown(gdiToken);

	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

//static wostringstream mon2wos;
LRESULT CALLBACK COteAgent::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		int i;
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (i = OTE_AG_ID_MON2_STATIC_UNI; i <= OTE_AG_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (i = OTE_AG_ID_MON2_RADIO_RCV; i <= OTE_AG_ID_MON2_RADIO_INFO; i++) {
			if (i == OTE_AG_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//CHECK BOX
		i = OTE_AG_ID_MON2_CB_BODY;

		st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);

		//PB
		for (i = OTE_AG_ID_MON2_PB_PAGE_UP_UNI; i <= OTE_AG_ID_MON2_PB_PAGE_UP_MOT; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(OTE_AG_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, OTE_AG_ID_MON2_TIMER, OTE_AG_PRM_MON2_TIMER_MS, NULL);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		int _Id = wmId - OTE_AG_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{
		case OTE_AG_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_RCV;
		}break;
		case OTE_AG_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_SND;
		}break;
		case OTE_AG_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = OTE_AG_ID_MON2_RADIO_INFO;
		}break;
		case OTE_AG_ID_MON2_CB_BODY: {
			if (BST_CHECKED == SendMessage(st_mon2.hctrl[OTE_AG_ID_MON2_CB_BODY], BM_GETCHECK, 0, 0))
				st_mon2.is_body_disp_mode = true;
			else
				st_mon2.is_body_disp_mode = false;
		}break;
		case OTE_AG_ID_MON2_PB_PAGE_UP_UNI: {
			if (++st_mon2.ipage_uni >= OTE_AG_ID_MON2_N_PAGE_UP_UNI)st_mon2.ipage_uni = 0;
		}break;
		case OTE_AG_ID_MON2_PB_PAGE_UP_MPC: {
			if (++st_mon2.ipage_mpc >= OTE_AG_ID_MON2_N_PAGE_UP_MPC)st_mon2.ipage_mpc = 0;
		}break;
		case OTE_AG_ID_MON2_PB_PAGE_UP_MOT: {
			if (++st_mon2.ipage_mot >= OTE_AG_ID_MON2_N_PAGE_UP_MOT)st_mon2.ipage_mot = 0;
		}break;

		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}break;
	}
	case WM_TIMER: {
		//UniCast送信
		HRESULT hr;
		if(pOteCsInf->st_body.remote)
			hr = snd_uni2pc(set_msg_u(true, OTE_CODE_REQ_OPE_ACTIVE, OTE_STAT_MODE_OPE), &pUSockPC->addr_in_dst);
		else 
			hr = snd_uni2pc(set_msg_u(false, OTE_CODE_REQ_MON, OTE_STAT_MODE_MON), &pUSockPC->addr_in_dst);

		//送信成功 AND 正常受信後
		if((hr==S_OK) && (st_work.cc_comm_chk_cnt == 0))
			st_work.cc_comm_chk_cnt = 1; //開始カウントセット


		QueryPerformanceCounter(&start_count_s);  // 送信時カウント値取り込み

		//MultiCast送信
		snd_mul2pc(set_msg_m()); 	//PCへ送信
		snd_mul2ote(set_msg_m());	//OTEへ送信

		//モニター表示
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			//ラベルにIP情報出力
			if (pUSockPC != NULL) {
				addr = pUSockPC->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"UNI>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockPC->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockPC->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_UNI], st_mon2.wo_work.str().c_str());
			}
			if (pMSockPC != NULL) {
				addr = pMSockPC->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"MPC>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockPC->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockPC->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_MUL_PC], st_mon2.wo_work.str().c_str());
			}
			if (pMSockOte != NULL) {
				addr = pMSockOte->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work << L"MOTE>>IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockOte->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pMSockOte->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_LABEL_MUL_OTE], st_mon2.wo_work.str().c_str());
			}
			
			//電文内容表示出力
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc.str(L""); st_mon2.wo_mote.str(L"");
			if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_RCV) {
				if (st_mon2.is_body_disp_mode) {
					LPST_PC_U_BODY	pb0 = &(pOteCCIf->st_msg_pc_u_rcv.body.st);
					LPST_PC_M_BODY	pb1 = &(pOteCCIf->st_msg_pc_m_rcv.body.st);
					LPST_OTE_M_BODY pb2 = &(pOteCCIf->st_msg_ote_m_rcv.body.st);
					st_mon2.wo_uni	<< L"[BODY P" << st_mon2.ipage_uni << L"]" << L"\n";
					st_mon2.wo_mpc	<< L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
					st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";
					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@LAMP:";
						for (int i = 0; i < 20; i++) {
							st_mon2.wo_uni << " [" << i << "]" << pb0->lamp[i].code;
						}
					}
					else if (st_mon2.ipage_uni == 1) {
						st_mon2.wo_uni << L"@LAMP:";
						for (int i = 21; i < 40; i++) {
							st_mon2.wo_uni << " [" << i << "]" << pb0->lamp[i].code;
						}
					}
					else if (st_mon2.ipage_uni == 2) {
						st_mon2.wo_uni << L"@SL_BRK_FB:";
						for (int i = 0; i < 3; i++) {
							st_mon2.wo_uni << " [" << i << "]" << pb0->sl_brk_fb[i];
						}
					}
					else {
						st_mon2.wo_uni << L"@ PNL CTRL:";
						for (int i = 21; i < 40; i++) {
							st_mon2.wo_uni << " [" << i << "]" << pOteCsInf->pnl_ctrl[i];
						}
					}
				}
				else {
					LPST_OTE_HEAD  ph0 = &(pOteCCIf->st_msg_pc_u_rcv.head); 
					LPST_OTE_HEAD  ph1 = &(pOteCCIf->st_msg_pc_m_rcv.head); 
					LPST_OTE_HEAD  ph2 = &(pOteCCIf->st_msg_ote_m_rcv.head);
					st_mon2.wo_uni << L"[HEAD]" << L"ID:" << ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option
						<< L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port);// << L"\n";
					st_mon2.wo_uni << L"        CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid<< L"\n";
					st_mon2.wo_mpc << L"[HEAD]" << L" CODE:" << ph1->code << L"\n";
					st_mon2.wo_mote << L"[HEAD]" << L"CODE:" << ph2->code << L"\n";
				}
			}
			else if (st_mon2.sock_inf_id == OTE_AG_ID_MON2_RADIO_SND) {
				if (st_mon2.is_body_disp_mode) {
					LPST_OTE_U_BODY pb0 = &st_work.st_msg_ote_u_snd.body.st;
					LPST_OTE_M_BODY pb1 = &st_work.st_msg_ote_m_snd.body.st;

					st_mon2.wo_uni << L"[BODY P" << st_mon2.ipage_uni << L"]" ;
					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@ESTOP:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::estop]
							<< L"@主幹入:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::syukan_on] << L"@主幹切:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]
							<< L"@RMT:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::remote] << L"@FRESET:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]
							<< L"@MH_SPD:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode] << L"@BH_MODE:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode] << L"\n";
						st_mon2.wo_uni << L"Notch: MH "<< pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_mh] << L" AH " << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]
							<< L" BH " << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_bh] << L" SL " << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_sl] << L" GT " << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_gt] << L"\n";
						st_mon2.wo_uni << L"@SL BRK COM:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::sl_brk] << L"@Bypass:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::bypass] << L"@Alm Stp:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::alm_stop] << L"@Siren:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]
							<< L"@MLamp:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]+ pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]+ pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3] << L"\n";
					}
					else if (st_mon2.ipage_uni == 1) {
						st_mon2.wo_uni << L"@ PNL CTRL:";
						for (int i = 0; i < 20; i++) {
							st_mon2.wo_uni << " [" << i << "]" << pOteCsInf->pnl_ctrl[i];
						}
					}
					else if (st_mon2.ipage_uni == 2) {

					}
					else {

					}

					st_mon2.wo_mpc << L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
					st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";
				}
				else {
					LPST_OTE_HEAD	ph0 = &st_work.st_msg_ote_u_snd.head;
					LPST_OTE_HEAD	ph1 = &st_work.st_msg_ote_m_snd.head;

					st_mon2.wo_uni << L"[HEAD]" << L" ID:" << ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option
						<< L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port) << "\n"
						<< L"        CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid << L"\n";
					st_mon2.wo_mpc << L"[HEAD] -\n";
					st_mon2.wo_mote << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
				}
			}
			else {
				st_mon2.wo_uni	<< L"No Message";
				st_mon2.wo_mpc	<< L"No Message";
				st_mon2.wo_mote << L"No Message";
			}
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());

			//通信カウントをタイトルバーに表示
			st_mon2.wo_work.str(L""); st_mon2.wo_work << L"MKCC_IF% PC_U (R:" << rcv_count_pc_u << L" S:" << snd_count_ote_u << L")  PC_M (R:" << rcv_count_pc_m << L")  OTE_M (R:" << rcv_count_ote_m << L" S PC:" << snd_count_m2pc << L"  S OTE:" << snd_count_m2ote << L")";
			SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

			//応答遅延時間をMSGに表示
			st_mon2.wo_work.str(L""); st_mon2.wo_work << L"応答遅延(μsec) MAX:" << res_delay_max << L" MIN:" << res_delay_min;
			SetWindowText(st_mon2.hctrl[OTE_AG_ID_MON2_STATIC_MSG], st_mon2.wo_work.str().c_str());
		}

		//通信ステータス
		update_sock_stat();

	}break;
	case ID_SOCK_EVENT_OTE_UNI: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			if (S_OK == rcv_uni_ote(&(pOteCCIf->st_msg_pc_u_rcv)) ){
				st_work.id_conected_crane = (pOteCCIf->st_msg_pc_u_rcv.head.myid.serial_no & 0x0000FFFF);
			}
			else {
				st_work.id_conected_crane = CRANE_ID_NULL;
			}
			QueryPerformanceCounter(&end_count_r);    // 応答受信時のカウント数
			LONGLONG lspan = (end_count_r.QuadPart - start_count_s.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
			if (res_delay_max < lspan) res_delay_max = lspan;
			if (res_delay_min > lspan) res_delay_min = lspan;
			if (rcv_count_pc_u % 50 == 0) {
				res_delay_max = 0;
				res_delay_min = 10000000;
			}

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
		//OTE通信ヘッダに緊急停止要求有
		if (pOteCCIf->st_msg_pc_u_rcv.head.code == OTE_CODE_REQ_ESTP) {
			st_work.stop_req_mode |= OTE_STOP_REQ_MODE_ESTOP;
		}
	}break;
	case ID_SOCK_EVENT_OTE_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_ote(&st_work.st_msg_ote_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		default:break;
		}
	}break;
	case ID_SOCK_EVENT_PC_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_pc(&st_work.st_msg_pc_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		default:break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, OTE_AG_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

/// <summary>
/// SCADA パネル通信状態ランプ表示用ステータス更新関数
/// </summary>
void COteAgent::update_sock_stat() {
	INT32 sock_stat = pUSockPC->status;
	if (sock_stat & CSOCK_STAT_STANDBY) {
		st_work.cc_com_stat_r = ID_PNL_SOCK_STAT_STANDBY;
		st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_STANDBY;

		if (sock_stat & CSOCK_STAT_ACT_RCV) 
			st_work.cc_com_stat_r = ID_PNL_SOCK_STAT_ACT_RCV;
		else if (sock_stat & CSOCK_STAT_RCV_ERR) 
			st_work.cc_com_stat_r = ID_PNL_SOCK_STAT_RCV_ERR;
		else;

		if (sock_stat & CSOCK_STAT_ACT_SND) 
			st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_ACT_SND;
		else if (sock_stat & CSOCK_STAT_SND_ERR) 
			st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_SND_ERR;
		else;
	}
	else if (sock_stat == CSOCK_STAT_CLOSED)	st_work.cc_com_stat_r = st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_CLOSED;
	else if (sock_stat == CSOCK_STAT_INIT)		st_work.cc_com_stat_r = st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_INIT;
	else if (sock_stat == CSOCK_STAT_INIT_ERROR)st_work.cc_com_stat_r = st_work.cc_com_stat_s = ID_PNL_SOCK_STAT_INIT_ERROR;
	else; 

	return;
}
HWND COteAgent::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("OTE_AG_MON1");
		wcex.lpszClassName = TEXT("OTE_AG_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("OTE_AG_MON1"), TEXT("OTE_AG_MON1"), WS_OVERLAPPEDWINDOW,
			OTE_AG_MON1_WND_X, OTE_AG_MON1_WND_Y, OTE_AG_MON1_WND_W, OTE_AG_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);

		wos.str(L"");
		if (st_mon1.hwnd_mon != NULL) wos << L"Succeed : MON1 open";
		else                          wos << L"!! Failed : MON1 open";
		msg2listview(wos.str());

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
		wcex.lpszMenuName = TEXT("OTE_AG_MON2");
		wcex.lpszClassName = TEXT("OTE_AG_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("OTE_AG_MON2"), TEXT("OTE_AG_MON2"), WS_OVERLAPPEDWINDOW,
			OTE_AG_MON2_WND_X, OTE_AG_MON2_WND_Y, OTE_AG_MON2_WND_W, OTE_AG_MON2_WND_H,
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
void COteAgent::close_monitor_wnd(int id) {
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
void COteAgent::show_monitor_wnd(int id) {
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
void COteAgent::hide_monitor_wnd(int id) {
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

///###	タブパネルのListViewにメッセージを出力
void COteAgent::msg2listview(wstring wstr) {

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
void COteAgent::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void COteAgent::set_panel_tip_txt() {
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
void COteAgent::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"MKCC IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void COteAgent::set_item_chk_txt() {
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"WND");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
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
