
#include "CSockLib.H"
#include "CCcCS.h"
#include "resource.h"
#include "framework.h"
#include "OTE_DEF.H"
#include "CCrane.H"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;

extern CSharedMem* pAuxInfObj;

extern CCrane* pCrane;
extern ST_DEVICE_CODE g_my_code;


//ソケット
static CSockUDP* pUSockOte;	//ユニキャストOTE通信受信用
static CSockUDP* pMSockOte;	//マルチキャストOTE通信受信用
static CSockUDP* pMSockPC;	//マルチキャストPC通信受信用

static SOCKADDR_IN addrin_pc_m2pc_snd;
static SOCKADDR_IN addrin_pc_m2ote_snd;

ST_CS_MON1 CCcCS::st_mon1;
ST_CS_MON2 CCcCS::st_mon2;

ST_CC_CS_INF CCcCS::st_cs_work;
ST_CC_OTE_INF CCcCS::st_ote_work;

//共有メモリ
static LPST_CC_ENV_INF		pEnv_Inf	= NULL;
static LPST_CC_CS_INF		pCS_Inf		= NULL;
static LPST_CC_PLC_IO		pPLC_IO		= NULL;
static LPST_CC_AGENT_INF	pAgent_Inf	= NULL;
static LPST_CC_OTE_INF		pOTE_Inf	= NULL;
static LPST_CC_POL_INF		pPolInf		= NULL;

static LPST_AUX_CS_INF		pAUX_CS_Inf = NULL;	

static ST_PLC_IO_WIF* pPlcWIf = NULL;
static ST_PLC_IO_RIF* pPlcRIf = NULL;

static LONG rcv_count_ote_u = 0, snd_count_ote_u = 0;
static LONG rcv_count_pc_m = 0, snd_count_m2pc = 0;
static LONG rcv_count_ote_m = 0, snd_count_m2ote = 0;

/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CCcCS::CCcCS() {

}
CCcCS::~CCcCS() {

}

HRESULT CCcCS::initialize(LPVOID lpParam) {
	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_CC_CS_INF);

	set_outbuf(pCsInfObj->get_pMap());

	//### 共有メモリ
	pEnv_Inf	= (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPLC_IO		= (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pOTE_Inf	= (LPST_CC_OTE_INF)pOteInfObj->get_pMap();
	pCS_Inf		= (LPST_CC_CS_INF)pCsInfObj->get_pMap();
	pAgent_Inf	= (LPST_CC_AGENT_INF)pAgInfObj->get_pMap();
	pPolInf		= (LPST_CC_POL_INF)(pPolInfObj->get_pMap());

	pAUX_CS_Inf = (LPST_AUX_CS_INF)pAuxInfObj->get_pMap();

	//### クレーンオブジェクト取得
	if (pCrane == NULL) {
		wos.str(L""); wos << L"Initialize : CraneObject NULL"; msg2listview(wos.str());
		return S_FALSE;
	}
	else {
		pPlcWIf = &(pCrane->pPlc->plc_io_wif);
		pPlcRIf = &(pCrane->pPlc->plc_io_rif);
	}

	if((pEnv_Inf == NULL) || (pPLC_IO == NULL) || (pOTE_Inf == NULL) || (pCS_Inf == NULL) || (pAgent_Inf == NULL))
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

	if (st_mon2.hwnd_mon == NULL) {
		wos << L"Err(MON2 NULL Handle!!):";
		msg2listview(wos.str()); wos.str(L"");
		return hr;
	}

	//##ソケットソケット生成・設定
	//##ユニキャスト
	if (pUSockOte->init_sock(st_mon2.hwnd_mon, pUSockOte->addr_in_rcv) != S_OK) {//init_sock():bind()→非同期化まで実施
		wos << L"OTE U SockErr:" << pUSockOte->err_msg.str(); err |= SOCK_NG_UNICAST; hr = S_FALSE;
		pUSockOte = NULL;
	}
	else wos << L"OTE U Socket init OK";msg2listview(wos.str()); wos.str(L"");
	
	//#マルチキャスト
	SOCKADDR_IN addr_buf;
	pMSockPC->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_PC, NULL);//PCマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockPC->init_sock(st_mon2.hwnd_mon, pMSockPC->addr_in_rcv, addr_buf) != S_OK) {//init_sock_m():bind()まで実施 + マルチキャストグループへ登録
		wos << L"PC M SockErr:"<< pMSockPC->err_msg.str(); hr = S_FALSE;
		pMSockPC = NULL;
	}
	else wos << L"PC M Socket init OK";	msg2listview(wos.str()); wos.str(L"");

	pMSockOte->set_sock_addr(&addr_buf, OTE_IF_MULTI_IP_OTE, NULL);//OTEマルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pMSockOte->init_sock(st_mon2.hwnd_mon, pMSockOte->addr_in_rcv, addr_buf) != S_OK) {
		wos << L"OTE M SockErr:" << pMSockOte->err_msg.str(); hr = S_FALSE;
		pMSockOte = NULL;
	}	
	else  wos << L"OTE M Socket init OK"; msg2listview(wos.str()); wos.str(L"");

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	st_ote_work.st_msg_pc_u_snd.head.addr = pUSockOte->addr_in_rcv;
	st_ote_work.st_msg_pc_m_snd.head.addr = pMSockOte->addr_in_rcv;

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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"CS_INF");
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

