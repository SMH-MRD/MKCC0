
#include "CSockLib.H"
#include "CCcCS.h"
#include "resource.h"
#include "framework.h"
#include "OTE_DEF.H"

//ソケット
static CSockUDP* pUSockOte;	//ユニキャストOTE通信受信用
static CSockUDP* pMSockOte;	//マルチキャストOTE通信受信用
static CSockUDP* pMSockPC;	//マルチキャストPC通信受信用

static SOCKADDR_IN addrin_pc_m2pc_snd;
static SOCKADDR_IN addrin_pc_m2ote_snd;

ST_CS_MON1 CClientService::st_mon1;
ST_CS_MON2 CClientService::st_mon2;

ST_CC_CS_INF CClientService::st_work;

//共有メモリ参照用定義
static CSharedMem* pOteInfObj;
static CSharedMem* pCraneStatObj;
static CSharedMem* pSimInfObj;
static CSharedMem* pPLCioObj;
static CSharedMem* pCSInfObj;
static CSharedMem* pAgentInfObj;
static CSharedMem* pSwayIO_Obj;

static LPST_CC_CRANE_STAT	pCraneStat;
static LPST_CC_CS_INF		pCS_Inf;
static LPST_CC_PLC_IO		pPLC_IO;
static LPST_CC_AGENT_INF	pAgent_Inf;
static LPST_CC_OTE_INF		pOTE_Inf;

static LONG rcv_count_ote_u = 0, snd_count_ote_u = 0;
static LONG rcv_count_pc_m = 0, snd_count_m2pc = 0;
static LONG rcv_count_ote_m = 0, snd_count_m2ote = 0;

/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CClientService::CClientService() {
	// 共有メモリオブジェクトのインスタンス化
	pOteInfObj		= new CSharedMem;
	pCraneStatObj	= new CSharedMem;
	pSimInfObj		= new CSharedMem;
	pPLCioObj		= new CSharedMem;
	pCSInfObj		= new CSharedMem;
	pAgentInfObj	= new CSharedMem;

}
CClientService::~CClientService() {
	// 共有メモリオブジェクトの解放
	delete pOteInfObj;
	delete pCraneStatObj;
	delete pSimInfObj;
	delete pPLCioObj;
	delete pCSInfObj ;
	delete pAgentInfObj;
	delete pSwayIO_Obj;
}

