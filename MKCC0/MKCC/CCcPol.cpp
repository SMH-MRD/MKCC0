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
extern CSharedMem* pPlcIoObj;
extern CSharedMem* pJobIoObj;
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
	pPlcIO = (LPST_CC_PLC_IO)(pPlcIoObj->get_pMap());
	pJobIO = (LPST_JOB_IO)(pJobIoObj->get_pMap());
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
		st_com_work.acc_time2Vmax[i] = st_com_work.vmax_abs[i] / pCrane->pSpec->axis_spec[i].accdec[ID_FWD][ID_ACC];
		if (st_com_work.acc_time2Vmax[i] < 0.0) st_com_work.acc_time2Vmax[i] *= -1.0;
		st_com_work.dec_time2Vmax[i] = st_com_work.vmax_abs[i] / pCrane->pSpec->axis_spec[i].accdec[ID_FWD][ID_DEC];
		if (st_com_work.dec_time2Vmax[i] < 0.0) st_com_work.dec_time2Vmax[i] *= -1.0;

		if ((i == ID_BOOM_H) || (i == ID_SLEW)) {
			//吊点の加速度
			st_com_work.a_hp_abs[i][POL_ID_START_POINT] = pEnv->cal_acc_hp(i, pPlcIO->stat_axis[ID_BOOM_H].pos_fb, pPlcIO->stat_axis[i].pos_fb);
			st_com_work.a_hp_abs[i][POL_ID_END_POINT] = pEnv->cal_acc_hp(i, pcom->target.pos[ID_BOOM_H], pcom->target.pos[i]);

			//加速時振れ中心
			st_com_work.pp_th0[i][ID_ACC] = st_com_work.a_hp_abs[i][POL_ID_START_POINT] / GA;
			//減速時振れ中心
			st_com_work.pp_th0[i][ID_DEC] = -st_com_work.a_hp_abs[i][POL_ID_END_POINT] / GA;
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

/// <summary>
/// 
/// </summary>
/// <param name="jobtype"></param>
/// <param name="pseq"></param>
/// <param name="is_fbtype"></param>
/// <param name="pwork"></param>
/// <returns></returns>
int CCcPol::set_seq_semiauto_bh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {

	//#レシピ条件セット
	//軸ID
	int id = pseq->axis_id = ID_BOOM_H;

	//移動方向
	pseq->direction = pwork->motion_dir[id];
	double D_abs = pwork->dist_for_target_abs[id];	//残り移動距離

	double v2 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_2] * pPlcIO->v_ratio[ID_BOOM_H];
	double v3 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_3] * pPlcIO->v_ratio[ID_BOOM_H];
	double v4 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_4] * pPlcIO->v_ratio[ID_BOOM_H];
	double acc_s = pwork->a_abs[id][POL_ID_START_POINT], acc_e = pwork->a_abs[id][POL_ID_END_POINT];//スタート位置加速度と停止時軸加速度
	double acc_average = 0.5 * (acc_s + acc_e);
	double acc_s_hp = pwork->a_hp_abs[id][POL_ID_START_POINT], acc_hp_e = pwork->a_abs[id][POL_ID_END_POINT];//スタート位置加速度と停止時の吊点加速度

	double checkS4 = 0.5 * v4 * v4 / acc_s + v3 * (0.5 * pwork->T[ID_BOOM_H] - v3 / acc_s) + 0.5 * v4 * v4 / acc_e + v3 * (0.5 * pwork->T[ID_BOOM_H] - v3 / acc_e);	//4ノッチ2段加減速度最低移動距離
	double checkS3 = 0.5 * v3 * v3 / acc_s + v2 * (0.5 * pwork->T[ID_BOOM_H] - v2 / acc_s) + 0.5 * v3 * v3 / acc_e + v2 * (0.5 * pwork->T[ID_BOOM_H] - v2 / acc_e);	//3ノッチ2段加減速度最低移動距離

	int vfb_delay_count = (int)(pwork->vfb_delay_time[ID_BOOM_H] / ((double)pwork->agent_scan_ms / 1000.0));

	if (jobtype == ID_JOBTYPE_ANTISWAY) {
		if (((pwork->dist_for_target_abs[ID_BOOM_H] > pCrane->pSpec->auto_spec[id].as_pos_level[ID_LV_TRIGGER]) || (debug_mode & CODE_POLICY_DEBUG_AS_1SHOT))
			&& !(debug_mode & CODE_POLICY_DEBUG_AS_2SHOT))
		{
			pseq->motion_type = PTN_1SHOT_AS;							//ONE SHOT
		}
		else {
			pseq->motion_type = PTN_2SHOT_AS;							//TWO SHOT
		}
	}
	else if (D_abs > checkS3) {									//２段加減速パターンは３ノッチまで
		pseq->motion_type = PTN_HALF_T_AS;
	}
	else if (D_abs < v4 * v4 * (1.0 / acc_s + 1.0 / acc_e)) {	//Topスピードの２回インチング距離以下
		pseq->motion_type = PTN_2SHOT_MOVE0;
	}
	else {
		pseq->motion_type = PTN_ORDINARY;//パターン種別
	}

	LPST_MOTION_STEP pelement;
	//加速度が0.0はエラー　0割り防止
	if (pwork->a_abs[id][POL_ID_START_POINT] == 0.0) return POLICY_PTN_NG;

	/*### パターン作成 ###*/
	pseq->n_step = 0;														// ステップクリア

	//#######################################################################
	// ANTISWAY
	//#######################################################################
	if (jobtype == ID_JOBTYPE_ANTISWAY) {

		//ワンショット　＋　インチング移動 （目標までの距離が位置決め動作トリガ判定値以上）
		if (pseq->motion_type == PTN_1SHOT_AS) {

			bool is_sway_over1shot = false; //1回のインチングで完了不可フラグ

			pseq->motion_type = PTN_1SHOT_AS;							//ONE SHOT
			double v_top_abs = v4;
			double d_move_abs = 0.0;

			/*### STEP0  タイミング待ち　###*/
			pelement = &(pseq->steps[pseq->n_step++]);			// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_1SHOT;			// 位相タイミング待ち(目標へ近づく方で起動を掛ける 
			pelement->_t = pEnvInf->Ty * 2.0;					// タイミングチェック許容時間　振れ2周期分
			pelement->_v = 0.0;									// 速度0
			pelement->_p = pwork->target.pos[id];				// 目標位置　

			//移動方向は選択（Agentが決める）
			pelement->opt_i[ID_STEP_OPT_DIR] = ID_SELECT;

			//加速振れの4倍までは、１回の加減速で止められる（ただし、加速時間の制限考慮必要）
			double chk_d = pEnvInf->crane_stat.sway_amp_expected[ID_BOOM_H] / (2.0 * acc_s_hp / GA);
			if (chk_d > 2.0) {
				is_sway_over1shot = true;
				chk_d = 1.9999;
			}

			double ph = acos(1.0 - chk_d);	//acosの範囲は0～π
			double th0 = PI180 - ph;

			double ph_delay;
			if (debug_mode & CODE_POLICY_DEBUG_SIM_MODE) {
				ph_delay = PI360 * POL_PRM_FB_DELAY_BH_SIM / pwork->T[ID_BOOM_H];
			}
			else {
				ph_delay = PI360 * POL_PRM_FB_DELAY_BH / pwork->T[ID_BOOM_H];
			}
			th0 -= ph_delay;	//FB遅れ時間分位相を早める

			pelement->opt_d[ID_STEP_OPT_PHASE_FWD] = th0 - PI180;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_FWD] < -PI180) pelement->opt_d[ID_STEP_OPT_PHASE_FWD] += PI360;

			pelement->opt_d[ID_STEP_OPT_PHASE_REV] = th0;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_REV] > PI180) pelement->opt_d[ID_STEP_OPT_PHASE_REV] -= PI360;

			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = RAD1DEG * 2;
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP1 ワンショット出力###*/
			pelement->opt_i[ID_STEP_OPT_DIR] = pseq->direction;
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_1SHOT;					//加速時間出力
			pelement->_t = ph / pwork->w[ID_BOOM_H];//pCraneStat->w; +pwork->vfb_delay_time[ID_BOOM_H];	// FB遅れ時間分加算
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = v_top_abs;									// 出力速度

			d_move_abs = 0.5 * pelement->_t * pelement->_t * acc_s;		// 第一ステップ完了までの移動距離 (acc_sは軸の加速度）
			D_abs -= d_move_abs;

			//起動判定許容範囲セット		
			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = PI10;
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP2 ワンショット停止###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;						//加速時間出力
			pelement->_t = (pelement - 1)->_t;							// とりあえず制定時間1.0秒付加
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度

			d_move_abs = 0.5 * pelement->_t * pelement->_t * acc_s;	// 第一ステップ完了までの移動距離 
			D_abs -= d_move_abs;

			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;
		}
		else {
			bool is_sway_over1shot = false; //1回のインチングで完了不可フラグ
			pseq->motion_type = PTN_2SHOT_AS;					//その場振れ止め
			double v_top_abs = v4;
			double d_move_abs = 0.0;

			/*### STEP0  タイミング待ち　###*/
			pelement = &(pseq->steps[pseq->n_step++]);			// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_FIRST;		// 位相タイミング待ち(初回） 
			pelement->_t = pEnvInf->Ty * 2.0;					// タイミングチェック許容時間　振れ2周期分
			pelement->_v = 0.0;									// 速度0
			pelement->_p = pwork->target.pos[id];				// 目標位置　

			pelement->opt_i[ID_STEP_OPT_DIR] = pseq->direction;
			//加速振れの4倍までは、１回の加減速で止められる（ただし、加速時間の制限考慮必要）
			double chk_d = pEnvInf->crane_stat.sway_amp_expected[ID_BOOM_H] / (4.0 * acc_s_hp / GA);
			double ph;
			if (chk_d < 2.0) {
				is_sway_over1shot = false;
				ph = acos(1.0 - chk_d);
			}
			else {
				is_sway_over1shot = true;
				ph = PI180;
			}
			//起動開始位相,2回目待機位相
			double th0 = PI180 - ph;
			double phc = PI180 - 2.0 * ph;
			//速度FB遅れ分を考慮して開始位相を早める分の補正量を求める

			double ph_delay;
			if (debug_mode & CODE_POLICY_DEBUG_SIM_MODE) {
				ph_delay = PI360 * POL_PRM_FB_DELAY_BH_SIM / pwork->T[ID_BOOM_H];
			}
			else {
				ph_delay = PI360 * POL_PRM_FB_DELAY_BH / pwork->T[ID_BOOM_H];
			}

			//			int vfb_delay_count = (int)(pwork->vfb_delay_time[ID_BOOM_H] / ((double)pwork->agent_scan_ms / 1000.0));
			phc -= ph_delay;	//FB遅れ時間分位相を早める
			if (phc < 0.0) phc += PI360;
			th0 -= ph_delay;		//FB遅れ時間分位相を早める
			//起動判定位相セット
			pelement->opt_d[ID_STEP_OPT_PHASE_FWD] = th0 - PI180;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_FWD] < -PI180)pelement->opt_d[ID_STEP_OPT_PHASE_FWD] += PI360;
			pelement->opt_d[ID_STEP_OPT_PHASE_REV] = th0;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_REV] > PI180)pelement->opt_d[ID_STEP_OPT_PHASE_REV] -= PI360;
			//起動判定許容範囲セット		
			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = PI10;
			//速度FB遅れ分カウント値セット
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;


			/*### STEP1 2インチング　 1回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT1;					//加速時間出力
			pelement->_t = ph / pwork->w[ID_BOOM_H];								//停止からの起動時は遅れ時間考慮 
			pelement->_p = (pelement - 1)->_p;							// 目標位置（ターゲット位置）
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP2 2インチング　 位相待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_SECOND;				//減速＋2回目起動位相待ち
			pelement->_t = (pelement - 1)->_t + phc / pwork->w[ID_BOOM_H];			//減速時間＋位相待ち時間											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = 0;

			/*### STEP3 2インチング　 2回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT2;					//加速時間出力
			pelement->_t = (pelement - 2)->_t;							// １回目と同じ加速時間
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP4 2インチング　 減速待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;					// 減速時間出力
			pelement->_t = (pelement - 1)->_t;							// 加速時間と同じ時間
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

		}
#if 0
		/*### STEP8 END ###*/
		//微小位置決め
		pelement = &(pseq->steps[pseq->n_step++]);						// ステップのポインタセットして次ステップ用にカウントアップ
		pelement->type = CTR_TYPE_FINE_POS;								// 微小位置決め
		pelement->_t = FINE_POS_TIMELIMIT;								// 位置合わせ最大継続時間
		pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];		// １ノッチ速度
		pelement->_p = st_com_work.target.pos[id];						// 目標位置
		D_abs = 0.0;													// 残り距離変更なし
