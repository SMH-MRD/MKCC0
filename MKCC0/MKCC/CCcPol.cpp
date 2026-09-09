#include "CCcPol.h"
#include "resource.h"
#include "CCrane.H"
#include "CFaults.h"
#include "SmemAux.H"
#include "SWYSENSOR_DEF.H"
#include "CCcAgent.h"
#include "CCcEnv.h"
#include "CCcCS.h"
#include "phisics.h"

extern CSharedMem* pEnvInfObj;
extern CSharedMem* pPlcIOObj;
extern CSharedMem* pJobIOObj;
extern CSharedMem* pPolInfObj;
extern CSharedMem* pAgInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pSimuStatObj;
extern CSharedMem* pOteInfObj;
extern CSharedMem* pAuxInfObj;

extern BC_TASK_ID				st_task_id;
extern vector<CBasicControl*>	VectCtrlObj;	    //スレッドオブジェクトのポインタ


extern CSharedMem* pAuxCsInfObj;

extern CCrane* pCrane;

extern INT32 aux_slbrk_status;			//旋回ブレーキ	組み込み状況
extern INT32 aux_lanio_status;			//LANIO			組み込み状況
extern INT32 aux_sway_status;			//振れセンサ	組み込み状況
extern INT32 aux_gt_pos_sys_status;		//走行位置検出	組み込み状況

//共有メモリ
static LPST_CC_ENV_INF		pEnvInf		= NULL;
static LPST_CC_PLC_IO		pPlcIO		= NULL;
static LPST_JOB_IO			pJobIO		= NULL;
static LPST_CC_POL_INF		pPolInf		= NULL;
static LPST_CC_AGENT_INF	pAgentInf	= NULL;
static LPST_CC_CS_INF		pCsInf		= NULL;
static LPST_CC_SIM_INF		pSimInf		= NULL;
static LPST_CC_OTE_INF		pOteInf		= NULL;

static LPST_AUX_CS_INF		pAUX_CS_Inf = NULL;

static CAgent* pAgent = NULL;
static CCcEnv* pEnv = NULL;
static CCcCS* pCS = NULL;

ST_POL_MON1 CCcPol::st_mon1;
ST_POL_MON2 CCcPol::st_mon2;

ST_CC_ENV_INF CCcPol::st_work;

CCcPol::CCcPol() {

}
CCcPol::~CCcPol() {

}

HRESULT CCcPol::initialize(LPVOID lpParam) {

	HRESULT hr = S_OK;

	wos.str(L"初期化中…"); msg2host(wos.str());

	pEnvInf = (LPST_CC_ENV_INF)(pEnvInfObj->get_pMap());
	pPlcIO = (LPST_CC_PLC_IO)(pPlcIOObj->get_pMap());
	pJobIO = (LPST_JOB_IO)(pJobIOObj->get_pMap());
	pPolInf = (LPST_CC_POL_INF)(pPolInfObj->get_pMap());
	pAgentInf = (LPST_CC_AGENT_INF)(pAgInfObj->get_pMap());
	pCsInf = (LPST_CC_CS_INF)(pCsInfObj->get_pMap());
	pSimInf = (LPST_CC_SIM_INF)(pSimuStatObj->get_pMap());
	pOteInf = (LPST_CC_OTE_INF)(pOteInfObj->get_pMap());

	pAUX_CS_Inf = (LPST_AUX_CS_INF)pAuxCsInfObj->get_pMap();

	pAgent = (CAgent*)VectCtrlObj[st_task_id.AGENT];
	pEnv = (CCcEnv*)VectCtrlObj[st_task_id.ENV];
	pCS = (CCcCS*)VectCtrlObj[st_task_id.CS];

	crane_id = pCrane->st_crane_inf.crane_id;
	switch (pCrane->st_crane_inf.crane_type) {
	case CRANE_TYPE_ID_JC:
		fp_fault_check = fault_check_JC;
		break;
	case CRANE_TYPE_ID_GC:
		fp_fault_check = fault_check_GC;
		break;
	case CRANE_TYPE_ID_OHC:
		fp_fault_check = fault_check_OHC;
		break;
	default:

		break;
	}
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

	CCcPol* pPolObj = (CCcPol*)lpParam;
	int code = 0;
	return S_OK;
}