HRESULT CClientService::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_CC_CS_INF);
	if (OK_SHMEM != pOteInfObj->create_smem(SMEM_CS_INF_CC_NAME, out_size, MUTEX_CS_INF_CC_NAME)) {
		err |= SMEM_NG_CS_INF;
	}
	set_outbuf(pCSInfObj->get_pMap());

	//### 入力用共有メモリ取得
	if (OK_SHMEM != pSimInfObj->create_smem(SMEM_SIM_INF_CC_NAME, sizeof(ST_SIM_INF_CC), MUTEX_SIM_INF_CC_NAME)) {
		err |= SMEM_NG_SIM_INF;	hr = S_FALSE;
	}
	if (OK_SHMEM != pCraneStatObj->create_smem(SMEM_CRANE_STAT_CC_NAME, sizeof(ST_CC_CRANE_STAT), MUTEX_CRANE_STAT_CC_NAME)) {
		err |= SMEM_NG_CRANE_STAT; hr = S_FALSE;
	}
	if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_CC_PLC_IO), MUTEX_PLC_IO_NAME)) {
		err |= SMEM_NG_PLC_IO; hr = S_FALSE;
	}
	if (OK_SHMEM != pCSInfObj->create_smem(SMEM_CS_INF_CC_NAME, sizeof(ST_CC_CS_INF), MUTEX_CS_INF_CC_NAME)) {
		err |= SMEM_NG_CS_INF; hr = S_FALSE;
	}
	if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INF_CC_NAME, sizeof(ST_CC_AGENT_INF), MUTEX_AGENT_INF_CC_NAME)) {
		err |= SMEM_NG_AGENT_INF; hr = S_FALSE;
	}

	pCraneStat	= (LPST_CC_CRANE_STAT)(pCraneStatObj->get_pMap());
	pPLC_IO		= (LPST_CC_PLC_IO)(pPLCioObj->get_pMap());
	pOTE_Inf	= (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	pCS_Inf		= (LPST_CC_CS_INF)pCSInfObj->get_pMap();
	pAgent_Inf	= (LPST_CC_AGENT_INF)pAgentInfObj->get_pMap();

	if((pCraneStat == NULL) || (pPLC_IO == NULL) || (pOTE_Inf == NULL) || (pCS_Inf == NULL) || (pAgent_Inf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//### IFウィンドウOPEN
	WPARAM wp = MAKELONG(inf.index,WM_USER_WPH_OPEN_IF_WND);//HWORD:コマンドコード, LWORD:タスクインデックス
	LPARAM lp = BC_ID_MON2;
	SendMessage(inf.hwnd_opepane, WM_USER_TASK_REQ, wp, lp);
	
	//### 通信ソケットアドレスセット
	//##インスタンス生成
	pUSockOte = new CSockUDP(ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI);//#OTEユニキャスト受信
	pMSockPC = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MUL);	//#PCマルチキャスト受信
	pMSockOte = new CSockUDP(ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MUL);//#OTEマルチキャスト受信

	//受信アドレス
	pUSockOte->set_sock_addr(&pUSockOte->addr_in_rcv, OTE_IF_UNI_IP_PC, OTE_IF_UNI_PORT_PC);
	pMSockPC->set_sock_addr(&pMSockPC->addr_in_rcv,   OTE_IF_UNI_IP_PC, OTE_IF_MULTI_PORT_PC2PC);//受信アドレス
	pMSockOte->set_sock_addr(&pMSockOte->addr_in_rcv, OTE_IF_UNI_IP_PC, OTE_IF_MULTI_PORT_OTE2PC);//受信アドレス
	
	//送信先アドレス
	pUSockOte->set_sock_addr(&(pUSockOte->addr_in_dst), OTE_IF_UNI_IP_OTE0, OTE_IF_UNI_PORT_OTE);
	pMSockPC->set_sock_addr(&addrin_pc_m2pc_snd,  OTE_IF_MULTI_IP_PC, OTE_IF_MULTI_PORT_PC2PC);
	pMSockPC->set_sock_addr(&addrin_pc_m2ote_snd, OTE_IF_MULTI_IP_PC, OTE_IF_MULTI_PORT_PC2OTE);
	
	//### 通信ソケット生成/初期化
	//##WSA初期化
	wos.str(L"");
	if (pUSockOte->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pUSockOte->err_msg.str(); err |= SOCK_NG_UNICAST;   hr = S_FALSE;}
	if (pMSockPC->Initialize()  != S_OK) { wos << L"Err(IniWSA):" << pMSockPC->err_msg.str();  err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if (pMSockOte->Initialize() != S_OK) { wos << L"Err(IniWSA):" << pMSockOte->err_msg.str(); err |= SOCK_NG_MULTICAST; hr = S_FALSE;}
	if(hr==S_FALSE)msg2listview(wos.str()); wos.str(L"");

	//##ソケットソケット生成・設定
	//##ユニキャスト
	if (pUSockOte->init_sock(st_mon2.hwnd_mon, pUSockOte->addr_in_rcv) != S_OK) {//init_sock():bind()→非同期化まで実施
		wos << L"PC U SockErr:" << pUSockOte->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
	}
	else wos << L"OTE U Socket init OK";msg2listview(wos.str()); wos.str(L"");
	
	//#マルチキャスト
	SOCKADDR_IN addr_buf;
	pMSockPC->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_PC, NULL);//PCマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockPC->init_sock(st_mon2.hwnd_mon, pMSockPC->addr_in_rcv, addr_buf) != S_OK) {//init_sock_m():bind()まで実施 + マルチキャストグループへ登録
		wos << L"PC M SockErr:"<< pMSockPC->err_msg.str(); hr = S_FALSE;
	}
	else wos << L"PC M Socket init OK";	msg2listview(wos.str()); wos.str(L"");

	pMSockOte->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_OTE, NULL);//OTEマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockOte->init_sock(st_mon2.hwnd_mon, pMSockOte->addr_in_rcv, addr_buf) != S_OK) {
		wos << L"OTE M SockErr:" << pMSockOte->err_msg.str(); hr = S_FALSE;
	}	
	else  wos << L"OTE M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	pOTE_Inf->st_msg_pc_u_snd.head.addr = pUSockOte->addr_in_rcv;
	pOTE_Inf->st_msg_pc_m_snd.head.addr = pMSockOte->addr_in_rcv;

	if (hr == S_FALSE) {
		pUSockOte->Close();				//ソケットクローズ
		pMSockPC->Close();				//ソケットクローズ
		pMSockOte->Close();				//ソケットクローズ
		close_monitor_wnd(BC_ID_MON2);	//通信モニタクローズ
		wos.str(L""); wos << L"Initialize : SOCKET NG"; msg2listview(wos.str());
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"OTE IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ２CB状態セット	
	if(st_mon2.hwnd_mon !=NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

HRESULT CClientService::routine_work(void* pObj) {
	input();
	parse();
	output();
	return S_OK;
}

int CClientService::input() {


	return S_OK;
}

int CClientService::parse() {

	return S_OK;
}

int CClientService::close() {

	return 0;
}

/****************************************************************************/
/*   通信関数											                    */
/****************************************************************************/
/// <summary>
/// OTEユニキャスト電文受信処理
/// </summary>
HRESULT CClientService::rcv_uni_ote(LPST_OTE_U_MSG pbuf) {
	int nRtn = pUSockOte->rcv_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PCマルチキャスト電文受信処理 
/// </summary>
HRESULT CClientService::rcv_mul_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pMSockPC->rcv_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc << L"ERR rcv:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_pc_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// OTEマルチキャスト電文受信処理 
/// </summary>
HRESULT CClientService::rcv_mul_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pMSockOte->rcv_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR rcv:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
			return S_FALSE;
		}
	}
	rcv_count_ote_m++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PCユニキャスト電文送信処理 
/// </summary>
LPST_PC_U_MSG CClientService::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
	return &pOTE_Inf->st_msg_pc_u_snd;
}