HRESULT CCcCS::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

int CCcCS::input() {

	return S_OK;
}

int CCcCS::parse() {
	//#### 制御状態監視
		//### 操作有効端末通信途切れカウンタ　上限まで周期毎カウントアップ　カウントは操作有効端末有でクリア
	if (!(st_ote_work.ope_ote_silent_cnt & 0xFFFFFF00)) st_ote_work.ope_ote_silent_cnt++;

	//### 操作有効端末有無判定　異常,警報フラグ設定
	if (st_ote_work.st_ote_ctrl.id_ope_active) {
		st_cs_work.cs_ctrl.ope_pnl_status = CC_CS_CODE_OPEPNL_ACTIVE;
		pPolInf->pc_fault_map[FLTS_ID_RMT_OPE_DEACTIVE] &= ~FLTS_MASK_RMT_OPE_DEACTIVE;
	}
	else {
		st_cs_work.cs_ctrl.ope_pnl_status = CC_CS_CODE_OPEPNL_DEACTIVE;
		pPolInf->pc_fault_map[FLTS_ID_RMT_OPE_DEACTIVE] |= FLTS_MASK_RMT_OPE_DEACTIVE;
	}

	//### OTE通信異常フラグ設定
	if (pUSockOte == NULL)	st_ote_work.err_ote_comm |= CODE_CC_CS_OTE_COM_ERR_SOCK;	//ソケット生成失敗
	else					st_ote_work.err_ote_comm &= ~CODE_CC_CS_OTE_COM_ERR_SOCK;
	//受信、送信エラーは通信ウィンドウ処理部でフラグセット,リセット
	//CC_POL管理共有メモリにセット
	if(st_ote_work.err_ote_comm) pPolInf->pc_fault_map[FLTS_ID_ERR_CPC_RPC_COMM] |= FLTS_ID_ERR_CPC_RPC_COMM;
	else                         pPolInf->pc_fault_map[FLTS_ID_ERR_CPC_RPC_COMM] &= ~FLTS_ID_ERR_CPC_RPC_COMM;
	
	//### OTE送信データ設定
		//## st_ote_work.st_bodyの内容が送信バッファにコピーされる
		//## ランプ表示指令
	{
		UN_LAMP_COM* plamp_com = st_ote_work.st_body.lamp;
		if (!pPLC_IO->plc_enable) {	//PLC通信無効で操作関連モードクリア
			st_ote_work.st_ote_ctrl.id_ope_active = OTE_NON_OPEMODE_ACTIVE;
			st_ote_work.st_ote_ctrl.gpad_mode = L_OFF;
			st_ote_work.st_ote_ctrl.auto_sel = L_OFF;
			st_ote_work.st_ote_ctrl.auto_mode = L_OFF;
			//ランプクリア
			memset(plamp_com, 0, sizeof(UN_LAMP_COM) * N_OTE_PNL_CTRL);
		}
		else {
			//# PLC受信バッファをコピー
			memcpy(st_ote_work.st_body.buf_io_read, pPLC_IO->buf_io_read, sizeof(UN_PLC_RBUF));

			//クレーンオブジェクトからPLCIFバッファの信号読み取り⇒ランプ出力
			plamp_com[OTE_PNL_CTRLS::estop].st.com = (UINT8)pCrane->pPlc->rval(pPlcRIf->estop).i16;

			if (pCrane->pPlc->rval(pPlcRIf->syukan_mc_comp).i16) {
				plamp_com[OTE_PNL_CTRLS::syukan_on].st.com = CODE_PNL_COM_OFF;
				plamp_com[OTE_PNL_CTRLS::syukan_off].st.com = CODE_PNL_COM_ON;
			}
			else {
				plamp_com[OTE_PNL_CTRLS::syukan_on].st.com = CODE_PNL_COM_ON;
				plamp_com[OTE_PNL_CTRLS::syukan_off].st.com = CODE_PNL_COM_OFF;
			}
			pCrane->pPlc->rval(pPlcRIf->syukan_on).i16;
			pCrane->pPlc->rval(pPlcRIf->syukan_off).i16;

			plamp_com[OTE_PNL_CTRLS::fault_reset].st.com = (UINT8)pCrane->pPlc->rval(pPlcRIf->fault_reset_pb).i16;
			plamp_com[OTE_PNL_CTRLS::bypass].st.com = CODE_PNL_COM_ON;

			//#PLC側CSスイッチの状態
			plamp_com[OTE_PNL_CTRLS::mh_spd_mode].st.com	= (UINT8)pPLC_IO->stat_mh.mode;
			plamp_com[OTE_PNL_CTRLS::bh_r_mode].st.com		= (UINT8)pPLC_IO->stat_bh.mode;
			//#自動給脂　動力確立ランプ
			plamp_com[OTE_PNL_CTRLS::main_power].st.com = (UINT8)pCrane->pPlc->rval(pPlcRIf->douryoku_ok).i16;
			plamp_com[OTE_PNL_CTRLS::sl_auto_gr].st.com = (UINT8)pCrane->pPlc->rval(pPlcRIf->auto_kyusi).i16;

			if(pPLC_IO->plc_pnl_io_fb[OTE_PNL_CTRLS::motor_siren])	plamp_com[OTE_PNL_CTRLS::motor_siren].st.com = L_ON;
			else                                                    plamp_com[OTE_PNL_CTRLS::motor_siren].st.com = L_OFF;
			
			if (pPLC_IO->plc_pnl_io_fb[OTE_PNL_CTRLS::hd_lamp1])	plamp_com[OTE_PNL_CTRLS::hd_lamp1].st.com = L_ON;
			else                                                    plamp_com[OTE_PNL_CTRLS::hd_lamp1].st.com = L_OFF;

			if (pPLC_IO->plc_pnl_io_fb[OTE_PNL_CTRLS::hd_lamp2])	plamp_com[OTE_PNL_CTRLS::hd_lamp2].st.com = L_ON;
			else                                                    plamp_com[OTE_PNL_CTRLS::hd_lamp2].st.com = L_OFF;

			if (pPLC_IO->plc_pnl_io_fb[OTE_PNL_CTRLS::hd_lamp3])	plamp_com[OTE_PNL_CTRLS::hd_lamp3].st.com = L_ON;
			else                                                    plamp_com[OTE_PNL_CTRLS::hd_lamp3].st.com = L_OFF;

			//#ノッチ信号FB
			plamp_com[OTE_PNL_CTRLS::notch_mh].st.com = pPLC_IO->stat_mh.notch_ref;
			plamp_com[OTE_PNL_CTRLS::notch_bh].st.com = pPLC_IO->stat_bh.notch_ref;
			plamp_com[OTE_PNL_CTRLS::notch_sl].st.com = pPLC_IO->stat_sl.notch_ref;
			plamp_com[OTE_PNL_CTRLS::notch_gt].st.com = pPLC_IO->stat_gt.notch_ref;
		}

		//##　故障情報セット
		set_ote_flt_info();

		//## クレーン状態セット
		st_ote_work.st_body.st_load_stat[0].m		= (float)pEnv_Inf->crane_stat.m;								//荷重
		st_ote_work.st_body.bh_angle				= (float)pEnv_Inf->crane_stat.th.p;									//起伏角度


		//## 各軸状態
		st_ote_work.st_body.st_axis_set[ID_HOIST]	= pPLC_IO->stat_mh;	//主巻
		st_ote_work.st_body.st_axis_set[ID_BOOM_H]	= pPLC_IO->stat_bh;	//引込
		st_ote_work.st_body.st_axis_set[ID_SLEW]	= pPLC_IO->stat_sl;	//旋回
		st_ote_work.st_body.st_axis_set[ID_GANTRY]	= pPLC_IO->stat_gt;	//走行

		//## 旋回ブレーキFB
		st_ote_work.st_body.sl_brk_fb[0]			= pAUX_CS_Inf->fb_slbrk.d16;
		st_ote_work.st_body.sl_brk_fb[1]			= pAUX_CS_Inf->fb_slbrk.d17;
		st_ote_work.st_body.sl_brk_fb[2]			= pAUX_CS_Inf->fb_slbrk.d18;
		st_ote_work.st_body.sl_brk_fb[3]			= pAUX_CS_Inf->fb_slbrk.d19 / 600;//9000->15
}

	return S_OK;
}
int CCcCS::output() {          //出力処理
	//共有メモリ出力処理
	//CS_INF 制御コントロール情報（モード等）
	
	//### CCCsで判定した内容を共有メモリへコピー
	memcpy_s(&(pCS_Inf->cs_ctrl), sizeof(ST_CC_CS_CTRL), &st_cs_work.cs_ctrl, sizeof(ST_CC_CS_CTRL));
	
	//### OTEからの接続情報（接続要求内容,モード等）を共有メモリへセット
	memcpy_s(&(pOTE_Inf->st_ote_ctrl), sizeof(ST_CC_OTE_CTRL), &st_ote_work.st_ote_ctrl, sizeof(ST_CC_OTE_CTRL));
	
	//### 旋回ブレーキ操作信号設定をAUXプロセスへの出力用共有メモリへセット
	pAUX_CS_Inf->com_slbrk.pc_com_brk_level = pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::sl_brk] & 0x000f;
	pAUX_CS_Inf->com_slbrk.pc_com_hw_brk = pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::sl_brk] & BIT4;
	pAUX_CS_Inf->com_slbrk.pc_com_reset = pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::sl_brk] & BIT5;
		
	return STAT_OK;
}