static double check_d;
HRESULT CCcPol::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		wos << L"SWAY X:" << check_d;
		msg2host(wos.str());
	}
	input();
	parse();
	output();
	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;


int CCcPol::input() {

	if (aux_sway_status) {
		check_d = pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].amp_p2p;
	}
	return S_OK;
}

int CCcPol::parse() {           //メイン処理
//### 制御PC検出異常、警報状態設定処理
 
	fp_fault_check(crane_id);

	return STAT_OK;
}
int CCcPol::output() {          //出力処理
	return STAT_OK;
}

int CCcPol::close() {

	return 0;
}

HRESULT CCcPol::fault_check_JC(int crane_id) {
	//## 機上PLC通信異常
//CC_AGENTでセットされる　PLCヘルシーチェック

//## 操作端末通信異常
//CC_CSでセットされる　	ソケット未生成,送受信時のエラー

//## 旋回ブレーキ通信異常
//CC_AGENTでセットされる　	AUX_CSとのヘルシーチェック

//## 遠隔モードスイッチ警報
	INT16 rmtsw = pPlcIO->remote_mode_sw;
	if (rmtsw) {
		pPolInf->pc_fault_map[FLTS_ID_RMTSW_OFF] &= ~FLTS_MASK_RMTSW_OFF;
	}
	else {
		pPolInf->pc_fault_map[FLTS_ID_RMTSW_OFF] |= FLTS_MASK_RMTSW_OFF;
	}

	//## 有効操作端末無し警報
	//CC_CSでセットされる　有効操作端末無し
	// 
	//## 旋回ブレーキ関連
	//CC_AGENTでセットされる
	return S_OK;
}
HRESULT CCcPol::fault_check_GC(int crane_id) {
	return S_OK;
}
HRESULT CCcPol::fault_check_OHC(int crane_id) {
	return S_OK;
}

/****************************************************************************/
/*　　COMMAND 処理															*/
/****************************************************************************/
/// <summary>
/// AGENTからのコマンド要求処理
/// </summary>
/// <param name="pjob_set"></param>
/// <returns></returns>
LPST_COMMAND_SET CCcPol::req_command(LPST_JOB_SET pjob_set) {

	if (pjob_set == NULL) return NULL;	//NULL jobにはNULLリターン

	int _i_hot_com = pjob_set->i_hot_com;
	LPST_COMMAND_SET pcom_set = NULL;

	if (pjob_set->status & STAT_TRIGED) {										//JOBのステータスが実行待ち
		_i_hot_com = pjob_set->i_hot_com = 0;									//起動時は、実行レシピのインデックスは、0
		pcom_set = setup_job_command(pjob_set, _i_hot_com);
		pjob_set->com[_i_hot_com].com_status = STAT_STANDBY;					//コマンドステータス更新

		pCS->update_job_status(pjob_set, STAT_STANDBY);							//JOBのステータスをSTANDBYに更新
	}
	else if (pjob_set->status & STAT_SUSPENDED) {								//JOB中断中
		pcom_set = setup_job_command(pjob_set, _i_hot_com);						//中断中のコマンドを再計算
		pjob_set->com[_i_hot_com].com_status = STAT_STANDBY;					//コマンドステータス更新
	}
	else if (pjob_set->status & STAT_ACTIVE) {									//JOB実行中からの呼び出し＝ 次のレシピ実行待ち
		if (pjob_set->i_hot_com < (pjob_set->n_com - 1)) {						//次のレシピ有 
			//現在のコマンド完了済で次のコマンド計算　完了していなければ今のコマンドを再計算
			if (pjob_set->com[pjob_set->i_hot_com].com_status & STAT_END) {	//完了している
				_i_hot_com += 1;
			}
			pcom_set = setup_job_command(pjob_set, _i_hot_com);
			pjob_set->com[_i_hot_com].com_status = STAT_STANDBY;	//コマンドステータス更新
		}
		else {	//実行中で次コマンド無 
			//現在のコマンド完了済でNULL　完了していなければ今のコマンドを再計算
			if (pjob_set->com[pjob_set->i_hot_com].com_status & STAT_END) {//完了している
				pcom_set = NULL;
				pCS->update_job_status(pjob_set, STAT_SEQ_ERR_END);		//JOBのステータスをSEQ　ERROR　END更新
			}
			else {
				pcom_set = setup_job_command(pjob_set, _i_hot_com);						//中断中のコマンドを再計算
				pjob_set->com[_i_hot_com].com_status = STAT_STANDBY;					//コマンドステータス更新
			}
		}
	}
	else;


	if (pcom_set != NULL) {
		//### コマンドコードセット
		pcom_set->com_code.i_list = pjob_set->list_id;
		pcom_set->com_code.i_job = pjob_set->job_id;
		pcom_set->com_code.type = pjob_set->type;
	}
	return pcom_set;
}