HRESULT CClientService::snd_uni2ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	if (pUSockOte->snd_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR snd:" << pUSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PCマルチキャスト電文送信処理 
/// </summary>

//マルチキャストメッセージセット
LPST_PC_M_MSG CClientService::set_msg_m(INT32 code, INT32 stat) {	
	return &pOTE_Inf->st_msg_pc_m_snd;
}

//PCへ送信
HRESULT CClientService::snd_mul2pc(LPST_PC_M_MSG pbuf) {
	if (pMSockPC->snd_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m2pc_snd) == SOCKET_ERROR) {
	
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_mpc.str(L""); st_mon2.wo_mpc << L"ERR snd:" << pMSockPC->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2pc++;
	return S_OK;
}

//OTEへ送信
HRESULT CClientService::snd_mul2ote(LPST_PC_M_MSG pbuf) {
	if (pMSockOte->snd_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m2ote_snd) == SOCKET_ERROR) {

		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
			st_mon2.wo_mote.str(L""); st_mon2.wo_mote << L"ERR snd:" << pMSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}
		return S_FALSE;
	}
	snd_count_m2ote++;
	return S_OK;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/

LRESULT CALLBACK CClientService::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[CS_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[CS_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[CS_ID_MON1_STATIC_1].x, st_mon1.pt[CS_ID_MON1_STATIC_1].y,
			st_mon1.sz[CS_ID_MON1_STATIC_1].cx, st_mon1.sz[CS_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(CS_ID_MON1_CTRL_BASE + CS_ID_MON1_STATIC_1), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, CS_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, CS_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

static wostringstream mon2wos;
LRESULT CALLBACK CClientService::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = CS_ID_MON2_STATIC_UNI; i <= CS_ID_MON2_STATIC_MSG; i++){
		st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon2.pt[i].x, st_mon2.pt[i].y,st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (int i = CS_ID_MON2_RADIO_RCV; i <= CS_ID_MON2_RADIO_INFO; i++) {
			if (i == CS_ID_MON2_RADIO_RCV) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);

				st_mon2.sock_inf_id = CS_ID_MON2_RADIO_RCV;
				SendMessage(st_mon2.hctrl[i], BM_SETCHECK, BST_CHECKED, 0L);
			}
			else
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
					hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		UINT rtn = SetTimer(hWnd, CS_ID_MON2_TIMER, CS_PRM_MON2_TIMER_MS,NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - CS_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{

		case CS_ID_MON2_RADIO_RCV: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_RCV;
		}break;
		case CS_ID_MON2_RADIO_SND: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_SND;
		}break;
		case CS_ID_MON2_RADIO_INFO: {
			st_mon2.sock_inf_id = CS_ID_MON2_RADIO_INFO;
		}break;
		default:
			return DefWindowProc(hWnd, msg, wp, lp);
		}
	}break;
	case WM_TIMER: {
		//UniCast送信
			//折り返し送信
		//MultiCast送信
		snd_mul2pc(set_msg_m(0,0));		//PCへ送信
		snd_mul2ote(set_msg_m(0, 0));	//OTEへ送信

		//通信カウントをタイトルバーに表示
		st_mon2.wo_work.str(L""); st_mon2.wo_work << L"MKCC_IF% PC_U (R:" << rcv_count_ote_u << L" S:" << snd_count_ote_u << L")  PC_M (R:" << rcv_count_pc_m << L")  OTE_M (R:" << rcv_count_ote_m << L" S PC:" << snd_count_m2pc << L"  S OTE:" << snd_count_m2ote << L")";
		SetWindowText(st_mon2.hwnd_mon, st_mon2.wo_work.str().c_str());

		//モニター表示
		if (st_mon2.is_monitor_active) {
			SOCKADDR_IN	addr;
			if (pUSockOte != NULL) {
				addr = pUSockOte->addr_in_rcv; st_mon2.wo_work.str(L"");
				st_mon2.wo_work <<L"UNI>>IP R:"<<addr.sin_addr.S_un.S_un_b.s_b1<< L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
								<< htons(addr.sin_port) << L" ";
				addr = pUSockOte->addr_in_dst;
				st_mon2.wo_work << L" S:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				addr = pUSockOte->addr_in_from; ;
				st_mon2.wo_work << L" F:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
					<< htons(addr.sin_port) << L" ";
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_UNI], st_mon2.wo_work.str().c_str());
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
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_MUL_PC], st_mon2.wo_work.str().c_str());
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
				SetWindowText(st_mon2.hctrl[CS_ID_MON2_LABEL_MUL_OTE], st_mon2.wo_work.str().c_str());
			}
			
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc.str(L""); st_mon2.wo_mote.str(L"");
			if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {

				LPST_OTE_HEAD	ph0 = &pOTE_Inf->st_msg_ote_u_rcv.head;
				LPST_OTE_U_BODY pb0 = &pOTE_Inf->st_msg_ote_u_rcv.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:"<<ph0->code << L"\n";
				st_mon2.wo_uni << L"[BODY]" << L"OPEMODE:"<<pb0->ope_mode;

				LPST_OTE_HEAD  ph1 = &pOTE_Inf->st_msg_pc_m_rcv.head;
				LPST_PC_M_BODY pb1 = &pOTE_Inf->st_msg_pc_m_rcv.body;
				st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
				st_mon2.wo_mpc << L"[BODY]";

				LPST_OTE_HEAD  ph2 = &pOTE_Inf->st_msg_ote_m_rcv.head;
				LPST_OTE_M_BODY pb2 = &pOTE_Inf->st_msg_ote_m_rcv.body;
				st_mon2.wo_mote << L"[HEAD]" << L"CODE:" << ph2->code << L"\n";
				st_mon2.wo_mote << L"[BODY]";
			}
			else if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {

				LPST_OTE_HEAD	ph0 = &pOTE_Inf->st_msg_pc_u_snd.head;
				LPST_PC_U_BODY  pb0 = &pOTE_Inf->st_msg_pc_u_snd.body;
				st_mon2.wo_uni << L"[HEAD]" << L"CODE:" << ph0->code << L"\n";
				st_mon2.wo_uni << L"[BODY]";

				LPST_OTE_HEAD  ph1 = &pOTE_Inf->st_msg_pc_m_snd.head;
				LPST_PC_M_BODY pb1 = &pOTE_Inf->st_msg_pc_m_snd.body;
				st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
				st_mon2.wo_mpc << L"[BODY]";

				st_mon2.wo_mote << L"[HEAD] -\n";
				st_mon2.wo_mote << L"[BODY] -";
			}
			else {
				st_mon2.wo_uni  << L"No Message"; 
				st_mon2.wo_mpc  << L"No Message"; 
				st_mon2.wo_mote << L"No Message"; 
			}
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_PC], st_mon2.wo_mpc.str().c_str());
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_MUL_OTE], st_mon2.wo_mote.str().c_str());
		}

	}break;

	case ID_SOCK_EVENT_OTE_UNI: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			//OTEからのユニキャストメッセージ受信
			if(rcv_uni_ote(&pOTE_Inf->st_msg_ote_u_rcv) == S_OK){
				//折り返しアンサバック 送信元へ返送
				pOTE_Inf->addr_in_from_oteu = pUSockOte->addr_in_from;
				pUSockOte->addr_in_dst.sin_family = AF_INET;
				pUSockOte->addr_in_dst.sin_port = htons(OTE_IF_UNI_PORT_OTE);
				pUSockOte->addr_in_dst.sin_addr = pUSockOte->addr_in_from.sin_addr;

				if (pOTE_Inf->id_ope_active == OTE_NON_OPEMODE_ACTIVE) //操作モードの端末無
					snd_uni2ote(set_msg_u(true, 0, pOTE_Inf->id_ope_active), &pUSockOte->addr_in_dst);
				else 
					snd_uni2ote(set_msg_u(false, 0, pOTE_Inf->id_ope_active), &pUSockOte->addr_in_dst);
			}

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}

		//OTE通信ヘッダに緊急停止要求有
		if (pOTE_Inf->st_msg_ote_u_rcv.head.code == OTE_CODE_REQ_ESTP) {
			pOTE_Inf->stop_req_mode |= OTE_STOP_REQ_MODE_ESTOP;
		}

	}break;

	case ID_SOCK_EVENT_OTE_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_ote(&pOTE_Inf->st_msg_ote_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;
	case ID_SOCK_EVENT_PC_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_pc(&pOTE_Inf->st_msg_pc_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, CS_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}

HWND CClientService::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("CS_MON1");
		wcex.lpszClassName = TEXT("CS_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("CS_MON1"), TEXT("CS_MON1"), WS_OVERLAPPEDWINDOW,
			CS_MON1_WND_X, CS_MON1_WND_Y, CS_MON1_WND_W, CS_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("CS_MON2");
		wcex.lpszClassName = TEXT("CS_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("CS_MON2"), TEXT("CS_MON2"), WS_OVERLAPPEDWINDOW,
			CS_MON2_WND_X, CS_MON2_WND_Y, CS_MON2_WND_W, CS_MON2_WND_H,
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
void CClientService::close_monitor_wnd(int id) {
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
void CClientService::show_monitor_wnd(int id) {
	if ((id == BC_ID_MON1)&&(st_mon1.hwnd_mon != NULL)) {
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
void CClientService::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CClientService::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, lp);	
		}
		else if (wp == WM_USER_WPH_CLOSE_IF_WND) close_monitor_wnd(lp);	
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
//				close_monitor_wnd(BC_ID_MON2);
				hide_monitor_wnd(BC_ID_MON2);
			}
		}break;
		}
	}
	return 0;
};

///###	タブパネルのListViewにメッセージを出力
void CClientService::msg2listview(wstring wstr) {

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
void CClientService::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CClientService::set_panel_tip_txt() {
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
void CClientService::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"OTE IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CClientService::set_item_chk_txt() {
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





