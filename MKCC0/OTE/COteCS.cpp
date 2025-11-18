#include "CMCProtocol.h"
#include "CSockLib.H"
#include "COteCS.h"
#include "resource.h"
#include "CGamePad.h"
#include "CCrane.h"
#include "SmemAux.h"
#include "COteEnv.h"
#include "CPanelObj.h"

extern CSharedMem* pOteEnvInfObj;
extern CSharedMem* pOteCsInfObj;
extern CSharedMem* pOteCcInfObj;
extern CSharedMem* pOteUiObj;
extern ST_DEVICE_CODE g_my_code; //端末コード

extern CCrane* pCrane;

extern vector<CBasicControl*>	VectCtrlObj;
extern BC_TASK_ID st_task_id;


//ソケット
static CMCProtocol* pMCSock;				//MCプロトコルオブジェクトポインタ

//クラススタティックメンバ
ST_OTE_CS_MON1 COteCS::st_mon1;
ST_OTE_CS_MON2 COteCS::st_mon2;

ST_OTE_CS_INF COteCS::st_work;
ST_OTE_CS_OBJ COteCS::st_obj;

static COteEnv* pEnvObj;

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

static LPST_PLC_RBUF_HHGG38 pin_opepnl;
static INT16 dbg_plc_yout[4];

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

	//### Environmentのインスタンスポインタ取得
	pEnvObj = (COteEnv*)VectCtrlObj[st_task_id.ENV];
	
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
			if (pMCSock->Initialize(st_mon2.hwnd_mon, PLC_IF_TYPE_OTE_DEBUG) != S_OK) {
				wos << L"Initialize : MC Init NG"; msg2listview(wos.str()); wos.str(L"");
				wos << L"Err :" << pMCSock->msg_wos.str(); msg2listview(wos.str()); wos.str(L"");
				return S_FALSE;
			}
		}
		wos << L"MCProtocol Init OK"; msg2listview(wos.str());
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO0, L"Product");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO1, L"Debug");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MODE_RADIO2, L"Debug2");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK2, L"PLC IF");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_MON_CHECK1, L"GPAD IF");
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ２ CB状態セット	
	//if (st_mon2.hwnd_mon != NULL)
	//	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	//else
	//	SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

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
		wos << L"  OPE SRC CODE:" << hex << pOteCsInf->ope_source_mode;
		wos << L"  RMT_MODE:" << st_work.st_body.remote;

		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;

/// <summary>
/// #### 操作入力内容の取り込み
/// #### 信号の内容によってSource(操作台,PC Winパネル,GPadを選択して取り込み
/// </summary>
/// <returns></returns>