int CCcCS::close() {

	return 0;
}

/****************************************************************************/
/*   通信関数											                    */
/****************************************************************************/
void CCcCS::set_ote_flt_info() {
	INT16 com_ote;
	
	//故障表示内容要求コード設定(OTEからの受信データ)
	if (st_ote_work.st_ote_ctrl.id_ope_active == OTE_NON_OPEMODE_ACTIVE) {//操作権獲得端末が無い時はモニタモードの端末の要求を受付
		com_ote = pOTE_Inf->st_msg_ote_u_rcv_mon.body.st.faults_disp_req;
	}
	else {
		com_ote = pOTE_Inf->st_msg_ote_u_rcv.body.st.faults_disp_req;
	}

	if (ote_disp_com_hold != com_ote) {//要求内容が変わったらマスク更新
		for (int i = 0; i < N_PLC_FAULT_BUF; i++) {
			disp_mask[i] = 0;
			if (com_ote & FAULT_CLEAR)break; //クリア要求なら全てクリア
			if (com_ote & FAULT_HEAVY1) disp_mask[i] |= pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::HEVY1][i];
			if (com_ote & FAULT_HEAVY2) disp_mask[i] |= pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::HEVY2][i];
			if (com_ote & FAULT_HEAVY3) disp_mask[i] |= pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::HEVY3][i];
			if (com_ote & FAULT_LIGHT) disp_mask[i] |= pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::LIGHT][i];
			if (com_ote & FAULT_INTERLOCK) disp_mask[i] |= pCrane->pFlt->flt_list.plc_fault_mask[FAULT_TYPE::IL][i];
		}
		st_ote_work.st_body.faults_set.set_type = com_ote;//表示要求タイプアンサバックセット
	}

	int flt_count = 0;	//表示故障数
	INT16 i16work;
	if (com_ote & FAULT_HISTORY) {	//履歴表示要求時
		i16work = pEnv_Inf->crane_stat.fault_list.iw_history ;	//履歴書き込みポインタ
		for (int i = 0; i < N_OTE_PC_SET_PLC_FLT;i++) {
			i16work--;
			if (i16work < 0) i16work= N_FAULTS_HISTORY_BUF-1;
			st_ote_work.st_body.faults_set.codes_plc[i] = pEnv_Inf->crane_stat.fault_list.history[i16work].code;	//履歴コードセット
			st_ote_work.st_body.faults_set.codes_plc[i] *= pEnv_Inf->crane_stat.fault_list.history[i16work].status;	//クリアは-コード
		
		}
		flt_count = N_OTE_PC_SET_PLC_FLT + N_OTE_PC_SET_PC_FLT;
	}
	else {							//現在発生分要求時
		for (int i = 0; i < N_PLC_FAULT_BUF; i++) {//PLC IOの故障バッファ数ループ
			i16work = disp_mask[i] & pEnv_Inf->crane_stat.fault_list.faults_detected_map[FAULT_TYPE::BASE][i];

			//表示カウント数オーバーまたは故障無しでスキップ
			if ((flt_count >= N_OTE_PC_SET_PLC_FLT) || (i16work == 0)) continue;

			for (int j = 0; j < 16; j++) {
				if (flt_count >= N_OTE_PC_SET_PLC_FLT) break;	//表示故障数上限

				if (i16work & (1 << j)) {	//検出ありの時
					st_ote_work.st_body.faults_set.codes_plc[flt_count] = 16 * i + j;
					//最初の故障0bit目の故障コードが301なのでリスト参照用に+１を入れる
					st_ote_work.st_body.faults_set.codes_plc[flt_count]++;
					flt_count++;
				}
			}
		}
		st_ote_work.st_body.faults_set.set_plc_count = flt_count;	//表示PLC故障数セット

		st_ote_work.st_body.faults_set.set_pc_count = 0;

		if (com_ote & FAULT_PC_CTRL) {//PC制御系故障表示要求時
			flt_count = 0;	//PC故障数カウント用クリア
			for (int i = 0; i < N_PC_FAULT_BUF; i++) {//PCの故障バッファ数ループ

				i16work = pPolInf->pc_fault_map[i];
				//表示カウント数オーバーまたは故障無しでスキップ
				if ((flt_count >= N_OTE_PC_SET_PC_FLT) || (i16work == 0)) continue;


				for (int j = 0; j < 16; j++) {
					if (flt_count >= N_OTE_PC_SET_PC_FLT) break;	//表示故障数上限

					if (i16work & (1 << j)) {	//検出ありの時
						st_ote_work.st_body.faults_set.codes_pc[flt_count] = 16 * i + j;
						//最初の故障0bit目の故障コードが550なのでリスト参照用に+550を入れる
						st_ote_work.st_body.faults_set.codes_pc[flt_count]+= N_PC_FLT_CODE_OFFSET;
						flt_count++;
						st_ote_work.st_body.faults_set.set_pc_count++;	//PC故障数カウントアップ
					}
				}
			}
			st_ote_work.st_body.faults_set.set_pc_count = flt_count;	//PC故障数セット
		}
	}
	//st_ote_work.st_body.faults_set.set_plc_count = flt_count;	//表示故障数セット
	ote_disp_com_hold = com_ote;						//表示内容要求前回値保持

	return; 
}