#endif
	}
	//#######################################################################
	// SEMI AUTO
	//#######################################################################
	else if (jobtype == ID_JOBTYPE_SEMI) {

		/*### STEP0  待機　###*/
			//top_speed計算
		double v_top_abs = 0.0, v_half = 0.0;
		double d_move_abs = 0.0, d_accdec, ta_s, ta_e, tcmax;
		int n = 0, i;

		switch (pseq->motion_type) {
		case PTN_ORDINARY: {	//単純移動（台形）パターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_TIME;						// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			for (i = (NOTCH_MAX - 1); i > 0; i--) {
				v_top_abs = pCrane->pSpec->axis_spec[id].Notch_pad_f[i];
				ta_s = v_top_abs / st_com_work.a_abs[id][POL_ID_START_POINT];		//加速時間(開始地点）
				ta_e = v_top_abs / st_com_work.a_abs[id][POL_ID_END_POINT];			//減速時間(終了地点）
				d_accdec = 0.5 * v_top_abs * (ta_s + ta_e);							//加速距離＋減速距離
				tcmax = (D_abs - d_accdec) / v_top_abs;								//定速度出力時間
				if (tcmax > 1.0)
					break;												//定速度出力時間1.0以上でブレーク
			}
			v_half = 0.5 * v_top_abs;
		}break;
		case PTN_HALF_T_AS: {		//２段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_TIME;						// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			if (D_abs > checkS4) {
				v_top_abs = v4;	v_half = v3;
			}
			else if (D_abs > v3) {
				v_top_abs = v3;
				v_half = v2;
			}
			else {
				v_top_abs = 0.0;
				v_half = 0.0;
			}
		}break;
		case PTN_2SHOT_MOVE0: {		//２段インチング移動パターン

			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_FIRST;				// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			if (pwork->dist_for_target[ID_BOOM_H] > 0.0) {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_REV;
			}
			else if (pwork->dist_for_target[ID_BOOM_H] < 0.0) {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_FWD;
			}
			else {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_STOP;
			}

			v_top_abs = sqrt((D_abs * acc_s * acc_e) / (acc_s + acc_e));
			v_half = 0.5 * v_top_abs;


		}break;
		default:return POLICY_PTN_NG;
		}

		ta_s = v_top_abs / acc_s;	ta_e = v_top_abs / acc_e;

		/*### STEP1 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY: {	//単純移動パターン
			//1ノッチでも定速度出ないときはSTEPを飛ばす
			if (tcmax < 0.0)break;

			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_POS;
			pelement->_t = tcmax + ta_s;
			d_move_abs = v_top_abs * tcmax + 0.5 * v_top_abs * v_top_abs / acc_s;	// 減速開始点までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = pwork->target.pos[ID_BOOM_H] + 0.5 * v_top_abs * v_top_abs / acc_e;	// 目標位置
				pelement->_v = -v_top_abs;															// 出力速度
			}
			else {
				pelement->_p = pwork->target.pos[ID_BOOM_H] - 0.5 * v_top_abs * v_top_abs / acc_e;// 目標位置
				pelement->_v = v_top_abs;										// 出力速度
			}
			D_abs -= d_move_abs;
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME;								//
			pelement->_t = 0.5 * pwork->T[ID_BOOM_H];										// 
			if (v_half / acc_s > pelement->_t) pelement->_t += pwork->T[ID_BOOM_H];		//半周期で加速しきらない時＋T

			d_move_abs = v_half * pelement->_t - 0.5 * v_half * v_half / acc_average;	// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs;			// 目標位置
				pelement->_v = -v_half;									// 出力速度
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs;			// 目標位置
				pelement->_v = v_half;									// 出力速度
			}
			D_abs -= d_move_abs;
		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT1;					//位置到達待ちステップ出力
			pelement->_t = v_top_abs / acc_s;							// 

			d_move_abs = 0.5 * v_top_abs * v_top_abs / acc_s;			// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs;// 目標位置
				pelement->_v = -v4;								// 出力速度
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs;// 目標位置
				pelement->_v = v4;								// 出力速度
			}
			D_abs -= d_move_abs;
		}break;
		default:return POLICY_PTN_NG;
		}

		/*### STEP2 速度ステップ出力 ###*/

		//*****目標位置付近の加速度で再計算*******

		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{																		// 出力するノッチ速度を計算して設定
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//減速停止
			pelement->_t = ta_e;												//減速時間
			pelement->_v = 0.0;													//速度0
			pelement->_p = st_com_work.target.pos[id];							// 目標位置(遅れ時間1sec考慮）
			D_abs = 0.0;														// 残り距離更新

		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_POS;									//位置到達待ちステップ出力

			//	D_abs = 0.5 * pwork->T * v_half;			//減速停止距離
			if (v_half / acc_e > 0.5 * pwork->T[ID_BOOM_H]) {
				D_abs = v_half * v_half / acc_e + 1.5 * pwork->T[ID_BOOM_H] * v_half;//v_halfの減速時間が半周期を越える時は＋T
			}
			else {
				D_abs = v_half * v_half / acc_e + 0.5 * pwork->T[ID_BOOM_H] * v_half;
			}

			pelement->_t = (pwork->dist_for_target_abs[id] - D_abs * 2.0) / v_top_abs + v_half / acc_e;

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs + 0.3 * v_top_abs;// 目標位置 0.3s　遅れ時間考慮
				pelement->_v = -v_top_abs;									// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs - 0.3 * v_top_abs;// 目標位置 0.3s　遅れ時間考慮
				pelement->_v = v_top_abs;									// 出力速度
			}

		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_SECOND;						//時間到達待ちステップ出力
			pelement->_t = 0.5 * pwork->T[ID_BOOM_H] - ta_s;								//π-φ
			if (pelement->_t < 0.0)pelement->_t += pwork->T[ID_BOOM_H];

			D_abs = v_top_abs * ta_e;						// 残り2回目インチング距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = 0.0;								// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = 0.0;								// 出力速度
			}
		}break;
		default:return POLICY_PTN_NG;
		}

		/*### STEP3 速度ステップ出力 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{
			//無し
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME;								//時間到達待ちステップ出力

			D_abs = 0.5 * v_half * v_half / acc_e;			// 第一ステップ完了までの移動距離

			pelement->_t = 0.5 * pwork->T[ID_BOOM_H];
			if (v_half / acc_e > 0.5 * pwork->T[ID_BOOM_H]) pelement->_t += pwork->T[ID_BOOM_H];

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = -v_half;									// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = v_half;									// 出力速度
			}
		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT2;							//位置到達待ちステップ出力
			pelement->_t = v_top_abs / acc_e;									// 

			D_abs = 0.5 * v_top_abs * v_top_abs / acc_e;						// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = -v4;								// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = v4;								// 出力速度
			}
		}break;
		default:return POLICY_PTN_NG;
		}

		/*### STEP4 速度ステップ出力 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//速度到達待ちステップ出力
			pelement->_t = v_half / acc_e;										// 

			D_abs = 0.0;
			pelement->_p = st_com_work.target.pos[id];// 目標位置
			pelement->_v = 0.0;									// 出力速度

		}break;
		case PTN_2SHOT_MOVE0: {	//単純移動パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//速度到達待ちステップ出力
			pelement->_t = v_top_abs / acc_e;										// 

			D_abs = 0.0;
			pelement->_p = st_com_work.target.pos[id];// 目標位置
			pelement->_v = 0.0;								// 出力速度

		}break;
		default:return POLICY_PTN_NG;
		}
#if 0
		/*### STEP END ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		case PTN_HALF_T_AS:
		case PTN_2SHOT_MOVE0:
		{//微小位置決め
			pelement = &(pseq->steps[pseq->n_step++]);						// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_FINE_POS;								// 微小位置決め
			pelement->_t = FINE_POS_TIMELIMIT;								// 位置合わせ最大継続時間
			pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];		// １ノッチ速度
			pelement->_p = st_com_work.target.pos[id];						// 目標位置
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//目標位置の校正
			D_abs = 0.0;													// 残り距離変更なし
		}break;
		default:return POLICY_PTN_NG;
		}
#endif
	}
	//#######################################################################
	// JOB
	//#######################################################################
	else {
		return 0;
	}

	//時間条件のスキャンカウント値セット
	for (int i = 0; i < pseq->n_step; i++) {
		pseq->steps[i].time_count = (int)(pseq->steps[i]._t / pwork->agent_scan);
		pseq->steps[i].status = STAT_STANDBY;
	}

	//ステップシーケンス準備完
	pseq->seq_status = STAT_STANDBY;

	//実行ステップ初期化
	pseq->i_hot_step = 0;

	return POLICY_PTN_OK;
}

/// <summary>
/// 
/// </summary>
/// <param name="jobtype"></param>
/// <param name="pseq"></param>
/// <param name="is_fbtype"></param>
/// <param name="pwork"></param>
/// <returns></returns>
int CCcPol::set_seq_semiauto_slw(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {
	//#レシピ条件セット
//軸ID
	int id = pseq->axis_id = ID_SLEW;

	//移動方向
	pseq->direction = pwork->motion_dir[id];
	double D_abs = pwork->dist_for_target_abs[id];	//残り移動距離

	double v2 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_2] * pPlcIO->v_ratio[ID_SLEW];
	double v3 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_3] * pPlcIO->v_ratio[ID_SLEW];
	double v4 = pCrane->pSpec->axis_spec[id].Notch_pad_f[NOTCH_4] * pPlcIO->v_ratio[ID_SLEW];
	double acc_s = pwork->a_abs[id][POL_ID_START_POINT], acc_e = pwork->a_abs[id][POL_ID_END_POINT];//スタート位置加速度と停止時加速度
	double acc_average = 0.5 * (acc_s + acc_e);
	double acc_s_hp = pwork->a_hp_abs[id][POL_ID_START_POINT], acc_hp_e = pwork->a_abs[id][POL_ID_END_POINT];//スタート位置加速度と停止時加速度

	//4ノッチ2段加減速度最低移動距離
	double checkS4 = 0.5 * v4 * v4 / acc_s + 2.0 * v3 * (0.5 * pwork->T[ID_SLEW] - v3 / acc_s) + 0.5 * v4 * v4 / acc_e + 2.0 * v3 * (0.5 * pwork->T[ID_SLEW] - v3 / acc_e);	//半周期で加速しきらないケースを考慮して定速部は2倍で見積もる
	//3ノッチ2段加減速度最低移動距離
	double checkS3 = 0.5 * v3 * v3 / acc_s + 2.0 * v2 * (0.5 * pwork->T[ID_SLEW] - v2 / acc_s) + 0.5 * v3 * v3 / acc_e + 2.0 * v2 * (0.5 * pwork->T[ID_SLEW] - v2 / acc_e);	//半周期で加速しきらないケースを考慮して定速部は2倍で見積もる

	//#レシピ条件セット

	int vfb_delay_count = (int)(pwork->vfb_delay_time[ID_SLEW] / ((double)pwork->agent_scan_ms / 1000.0));


	if (jobtype == ID_JOBTYPE_ANTISWAY) {
		if (((pwork->dist_for_target_abs[ID_SLEW] > pCrane->pSpec->auto_spec[ID_SLEW].as_pos_level[ID_LV_TRIGGER]) || (debug_mode & CODE_POLICY_DEBUG_AS_1SHOT))
			&& !(debug_mode & CODE_POLICY_DEBUG_AS_2SHOT))
		{
			pseq->motion_type = PTN_1SHOT_AS;							//ONE SHOT
		}
		else {
			pseq->motion_type = PTN_2SHOT_AS;							//TWO SHOT
		}
	}
	else if (D_abs > checkS3) {
		pseq->motion_type = PTN_HALF_T_AS;
	}
	else if (D_abs < v4 * v4 * (1.0 / acc_s + 1.0 / acc_e)) {	//２段加減速パターンは３ノッチまで
		pseq->motion_type = PTN_2SHOT_MOVE0;
	}
	else {
		pseq->motion_type = PTN_ORDINARY;//パターン種別
	}

	LPST_MOTION_STEP pelement;
	//加速度が0.0はエラー　0割り防止
	if (pwork->a_abs[id][POL_ID_START_POINT] == 0.0) return POLICY_PTN_NG;

	/*### パターン作成 ###*/
	pseq->n_step = 0;

	//#######################################################################
	// ANTISWAY
	//#######################################################################
	if (jobtype == ID_JOBTYPE_ANTISWAY) {
		//ワンショット　＋　インチング移動 （目標までの距離が位置決め動作トリガ判定値以上）
		if (pseq->motion_type == PTN_1SHOT_AS) {

			bool is_sway_over1shot = false; //1回のインチングで完了不可フラグ

			pseq->motion_type = PTN_1SHOT_AS;							//ONE SHOT
			double v_top_abs = v4;
			double d_move_abs = 0.0;

			/*### STEP0  タイミング待ち　###*/
			pelement = &(pseq->steps[pseq->n_step++]);			// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_1SHOT;			// 位相タイミング待ち(目標へ近づく方で起動を掛ける 
			pelement->_t = pEnvInf->Tx* 2.0;					// タイミングチェック許容時間　振れ2周期分
			pelement->_v = 0.0;									// 速度0
			pelement->_p = pwork->target.pos[id];				// 目標位置　
			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

			//移動方向は選択制度（Agentが決める）
			pelement->opt_i[ID_STEP_OPT_DIR] = ID_SELECT;

			//加速振れの4倍までは、１回の加減速で止められる（ただし、加速時間の制限考慮必要）
			double chk_d = pEnvInf->crane_stat.sway_amp_expected[ID_SLEW] / (2.0 * acc_s_hp / GA);
			if (chk_d > 2.0) {
				is_sway_over1shot = true;
				chk_d = 1.9999;
			}

			double ph = acos(1.0 - chk_d);	//acosの範囲は0～π
			double th0 = PI180 - ph;

			double ph_delay;
			if (debug_mode & CODE_POLICY_DEBUG_SIM_MODE) {
				ph_delay = PI360 * POL_PRM_FB_DELAY_SLEW_SIM / pwork->T[ID_SLEW];
			}
			else {
				ph_delay = PI360 * POL_PRM_FB_DELAY_SLEW / pwork->T[ID_SLEW];
			}
			th0 -= ph_delay;	//FB遅れ時間分位相を早める

			pelement->opt_d[ID_STEP_OPT_PHASE_FWD] = th0 - PI180;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_FWD] < -PI180) pelement->opt_d[ID_STEP_OPT_PHASE_FWD] += PI360;

			pelement->opt_d[ID_STEP_OPT_PHASE_REV] = th0;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_REV] > PI180) pelement->opt_d[ID_STEP_OPT_PHASE_REV] -= PI360;

			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = RAD1DEG;
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;


			/*### STEP1 ワンショット出力###*/
			pelement->opt_i[ID_STEP_OPT_DIR] = pseq->direction;

			pelement = &(pseq->steps[pseq->n_step++]);						//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_1SHOT;						//加速時間出力

			pelement->_t = ph / pwork->w[ID_SLEW] * PRM_1SHOT_PH_ADJUST;	//pCraneStat->w * PRM_1SHOT_PH_ADJUST;	// 
			pelement->_p = (pelement - 1)->_p;								// 目標位置
			pelement->_v = v_top_abs;										// 出力速度

			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする
			d_move_abs = 0.5 * pelement->_t * pelement->_t * acc_s;		// 第一ステップ完了までの移動距離 (acc_sは軸の加速度）
			D_abs -= d_move_abs;

			//起動判定許容範囲セット		
			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = RAD1DEG;
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP2 ワンショット停止###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME;						//加速時間出力
			pelement->_t = (pelement - 1)->_t;							// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度

			d_move_abs = 0.5 * pelement->_t * pelement->_t * acc_s;	// 第一ステップ完了までの移動距離 
			D_abs -= d_move_abs;
			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする

			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;