int COteCS::input(){

	memset(pOteCsInf->pnl_ctrl, 0, sizeof(pOteCsInf->pnl_ctrl));//パネル入力クリア


	//### 操作卓PLC信号取り込み(pOteCsInf->buf_opepnl_readにMCプロトコルで読込)
	if(pin_opepnl==NULL)
		pin_opepnl = (LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read;//操作卓信号入力// 

	//### PCWindowパネル信号取り込み(pOteUi->pnl_ctrl[])
	//## ScadでpOteUi->pnl_ctrl[]にセットされている	

	//### ゲームパッド取り込み(pOteCsInf->gpad_in)
	if ((pPad != NULL) && st_work.st_body.game_pad_mode) {
		if (pPad->PollController(pPad->controllerId)) {//GamePad状態取り込み⇒以降、共有メモリに展開
			st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
		}
		//GamePadのDI値を共有メモリにセット（Valueオブジェクトにセット⇒Boolに変換して共有メモリにセット）
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

		//GamePadのアナログ値をValueオブジェクトにセット⇒ノッチ数に変換して共有メモリにセット
		st_obj.pad_mh->set(pPad->get_RY());
		st_obj.pad_bh->set(pPad->get_LY());
		st_obj.pad_sl->set(pPad->get_LX());
		st_obj.pad_gt->set(pPad->get_RX());

		pOteCsInf->gpad_in.pad_mh = -st_obj.pad_mh->get_notch();
		pOteCsInf->gpad_in.pad_bh = st_obj.pad_bh->get_notch();
		pOteCsInf->gpad_in.pad_sl = st_obj.pad_sl->get_notch();
		pOteCsInf->gpad_in.pad_gt = st_obj.pad_gt->get_notch();
	}
	
	//## 遠隔操作台信号取り込み（モメンタリ & 非常停止は 遠隔操作台優先）
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL) {
	
		//非常停止
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			= (pin_opepnl->xin[4] & 0x0020);
	
		//旋回フットブレーキ(0-15)
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]	= pin_opepnl->ai_sl_foot;

		//モメンタリスイッチ （ハードSW）
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		= pin_opepnl->xin[1] & 0x0200;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		= pin_opepnl->xin[1] & 0x0400;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		= pin_opepnl->xin[1] & 0x0100;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			= pin_opepnl->xin[1] & 0x6000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]		= pin_opepnl->xin[1] & 0x0080;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]		= pin_opepnl->xin[1] & 0x8000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]		= pin_opepnl->xin[2] & 0x0300;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]		= pin_opepnl->xin[3] & 0x3000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]			= pin_opepnl->xin[3] & 0xc000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust]		= pin_opepnl->xin[2] & 0x000f;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust]		= pin_opepnl->xin[2] & 0x00f0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camAselect]		= pin_opepnl->xin[2] & 0x1c00;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camBselect]		= pin_opepnl->xin[2] & 0xe000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisA]			= pin_opepnl->xin[0] & 0x3000;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisB]			= pin_opepnl->xin[0] & 0xc000;

		//自動,、遠隔（タッチパネル）
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			= pin_opepnl->auto_sw & 0x0001;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::auto_mode]		= pin_opepnl->auto_sw & 0x0002;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::game_pad]		= pin_opepnl->auto_sw & 0x0004;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_mh]			= pin_opepnl->auto_sw & 0x0010;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_bh]			= pin_opepnl->auto_sw & 0x0020;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_sl]			= pin_opepnl->auto_sw & 0x0040;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_gt]			= pin_opepnl->auto_sw & 0x0080;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_ah]			= pin_opepnl->auto_sw & 0x0100;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::ote_type]		= pin_opepnl->auto_sw & 0x0200;

		//遠隔操作台優先（オルタネートSW）
		//照明
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]		= pin_opepnl->lamp_sw & 0x0001;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]		= pin_opepnl->lamp_sw & 0x0002;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]		= pin_opepnl->lamp_sw & 0x0004;
		//CS
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]		= pin_opepnl->mh_mode_cs;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode]		= pin_opepnl->bh_mode_cs;
		//旋回ブレーキ
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]		= pin_opepnl->notch_L1;
	
	}
	else {//オルタネートSWは操作台無効時のみPCパネル指令受付（GpadはオルタネートSW無し）
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1] ;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]		= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_aux];
	}
	
	//## ゲームパッド信号取り込み（モメンタリ）
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_GPAD) {
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		|= pOteCsInf->gpad_in.syukan_on;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		|= pOteCsInf->gpad_in.syukan_off;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		|= pOteCsInf->gpad_in.f_reset;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			|= pOteCsInf->gpad_in.estop;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			|= pOteCsInf->gpad_in.bypass;
		
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			|= pOteCsInf->gpad_in.remote;

		pOteCsInf->gpad_in.kidou_r;
		pOteCsInf->gpad_in.kidou_l;
		pOteCsInf->gpad_in.pan_l;
		pOteCsInf->gpad_in.pan_r;
		pOteCsInf->gpad_in.tilt_u;
		pOteCsInf->gpad_in.tilt_d;
		pOteCsInf->gpad_in.zoom_f;
		pOteCsInf->gpad_in.zoom_n;

		//旋回ブレーキペダル(0-15)
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk] = (pOteCsInf->gpad_in.trig_l+ pOteCsInf->gpad_in.trig_r) / 0x10;
	}
	
	//## PC Winパネル信号取り込み（モメンタリ）
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_PCPNL) {
	
		//非常停止
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::estop];
		//旋回フットブレーキ
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]	|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::sl_brk];

		//モメンタリスイッチ （ハードSW）
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::fault_reset];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::bypass]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::motor_siren];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camAselect]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camAselect]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camBselect]		|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::camBselect]	;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisA]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::irisA]		;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisB]			|=  pOteUi->pnl_ctrl[OTE_PNL_CTRLS::irisB]		;

		//自動,、遠隔（タッチパネル）
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::remote];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::auto_mode]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::auto_mode];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::game_pad]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::game_pad];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_mh]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_mh];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_bh]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_bh];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_sl]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_sl];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_gt]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_gt];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::asel_ah]			|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::asel_ah];
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::ote_type]		|= pOteUi->pnl_ctrl[OTE_PNL_CTRLS::ote_type];
	}

	//## ノッチ指令値取り込み

	if(pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL){
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]	= pin_opepnl->notch_RY0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]	= pin_opepnl->notch_LY0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]	= pin_opepnl->notch_LX0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]	= pin_opepnl->notch_R1;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]	= pin_opepnl->notch_RX0;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	= pin_opepnl->notch_L1;
	}
	//ノッチ指令値は、GPAD Modeの時は、GPAD > 操作台
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_GPAD) {
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]	= pOteCsInf->gpad_in.pad_mh;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]	= pOteCsInf->gpad_in.pad_bh;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]	= pOteCsInf->gpad_in.pad_sl;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]	= pOteCsInf->gpad_in.pad_gt;
		pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]	= pOteCsInf->gpad_in.pad_ah;
	}
	//PCパネルのノッチPB入力は操作台またはGPAD指令OFFの時のみ有効(他設定値が0の時のみ上書き
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_PCPNL) {
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_mh] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_mh];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_bh] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_bh];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_sl] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_sl];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_gt] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_gt];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_ah] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_ah];
		if (!(pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]))	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux] = pOteUi->pnl_ctrl[OTE_PNL_CTRLS::notch_aux];
	}

	//## GOT指令値取り込み（クレーン接続）
	if (pOteCsInf->ope_source_mode & OTE_OPE_SOURCE_CODE_OPEPNL) {
		pOteCsInf->GOT_command = pin_opepnl->got_command;	
		pOteCsInf->GOT_crane_select = pin_opepnl->got_crane_selected;
	}

	//### 旋回ブレーキ指令信号整形
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		< 0	)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		= 0;
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		> 15)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		= 15;
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	> 0	)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_HW_BRK; //HWブレーキ
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	== -1)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_RESET; //リセット
	if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::notch_aux]	== -2)	pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::sl_brk]		|= AUX_SLBRK_COM_FREE; //フリー(AUTO/MANUAL)

	return S_OK;
}