/// <summary>
/// OTEユニキャスト電文受信処理
/// </summary>
 
static ST_OTE_U_MSG chkbuf_u_msg;
HRESULT CCcCS::rcv_uni_ote(LPST_OTE_U_MSG pbuf) {
	//# 操作有効端末との通信断で有効端末クリア
	if (st_ote_work.ope_ote_silent_cnt > OTE_IF_RELEASE_COUNTUP) {
		st_ote_work.st_ote_ctrl.id_ope_active = OTE_NON_OPEMODE_ACTIVE;			//保持IPアドレスクリア
		st_ote_work.st_ote_ctrl.addr_in_active_ote.sin_addr.S_un.S_addr = 0;	//保持IPアドレスクリア
		st_ote_work.st_ote_ctrl.active_ote_type = OTE_STAT_TYPE_UNKOWN;
	}

	//# 一旦チェックバッファに受信(操作有効/無効端末切り分けの為）
	if (SOCKET_ERROR == pUSockOte->rcv_msg((char*)(&chkbuf_u_msg), sizeof(ST_PC_U_MSG))) {
		if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
			st_mon2.wo_uni.str(L""); st_mon2.wo_uni << L"ERR rcv:" << pUSockOte->err_msg.str();
			SetWindowText(st_mon2.hctrl[CS_ID_MON2_STATIC_UNI], st_mon2.wo_uni.str().c_str());
			return S_FALSE;
		}
	}

	//# 操作有効/無効切り分け処理
	//## 受信電文ヘッダ部に操作モード接続要求があれば登録、なければモニタ用バッファに受信電文コピー
	if (st_ote_work.st_ote_ctrl.id_ope_active == OTE_NON_OPEMODE_ACTIVE) {	//操作有効端末無し
		//送信元の端末がモニタモードから操作モードへの要求有で端末セット
		if ((chkbuf_u_msg.head.status == OTE_STAT_MODE_OPE) && (chkbuf_u_msg.head.code == OTE_CODE_REQ_OPE_ACTIVE)
			&& pPLC_IO->plc_enable) {//送信元がリモート要求でPLCヘルシーOK
			st_ote_work.st_ote_ctrl.id_ope_active = chkbuf_u_msg.head.myid.serial_no;
			st_ote_work.st_ote_ctrl.addr_in_active_ote = pUSockOte->addr_in_from;	//運転有効IP保持
			*pbuf = chkbuf_u_msg;													//運転用バッファにコピー
			st_ote_work.ope_ote_silent_cnt = 0;										//サイレントカウンタクリア
		}
		else pOTE_Inf->st_msg_ote_u_rcv_mon = chkbuf_u_msg;						//モニタ要求対象用バッファにコピー
	}

	//操作有効端末が登録済で送信元アドレスが登録内容と一致の時
	//## 受信電文ヘッダ部にモニタモード接続要求があれば操作端末登録解除、なければ制御用バッファに受信電文コピー
	else if (st_ote_work.st_ote_ctrl.addr_in_active_ote.sin_addr.S_un.S_addr 
		== pUSockOte->addr_in_from.sin_addr.S_un.S_addr) {//送信元IPが保持中のIPと一致
		//操作モードの端末だんまりカウンタクリア
		st_ote_work.ope_ote_silent_cnt = 0;
		//送信元がモニタ要求か端末のモードがモニタの時
		if ((chkbuf_u_msg.head.code == OTE_CODE_REQ_MON) || (chkbuf_u_msg.head.status == OTE_STAT_MODE_MON)) {
			st_ote_work.st_ote_ctrl.id_ope_active = OTE_NON_OPEMODE_ACTIVE;	//有効ID無し
			pUSockOte->addr_in_from.sin_addr.S_un.S_addr = 0;				//保持IPアドレスクリア
			pOTE_Inf->st_msg_ote_u_rcv_mon = chkbuf_u_msg;				//モニタ要求対象用バッファにコピー
		}
		else {
			*pbuf = chkbuf_u_msg;											//制御用バッファにコピー
		}
	}
	//登録操作有効端末があって、その他の端末からの受信時
	else {
		st_ote_work.st_msg_ote_u_rcv_mon = chkbuf_u_msg;					//モニタ要求対象用バッファにコピー
	}
	
	//# ヘッダ情報の緊急停止指令は無条件に取り込み
	if (chkbuf_u_msg.head.code & OTE_MASK_REQ_STOP)
		st_ote_work.st_ote_ctrl.stop_req_mode |= OTE_MASK_REQ_STOP;

	if (chkbuf_u_msg.head.code & OTE_MASK_REQ_FAULT_RESET)
		st_ote_work.st_ote_ctrl.stop_req_mode &= ~OTE_MASK_REQ_STOP;

	rcv_count_ote_u++;
	return S_OK;
}
/****************************************************************************/
/// <summary>
/// PCマルチキャスト電文受信処理 
/// </summary>
HRESULT CCcCS::rcv_mul_pc(LPST_PC_M_MSG pbuf) {
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
HRESULT CCcCS::rcv_mul_ote(LPST_OTE_M_MSG pbuf) {
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

/// <summary>
/// 
/// </summary>
/// <param name="is_monitor_mode"></param>
/// <param name="code"></param>
/// <param name="stat"></param>
/// <returns></returns>
LPST_PC_U_MSG CCcCS::set_msg_u(BOOL is_monitor_mode, INT32 code, INT32 stat) {
	
	//#Header部
	st_ote_work.st_msg_pc_u_snd.head.myid	= pEnv_Inf->device_code;
	st_ote_work.st_msg_pc_u_snd.head.addr	= pUSockOte->addr_in_rcv;
	st_ote_work.st_msg_pc_u_snd.head.code	= code;
	st_ote_work.st_msg_pc_u_snd.head.status = stat;
	st_ote_work.st_msg_pc_u_snd.head.tgid	= st_ote_work.st_ote_ctrl.id_ope_active;
	
	//#Body部
	memcpy_s(&st_ote_work.st_msg_pc_u_snd.body, sizeof(ST_PC_U_BODY), &st_ote_work.st_body, sizeof(ST_PC_U_BODY) );

	return &st_ote_work.st_msg_pc_u_snd;
}

HRESULT CCcCS::snd_uni2ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

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
LPST_PC_M_MSG CCcCS::set_msg_m(INT32 code, INT32 stat) {	
	return &st_ote_work.st_msg_pc_m_snd;
}

//PCへ送信
HRESULT CCcCS::snd_mul2pc(LPST_PC_M_MSG pbuf) {
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
HRESULT CCcCS::snd_mul2ote(LPST_PC_M_MSG pbuf) {
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

LRESULT CALLBACK CCcCS::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
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
		st_mon1.wo_work.str(L"");
		st_mon1.wo_work << L"##CS_Inf  SLBRK COM: LEVEL " << pAUX_CS_Inf->com_slbrk.pc_com_brk_level
			<< L" HW " << pAUX_CS_Inf->com_slbrk.pc_com_hw_brk
			<< L" RESET" << pAUX_CS_Inf->com_slbrk.pc_com_reset
			<< L" FREE" << pAUX_CS_Inf->com_slbrk.pc_com_autosel;

			SetWindowText(st_mon1.hctrl[CS_ID_MON1_STATIC_1], st_mon1.wo_work.str().c_str());
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
LRESULT CALLBACK CCcCS::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		int i;
		for ( i = CS_ID_MON2_STATIC_UNI; i <= CS_ID_MON2_STATIC_MSG; i++){
		st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon2.pt[i].x, st_mon2.pt[i].y,st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}
		//RADIO PB
		for (i = CS_ID_MON2_RADIO_RCV; i <= CS_ID_MON2_RADIO_INFO; i++) {
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

		//CHECK BOX
		i = CS_ID_MON2_CB_BODY;

		st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
			st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(CS_ID_MON2_CTRL_BASE + i), hInst, NULL);

		//PB
		for (i = CS_ID_MON2_PB_PAGE_UP_UNI; i <= CS_ID_MON2_PB_PAGE_UP_MOT; i++) {
				st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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

		case CS_ID_MON2_CB_BODY       :{
			if (BST_CHECKED == SendMessage(st_mon2.hctrl[CS_ID_MON2_CB_BODY], BM_GETCHECK, 0, 0))
				st_mon2.is_body_disp_mode = true;
			else
				st_mon2.is_body_disp_mode = false;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_UNI:{
			if(++st_mon2.ipage_uni >= CS_ID_MON2_N_PAGE_UP_UNI)st_mon2.ipage_uni=0;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_MPC:{
			if (++st_mon2.ipage_mpc >= CS_ID_MON2_N_PAGE_UP_MPC)st_mon2.ipage_mpc = 0;
		}break;
		case CS_ID_MON2_PB_PAGE_UP_MOT:{
			if (++st_mon2.ipage_mot >= CS_ID_MON2_N_PAGE_UP_MOT)st_mon2.ipage_mot = 0;
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
			//ラベルにIP情報出力
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

			//電文内容表示出力
			st_mon2.wo_uni.str(L""); st_mon2.wo_mpc.str(L""); st_mon2.wo_mote.str(L"");
			if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_RCV) {
				if (st_mon2.is_body_disp_mode) {
					LPST_OTE_U_BODY pb0 = &pOTE_Inf->st_msg_ote_u_rcv.body.st;
					LPST_PC_M_BODY	pb1 = &pOTE_Inf->st_msg_pc_m_rcv.body.st;
					LPST_OTE_M_BODY pb2 = &pOTE_Inf->st_msg_ote_m_rcv.body.st;

					st_mon2.wo_uni << L"[BODY P"<< st_mon2.ipage_uni <<L"]" ;

					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@ESTOP:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::estop]
							<< L"@主幹入:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]
							<< L"@主幹切:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]
							<< L"@RMT:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::remote]
							<< L"@FRESET:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]
							<< L"@BYPASS:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::bypass]
							<< L"@MH_MODE:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]
							<< L"@BH_MODE:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode]

							<< L"\n@notch mh:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]
							<< L" bh:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]
							<< L" sl:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]
							<< L" gt:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]
							<< L" sl_brk:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]
							<< L" Mlamp:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1] + pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2] + pb0->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]
							<< L" alm stp:" << pb0->pnl_ctrl[OTE_PNL_CTRLS::alm_stop] ;
					}
					else if (st_mon2.ipage_uni == 1) {
					}
					else if (st_mon2.ipage_uni == 2) {
					}
					else {

					}

					st_mon2.wo_mpc << L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
					st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";

					st_mon2.wo_mpc << L"[BODY P"<< st_mon2.ipage_mpc <<L"]" << L"\n";
					st_mon2.wo_mote<< L"[BODY P"<< st_mon2.ipage_mot <<L"]" << L"\n";
				}
				else {
					LPST_OTE_HEAD ph0 = &pOTE_Inf->st_msg_ote_u_rcv.head;
					LPST_OTE_HEAD ph1 = &pOTE_Inf->st_msg_pc_m_rcv.head;
					LPST_OTE_HEAD ph2 = &pOTE_Inf->st_msg_ote_m_rcv.head;
					st_mon2.wo_uni	<< L"[HEAD]" << L" ID:"<<ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option 
								<< L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port)<<L"\n"
								<< L"       CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid <<L"\n";
					st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
					st_mon2.wo_mote<< L"[HEAD]" << L"CODE:" << ph2->code << L"\n";
				}
			}
			else if (st_mon2.sock_inf_id == CS_ID_MON2_RADIO_SND) {
				if (st_mon2.is_body_disp_mode) {
					LPST_PC_U_BODY pb0 = &st_ote_work.st_msg_pc_u_snd.body.st;
					LPST_PC_M_BODY pb1 = &st_ote_work.st_msg_pc_m_snd.body.st;

					st_mon2.wo_uni << L"[BODY P" << st_mon2.ipage_uni << L"]";

					if (st_mon2.ipage_uni == 0) {
						st_mon2.wo_uni << L"@ESTOP:" << pb0->lamp[OTE_PNL_CTRLS::estop].code
							<< L"@主幹入:" << pb0->lamp[OTE_PNL_CTRLS::syukan_on].code
							<< L"@主幹切:" << pb0->lamp[OTE_PNL_CTRLS::syukan_off].code
							<< L"@RMT:" << pb0->lamp[OTE_PNL_CTRLS::remote].code
							<< L"@FRESET:" << pb0->lamp[OTE_PNL_CTRLS::fault_reset].code
							<< L"@BYPASS:" << pb0->lamp[OTE_PNL_CTRLS::bypass].code
							<< L"@MH_MODE:" << pb0->lamp[OTE_PNL_CTRLS::mh_spd_mode].code
							<< L"@BH_MODE:" << pb0->lamp[OTE_PNL_CTRLS::bh_r_mode].code
							<< L"@SLBRK_FB: D16 " << pb0->sl_brk_fb[0] << L" D17 " << pb0->sl_brk_fb[1] << L" D18 " << pb0->sl_brk_fb[2] << L" D19 " << pb0->sl_brk_fb[3];


						st_mon2.wo_mpc << L"[BODY P" << st_mon2.ipage_mpc << L"]" << L"\n";
						st_mon2.wo_mote << L"[BODY P" << st_mon2.ipage_mot << L"]" << L"\n";
					}
					else {
						st_mon2.wo_uni << L"[BODY P"<< st_mon2.ipage_uni <<L"]" << L"\n";
						st_mon2.wo_mpc << L"[BODY P"<< st_mon2.ipage_mpc <<L"]" << L"\n";
						st_mon2.wo_mote<< L"[BODY P"<< st_mon2.ipage_mot <<L"]" << L"\n";
					}
				}
				else {
					LPST_OTE_HEAD ph0 = &st_ote_work.st_msg_pc_u_snd.head;
					LPST_OTE_HEAD ph1 = &st_ote_work.st_msg_pc_m_snd.head;
					st_mon2.wo_uni << L"[HEAD]" << L"ID:" << ph0->myid.crane_id << L" PC:" << ph0->myid.pc_type << L" Seral:" << ph0->myid.serial_no << L" Opt:" << ph0->myid.option;
					st_mon2.wo_uni << L" IP:" << ph0->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << ph0->addr.sin_addr.S_un.S_un_b.s_b4 << L":" << htons(ph0->addr.sin_port) << L"\n";
					st_mon2.wo_uni << L"       CODE:" << ph0->code << L" STAT:" << ph0->status << L" TGID:" << ph0->tgid << L"\n";

					st_mon2.wo_mpc << L"[HEAD]" << L"CODE:" << ph1->code << L"\n";
					st_mon2.wo_mote << L"[HEAD] -\n";
				}
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
				st_ote_work.addr_in_from_oteu = pUSockOte->addr_in_from;
				pUSockOte->addr_in_dst.sin_family = AF_INET;
				pUSockOte->addr_in_dst.sin_port = htons(OTE_IF_UNI_PORT_OTE);
				pUSockOte->addr_in_dst.sin_addr = pUSockOte->addr_in_from.sin_addr;

				HRESULT hr;
				if (st_ote_work.st_ote_ctrl.id_ope_active == OTE_NON_OPEMODE_ACTIVE) //操作モードの端末無
					hr = snd_uni2ote(set_msg_u(true, 0, st_ote_work.st_ote_ctrl.id_ope_active), &pUSockOte->addr_in_dst);
				else 
					hr = snd_uni2ote(set_msg_u(false, 0, st_ote_work.st_ote_ctrl.id_ope_active), &pUSockOte->addr_in_dst);
				
				if (hr == S_OK) st_ote_work.err_ote_comm &= ~CODE_CC_CS_OTE_COM_ERR_SND;//送信エラークリア
				else            st_ote_work.err_ote_comm |= CODE_CC_CS_OTE_COM_ERR_SND;//送信エラー

				st_ote_work.err_ote_comm &= ~CODE_CC_CS_OTE_COM_ERR_RCV;//受信エラークリア

			}
			else {
				st_ote_work.err_ote_comm |= CODE_CC_CS_OTE_COM_ERR_RCV;//受信エラー
			}

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}

		//OTE通信ヘッダに緊急停止要求有
		if (st_ote_work.st_msg_ote_u_rcv.head.code == OTE_CODE_REQ_ESTP) {
			st_ote_work.st_ote_ctrl.stop_req_mode |= OTE_STOP_REQ_MODE_ESTOP;
		}

	}break;
	case ID_SOCK_EVENT_OTE_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_ote(&st_ote_work.st_msg_ote_m_rcv);
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;
	case ID_SOCK_EVENT_PC_MUL: {
		int nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			rcv_mul_pc(&st_ote_work.st_msg_pc_m_rcv);
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

HWND CCcCS::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
void CCcCS::close_monitor_wnd(int id) {
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
void CCcCS::show_monitor_wnd(int id) {
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
void CCcCS::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CCcCS::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
void CCcCS::msg2listview(wstring wstr) {

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
void CCcCS::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CCcCS::set_panel_tip_txt() {
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
void CCcCS::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"OTE IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CCcCS::set_item_chk_txt() {
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