/// <summary>
/// 
/// </summary>
/// <param name="pcom"></param>
/// <param name="code"></param>
/// <returns></returns>
int CCcPol::update_command_status(LPST_COMMAND_SET pcom, int code) {

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//現状　SEMIAUTOの1job　1コマンドのみ対象 それ以外は後で
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (pcom == NULL)return STAT_NAK;
	LPST_JOB_SET pjob_set = &pJobIO->job_list[pcom->com_code.i_list].job[pcom->com_code.i_job];//紐付きJOB

	LPST_COMMAND_SET pcom_seq = &pjob_set->com[pjob_set->i_hot_com];
	switch (code) {
		//コマンド終了
	case STAT_END: {
		if (pjob_set->n_com == (pjob_set->i_hot_com + 1)) {	//コマンドシーケンスの最後のコマンドの時
			pcom_seq->com_status = code;							//コマンドのステータスを報告内容に更新
			pCS->update_job_status(pjob_set, STAT_END);				//JOBのステータス更新
		}
	}break;
	case STAT_ABNORMAL_END: {
		pcom_seq->com_status = code;							//コマンドのステータスを報告内容に更新
		pCS->update_job_status(pjob_set, STAT_ABNORMAL_END);	//JOBのステータス更新
	}break;
	case STAT_ABOTED: {
		if (pCsInf->cs_ctrl.auto_mode == L_ON) {
			pcom_seq->com_status = STAT_SUSPENDED;				//コマンドのステータスを報告内容に更新
			pCS->update_job_status(pjob_set, STAT_SUSPENDED);	//JOBのステータス更新
		}
		else {
			pcom_seq->com_status = code;							//コマンドのステータスを報告内容に更新
			pCS->update_job_status(pjob_set, STAT_ABOTED);			//JOBのステータス更新
		}

	}break;

					//コマンド開始
	case STAT_ACTIVE: {
		pCS->update_job_status(pjob_set, STAT_ACTIVE);			//JOBのステータス更新
		pcom_seq->com_status = code;							//コマンドのステータスを報告内容に更新
	}break;

					//実行中断
	case STAT_SUSPENDED: {
		pcom_seq->com_status = code;							//コマンドのステータスを報告内容に更新
		pCS->update_job_status(pjob_set, STAT_SUSPENDED);		//JOBのステータス更新
	}break;
	default: break;
	}
	//return req_command(pjob_set);
	return STAT_ACK;
}