static INT16 ope_plc_cnt;
static UINT16 ope_plc_chk_cnt=0;
static INT16 pnl_ctrl_last[N_OTE_PNL_CTRL];
//#### モード,指令値設定　
int COteCS::parse() 
{           
	//### 異常チェック
	{
		//## 操作卓ヘルシーチェック
		if (ope_plc_cnt == ((LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read)->plc_healthy) {//前回値から変化なし
			ope_plc_chk_cnt++;
			if(ope_plc_chk_cnt > 40) {					//PLC通信異常
				pOteCsInf->ope_plc_stat = L_OFF;
				pOteCsInf->ote_error |= FLTS_MASK_ERR_RPC_RPLC_COMM;
			}
		}
		else {//PLC通信正常
			pOteCsInf->ope_plc_stat = L_ON;
			ope_plc_chk_cnt = 0;
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_RPC_RPLC_COMM;
		}
		ope_plc_cnt = ((LPST_PLC_RBUF_HHGG38)pOteCsInf->buf_opepnl_read)->plc_healthy;

		//## 制御PC通信異常チェック
		if(pOteCCInf->cc_comm_chk_cnt >= PRM_OTE_PC_COM_TMOV_CNT)
			pOteCsInf->ote_error |= FLTS_MASK_ERR_CPC_RPC_COMM;
		else
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_CPC_RPC_COMM;

		//## 非常停止チェック
		if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]) {
			pOteCsInf->ote_error |= FLTS_MASK_ERR_RPC_ESTP;
		}
		else {
			pOteCsInf->ote_error &= ~FLTS_MASK_ERR_RPC_ESTP;
		}

		pOteCsInf->rpc_flt_count = 0;
		for (int j = 0; j < 16; j++) {
			if (pOteCsInf->rpc_flt_count >= OTE_PC_FLT_DETECT_MAX) break;	//表示故障数上限

			if (pOteCsInf->ote_error & (1 << j)) {	//検出ありの時
				pOteCsInf->rpc_flt_codes[pOteCsInf->rpc_flt_count] = 850 + j;
				pOteCsInf->rpc_flt_count++;	//PC故障数カウントアップ
			}
		}
	}
	
	//### モード設定
	{
		//### 遠隔リモート操作有効化設定
		//遠隔PBトリガ検出
		if (pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote] && (pnl_ctrl_last[OTE_PNL_CTRLS::remote] == L_OFF)) {
			if (st_work.st_body.remote == CODE_PNL_COM_SELECTED)			// 遠隔操作モード選択,承認待ち　
				st_work.st_body.remote = CODE_PNL_COM_OFF;					//		⇒　遠隔操作モード選択要求OFF
			else if (st_work.st_body.remote == CODE_PNL_COM_ACTIVE)			// 遠隔操作モード選択,承認済
				st_work.st_body.remote = CODE_PNL_COM_OFF;					//		⇒　遠隔操作モード選択要求OFF
			else															// 遠隔モニターモード選択
				st_work.st_body.remote = CODE_PNL_COM_SELECTED;				//		⇒　遠隔操作モード選択,承認待ち
		}
		//制御PCからの承認確認（通信ヘッダのIDが自IDと同じ場合に承認）
		if ((st_work.st_body.remote == CODE_PNL_COM_SELECTED) && (pOteCCInf->cc_active_ote_id == g_my_code.serial_no))
			st_work.st_body.remote = CODE_PNL_COM_ACTIVE;
		//遠隔操作接続解除でOFF
		if((pOteCCInf->cc_active_ote_id != g_my_code.serial_no)&&(st_work.st_body.remote == CODE_PNL_COM_ACTIVE))
			st_work.st_body.remote = CODE_PNL_COM_OFF;
		if (!(pOteUi->pc_pnl_active))
			 st_work.st_body.remote = CODE_PNL_COM_OFF;

		//前回値保持
		pnl_ctrl_last[OTE_PNL_CTRLS::remote] = pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::remote];

		//### GamePadモード設定
		if (CODE_TRIG_ON == st_obj.game_pad_pb.chk_trig(pOteUi->pnl_ctrl[OTE_PNL_CTRLS::game_pad])) {
			if (st_work.st_body.game_pad_mode == CODE_PNL_COM_ACTIVE)
				st_work.st_body.game_pad_mode = CODE_PNL_COM_OFF;
			else
				st_work.st_body.game_pad_mode = CODE_PNL_COM_ACTIVE;
		}
		//### 操作卓タイプ設定(遠隔操作台タイプ/タブレット/遠隔デバイス)
		// 次ステップで対応
		//if (st_obj.ote_type.chk_trig(pOteUi->pnl_ctrl[OTE_PNL_CTRLS::ote_type])) {
		//	st_work.st_body.ote_type = st_obj.ote_type.get();
		//}

		//## 操作デバイスモード設定
		//#操作卓
		if (pOteCsInf->ope_plc_stat)
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_OPEPNL;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_OPEPNL;
		//#GamePad
		if ((pPad != NULL) && (st_work.st_body.game_pad_mode == CODE_PNL_COM_ACTIVE))
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_GPAD;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_GPAD;
		//#PCパネル
		if (pOteUi->pc_pnl_active == L_ON)
			pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_PCPNL;
		else
			pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_PCPNL;
		//#携帯パネル（次ステップで対応）
		//if (pOteUi->pc_pnl_active == L_ON)
		//	pOteCsInf->ope_source_mode |= OTE_OPE_SOURCE_CODE_PCPNL;
		//else
		//	pOteCsInf->ope_source_mode &= ~OTE_OPE_SOURCE_CODE_PCPNL;
	}


	return STAT_OK;
}