#if 0
			/*### STEP3 2インチング パラメータ計算###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_CAL_2INCH;					//加速時間出力
			pelement->_t = 0.0;											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度


			/*### STEP4 2インチング　 1回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2INCH1;					//加速時間出力
			pelement->_t = 0.0;											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP5 2インチング　 位相待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2INCH_MOVE;					//加速時間出力
			pelement->_t = 0.0;											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = 0;

			/*### STEP6 2インチング　 2回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2INCH2;					//加速時間出力
			pelement->_t = 0.0;											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP7 2インチング　 減速待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_TIME_2INCH2;					//加速時間出力
			pelement->_t = 0.0;											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			/*### STEP8 END ###*/
			//微小位置決め
			pelement = &(pseq->steps[pseq->n_step++]);						// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_FINE_POS;								// 微小位置決め
			pelement->_t = FINE_POS_TIMELIMIT;								// 位置合わせ最大継続時間
			pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];		// １ノッチ速度
			pelement->_p = st_com_work.target.pos[id];						// 目標位置
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//目標位置の校正
			D_abs = 0.0;													// 残り距離変更なし
#endif
		}
		else {//2SHOT
			bool is_sway_over1shot = false; //1回のインチングで完了不可フラグ
			pseq->motion_type = PTN_2SHOT_AS;					//その場振れ止め
			double v_top_abs = v4;
			double d_move_abs = 0.0;

			/*### STEP0  タイミング待ち　###*/
			pelement = &(pseq->steps[pseq->n_step++]);			// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_FIRST;		// 位相タイミング待ち(初回） 
			pelement->_t = pEnvInf->Tx * 2.0;					// タイミングチェック許容時間　振れ2周期分
			pelement->_v = 0.0;									// 速度0
			pelement->_p = pwork->target.pos[id];				// 目標位置　
			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

			pelement->opt_i[ID_STEP_OPT_DIR] = pseq->direction;
			//加速振れの4倍までは、１回の加減速で止められる（ただし、加速時間の制限考慮必要）
			double chk_d = pEnvInf->crane_stat.sway_amp_expected[ID_SLEW] / (4.0 * acc_s_hp / GA);
			double ph;
			if (chk_d < 2.0) {
				is_sway_over1shot = false;
				ph = acos(1.0 - chk_d);
			}
			else {
				is_sway_over1shot = true;
				ph = PI180;
			}
			//起動開始位相,2回目待機位相
			double th0 = PI180 - ph;
			double phc = PI180 - 2.0 * ph;

			//速度FB遅れ分を考慮して開始位相を早める分の補正量を求める

			double ph_delay;
			if (debug_mode & CODE_POLICY_DEBUG_SIM_MODE) {
				ph_delay = PI360 * POL_PRM_FB_DELAY_SLEW_SIM / pwork->T[ID_SLEW];
			}
			else {
				ph_delay = PI360 * POL_PRM_FB_DELAY_SLEW / pwork->T[ID_SLEW];
			}
			//int vfb_delay_count = (int)(pwork->vfb_delay_time[ID_SLEW] / ((double)pwork->agent_scan_ms / 1000.0));
			phc -= ph_delay;	//FB遅れ時間分位相を早める
			if (phc < 0.0) phc += PI360;

			th0 -= ph_delay;		//FB遅れ時間分位相を早める
			//起動判定位相セット
			pelement->opt_d[ID_STEP_OPT_PHASE_FWD] = th0 - PI180;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_FWD] < -PI180)pelement->opt_d[ID_STEP_OPT_PHASE_FWD] += PI360;
			pelement->opt_d[ID_STEP_OPT_PHASE_REV] = th0;
			if (pelement->opt_d[ID_STEP_OPT_PHASE_REV] > PI180)pelement->opt_d[ID_STEP_OPT_PHASE_REV] -= PI360;
			//起動判定許容範囲セット		
			pelement->opt_d[ID_STEP_OPT_PHASE_CHK_RANGE] = PI10;
			//速度FB遅れ分カウント値セット
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;


			/*### STEP1 2インチング　 1回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT1;					//加速時間出力
			pelement->_t = ph / pwork->w[ID_SLEW];						//停止からの起動時は遅れ時間考慮 
			pelement->_p = (pelement - 1)->_p;							// 目標位置（ターゲット位置）
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

			/*### STEP2 2インチング　 位相待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_SECOND;				//減速＋2回目起動位相待ち
			pelement->_t = (pelement - 1)->_t + phc / pwork->w[ID_SLEW];			//減速時間＋位相待ち時間											// 
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = 0;

			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

			/*### STEP3 2インチング　 2回目出力###*/
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT2;					//加速時間出力
			pelement->_t = (pelement - 2)->_t;							// １回目と同じ加速時間
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = v_top_abs;									// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

			/*### STEP4 2インチング　 減速待ち###*/
			pelement = &(pseq->steps[pseq->n_step++]);					// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;					// 減速時間出力
			pelement->_t = (pelement - 1)->_t;							// 加速時間と同じ時間
			pelement->_p = (pelement - 1)->_p;							// 目標位置
			pelement->_v = 0.0;											// 出力速度
			pelement->opt_i[ID_STEP_OPT_VFB_DELAY_COUNT] = vfb_delay_count;

			CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));		// 目標位置の校正（-180°～180°の表現にする