/// <summary>
/// 
/// </summary>
/// <param name="pcom_seq"></param>
/// <param name="icom"></param>
/// <returns></returns>
LPST_COMMAND_SET CCcPol::setup_job_command(LPST_JOB_SET pjob, int icom) {
	{

		LPST_COMMAND_SET pcom_set = &pjob->com[icom];

		//半自動は、巻、旋回、引込 補巻が対象
		for (int i = 0; i < MOTION_ID_MAX; i++) pcom_set->seq_mode[i] = L_OFF;//パターン作成フラグクリア

		if (pjob->type == ID_JOBTYPE_SEMI) {
			for (int k = 0; k < MOTION_ID_MAX; k++) {//OTE動作選択のあるもののみパターン作成
				if (pCsInf->cs_ctrl.auto_status[k]) pcom_set->seq_mode[k] = L_ON;
			}
		}
		else if (pjob->type == ID_JOBTYPE_ANTISWAY) {
			for (int k = 0; k < MOTION_ID_MAX; k++) {//旋回、引込でOTE動作選択のあるもののみパターン作成
				if ((k == ID_BOOM_H) || (k == ID_SLEW)) {
					if (pCsInf->cs_ctrl.auto_status[k]) pcom_set->seq_mode[k] = L_ON;
				}
				else {
					pcom_set->seq_mode[k] = L_OFF;
				}
			}
		}
		else if (pjob->type == ID_JOBTYPE_JOB) {
			pcom_set->seq_mode[ID_HOIST] = L_ON;
			pcom_set->seq_mode[ID_SLEW] = L_ON;
			pcom_set->seq_mode[ID_BOOM_H] = L_ON;
			pcom_set->seq_mode[ID_AHOIST] = L_ON;
		}
		else return NULL;

		pcom_set->target = pjob->targets[pjob->i_hot_com];//目標位置セット

		set_com_workbuf(pcom_set);	//半自動パターン作成作業用構造体（st_com_work）にデータ取り込み

		bool is_fb_antisway = false;
		if (pCsInf->cs_ctrl.antisway_mode == L_ON) {
			is_fb_antisway = true;
		}
		//コマンドセットに目標位置セット
		pcom_set->target = st_com_work.target;
		//旋回,引込,巻のレシピセット　set_seq_semiauto_bh(JOBタイプ,レシピアドレス,isFBタイプ,レシピ設定条件バッファアドレス
		set_seq_semiauto_bh(pjob->type, &(pcom_set->seq[ID_BOOM_H]), is_fb_antisway, &st_com_work);
		set_seq_semiauto_slw(pjob->type, &(pcom_set->seq[ID_SLEW]), is_fb_antisway, &st_com_work);
		set_seq_semiauto_mh(pjob->type, &(pcom_set->seq[ID_HOIST]), is_fb_antisway, &st_com_work);
		set_seq_semiauto_ah(pjob->type, &(pcom_set->seq[ID_AHOIST]), is_fb_antisway, &st_com_work);
		return pcom_set;
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="pcom"></param>
/// <returns></returns>
LPST_POLICY_COM_WORK   CCcPol::set_com_workbuf(LPST_COMMAND_SET pcom) {

	st_com_work.agent_scan_ms = pAgent->inf.cycle_ms;					//AGENTタスクのスキャンタイム msec
	st_com_work.agent_scan = 0.001 * (double)st_com_work.agent_scan_ms; //AGENTタスクのスキャンタイム sec
	st_com_work.target = pcom->target;									//目標位置

	for (int i = 0; i < MOTION_ID_MAX; i++) {
		//現在位置
		st_com_work.pos[i] = pPlcIO->stat_axis[i].pos_fb;
		//現在速度
		st_com_work.v[i] = pPlcIO->stat_axis[i].v_fb;
		//移動距離　方向と目標までの距離
		st_com_work.dist_for_target[i] = st_com_work.target.pos[i] - st_com_work.pos[i];
		if (i == ID_SLEW) {		//旋回は、絶対値が180を越えるときは逆方向が近い
			if (st_com_work.dist_for_target[ID_SLEW] > PI180)		st_com_work.dist_for_target[ID_SLEW] -= PI360;
			else if (st_com_work.dist_for_target[ID_SLEW] < -PI180) st_com_work.dist_for_target[ID_SLEW] += PI360;
			else;
		}

		if (st_com_work.dist_for_target[i] < -pCrane->pSpec->auto_spec[i].as_pos_level[ID_LV_TRIGGER]) {
			st_com_work.motion_dir[i] = ID_REV;
			st_com_work.dist_for_target_abs[i] = -1.0 * st_com_work.dist_for_target[i];
		}
		else if (st_com_work.dist_for_target[i] > pCrane->pSpec->auto_spec[i].as_pos_level[ID_LV_TRIGGER]) {
			st_com_work.motion_dir[i] = ID_FWD;
			st_com_work.dist_for_target_abs[i] = st_com_work.dist_for_target[i];
		}
		else if (pcom->com_code.type == ID_JOBTYPE_ANTISWAY) {//振れ止めJOBの時は目標までの距離が0の時があるのでSELECT設定とする

			st_com_work.motion_dir[i] = ID_SELECT;
			st_com_work.dist_for_target_abs[i] = 0.0;
		}
		else {
			st_com_work.motion_dir[i] = ID_STOP;
			st_com_work.dist_for_target_abs[i] = 0.0;
		}

		//最大速度
		st_com_work.vmax_abs[i] = pCrane->pSpec->axis_spec[i].Notch_spd_f[pPlcIO->stat_axis[i].mode][NOTCH_MAX - 1];

		//動作軸加速度（開始位置,終了位置）
		st_com_work.a_abs[i][POL_ID_START_POINT] = pEnv->cal_acc(i, pPlcIO->stat_axis[i].pos_fb);
		st_com_work.a_abs[i][POL_ID_END_POINT] = pEnv->cal_acc(i, pcom->target.pos[i]);
		//最大加速時間
		st_com_work.acc_time2Vmax[i] = st_com_work.vmax_abs[i] / pCraneStat->spec.accdec[i][FWD][ACC];
		if (st_com_work.acc_time2Vmax[i] < 0.0) st_com_work.acc_time2Vmax[i] *= -1.0;
		st_com_work.dec_time2Vmax[i] = st_com_work.vmax_abs[i] / pCraneStat->spec.accdec[i][FWD][DEC];
		if (st_com_work.dec_time2Vmax[i] < 0.0) st_com_work.dec_time2Vmax[i] *= -1.0;

		if ((i == ID_BOOM_H) || (i == ID_SLEW)) {
			//吊点の加速度
			st_com_work.a_hp_abs[i][POL_ID_START_POINT] = pEnv->cal_acc_hp(i, pPLC_IO->pos[ID_BOOM_H], pPLC_IO->pos[i]);
			st_com_work.a_hp_abs[i][POL_ID_END_POINT] = pEnv->cal_acc_hp(i, pcom->target.pos[ID_BOOM_H], pcom->target.pos[i]);

			//加速時振れ中心
			st_com_work.pp_th0[i][ACC] = st_com_work.a_hp_abs[i][POL_ID_START_POINT] / GA;
			//減速時振れ中心
			st_com_work.pp_th0[i][DEC] = -st_com_work.a_hp_abs[i][POL_ID_END_POINT] / GA;
		}
	}

	//巻きの目標位置が上の時は、巻上後に旋回引き込み動作をするので目標位置の周期でパターンを作る
	if (debug_mode & CODE_POLICY_DEBUG_SIM_MODE) {
		if (st_com_work.target.pos[ID_HOIST] > st_com_work.pos[ID_HOIST]) {
			st_com_work.T[ID_BOOM_H] = st_com_work.T[ID_SLEW] = pEnv->cal_T(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_HOIST);
			st_com_work.w[ID_BOOM_H] = st_com_work.w[ID_SLEW] = pEnv->cal_w(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_HOIST);	//振れ角周波数
			st_com_work.w2[ID_BOOM_H] = st_com_work.w2[ID_SLEW] = pEnv->cal_w2(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_HOIST);							//振れ角周波数2乗
		}
		else {
			//振れ周期
			st_com_work.T[ID_BOOM_H]	= pEnvInf->Ty;	st_com_work.T[ID_SLEW]	= pEnvInf->Tx;	
			//振れ角周波数
			st_com_work.w[ID_BOOM_H]	= pEnvInf->wy;	st_com_work.w[ID_SLEW]	= pEnvInf->wx;
			st_com_work.w2[ID_BOOM_H]	= pEnvInf->w2y;	st_com_work.w2[ID_SLEW] = pEnvInf->w2x;
		}
	}
	else {
		if (st_com_work.target.pos[ID_HOIST] > st_com_work.pos[ID_HOIST]) {
			st_com_work.T[ID_BOOM_H] = pEnv->cal_T(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.T[ID_SLEW] = pEnv->cal_T(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ周期
			st_com_work.w[ID_BOOM_H] = pEnv->cal_w(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.w[ID_SLEW] = pEnv->cal_w(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ角周波数
			st_com_work.w2[ID_BOOM_H] = pEnv->cal_w2(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.w2[ID_SLEW] = pEnv->cal_w2(st_com_work.target.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ角周波数2乗
		}
		else {
			st_com_work.T[ID_BOOM_H] = pEnv->cal_T(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.T[ID_SLEW] = pEnv->cal_T(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ周期

			st_com_work.w[ID_BOOM_H] = pEnv->cal_w(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.w[ID_SLEW] = pEnv->cal_w(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ角周波数

			st_com_work.w2[ID_BOOM_H] = pEnv->cal_w2(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_BOOM_H);
			st_com_work.w2[ID_SLEW] = pEnv->cal_w2(st_com_work.pos[ID_HOIST], st_com_work.target.pos[ID_BOOM_H], ID_SLEW);//振れ角周波数2乗
		}
	}
	return &st_com_work;
}

int CCcPol::set_seq_semiauto_bh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {
	return 0;
}
int CCcPol::set_seq_semiauto_slw(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {
	return 0;
}
int CCcPol::set_seq_semiauto_mh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {
	return 0;
}
int CCcPol::set_seq_semiauto_ah(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {
	return 0;
}


/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
static wostringstream monwos;

LRESULT CALLBACK CCcPol::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[POL_ID_MON1_STATIC_GPAD] = CreateWindowW(TEXT("STATIC"), st_mon1.text[POL_ID_MON1_STATIC_GPAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[POL_ID_MON1_STATIC_GPAD].x, st_mon1.pt[POL_ID_MON1_STATIC_GPAD].y,
			st_mon1.sz[POL_ID_MON1_STATIC_GPAD].cx, st_mon1.sz[POL_ID_MON1_STATIC_GPAD].cy,
			hWnd, (HMENU)(POL_ID_MON1_CTRL_BASE + POL_ID_MON1_STATIC_GPAD), hInst, NULL);

		//表示更新用タイマー
		SetTimer(hWnd, POL_ID_MON1_TIMER, st_mon1.timer_ms, NULL);

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
		KillTimer(hWnd, POL_ID_MON1_TIMER);
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
};

LRESULT CALLBACK CCcPol::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
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

HWND CCcPol::open_monitor_wnd(HWND h_parent_wnd, int id) {

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
		wcex.lpszMenuName = TEXT("POL_MON1");
		wcex.lpszClassName = TEXT("POL_MON1");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon1.hwnd_mon = CreateWindowW(TEXT("POL_MON1"), TEXT("POL_MON1"), WS_OVERLAPPEDWINDOW,
			POL_MON1_WND_X, POL_MON1_WND_Y, POL_MON1_WND_W, POL_MON1_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);
		show_monitor_wnd(id);
	}
	else if (id == BC_ID_MON2) {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Mon1Proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = TEXT("POL_MON2");
		wcex.lpszClassName = TEXT("POL_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon  = CreateWindowW(TEXT("POL_MON2"), TEXT("POL_MON2"), WS_OVERLAPPEDWINDOW,
			POL_MON2_WND_X, POL_MON2_WND_Y, POL_MON2_WND_W, POL_MON2_WND_H,
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
void CCcPol::close_monitor_wnd(int id) {
	if (id == BC_ID_MON1)
		DestroyWindow(st_mon1.hwnd_mon);
	else if (id == BC_ID_MON2)
		DestroyWindow(st_mon2.hwnd_mon);
	else;
	return;
}
void CCcPol::show_monitor_wnd(int id) {
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
void CCcPol::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CCcPol::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
void CCcPol::msg2listview(wstring wstr) {

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
void CCcPol::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのEdit Box説明テキストを設定
void CCcPol::set_panel_tip_txt() {
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
void CCcPol::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CCcPol::set_item_chk_txt() {
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