static INT16 ote_helthy = 0; //ヘルシー値
//#### 出力処理　
int COteCS::output() {          

//### 遠隔操作卓PLCへの出力処理	   	
{
	//##アクセス用ポインタセット
	LPST_PLC_WBUF_HHGG38 pPcWBuf = (LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write;//書き込みバッファポインタ
	LPST_PC_U_BODY pBody = (LPST_PC_U_BODY)&pOteCCInf->st_msg_pc_u_rcv.body;

	//##PC Healthy
	pPcWBuf->pc_healthy = ote_helthy++;					//PCヘルシー値
	//##PC Status
	
	//接続中クレーンID
	pPcWBuf->crane_id = (INT16)(pOteCCInf->id_conected_crane & 0x0000FFFF);	//接続先クレーンID	

	//##操作卓遠隔モード表示
	pPcWBuf->rmt_status = st_work.st_body.remote;

	//##操作卓ハードランプ
	INT16 plc_yo_buf = 0;
	//主幹ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_on].code)	plc_yo_buf |= 0x0040;
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::syukan_off].code)	plc_yo_buf |= 0x0080;
	//高圧ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_trolley].code)plc_yo_buf |= 0x0001;
	else																		plc_yo_buf |= 0x0002;
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_gantry].code)	plc_yo_buf |= 0x0004;
	else																		plc_yo_buf |= 0x0008;
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hv_aux].code)	plc_yo_buf |= 0x0010;
	else																		plc_yo_buf |= 0x0020;
	//故障ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::fault_lamp].code)plc_yo_buf |= 0x0100;
	//警報ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::alm_lamp].code)	plc_yo_buf |= 0x0200;
	//運転準備完了ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::ope_ready].code)	plc_yo_buf |= 0x0800;
	//ブザー
	INT16 bz_code = (INT16)pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::buzzer].code;
	plc_yo_buf &= 0x0FFF;
	if (bz_code) {
		if (bz_code & 0x0001)plc_yo_buf |= 0x1000;	//故障
		else if (bz_code & 0x0001)plc_yo_buf |= 0x1000;	//重故障
		else if (bz_code & 0x0002)plc_yo_buf |= 0x2000;	//主幹投入
		else if (bz_code & 0x0004)plc_yo_buf |= 0x4000;	//渋滞
		else if (bz_code & 0x0008)plc_yo_buf |= 0x8000;	//軽故障
		else;
	}
	//デバッグ用　opepanのMode1スイッチでFunc1のチェックBOX1のトリガでEDIT BOXにセットしたビットのみ強制出力
	if (inf.mode_id == MODE_OTE_CS_APP_DEBUG) {
		plc_yo_buf = dbg_plc_yout[0];
	}
	//#↑でセットした内容をIFバッファにセット
	pPcWBuf->lamp1 = plc_yo_buf;

	//##操作卓GOTランプ
	plc_yo_buf = 0;
	//動力確立ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::main_power].code)	plc_yo_buf |= 0x0001;
	//自動給脂ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::sl_auto_gr].code)	plc_yo_buf |= 0x0002;
	//サイレンランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::motor_siren].code)	plc_yo_buf |= 0x8000;
	//照明ランプ
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp1].code)	plc_yo_buf |= 0x0800;
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp2].code)	plc_yo_buf |= 0x1010;
	if (pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::hd_lamp3].code)	plc_yo_buf |= 0x2000;

	//#IFバッファにセット
	pPcWBuf->lamp2 = plc_yo_buf;
	
	//##GOT運転監視
	pPcWBuf->mh_hight = pBody->st_axis_set[ID_HOIST].pos_fb;	//揚程
	pPcWBuf->mh_load = pBody->st_load_stat[0].m / 10.0;			//荷重0.1t単位→t単位
	pPcWBuf->r = pBody->st_axis_set[ID_BOOM_H].pos_fb;			//半径
	pPcWBuf->wind_spd = pBody->wind_spd;						//風速
	
	//##旋回ブレーキFB信号セット
	pPcWBuf->sl_brk_fb1 = pBody->sl_brk_fb[0];				//旋回ブレーキFB1
	pPcWBuf->sl_brk_fb2 = pBody->sl_brk_fb[1];				//旋回ブレーキFB2
	pPcWBuf->sl_brk_fb3 = pBody->sl_brk_fb[2];				//旋回ブレーキFB3
	pPcWBuf->sl_brk_fb4 = pBody->sl_brk_fb[3];				//旋回ブレーキFB4
	
	
	//##GOT故障監視
	LPST_OTE_PC_FLTS_SET pFltSet = (LPST_OTE_PC_FLTS_SET)&pBody->faults_set;

	int index = 0,nflt=0;

	if (!(pOteCsInf->ote_error & FLTS_MASK_ERR_CPC_RPC_COMM)) {//制御PCとの通信断でスルー
		nflt = pFltSet->set_plc_count;
		if (nflt >= N_OTE_OPE_PLC_FAULT_BUF)nflt = N_OTE_OPE_PLC_FAULT_BUF;
		//PLC フォルト
		for (int i = 0; i < nflt; i++, index++)((LPST_PLC_WBUF_HHGG38)
			pOteCsInf->buf_opepnl_write)->fault_code[index] = pFltSet->codes_plc[i] + 300;

		//制御PC フォルト
		nflt = pFltSet->set_pc_count;
		if (nflt + index >= N_OTE_OPE_PLC_FAULT_BUF) nflt = N_OTE_OPE_PLC_FAULT_BUF - index;
		for (int i = 0; i < nflt; i++, index++)
			((LPST_PLC_WBUF_HHGG38)	pOteCsInf->buf_opepnl_write)->fault_code[index] = pFltSet->codes_pc[i] + 300;
	}
	//遠隔PC フォルト
	for (int i = 0; i < pOteCsInf->rpc_flt_count; i++, index++) {
		((LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write)->fault_code[index] = pOteCsInf->rpc_flt_codes[index];
	}

	//残りは0クリア
	for(;index< N_OTE_OPE_PLC_FAULT_BUF;index++) 
		((LPST_PLC_WBUF_HHGG38)pOteCsInf->buf_opepnl_write)->fault_code[index] = 0;


	
	//##GOT状態監視
	pPcWBuf->mh_set		= pBody->st_axis_set[ID_HOIST];			//主巻
	pPcWBuf->bh_set		= pBody->st_axis_set[ID_BOOM_H];		//引込
	pPcWBuf->sl_set		= pBody->st_axis_set[ID_SLEW];			//旋回
	pPcWBuf->gt_set		= pBody->st_axis_set[ID_GANTRY];		//走行

	//主巻、引込モードスイッチ
	pPcWBuf->mh_set.mode = pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::mh_spd_mode].st.com;
	pPcWBuf->bh_set.mode = pOteCCInf->st_msg_pc_u_rcv.body.st.lamp[OTE_PNL_CTRLS::bh_r_mode].st.com;

	}