#if 0
			/*### STEP5 END ###*/
			//微小位置決め
			pelement = &(pseq->steps[pseq->n_step++]);						// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_FINE_POS;								// 微小位置決め
			pelement->_t = FINE_POS_TIMELIMIT;								// 位置合わせ最大継続時間
			pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];		// １ノッチ速度
			pelement->_p = st_com_work.target.pos[id];						// 目標位置
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//目標位置の校正
			D_abs = 0.0;// 残り距離変更なし
#endif
		}
	}

	//#######################################################################
	// SEMI AUTO
	//#######################################################################
	else if (jobtype == ID_JOBTYPE_SEMI) {

		/*### STEP0  待機　###*/
			//top_speed計算
		double v_top_abs = 0.0, v_half = 0.0;
		double d_move_abs = 0.0, d_accdec, ta_s, ta_e, tcmax;
		int n = 0, i;

		switch (pseq->motion_type) {
		case PTN_ORDINARY: {	//単純移動（台形）パターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_TIME;						// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			for (i = (NOTCH_MAX - 1); i > 0; i--) {
				v_top_abs = pCrane->pSpec->axis_spec[id].Notch_spd_f[pPlcIO->spd_mode[ID_SLEW]][i];
				ta_s = v_top_abs / st_com_work.a_abs[id][POL_ID_START_POINT];		//加速時間(開始地点）
				ta_e = v_top_abs / st_com_work.a_abs[id][POL_ID_END_POINT];			//減速時間(終了地点）
				d_accdec = 0.5 * v_top_abs * (ta_s + ta_e);							//加速距離＋減速距離
				tcmax = (D_abs - d_accdec) / v_top_abs;								//定速度出力時間
				if (tcmax > 1.0)
					break;												//定速度出力時間1.0以上でブレーク
			}
			v_half = 0.5 * v_top_abs;
		}break;
		case PTN_HALF_T_AS: {		//２段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_TIME;						// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			if (D_abs > checkS4) {
				v_top_abs = v4;	v_half = v3;
			}
			else if (D_abs > v3) {
				v_top_abs = v3;
				v_half = v2;
			}
			else {
				v_top_abs = 0.0;
				v_half = 0.0;
			}
		}break;
		case PTN_2SHOT_MOVE0: {		//２段インチング移動パターン

			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_FIRST;				// 時間他軸位置待ち
			pelement->_t = TIME_LIMIT_CONFIRMATION;						// 待機時間
			pelement->_v = 0.0;											// 速度0
			pelement->_p = pwork->pos[id];								// 目標位置　現在位置

			if (pwork->dist_for_target[ID_SLEW] > 0.0) {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_REV;
			}
			else if (pwork->dist_for_target[ID_SLEW] < 0.0) {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_FWD;
			}
			else {
				pelement->opt_i[ID_STEP_OPT_DIR] = ID_STOP;
			}

			v_top_abs = sqrt((D_abs * acc_s * acc_e) / (acc_s + acc_e));
			v_half = 0.5 * v_top_abs;


		}break;
		default:return POLICY_PTN_NG;
		}
		CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする
		ta_s = v_top_abs / acc_s;	ta_e = v_top_abs / acc_e;


		/*### STEP1 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY: {	//単純移動パターン
			//1ノッチでも定速度出ないときはSTEPを飛ばす
			if (tcmax < 0.0)break;

			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_POS;
			pelement->_t = tcmax + ta_s;
			d_move_abs = v_top_abs * tcmax + 0.5 * v_top_abs * v_top_abs / acc_s;	// 減速開始点までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = pwork->target.pos[ID_SLEW] + 0.5 * v_top_abs * v_top_abs / acc_e;	// 目標位置
				pelement->_v = -v_top_abs;															// 出力速度
			}
			else {
				pelement->_p = pwork->target.pos[ID_SLEW] - 0.5 * v_top_abs * v_top_abs / acc_e;// 目標位置
				pelement->_v = v_top_abs;										// 出力速度
			}
			D_abs -= d_move_abs;
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME;								//
			pelement->_t = 0.5 * pwork->T[ID_SLEW];										// 
			if (v_half / acc_s > pelement->_t) pelement->_t += pwork->T[ID_SLEW];		//半周期で加速しきらない時＋T

			d_move_abs = v_half * pelement->_t - 0.5 * v_half * v_half / acc_average;	// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs;			// 目標位置
				pelement->_v = -v_half;									// 出力速度
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs;			// 目標位置
				pelement->_v = v_half;									// 出力速度
			}
			D_abs -= d_move_abs;
		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);					//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT1;					//位置到達待ちステップ出力
			pelement->_t = v_top_abs / acc_s;							// 

			d_move_abs = 0.5 * v_top_abs * v_top_abs / acc_s;			// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs;// 目標位置
				pelement->_v = -v4;								// 出力速度
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs;// 目標位置
				pelement->_v = v4;								// 出力速度
			}
			D_abs -= d_move_abs;
		}break;
		default:return POLICY_PTN_NG;
		}

		CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする

		/*### STEP2 速度ステップ出力 ###*/

		//*****目標位置付近の加速度で再計算*******

		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{																		// 出力するノッチ速度を計算して設定
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//減速停止
			pelement->_t = ta_e;												//減速時間
			pelement->_v = 0.0;													//速度0
			pelement->_p = st_com_work.target.pos[id];							// 目標位置(遅れ時間1sec考慮）
			D_abs = 0.0;														// 残り距離更新

		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_POS;									//位置到達待ちステップ出力

			//	D_abs = 0.5 * pwork->T * v_half;			//減速停止距離
			if (v_half / acc_e > 0.5 * pwork->T[ID_SLEW]) {
				D_abs = v_half * v_half / acc_e + 1.5 * pwork->T[ID_SLEW] * v_half;//v_halfの減速時間が半周期を越える時は＋T
			}
			else {
				D_abs = v_half * v_half / acc_e + 0.5 * pwork->T[ID_SLEW] * v_half;
			}

			pelement->_t = (pwork->dist_for_target_abs[id] - D_abs * 2.0) / v_top_abs + v_half / acc_e;

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs + 0.3 * v_top_abs;// 目標位置 0.3s　遅れ時間考慮
				pelement->_v = -v_top_abs;									// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs - 0.3 * v_top_abs;// 目標位置 0.3s　遅れ時間考慮
				pelement->_v = v_top_abs;									// 出力速度
			}

		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_WAIT_PH_2SHOT_SECOND;						//時間到達待ちステップ出力
			pelement->_t = 0.5 * pwork->T[ID_SLEW] - 2.0 * ta_s;								//π-φ
			if (pelement->_t < 0.0)pelement->_t += pwork->T[ID_SLEW];
			pelement->_t += ta_s;

			D_abs = v_top_abs * ta_e;						// 残り2回目インチング距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = 0.0;								// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = 0.0;								// 出力速度
			}

		}break;
		default:return POLICY_PTN_NG;
		}
		CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする

		/*### STEP3 速度ステップ出力 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{
			//無し
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME;								//時間到達待ちステップ出力

			D_abs = 0.5 * v_half * v_half / acc_e;			// 第一ステップ完了までの移動距離

			pelement->_t = 0.5 * pwork->T[ID_SLEW];
			if (v_half / acc_e > 0.5 * pwork->T[ID_SLEW]) pelement->_t += pwork->T[ID_SLEW];

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = -v_half;									// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = v_half;									// 出力速度
			}
		}break;
		case PTN_2SHOT_MOVE0: {	//2段インチングパターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_TIME_2SHOT2;							//位置到達待ちステップ出力
			pelement->_t = v_top_abs / acc_e;									// 

			D_abs = 0.5 * v_top_abs * v_top_abs / acc_e;						// 第一ステップ完了までの移動距離 

			if (pseq->direction == ID_REV) {
				pelement->_p = st_com_work.target.pos[id] + D_abs;// 目標位置
				pelement->_v = -v4;								// 出力速度
			}
			else {
				pelement->_p = st_com_work.target.pos[id] - D_abs;// 目標位置
				pelement->_v = v4;								// 出力速度
			}
		}break;
		default:return POLICY_PTN_NG;
		}

		CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする

		/*### STEP4 速度ステップ出力 ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		{
		}break;

		case PTN_HALF_T_AS: {	//2段加減速パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//速度到達待ちステップ出力
			pelement->_t = v_half / acc_e;										// 

			D_abs = 0.0;
			pelement->_p = st_com_work.target.pos[id];// 目標位置
			pelement->_v = 0.0;									// 出力速度

		}break;
		case PTN_2SHOT_MOVE0: {	//単純移動パターン
			pelement = &(pseq->steps[pseq->n_step++]);							//ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_VOUT_V;									//速度到達待ちステップ出力
			pelement->_t = v_top_abs / acc_e;										// 

			D_abs = 0.0;
			pelement->_p = st_com_work.target.pos[id];// 目標位置
			pelement->_v = 0.0;								// 出力速度

		}break;
		default:return POLICY_PTN_NG;
		}
		CPhaseHelper::fit_ph_range_upto_pi(&(pelement->_p));				//目標位置の校正（-180°～180°の表現にする
#if 0
		/*### STEP END ###*/
		switch (pseq->motion_type) {
		case PTN_ORDINARY:	//単純移動パターン
		case PTN_HALF_T_AS:
		case PTN_2SHOT_MOVE0:
		{//微小位置決め
			pelement = &(pseq->steps[pseq->n_step++]);						// ステップのポインタセットして次ステップ用にカウントアップ
			pelement->type = CTR_TYPE_FINE_POS;								// 微小位置決め
			pelement->_t = FINE_POS_TIMELIMIT;								// 位置合わせ最大継続時間
			pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];		// １ノッチ速度
			pelement->_p = st_com_work.target.pos[id];						// 目標位置
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//目標位置の校正
			D_abs = 0.0;													// 残り距離変更なし
		}break;
		default:return POLICY_PTN_NG;
		}
