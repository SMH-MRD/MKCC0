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
extern ST_DEVICE_CODE g_my_code;

//共有メモリ
static LPST_CC_ENV_INF		pEnv_Inf	= NULL;
static LPST_CC_CS_INF		pCS_Inf		= NULL;
static LPST_CC_PLC_IO		pPLC_IO		= NULL;
static LPST_CC_AGENT_INF	pAgent_Inf	= NULL;
static LPST_CC_OTE_INF		pOTE_Inf	= NULL;
static LPST_CC_SIM_INF		pSim_Inf	= NULL;


ST_SIM_MON1 CSim::st_mon1;
ST_SIM_MON2 CSim::st_mon2;

ST_CC_SIM_INF CSim::st_sim_inf;
ST_CC_SIM_WORK CSim::st_sim_work; 

CSpec* CSim::pspec;

CSim::CSim() {

}
CSim::~CSim() {
	if(pSimJC  != nullptr) delete pSimJC;
	if(pSimGC  != nullptr) delete pSimGC;
	if(pSimOHC != nullptr) delete pSimOHC;
}

HRESULT CSim::initialize(LPVOID lpParam) {

	//### 入出力用共有メモリ取得
	pAgent_Inf	= (LPST_CC_AGENT_INF)	pAgInfObj->get_pMap();
	pEnv_Inf	= (LPST_CC_ENV_INF)		pEnvInfObj->get_pMap();
	pPLC_IO		= (LPST_CC_PLC_IO)		pPlcIoObj->get_pMap();
	pCS_Inf		= (LPST_CC_CS_INF)		pCsInfObj->get_pMap();
	pOTE_Inf	= (LPST_CC_OTE_INF)		pOteInfObj->get_pMap();
	pSim_Inf	= (LPST_CC_SIM_INF)		pSimuStatObj->get_pMap();


	if (pCrane == NULL)return S_FALSE;

	crane_id = pCrane->st_crane_inf.crane_id;
	crane_type = pCrane->st_crane_inf.crane_type;
	pspec = pCrane->pSpec;

	CSim* pSimObj = (CSim*)lpParam;
	int code = 0;

	//クレーンタイプ別初期設定
	switch (crane_type) {
	case CRANE_TYPE_ID_JC:setup_JC(crane_id); break;
	case CRANE_TYPE_ID_GC:setup_GC(crane_id); break;
	case CRANE_TYPE_ID_OHC:setup_OHC(crane_id); break;
	default: setup_JC(crane_id); break;
	}

	//operation panel　初期設定
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

	return S_OK;
}