//### 制御PCへの出力処理

	pOteCsInf->st_body.ote_err[0] = pOteCsInf->ote_error;	//遠隔操作PC検出故障セット
//##　送信バッファ内容出力（CSで収集したユーザ操作内容）を共有メモリにコピー
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


/// <summary>
/// SCADA パネル通信状態ランプ表示用ステータス更新関数
/// </summary>
void COteCS::update_sock_stat() {
	INT32 sock_stat = pMCSock->get_sock_status();
	if (sock_stat & CSOCK_STAT_STANDBY) {
		st_work.plc_com_stat_r = ID_PNL_SOCK_STAT_STANDBY;
		st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_STANDBY;

		if (sock_stat & CSOCK_STAT_ACT_RCV)
			st_work.plc_com_stat_r = ID_PNL_SOCK_STAT_ACT_RCV;
		else if (sock_stat & CSOCK_STAT_RCV_ERR)
			st_work.plc_com_stat_r = ID_PNL_SOCK_STAT_RCV_ERR;
		else;

		if (sock_stat & CSOCK_STAT_ACT_SND)
			st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_ACT_SND;
		else if (sock_stat & CSOCK_STAT_SND_ERR)
			st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_SND_ERR;
		else;
	}
	else if (sock_stat == CSOCK_STAT_CLOSED)	st_work.plc_com_stat_r = st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_CLOSED;
	else if (sock_stat == CSOCK_STAT_INIT)		st_work.plc_com_stat_r = st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_INIT;
	else if (sock_stat == CSOCK_STAT_INIT_ERROR)st_work.plc_com_stat_r = st_work.plc_com_stat_s = ID_PNL_SOCK_STAT_INIT_ERROR;
	else;
	
	return ;
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
static int n_page_w = 5, i_page_w = 0, i_ref_w = 0;//モニタ画面表示用
static int n_page_r = 5, i_page_r = 0, i_ref_r = 0;//モニタ画面表示用

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
		//CB
		int i = OTE_CS_ID_MON2_CB_DISP_IO;
		st_mon2.hctrl[i] = CreateWindowW(TEXT("BUTTON"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX ,
			st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
			hWnd, (HMENU)(OTE_CS_ID_MON2_CTRL_BASE + i), hInst, NULL);

		UINT rtn = SetTimer(hWnd, OTE_CS_ID_MON2_TIMER, OTE_CS_PRM_MON2_TIMER_MS, NULL);

		//UINT rtn = SetTimer(hWnd, OTE_CS_ID_MON2_TIMER, 10, NULL);

				//IFデータ表示ページ数計算
		n_page_w = OTE_MC_SIZE_W_WRITE / (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN);
		if (OTE_MC_SIZE_W_WRITE % (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_w++;
		n_page_r = OTE_MC_SIZE_W_READ / (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN);
		if (OTE_MC_SIZE_W_READ % (OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN)) n_page_r++;

		monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());

		monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());


		st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
		monwos.str(L""); monwos << L"表示中";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_MSG_DISP_SEL], monwos.str().c_str());
		monwos.str(L""); monwos << L"16進";
		SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_DISP_DEC_SEL], monwos.str().c_str());
	}break;
	case WM_TIMER: {
		if (pMCSock == NULL)break;
		//3Eフォーマット Dデバイス書き込み要求送信
		if (is_write_req_turn) {//書き込み要求送信
			st_mon2.wo_req_w.str(L"");
			//3Eフォーマット Dデバイス書き込み要求送信
			if (pMCSock->send_write_req_D_3E(pOteCsInf->buf_opepnl_write) != S_OK) {
				st_mon2.wo_req_w << L"ERROR : send_read_req_D_3E()\n";
			}
			else snd_count_plc_w++;

			if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {
				if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_IO) {
					st_mon2.wo_req_w << L"送信内容\n";
					st_mon2.wo_req_w << L"Y80-8F【" << ((LPST_PLC_WBUF_HHGG38)(pOteCsInf->buf_opepnl_write))->lamp1 << L"】";
					st_mon2.wo_req_w << L"0,1:主高 2,3:走高 4,5:補高 6,7:主幹 8:故障 9:警報 B:運準 C-F:Buzzer\n";
				}
				else {

					if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)
						st_mon2.wo_req_w << std::hex;
					else
						st_mon2.wo_req_w << std::dec;
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

					//st_mon2.wo_req_w << L"PC Helthy:" << pOteCsInf->buf_opepnl_write[0];
					//データ部分1ページ10WORD　4行で切替表示
					for (int i = 0; i < OTE_CS_MON2_MSG_DISP_N__DATAROW; i++) {
						int no = OTE_MC_ADDR_W_WRITE + i_page_w * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN * OTE_CS_MON2_MSG_DISP_N__DATAROW + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
						st_mon2.wo_req_w << L"D" << dec << no << L" |";
						if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)	st_mon2.wo_req_w << hex;
						for (int j = 0; j < OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
							if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)
								st_mon2.wo_req_w << std::setw(4) << std::setfill(L'0') << pOteCsInf->buf_opepnl_write[i_ref_w + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
							else
								st_mon2.wo_req_w << std::setw(6) << std::setfill(L' ') << pOteCsInf->buf_opepnl_write[i_ref_w + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
						}
						st_mon2.wo_req_w << L"\n";
					}
				}

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
					<< L"#sub:"		<< std::hex << pMCSock->mc_req_msg_r.subcode
					<< L"#serial:" << pMCSock->mc_req_msg_r.serial
					<< L"#NW:"		<< pMCSock->mc_req_msg_r.nNW
					<< L"#PC:"		<< pMCSock->mc_req_msg_r.nPC
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
				monwos << L"IP R:" << addr.sin_addr.S_un.S_un_b.s_b1 << L"." << addr.sin_addr.S_un.S_un_b.s_b2 << L"." << addr.sin_addr.S_un.S_un_b.s_b3 << L"." << addr.sin_addr.S_un.S_un_b.s_b4 << L":"
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

		//通信ステータス
		update_sock_stat();

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);
		// 選択されたメニューの解析:
		switch (wmId - OTE_CS_ID_MON2_CTRL_BASE)
		{
		case OTE_CS_ID_MON2_PB_R_BLOCK_SEL: {
			if (++i_page_r >= n_page_r)i_page_r = 0;
			monwos.str(L""); monwos << L"R:" << i_page_r + 1 << L"/" << n_page_r;
			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_R_BLOCK_SEL], monwos.str().c_str());
			i_ref_r = i_page_r * OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
		}break;
		case OTE_CS_ID_MON2_PB_W_BLOCK_SEL: {
			if (++i_page_w >= n_page_w)i_page_w = 0;
			monwos.str(L""); monwos << L"W:" << i_page_w + 1 << L"/" << n_page_w;
			SetWindowText(st_mon2.hctrl[OTE_CS_ID_MON2_PB_W_BLOCK_SEL], monwos.str().c_str());
			i_ref_w = i_page_w * OTE_CS_MON2_MSG_DISP_N__DATAROW * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
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
		case OTE_CS_ID_MON2_CB_DISP_IO: {

			if (BST_UNCHECKED == SendMessage(st_mon2.hctrl[OTE_CS_ID_MON2_CB_DISP_IO], BM_GETCHECK, 0, 0))
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_HEX;
			else 
				st_mon2.msg_disp_mode = OTE_CS_MON2_MSG_DISP_IO;
	
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
			UINT nRtn = pMCSock->rcv_msg_3E(pOteCsInf->buf_opepnl_read);
			
			if (nRtn == MC_RES_READ) {//読み出し応答
				rcv_count_plc_r++;
				if ((st_mon2.msg_disp_mode != OTE_CS_MON2_MSG_DISP_OFF) && st_mon2.is_monitor_active) {

					if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_IO) {
						st_mon2.wo_res_r << L"受信内容\n";
						st_mon2.wo_res_r <<dec<< L"Notch RX0:" << pin_opepnl->notch_RX0 << L" RY0:" << pin_opepnl->notch_RY0 << L" LX0:" << pin_opepnl->notch_LX0 << L" LY0:" << pin_opepnl->notch_LY0 << L" R1:" << pin_opepnl->notch_R1 << L" L1:" << pin_opepnl->notch_L1 << L" SLBLK:" << pin_opepnl->ai_sl_foot;
						st_mon2.wo_res_r << hex << L" ESTP:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::estop]
							<< L" 主幹ON:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_on]
							<< L" 主幹OFF:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::syukan_off]
							<< L" ﾘｾｯﾄ:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::fault_reset]
							<< L" ﾊﾞｲﾊﾟｽ:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bypass]
							<< L" 警報停止:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::alm_stop]
							<< L" ｻｲﾚﾝ:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::motor_siren]
							<< L" 高圧ﾄﾛﾘ:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_trolley]
							<< L" 高圧走行:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_gantry]
							<< L" 高圧補機:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hv_aux]
							<< L" camAadj:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camA_adjust]
							<< L" camBadj:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camB_adjust]
							<< L" camAsel:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camAselect]
							<< L" camBsel:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::camBselect]
							<< L" irisA:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisA]
							<< L" irisB:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::irisB]
							<< L" lamp1:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp1]
							<< L" lamp2:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp2]
							<< L" lamp3:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::hd_lamp3]
							<< L" mh_spd:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::mh_spd_mode]
							<< L" bh_r:" << pOteCsInf->pnl_ctrl[OTE_PNL_CTRLS::bh_r_mode];

					}
					else {
						st_mon2.wo_res_r << L"Rr>>"
							<< L"#sub:" << std::hex << pMCSock->mc_res_msg_r.subcode
							<< L"#serial:" << pMCSock->mc_res_msg_r.serial
							<< L"#NW:" << pMCSock->mc_res_msg_r.nNW
							<< L"#PC:" << pMCSock->mc_res_msg_r.nPC
							<< L"#UIO:" << pMCSock->mc_res_msg_r.nUIO
							<< L"#Ucd:" << pMCSock->mc_res_msg_r.nUcode
							<< L"#len:" << pMCSock->mc_res_msg_r.len
							<< L"#end:" << pMCSock->mc_res_msg_r.endcode << L"\n";

						//				st_mon2.wo_res_r	<< L"PLC HEALTHY:" << pOteCsInf->buf_opepnl_read[0] << L"\n";
						for (int i = 0; i < OTE_CS_MON2_MSG_DISP_N__DATAROW; i++) {
							int no = OTE_MC_ADDR_W_READ + i_page_r * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN * OTE_CS_MON2_MSG_DISP_N__DATAROW + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN;
							st_mon2.wo_res_r << L"D" << dec << no << L" |";
							if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)	st_mon2.wo_res_r << hex;
							for (int j = 0; j < OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN; j++) {
								if (st_mon2.msg_disp_mode == OTE_CS_MON2_MSG_DISP_HEX)
									st_mon2.wo_res_r << std::setw(4) << std::setfill(L'0') << pOteCsInf->buf_opepnl_read[i_ref_r + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
								else
									st_mon2.wo_res_r << std::setw(6) << std::setfill(L' ') << pOteCsInf->buf_opepnl_read[i_ref_r + i * OTE_CS_MON2_MSG_DISP_N_DATA_COLUMN + j] << L"|";
							}
							st_mon2.wo_res_r << L"\n";
						}
					}
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
			case IDC_TASK_FUNC_RADIO1: {
				wstring wstr;
				int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr.c_str(), 128);
				if (n) {
					int shift = stoi(wstr.c_str());
					if((shift >= 0) &&(shift <= 15)) dbg_plc_yout[0] = 1<<shift;
				}
				else  dbg_plc_yout[0] = 0;
				//チェックを外す
				SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
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
		case IDC_TASK_MODE_RADIO0:
		{
			inf.mode_id = MODE_OTE_CS_APP_PRODUCT;
		}break;
		case IDC_TASK_MODE_RADIO1:
		{
			inf.mode_id = MODE_OTE_CS_APP_DEBUG;
		}break;
		case IDC_TASK_MODE_RADIO2:
		{
			inf.mode_id = MODE_OTE_CS_APP_DEBUG2;
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
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"Y80 OUT");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5, L"-");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6, L"-");
	}break;
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