#endif
	}
	//#######################################################################
	// JOB
	//#######################################################################
	else {
		return 0;
	}

	//時間条件のスキャンカウント値セット
	for (int i = 0; i < pseq->n_step; i++) {
		pseq->steps[i].time_count = (int)(pseq->steps[i]._t / pwork->agent_scan);
		pseq->steps[i].status = STAT_STANDBY;
	}
	//ステップシーケンス準備完
	pseq->seq_status = STAT_STANDBY;
	//実行ステップ初期化
	pseq->i_hot_step = 0;

	return POLICY_PTN_OK;
}

/// <summary>
/// 
/// </summary>
/// <param name="jobtype"></param>
/// <param name="pseq"></param>
/// <param name="is_fbtype"></param>
/// <param name="pwork"></param>
/// <returns></returns>
int CCcPol::set_seq_semiauto_mh(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {

	//#レシピ条件セット
	int id = pseq->axis_id = ID_HOIST;										//軸ID
	pseq->n_step = 0;														//ステップ数初期化
	pseq->direction = pwork->motion_dir[id];									//移動方向
	pseq->time_limit = POL_TM_OVER_CHECK_COUNTms / inf.cycle_ms;				//タイムオーバーカウント
	pseq->motion_type = PTN_ORDINARY;										//作成パターンのタイプ


	//#パターン計算用データセット
	double D_abs = pwork->dist_for_target_abs[id];							//残り移動距 絶対値								
	if (pwork->a_abs[id][POL_ID_START_POINT] == 0.0) return POLICY_PTN_NG;	//加速度が0.0はエラー　0割り防止


	/*### パターン作成 ###*/
	LPST_MOTION_STEP pelement;

	/*### STEP0  待機　###	引込、旋回位置待ち　巻上時：条件無し　巻下時： 引込・旋回共が目標位置の指定範囲内 */
	//確認待機
	pelement = &(pseq->steps[pseq->n_step++]);						//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_WAIT_TIME;									// 待機時間待ち
	pelement->_t = TIME_LIMIT_CONFIRMATION;									// 待ち時間
	pelement->_v = 0.0;														// 速度0
	pelement->_p = pwork->pos[id];											// 目標位置＝現在位置
	D_abs = D_abs;																	// 残り距離変更なし

	/*### STEP1,2 速度ステップ出力　###*/

	double v_top = 0.0;														//ステップ速度用
	double d_time_delay = 0.0;
	int n = 0, i;

	pelement = &(pseq->steps[pseq->n_step++]);								//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_VOUT_POS;										//位置到達待ち定速出力

	double ta = 0.0, v_top_abs = 0.0, d_accdec, tcmax;										//加速時間,定速時間

	for (i = (NOTCH_MAX - 1); i > 0; i--) {
		if (pseq->direction == ID_REV)	v_top = pCrane->pSpec->axis_spec[ID_HOIST].Notch_spd_r[pPlcIO->spd_mode[ID_HOIST]][i] * pPlcIO->v_ratio[ID_HOIST];
		else							v_top = pCrane->pSpec->axis_spec[ID_HOIST].Notch_spd_f[pPlcIO->spd_mode[ID_HOIST]][i] * pPlcIO->v_ratio[ID_HOIST];


		v_top_abs = v_top; if (v_top_abs < 0.0) v_top_abs *= -1.0;

		ta = v_top_abs / st_com_work.a_abs[id][POL_ID_START_POINT];
		d_accdec = v_top_abs * ta;									//加速＋減速距離
		tcmax = (D_abs - d_accdec) / v_top_abs;
		if (tcmax > 0.0) break;
	}

	if (tcmax < 0.0) {															//１ノッチまでの加速時間無し
		pelement->_t = 0.0;														// 加速時間　＋定速時間
		pelement->_v = 0.0;														// 速度
		D_abs = D_abs;															//残り移動距離更新
	}
	else {
		pelement->_t = tcmax + ta;												// 加速時間　＋定速時間
		pelement->_v = v_top;													// 速度
		double d_move_abs = v_top_abs * tcmax + d_accdec * 0.5;						// 減速開始点までの移動距離 
		D_abs -= d_move_abs;											//停止移動距離
	}

	if (pseq->direction == ID_REV) 	pelement->_p = st_com_work.target.pos[id] + d_accdec;				// 目標位置　ターゲット位置-減速距離
	else							pelement->_p = st_com_work.target.pos[id] - d_accdec;				// 目標位置　ターゲット位置-減速距離
	//	if (pseq->direction == ID_REV) 	pelement->_p = st_com_work.target.pos[id] + 0.5 * d_accdec;				// 目標位置　ターゲット位置-減速距離
	//	else							pelement->_p = st_com_work.target.pos[id] - 0.5 * d_accdec;				// 目標位置　ターゲット位置-減速距離

		/*### STEP2 停止　###*/
#if 1
	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_SMART_SLOW_DOWN;									//スマートスローダウン
	pelement->_t = ta * 2.0; 															//減速時間
	pelement->_v = 0.0;															// 速度0
	pelement->_p = st_com_work.target.pos[id];									// 目標位置
	D_abs = 0.0;																// 残り距離更新
#else

	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_VOUT_V;											//速度到達待ち
	pelement->_t = ta;															//減速時間
	pelement->_v = 0.0;															// 速度0
	pelement->_p = st_com_work.target.pos[id];									// 目標位置
	D_abs = 0.0;																// 残り距離更新
#endif
	/*### STEP3 位置合わせ　###*/
	pelement = &(pseq->steps[pseq->n_step++]);									// ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_FINE_POS;											// 微小位置決め
	pelement->_t = FINE_POS_TIMELIMIT;											// 位置合わせ最大継続時間
	pelement->_v = pCrane->pSpec->axis_spec[ID_HOIST].Notch_spd_f[pPlcIO->spd_mode[ID_HOIST]][NOTCH_1];					// １ノッチ速度
	pelement->_p = st_com_work.target.pos[id];									// 目標位置
	D_abs = 0.0;																// 残り距離変更なし


	//時間条件のスキャンカウント値セット
	for (int i = 0; i < pseq->n_step; i++) {
		pseq->steps[i].time_count = (int)(pseq->steps[i]._t / pwork->agent_scan);
		pseq->steps[i].status = STAT_STANDBY;
	}

	//ステップシーケンス準備完
	pseq->seq_status = STAT_STANDBY;

	//実行ステップ初期化
	pseq->i_hot_step = 0;

	return POLICY_PTN_OK;
}

/// <summary>
/// 
/// </summary>
/// <param name="jobtype"></param>
/// <param name="pseq"></param>
/// <param name="is_fbtype"></param>
/// <param name="pwork"></param>
/// <returns></returns>
int CCcPol::set_seq_semiauto_ah(int jobtype, LPST_MOTION_SEQ pseq, bool is_fbtype, LPST_POLICY_COM_WORK pwork) {

	//#レシピ条件セット
	int id = pseq->axis_id = ID_AHOIST;											//軸ID
	pseq->n_step = 0;															//ステップ数初期化
	pseq->direction = pwork->motion_dir[id];									//移動方向
	pseq->time_limit = POL_TM_OVER_CHECK_COUNTms / inf.cycle_ms;				//タイムオーバーカウント
	pseq->motion_type = PTN_ORDINARY;											//作成パターンのタイプ

	//#パターン計算用データセット
	double D_abs = pwork->dist_for_target_abs[id];								//残り移動距 絶対値								
	if (pwork->a_abs[id][POL_ID_START_POINT] == 0.0) return POLICY_PTN_NG;		//加速度が0.0はエラー　0割り防止

	/*### パターン作成 ###*/
	LPST_MOTION_STEP pelement;

	/*### STEP0  待機　###	引込、旋回位置待ち　巻上時：条件無し　巻下時： 引込・旋回共が目標位置の指定範囲内 */

	//巻き下げ時は、旋回、引込が目標付近着まで待機
	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_WAIT_TIME;										// 待機時間待ち
	pelement->_t = TIME_LIMIT_CONFIRMATION;										// 待ち時間
	pelement->_v = 0.0;															// 速度0
	pelement->_p = pwork->pos[id];												// 目標位置＝現在位置
	D_abs = D_abs;																// 残り距離変更なし

	/*### STEP1 速度ステップ出力　###*/
	double v_top = 0.0;															//ステップ速度用
	double d_time_delay = 0.0;
	int n = 0, i;

	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_VOUT_POS;											//位置到達待ち定速出力

	double ta = 0.0, v_top_abs = 0.0, tcmax, d_accdec;							//加速時間,定速時間

	for (i = (NOTCH_MAX - 1); i > 0; i--) {
		if (pseq->direction == ID_REV)	v_top = pCrane->pSpec->axis_spec[ID_AHOIST].Notch_spd_r[pPlcIO->spd_mode[ID_AHOIST]][i] * pPlcIO->v_ratio[ID_AHOIST];
		else							v_top = pCrane->pSpec->axis_spec[ID_AHOIST].Notch_spd_f[pPlcIO->spd_mode[ID_AHOIST]][i] * pPlcIO->v_ratio[ID_AHOIST];
		v_top_abs = v_top; if (v_top_abs < 0.0) v_top_abs *= -1.0;

		ta = v_top_abs / st_com_work.a_abs[id][POL_ID_START_POINT];
		d_accdec = v_top_abs * ta;									//加速＋減速距離
		tcmax = (D_abs - d_accdec) / v_top_abs;
		if (tcmax > 0.0) break;
	}

	if (tcmax < 0.0) {															//１ノッチまでの加速時間無し
		pelement->_t = 0.0;														// 加速時間　＋定速時間
		pelement->_v = 0.0;														// 速度
		D_abs = D_abs;															//停止移動距離更新
	}
	else {
		pelement->_t = tcmax + ta;												// 加速時間　＋定速時間
		pelement->_v = v_top;													// 速度
		double d_move_abs = v_top_abs * tcmax + d_accdec * 0.5;						// 減速開始点までの移動距離 
		D_abs -= d_move_abs;													//停止移動距離
	}

	if (pseq->direction == ID_REV) 	pelement->_p = st_com_work.target.pos[id] + d_accdec;				// 目標位置　ターゲット位置-減速距離×２
	else							pelement->_p = st_com_work.target.pos[id] - d_accdec;				// 目標位置　ターゲット位置-減速距離×２
	//	if (pseq->direction == ID_REV) 	pelement->_p = st_com_work.target.pos[id] + 0.5 * d_accdec;				// 目標位置　ターゲット位置-減速距離
	//	else							pelement->_p = st_com_work.target.pos[id] - 0.5 * d_accdec;				// 目標位置　ターゲット位置-減速距離

		/*### STEP2 停止　###*/
#if 1
	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_SMART_SLOW_DOWN;									//スマートスローダウン
	pelement->_t = ta * 2.0;														//減速時間
	pelement->_v = 0.0;															// 速度0
	pelement->_p = st_com_work.target.pos[id];									// 目標位置
	D_abs = 0.0;																// 残り距離更新

#else
	pelement = &(pseq->steps[pseq->n_step++]);									//ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_VOUT_V;													//速度到達待ち
	pelement->_t = ta;																	//減速時間
	pelement->_v = 0.0;																	// 速度0
	pelement->_p = st_com_work.target.pos[id];											// 目標位置
	D_abs = 0.0;																		// 残り距離更新
#endif
	/*### STEP3 位置合わせ　###*/
	pelement = &(pseq->steps[pseq->n_step++]);									// ステップのポインタセットして次ステップ用にカウントアップ
	pelement->type = CTR_TYPE_FINE_POS;													// 微小位置決め
	pelement->_t = FINE_POS_TIMELIMIT;													// 位置合わせ最大継続時間
	pelement->_v = pCrane->pSpec->axis_spec[ID_AHOIST].Notch_spd_f[pPlcIO->spd_mode[ID_AHOIST]][NOTCH_1];							// １ノッチ速度
	pelement->_p = st_com_work.target.pos[id];											// 目標位置
	D_abs = 0.0;																				// 残り距離変更なし


	//時間条件のスキャンカウント値セット
	for (int i = 0; i < pseq->n_step; i++) {
		pseq->steps[i].time_count = (int)(pseq->steps[i]._t / pwork->agent_scan);
		pseq->steps[i].status = STAT_STANDBY;
	}

	//ステップシーケンス準備完
	pseq->seq_status = STAT_STANDBY;

	//実行ステップ初期化
	pseq->i_hot_step = 0;

	return POLICY_PTN_OK;
}

void CCcPol::set_dbg_mode(int command) {

	switch (command) {
	case CODE_POLICY_DEBUG_SIM_MODE: {
		debug_mode |= CODE_POLICY_DEBUG_SIM_MODE;
		st_com_work.vfb_delay_time[ID_SLEW] = POL_PRM_FB_DELAY_SLEW_SIM;
		st_com_work.vfb_delay_time[ID_BOOM_H] = POL_PRM_FB_DELAY_BH_SIM;
	}break;
	case CODE_POLICY_DEBUG_DEACTIVE: {
		debug_mode &= ~CODE_POLICY_DEBUG_SIM_MODE;
		st_com_work.vfb_delay_time[ID_SLEW] = POL_PRM_FB_DELAY_SLEW;
		st_com_work.vfb_delay_time[ID_BOOM_H] = POL_PRM_FB_DELAY_BH;
	}break;
	default:break;
	}
	return;
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