HRESULT CSim::routine_work(void* pObj) {
	if (inf.total_act % 25 == 0) {
		wos.str(L""); wos << inf.status << L":" << inf.dt;
		//		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}

	if(pEnv_Inf->app_common_param.app_mode == MODE_ENV_APP_EMURATOR) {
		input();
		parse();
		output();
	}
	else Sleep(1000);

	return S_OK;
}

static UINT32	gpad_mode_last = L_OFF;
int CSim::input() {
	st_sim_work.helthy_cnt++;

		
	//スキャンタイムセット dtはマルチメディアタイマ　コールバックでセット
	pSimJC->set_dt(inf.dt);
	pLoad->set_dt(inf.dt);
	
	//移動極限状態
	for (int i = 0; i < MOTION_ID_MAX; i++) {
		pSimJC->is_fwd_endstop[i] = pPLC_IO->stat_axis[i].limit & PLC_IF_LIMIT_COM_FWD_EMR;
		pSimJC->is_rev_endstop[i] = pPLC_IO->stat_axis[i].limit & PLC_IF_LIMIT_COM_REV_EMR;
	}
	return 0;

	switch (crane_type) {
	case CRANE_TYPE_ID_OHC:
	{
	}break;
	case CRANE_TYPE_ID_GC:
	{
	}break;
	case CRANE_TYPE_ID_JC:
	default:
	{
		pSimJC->get_crane_status(&pEnv_Inf->crane_stat, pPLC_IO);	//ブレーキ状態更新
	}break;
	}
	return S_OK;
}
int CSim::parse() {						//メイン処理
	switch (crane_type) {
	case CRANE_TYPE_ID_JC:
	default: {
		pSimJC->timeEvolution();		//クレーンのドラム速度計算
		set_sensor_fb_JC(crane_id);		// センサフィードバック設定

		pLoad->timeEvolution();			//吊荷の位置,速度計算
		pLoad->r.add(pLoad->dr);		//吊荷位置更新
		pLoad->v.add(pLoad->dv);		//吊荷速度更新
		pLoad->update_relative_vec();	//吊荷吊点相対ベクトル更新(ロープベクトル　L,vL)
	}
		break;
	}
	return S_OK;
}
int CSim::output() {

	output_JC();  //クレーンの位置、速度情報セット 振れセンサIO情報セット

	//出力処理
	memcpy_s(pSim_Inf, sizeof(ST_CC_SIM_INF), &st_sim_inf, sizeof(ST_CC_SIM_INF));
	return S_OK;
}

void CSim::setup_JC(int id) {

	pSimJC = new CSimJC(id);			//クレーンオブジェクトインスタンス
	pSimJC->pSimStat = &st_sim_work.st_stat;

	pLoad = new CLoad();				//吊荷オブジェクとインスタンス
	pLoad->pMobBase = (CMob*)pSimJC;	//吊荷とクレーンを紐付け

	//計算パラメータセット
	//モータ1回転のPGカウント数=モータ1RPSでのカウント速度 
	st_sim_work.axis[ID_HOIST].RpsPGCnt		= pCrane->pSpec->axis_spec[ID_HOIST].CntPgR;	//主巻
	st_sim_work.axis[ID_BOOM_H].RpsPGCnt	= pCrane->pSpec->axis_spec[ID_BOOM_H].CntPgR;	//引込
	st_sim_work.axis[ID_SLEW].RpsPGCnt		= pCrane->pSpec->axis_spec[ID_SLEW].CntPgR;	//旋回
	st_sim_work.axis[ID_GANTRY].RpsPGCnt	= pCrane->pSpec->axis_spec[ID_GANTRY].CntPgR;	//走行

	//ドラム1回転のアブソコーダカウント数=ドラム1RPSでのカウント速度
	st_sim_work.axis[ID_HOIST].RpsABSOCnt	= pCrane->pSpec->axis_spec[ID_HOIST].CntAbsR;	//主巻
	st_sim_work.axis[ID_GANTRY].RpsABSOCnt	= pCrane->pSpec->axis_spec[ID_GANTRY].CntAbsR;	//走行
	//ドラム動作初期値　PG,アブソコーダカウント初期値セット
	init_drm_motion_JC(crane_id);
	//パラメータセット
	pSimJC->init_crane(crane_id);


	//旋回360°回転PGカウント= モータ1回転カウント×減速比×TTB径/ピニオン径
	st_sim_work.sl_cnt_pg360 = (INT32)(pspec->axis_spec[ID_SLEW].CntPgR * pspec->axis_spec[ID_SLEW].Gear_ratio* pspec->axis_spec[ID_SLEW].Ddrm1 / pspec->axis_spec[ID_SLEW].Ddrm0);

	//風速・トルク初期値
	switch (id) {
	case CRANE_ID_H6R602:
	{
		st_sim_inf.wind_spd_AI = (INT16)(SIM_PRM_WIND_SPEED_DEFAULT * 4000 / 60);//風速AI値 0-4000 0-60m/s
		st_sim_work.trq30[ID_HOIST] = st_sim_work.trq30[ID_BOOM_H]	= 300;
		st_sim_work.trq[ID_HOIST] = st_sim_work.trq[ID_BOOM_H]		= 1000;
	}break;
	case CRANE_ID_HHGQ18:
	{	
		st_sim_inf.wind_spd_AI = (INT16)(SIM_PRM_WIND_SPEED_DEFAULT * 800 / 60);//風速AI値 0-800 0-60m/s
		st_sim_work.trq30[ID_HOIST] = st_sim_work.trq30[ID_BOOM_H]	= 600;
		st_sim_work.trq[ID_HOIST]	= st_sim_work.trq[ID_BOOM_H]	= 4000;
	}break;
	case CRANE_ID_HHGH29:
	default:
	{
		st_sim_inf.wind_spd_AI = (INT16)(SIM_PRM_WIND_SPEED_DEFAULT * 800 / 60);//風速AI値 0-800 0-60m/s
		st_sim_work.trq30[ID_HOIST] = st_sim_work.trq30[ID_BOOM_H]	= 600;
		st_sim_work.trq[ID_HOIST]	= st_sim_work.trq[ID_BOOM_H]	= 4000;
	}break;
	}
	return;
}
void CSim::setup_GC(int id) {
	pSimGC = new CSimGC(id);	//シミュレーションオブジェクトインスタンス
	return;
}
void CSim::setup_OHC(int id) {
	pSimOHC = new CSimOHC(id);	//シミュレーションオブジェクトインスタンス
	return;
}

HRESULT CSim::init_drm_motion_JC(int id) {	//ドラムパラメータ設定(巻取量,層数,速度,加速度）
	//ドラム回転数計算用パラメータ初期値設定
	switch (id) {
	case CRANE_ID_H6R602:
	{
		st_sim_inf.hcount_mh = 97791220;		//主巻PG　(R21,H30）
		st_sim_inf.hcount_bh = 73925604;		//引込PG　(R56.4,H30）
		st_sim_inf.hcount_sl = 15000000;		//旋回PG　0°
		st_sim_inf.absocoder_mh = 60348;		//主巻アブソコーダ初期値開始位置(R21,H30）
		st_sim_inf.absocoder_gt = 160000;		//走行アブソコーダ初期値50m 50/(0.5π）* 1024
	}break;
	case CRANE_ID_HHGQ18:
	{
		st_sim_inf.hcount_mh = 98220337;		//主巻PG　(R21,H30）
		st_sim_inf.hcount_bh = 76676908;		//引込PG　(R21,H30）
		st_sim_inf.hcount_sl = 15000000;		//旋回PG　0°
		st_sim_inf.absocoder_mh = 51274;		//主巻アブソコーダ初期値3層開始位置(R21,H30）
		st_sim_inf.absocoder_gt = 32595;		//走行アブソコーダ初期値50m 50/(0.5π）* 1024
	}break;
	case CRANE_ID_HHGH29: 
	default:
	{
		st_sim_inf.hcount_mh = 91936418;		//主巻PG　(R30,H30）
		st_sim_inf.hcount_bh = 86673000;		//引込PG　(R30,H30）
		st_sim_inf.hcount_sl = 15000000;		//旋回PG　0°
		st_sim_inf.absocoder_mh = 50000;		//主巻アブソコーダ初期値3層開始位置(R45,H70）
		st_sim_inf.absocoder_gt = 32595;		//走行アブソコーダ初期値500m
	}break;
	}
	return S_OK;
}  

HRESULT CSim::set_sensor_fb_JC(int id) {				//トルク指令,高速カウンタ,アブソコーダ,LS他
	//速度FB,トルク指令
	//主巻の速度FB,トルク指令設定
	
	for (int i = 0; i < SIM_N_AXIS; i++) {
		st_sim_inf.trq_ref[i] = pSimJC->trq_fb[i];
		st_sim_inf.vfb[i] = (INT16)pSimJC->nv[i];
	}

	//### 位置情報関連
	//高速カウンタ,アブソコーダフィードバック設定
	//crane_stat.nd[].vはドラムRPS単位の速度値　PGカウントはモータのRPSでカウント計算
	if (pPLC_IO->stat_axis[ID_HOIST].brake) {
		st_sim_inf.hcount_mh += (INT32)(pEnv_Inf->crane_stat.nd[ID_HOIST].v * pspec->axis_spec[ID_HOIST].Gear_ratio * inf.dt * st_sim_work.axis[ID_HOIST].RpsPGCnt);	//主巻PGフィードバック
	
		//##########################################################################################
		//!!!!!! /6を入れないとアブソコーダのカウントが6倍位早くなってしまう　原因不明　宿題
		//#########################################################################################
		st_sim_inf.absocoder_mh	+= (INT32)(pEnv_Inf->crane_stat.nd[ID_HOIST].v	* inf.dt * st_sim_work.axis[ID_HOIST].RpsABSOCnt)/6;
		//st_sim_inf.absocoder_mh += (INT32)(pEnv_Inf->crane_stat.nd[ID_HOIST].v * inf.dt * st_work.axis[ID_HOIST].RpsABSOCnt);
	}

	if(pPLC_IO->stat_axis[ID_BOOM_H].brake)//!!!引込は正転でPGはマイナスカウント
		
		//##########################################################################################
		//!!!!!! /6を入れないとアブソコーダのカウントが6倍位早くなってしまう　原因不明　宿題
		//#########################################################################################
		st_sim_inf.hcount_bh	-= (INT32)(pEnv_Inf->crane_stat.nd[ID_BOOM_H].v  * pspec->axis_spec[ID_BOOM_H].Gear_ratio * inf.dt * st_sim_work.axis[ID_BOOM_H].RpsPGCnt)/6;
		//st_sim_inf.hcount_bh -= (INT32)(pEnv_Inf->crane_stat.nd[ID_BOOM_H].v * pspec->axis_spec[ID_BOOM_H].Gear_ratio * inf.dt * st_work.axis[ID_BOOM_H].RpsPGCnt);
	
	//下限リミット　入限 
	if (st_sim_inf.hcount_bh < pspec->axis_spec[ID_BOOM_H].CntPgSet0) 
		st_sim_inf.hcount_bh = pspec->axis_spec[ID_BOOM_H].CntPgSet0;
	
	//!!!旋回ブレーキは信号OFFで開
	if((!pPLC_IO->stat_axis[ID_SLEW].brake)&&((pEnv_Inf->crane_stat.nd[ID_SLEW].v<-0.00005)||(pEnv_Inf->crane_stat.nd[ID_SLEW].v>0.00005)))
		st_sim_inf.hcount_sl	+= (INT32)(pEnv_Inf->crane_stat.nd[ID_SLEW].v * pspec->axis_spec[ID_SLEW].Gear_ratio * inf.dt * st_sim_work.axis[ID_SLEW].RpsPGCnt);


	//プリセット エミュレーションでは2周以上でプリセット値に戻す
	if (st_sim_inf.hcount_sl > pspec->axis_spec[ID_SLEW].CntPgSet0 + st_sim_work.sl_cnt_pg360 * 2)
		st_sim_inf.hcount_sl = (INT32)pspec->axis_spec[ID_SLEW].CntPgSet0;
	if (st_sim_inf.hcount_sl < pspec->axis_spec[ID_SLEW].CntPgSet0 - st_sim_work.sl_cnt_pg360 * 2)
		st_sim_inf.hcount_sl = (INT32)pspec->axis_spec[ID_SLEW].CntPgSet0;

	if (pPLC_IO->stat_axis[ID_GANTRY].brake) {
		st_sim_inf.absocoder_gt += (INT32)(pEnv_Inf->crane_stat.nd[ID_GANTRY].v * inf.dt * st_sim_work.axis[ID_GANTRY].RpsABSOCnt);	//他は未使用なので0
	}

	//クレーン別ロジック
	switch (crane_id) {
	case CRANE_ID_H6R602:
	{
		//荷重
		st_sim_work.weight_mh = pspec->st_struct.Whook + st_sim_work.axis[ID_HOIST].load.m;	//フック質量＋荷重
		st_sim_inf.mlim_weight_AI = (INT16)(st_sim_work.weight_mh / 330000.0 * 1600);		//荷重　0－330t→ 0-1600(2V))

		//旋回半径	
		st_sim_inf.mlim_r_AI = (INT16)((pEnv_Inf->crane_stat.r.p - 21.0) / 41.0 * 1600.0);	//モーメントリミッタ半径AI入力計算値(0(21)-41(62)m→AD変換値 0-1600(2V))
	}break;
	case CRANE_ID_HHGQ18: 
	{
		//荷重
		st_sim_work.weight_mh = pspec->st_struct.Whook + st_sim_work.axis[ID_HOIST].load.m;	//フック質量＋荷重
		st_sim_inf.mlim_weight_AI = (INT16)(st_sim_work.weight_mh / 330000.0 * 1600);		//荷重　0－330t→ 0-1600(2V))

		//旋回半径	
		st_sim_inf.mlim_r_AI = (INT16)((pEnv_Inf->crane_stat.r.p - 21.0) / 41.0 * 1600.0);	//モーメントリミッタ半径AI入力計算値(0(21)-41(62)m→AD変換値 0-1600(2V))
	}break;
	case CRANE_ID_HHGH29: 
	default:
	{
		//荷重
		st_sim_work.weight_mh = pspec->st_struct.Whook + st_sim_work.axis[ID_HOIST].load.m;		//フック質量＋荷重
		st_sim_inf.mlim_weight_AI = (INT16)(st_sim_work.weight_mh / 80000.0 * 1600);			//荷重　フック質量AI入力計算値(kgf→AD変換値 80t->1600(2V))

		//旋回半径	
		st_sim_inf.mlim_r_AI = (INT16)((pEnv_Inf->crane_stat.r.p - 30.0) / 50.0 * 1600.0);	//モーメントリミッタ半径AI入力計算値(0(30)-50(80)m→AD変換値 0-1600(2V))
	}break;

	}

	return S_OK;
}            

/// <summary>
/// シミュレーション計算結果を共有メモリへ出力
/// </summary>
/// <returns></returns>
HRESULT CSim::output_JC() {
//クレーンの状態を作業用バッファにコピー

	st_sim_work.st_stat.a_fb[ID_HOIST]		= pSimJC->a0[ID_HOIST];
	st_sim_work.st_stat.a_fb[ID_GANTRY]		= pSimJC->a0[ID_GANTRY];
	st_sim_work.st_stat.a_fb[ID_SLEW]		= pSimJC->a0[ID_SLEW];
	st_sim_work.st_stat.a_fb[ID_BOOM_H]		= pSimJC->a0[ID_BOOM_H];
	st_sim_work.st_stat.a_fb[ID_AHOIST]		= pSimJC->a0[ID_AHOIST];

	st_sim_work.st_stat.v_fb[ID_HOIST]		= pSimJC->v0[ID_HOIST];
	st_sim_work.st_stat.v_fb[ID_GANTRY]		= pSimJC->v0[ID_GANTRY];
	st_sim_work.st_stat.v_fb[ID_SLEW]		= pSimJC->v0[ID_SLEW];
	st_sim_work.st_stat.v_fb[ID_BOOM_H]		= pSimJC->v0[ID_BOOM_H];
	st_sim_work.st_stat.v_fb[ID_AHOIST]		= pSimJC->v0[ID_AHOIST];

	st_sim_work.st_stat.pos[ID_HOIST]		= pSimJC->r0[ID_HOIST];
	st_sim_work.st_stat.pos[ID_GANTRY]		= pSimJC->r0[ID_GANTRY];
	st_sim_work.st_stat.pos[ID_SLEW]		= pSimJC->r0[ID_SLEW];
	st_sim_work.st_stat.pos[ID_BOOM_H]		= pSimJC->r0[ID_BOOM_H];
	st_sim_work.st_stat.pos[ID_AHOIST]		= pSimJC->r0[ID_AHOIST];

	st_sim_work.st_stat.L = pLoad->L;
	st_sim_work.st_stat.vL = pLoad->vL;


	//振れセンサの信号出力
	cal_sway_io_JC();

	return S_OK;
}

/// <summary>
/// クレーンxy座標をカメラxy座標に回転変換　→　カメラ用角度radに変換　
/// </summary>
/// <returns></returns>
HRESULT CSim::cal_sway_io_JC() {

	//## クレーンxy座標をカメラxy座標に回転変換

	double sin_ph_sl = -sin(pSimJC->r0[ID_SLEW]);   //sin(th_sl)
	double cos_ph_sl = -cos(pSimJC->r0[ID_SLEW]);   //cos(th_sl)

	double L		= st_sim_work.st_stat.lrm.p;   //主巻ロープ長
	double th_bh	= st_sim_work.st_stat.th.p;    //起伏角度
	double dth_bh	= st_sim_work.st_stat.th.v;    //起伏角速度

	double phx	= asin(((pLoad->L.x)	* sin_ph_sl + (pLoad->L.y)	* -cos_ph_sl)	/ L);   //振れ角旋回方向
	double phy	= asin(((pLoad->L.x)	* cos_ph_sl + (pLoad->L.y)	* sin_ph_sl)	/ L);   //振れ角引込方向

	double dphx = asin(((pLoad->vL.x)	* sin_ph_sl + (pLoad->vL.y) * -cos_ph_sl)	/ L);   //振れ角速度旋回方向
	double dphy = asin(((pLoad->vL.x)	* cos_ph_sl + (pLoad->vL.y) * sin_ph_sl)	/ L);   //振れ角速度引込方向

	double sin_phx	= sin(phx), cos_phx = cos(phx);
	double tan_thtx = (L * sin_phx) / (L * cos_phx);
	double thtx		= atan(tan_thtx);
	double dthtx	= L * dphx * (cos_phx + sin_phx * tan_thtx);

	double sin_phy = sin(phy), cos_phy = cos(phy);
	
	double tan_thty = (L * sin_phy) / (L * cos_phy);
	double thty = atan(tan_thty);
	double dthty = L * dphy * (cos_phy + sin_phy * tan_thty);
	dthty /= (L * cos_phy) * (1 + tan_thty * tan_thty);

	//メッセージバッファセット
	LPST_SWAY_SERVER_BODY pbody = &st_sim_inf.swy_serv_body;
	pbody->sway_data[ID_X].p;					pbody->sway_data[ID_Y].p;
	pbody->sway_data[ID_X].p0;					pbody->sway_data[ID_Y].p0;		//振れ0点pixcel値
	pbody->sway_data[ID_X].amp_p2p;				pbody->sway_data[ID_Y].amp_p2p;
	
	
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

		st_mon1.wo	<< L"[INV Ref V] mh:" << pPLC_IO->stat_axis[ID_HOIST].v_ref
					<< L" bh:" << pPLC_IO->stat_axis[ID_BOOM_H].v_ref
					<< L" sl:" << pPLC_IO->stat_axis[ID_SLEW].v_ref
					<< L" gt:" << pPLC_IO->stat_axis[ID_GANTRY].v_ref
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
		//### Function Radio Button 処理
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

		//### Item Check Box 処理
		case IDC_TASK_ITEM_CHECK1: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1: {
				wstring wstr;
				int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr.c_str(), 128);
				if(n) st_sim_work.axis[ID_HOIST].load.m = (double)stof(wstr.c_str());
				//チェックを外す
				SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

			}break;
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}
		}break;
		case IDC_TASK_ITEM_CHECK2: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1: {
				wstring wstr;
				int n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr.c_str(), 128);
				if (n) st_sim_inf.wind_spd_AI = (INT16)stoi(wstr.c_str()) * 4000/600;
				//チェックを外す
				SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);

			}break;
			case IDC_TASK_FUNC_RADIO4:
				set_item_chk_txt();
				break;
			default:break;
			}

		}break;
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
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"1:0.1トン単位";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM3), wstr.c_str());
		wstr = L"2:-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_STATIC_ITEM4), wstr.c_str());
	}break;
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
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"Param");
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
	case IDC_TASK_FUNC_RADIO1: {
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1, L"MH LOAD");
		SetDlgItemText(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2, L"WIND_SPD");
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



