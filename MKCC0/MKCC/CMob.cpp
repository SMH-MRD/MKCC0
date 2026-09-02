#include "CMob.h"
#include "CCrane.h"
#include "phisics.h"

extern CCrane* pCrane;

//計算時間短縮用変数
static double LmbCosAdb, LmLb,Lmb2;

/********************************************************************************/
/*       Moving Object                                                          */
/********************************************************************************/
CMob::CMob() {
	dt = 0.01;				//スキャンタイム
 	r = { 0.0,0.0,0.0 };
	v = { 0.0,0.0,0.0 };
	dr = { 0.0,0.0,0.0 };
	dv = { 0.0,0.0,0.0 };
	R0 = { 0.0,0.0,0.0};
}

CMob::~CMob() {}

//加速度ベクトル　継承先で再定義する
Vector3 CMob::A(Vector3& r, Vector3& v) {
	return r.clone();
};
void CMob::set_fex(double fx,double fy,double fz) {
	fex.x = fx;
	fex.y = fy;
	fex.z = fz;
	return;
};
void CMob::set_dt(double _dt) {
	dt = _dt;
	return;
};

//速度ベクトル
Vector3 CMob::V(Vector3& r, Vector3& v) {
	return v.clone();
}
//オイラー方による時間発展
void CMob::timeEvolution() {

	Vector3 v1 = V(r, v);
	Vector3 a1 = A(r, v);

	Vector3 _v1 = Vector3(r.x + v1.x * dt / 2.0, r.y + v1.y * dt / 2.0, r.z + v1.z * dt / 2.0);
	Vector3 _a1 = Vector3(v.x + a1.x * dt / 2.0, v.y + a1.y * dt / 2.0, v.z + a1.z * dt / 2.0);
	Vector3 v2 = V(_v1, _a1);
	Vector3 a2 = A(_v1, _a1);

	Vector3 _v2 = Vector3(r.x + v2.x * dt / 2.0, r.y + v2.y * dt / 2.0, r.z + v2.z * dt / 2.0);
	Vector3 _a2 = Vector3(v.x + a2.x * dt / 2.0, v.y + a2.y * dt / 2.0, v.z + a2.z * dt / 2.0);
	Vector3 v3 = V(_v2, _a2);
	Vector3 a3 = A(_v2, _a2);

	Vector3 _v3 = Vector3(r.x + v3.x * dt, r.y + v3.y * dt, r.z + v3.z * dt);
	Vector3 _a3 = Vector3(v.x + a3.x * dt, v.y + a3.y * dt, v.z + a3.z * dt);
	Vector3 v4 = V(_v3, _a3);
	Vector3 a4 = A(_v3, _a3);

	dr.x = dt * v.x;
	dr.y = dt * v.y;
	dr.z = dt * v.z;
	dv.x = dt / 6.0 * (a1.x + 2.0 * a2.x + 2.0 * a3.x + a4.x);
	dv.y = dt / 6.0 * (a1.y + 2.0 * a2.y + 2.0 * a3.y + a4.y);
	dv.z = dt / 6.0 * (a1.z + 2.0 * a2.z + 2.0 * a3.z + a4.z);
}

/********************************************************************************/
/*       Crane Object                                                          */
/********************************************************************************/
CSimJC::CSimJC(int _id) { 
	//仕様パラメータ構造体セット
	pStruct = pCrane->get_st_struct();
	pAxis_mh = pCrane->get_base_mh();
	pAxis_bh = pCrane->get_base_bh();
	pAxis_sl = pCrane->get_base_sl();
	pAxis_gt = pCrane->get_base_gt();
	pAxis_ah = pCrane->get_base_ah();


	//0速とみなす速度上限（ドラム回転速度）
	accdec_cut_spd_range[ID_HOIST]	= 0.005 * pAxis_mh->Rpm_rated / 60.0 / (pCrane->get_base_mh()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_BOOM_H] = 0.005 * pAxis_bh->Rpm_rated / 60.0 / (pCrane->get_base_mh()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_SLEW]	= 0.005 * pAxis_sl->Rpm_rated / 60.0 / (pCrane->get_base_mh()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_GANTRY] = 0.005 * pAxis_gt->Rpm_rated / 60.0 / (pCrane->get_base_mh()->Gear_ratio);	//0.5%	
	accdec_cut_spd_range[ID_AHOIST] = 0.005 * pAxis_ah->Rpm_rated / 60.0 / (pCrane->get_base_mh()->Gear_ratio);	//0.5%
	mh_load = pStruct->Whook;	//初期主巻荷重 フック重量
	ah_load = pStruct->Whook;	//初期補巻荷重 フック重量

	for (int i = 0; i < MOTION_ID_MAX;i++) {
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		Tf[i] = 1.0;
		brk_elaped_time[i] = 0.0;
		a0[i] = 0.0;
		v0[i] = 0.0;
		a_ref[i] = 0.0;
		v_ref[i] = 0.0;
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		trq_fb[i]=0.0;    //モータートルクFB
		motion_brake[i] = false;
	}

	r0[ID_GANTRY]	= SIM_INIT_X;
	r0[ID_HOIST]	= SIM_INIT_MH;	
	r0[ID_AHOIST]	= SIM_INIT_AH;	
	r0[ID_BOOM_H]	= SIM_INIT_R;
	r0[ID_SLEW]		= 0.0;

	source_mode = MOB_MODE_SIM;
	
}
CSimJC::~CSimJC() {}

/// <summary>
/// ドラム速度指令取り込み(rps)
/// </summary>
/// <param name="hoist_ref"></param>
/// <param name="gantry_ref"></param>
/// <param name="slew_ref"></param>
/// <param name="boomh_ref"></param>
/// <param name="ah_ref"></param>
void CSimJC::set_v_ref(double hoist_ref, double gantry_ref, double slew_ref, double boomh_ref, double ah_ref) {
	nv_ref[ID_HOIST]	= hoist_ref;
	nv_ref[ID_BOOM_H]	= boomh_ref;
	nv_ref[ID_SLEW]		= slew_ref;
	nv_ref[ID_GANTRY]	= gantry_ref;
	nv_ref[ID_AHOIST]	= ah_ref;
	return;
}
// ﾄﾙｸT(N・m）= F x R　= J x dω/dt  仕事率P=Tω=Mav　a=Tω/Mv=MT/r

//軸加速度
void CSimJC::Ac() {	//加速度計算

	//ドラム加速指令計算
	// #主巻
	{
		//ブレーキ閉
		if (!motion_brake[ID_HOIST]) na_ref[ID_HOIST] = 0.0;//回転加速度　0

		//速度指令に未達(ドラム回転速度　指令>FB）
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) > accdec_cut_spd_range[ID_HOIST]) {
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Ta0;		//正転加速指令
			else						na_ref[ID_HOIST] = pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Td0;		//逆転減速指令
		}
		//速度指令に未達(ドラム回転速度 指令<FB) 
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) < -accdec_cut_spd_range[ID_HOIST]) {
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = -pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Td0;		//正転減速指令
			else						na_ref[ID_HOIST] = -pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Ta0;		//逆転加速指令
		}
		//速度指令に到達
		else {
			na_ref[ID_HOIST] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_HOIST] > 0.0) && (is_fwd_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
		if ((na_ref[ID_HOIST] < 0.0) && (is_rev_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
	}

	// #補巻
	{
		//ブレーキ閉
		if (!motion_brake[ID_AHOIST]) na_ref[ID_AHOIST] = 0.0;
		//速度指令に未達
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) > accdec_cut_spd_range[ID_AHOIST]) {
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Ta0;		//正転加速指令
			else						na_ref[ID_AHOIST] = pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Td0;		//逆転減速指令
		}
		//速度指令に未達
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) < -accdec_cut_spd_range[ID_AHOIST]) {
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = -pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Td0;	//正転減速指令
			else						na_ref[ID_AHOIST] = -pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Ta0;	//逆転加速指令
		}
		//速度指令に到達
		else {
			na_ref[ID_AHOIST] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_AHOIST] > 0.0) && (is_fwd_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
		if ((na_ref[ID_AHOIST] < 0.0) && (is_rev_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
	}

	// #走行
	{
		if (!motion_brake[ID_GANTRY]) na_ref[ID_GANTRY] = 0.0;
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) > accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Ta0;	//正転加速
			else						na_ref[ID_GANTRY] = pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Td0;	//逆転減速
		}
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) < -accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = -pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Td0;	//正転減速
			else						na_ref[ID_GANTRY] = -pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Ta0;	//逆転加速
		}
		else {
			na_ref[ID_GANTRY] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_GANTRY] > 0.0) && (is_fwd_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
		if ((na_ref[ID_GANTRY] < 0.0) && (is_rev_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
	}

	// #引込
	{
		if (!motion_brake[ID_BOOM_H]) na_ref[ID_BOOM_H] = 0.0;
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) > accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Ta0;	//正転加速
			else						na_ref[ID_BOOM_H] = pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Td0;	//逆転減速
		}
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) < -accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = -pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Td0;	//正転減速
			else						na_ref[ID_BOOM_H] = -pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Ta0;	//逆転加速
		}
		else {
			na_ref[ID_BOOM_H] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_BOOM_H] > 0.0) && (is_fwd_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
		if ((na_ref[ID_BOOM_H] < 0.0) && (is_rev_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
	}

	// #旋回
	{
		if (!motion_brake[ID_SLEW]) na_ref[ID_SLEW] = 0.0;
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) > accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Ta0;//正転加速
			else						na_ref[ID_SLEW] = pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Td0;//逆転減速
		}
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) < -accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = -pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Td0;//正転減速
			else						na_ref[ID_SLEW] = -pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Ta0;//逆転加速
		}
		else {
			na_ref[ID_SLEW] = 0.0;
		}
	}

	//ドラム加速度計算　当面指令に対して一次遅れフィルタを入れる形で計算
	//一次遅れフィルタ式　Yk = (dt*Xk+Tf*Yk-1)/(dt+Tf)
	{
		//主巻
		if ((motion_brake[ID_HOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_HOIST].a = (dt * na_ref[ID_HOIST] + Tf[ID_HOIST] * pSimStat->nd[ID_HOIST].a )/ (dt + Tf[ID_HOIST]);
		}
		else {
			pSimStat->nd[ID_HOIST].a = 0.0;
		}
		//補巻
		if ((motion_brake[ID_AHOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_AHOIST].a = (dt * na_ref[ID_AHOIST] + Tf[ID_AHOIST] * pSimStat->nd[ID_AHOIST].a) / (dt + Tf[ID_AHOIST]);
		}
		else {
			pSimStat->nd[ID_AHOIST].a = 0.0;
		}
		//引込
		if ((motion_brake[ID_BOOM_H]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_BOOM_H].a = (dt * na_ref[ID_BOOM_H] + Tf[ID_BOOM_H] * pSimStat->nd[ID_BOOM_H].a) / (dt + Tf[ID_BOOM_H]);
		}
		else {
			pSimStat->nd[ID_BOOM_H].a = 0.0;
		}
		//旋回
		if ((motion_brake[ID_SLEW]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_SLEW].a = (dt * na_ref[ID_SLEW] + Tf[ID_SLEW] * pSimStat->nd[ID_SLEW].a) / (dt + Tf[ID_SLEW]);
		}
		else {
			pSimStat->nd[ID_SLEW].a = 0.0;
		}
		//走行
		if ((motion_brake[ID_GANTRY]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_GANTRY].a = (dt * na_ref[ID_GANTRY] + Tf[ID_GANTRY] * pSimStat->nd[ID_GANTRY].a) / (dt + Tf[ID_GANTRY]);
		}
		else {
			pSimStat->nd[ID_GANTRY].a = 0.0;
		}
		//0リミット
		for (int i = 0; i < 6; i++) if ((pSimStat->nd[i].a < 0.00001)&& (pSimStat->nd[i].a > -0.00001))pSimStat->nd[i].a = 0.0;
	}
	return;
}
Vector3 CSimJC::A(Vector3& _r, Vector3& _v) {
	Vector3 vec3;	
	return vec3;
}
void CSimJC::timeEvolution() {
	//クレーン部
	//ドラム加速度計算
	Ac();

	//ドラム速度計算(オイラー法）
	pSimStat->nd[ID_HOIST].v	+= pSimStat->nd[ID_HOIST].a * dt;	if (!motion_brake[ID_HOIST])	pSimStat->nd[ID_HOIST].v	= 0.0;
	pSimStat->nd[ID_AHOIST].v	+= pSimStat->nd[ID_AHOIST].a * dt;	if (!motion_brake[ID_AHOIST])	pSimStat->nd[ID_AHOIST].v	= 0.0;
	pSimStat->nd[ID_BOOM_H].v	+= pSimStat->nd[ID_BOOM_H].a * dt;	if (!motion_brake[ID_BOOM_H])	pSimStat->nd[ID_BOOM_H].v	= 0.0;
	pSimStat->nd[ID_SLEW].v		+= pSimStat->nd[ID_SLEW].a * dt;	if (!motion_brake[ID_SLEW])		pSimStat->nd[ID_SLEW].v		= 0.0;
	pSimStat->nd[ID_GANTRY].v	+= pSimStat->nd[ID_GANTRY].a * dt;	if (!motion_brake[ID_GANTRY])	pSimStat->nd[ID_GANTRY].v	= 0.0;
	
	//停止中は速度を0にする
	for (int i = 0; i < 6; i++) {
		if (pSimStat->nd[i].a == 0.0) {
			if ((pSimStat->nd[i].v < 0.001) && (pSimStat->nd[i].v > -0.001))
				pSimStat->nd[i].v = 0.0;
		}
	};

	//ドラム位置計算(オイラー法）
	pSimStat->nd[ID_HOIST].p	+= pSimStat->nd[ID_HOIST].v		* dt;
	pSimStat->nd[ID_AHOIST].p	+= pSimStat->nd[ID_AHOIST].v	* dt;
	pSimStat->nd[ID_GANTRY].p	+= pSimStat->nd[ID_GANTRY].v	* dt;
	pSimStat->nd[ID_BOOM_H].p	+= pSimStat->nd[ID_BOOM_H].v	* dt;
	pSimStat->nd[ID_SLEW].p		+= pSimStat->nd[ID_SLEW].v		* dt;

#if 0
	//クレーン状態セット
	set_d_th_from_nbh();	//引込ドラム回転状態からd,d'd'' th th' th''の状態をセットする
	set_bh_layer();         //引込ドラム状態をセットする
	set_mh_layer();         //主巻ドラム状態、ロープ状態をセットする
	set_ah_layer();         //補巻ドラム状態、ロープ状態をセットする
	set_sl_layer();         //旋回ドラム状態をセットする
	set_gt_layer();         //走行ドラム状態をセットする

	//軸加速度計算
	double thm = pSimStat->th.p;
	double tha = pSimStat->th.p - pspec->Alpa_a;

	//ロープ長加速度計算
	pSimStat->lrm.a = (
		- pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].a		//主巻ドラム回転分
		- pSimStat->db.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d変化分
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].a		//起伏ドラム回転分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->lra.a = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].a	//補巻ドラム回転分
		- pSimStat->d.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d変化分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	//h=Lm・sinθ　h'=Lm・θ'cosθ h''=Lm・θ''cosθ-θ'sinθ
	a0[ID_HOIST] = pspec->Lm * (pSimStat->th.a * cos(thm) - pSimStat->th.v * pSimStat->th.v * sin(thm)) + pSimStat->lrm.a;//吊点ｚ加速度＋ロープ長加速度
	a0[ID_AHOIST] = pspec->La * (pSimStat->th.a * cos(tha) - pSimStat->th.v * pSimStat->th.v * sin(tha)) + pSimStat->lra.a;//吊点ｚ加速度＋ロープ長加速度
	//r=Lm・cosθ　r'=-Lm・θ'sinθ r''=-Lm・(θ''sinθ-θ'cosθ)
	a0[ID_BOOM_H] = -pspec->Lm * (pSimStat->th.a * sin(thm) + pSimStat->th.v * cos(thm));

	a0[ID_SLEW] = pSimStat->nd[ID_SLEW].a * pspec->Kttb;						//ピニオン回転加速度×ピニオン径/TTB径
	a0[ID_GANTRY] = pSimStat->nd[ID_GANTRY].a * pCraneStat->Cdr[ID_GANTRY][1];	//ドラム回転加速度×車輪径


	pSimStat->lrm.v = (
		-pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].v	//主巻ドラム回転分
		- pSimStat->db.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d変化分
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].v	//起伏ドラム回転分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];
	pSimStat->lra.v = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].v	//補巻ドラム回転分
		- pSimStat->d.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d変化分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];


	v0[ID_HOIST]	= pspec->Lm * pSimStat->th.v * cos(pSimStat->th.p) - pSimStat->lrm.v;
	v0[ID_AHOIST]	= pspec->La * pSimStat->th.v * cos(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.v;
	v0[ID_SLEW]		= pSimStat->nd[ID_SLEW].v * PI360 * pspec->Kttb;
	v0[ID_GANTRY]	= pSimStat->nd[ID_GANTRY].v * pCraneStat->Cdr[ID_GANTRY][1];
	v0[ID_BOOM_H]	= -pspec->Lb * pSimStat->th.v * sin(pSimStat->th.p+pspec->Alpa_b);


	pSimStat->lrm.p = (
		pCraneStat->Cdr[ID_HOIST][0] 														//全ロープ
		- pSimStat->db.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]						//d部ロープ
		- pSimStat->l_drum[ID_BHMH] - pSimStat->l_drum[ID_HOIST]							//ドラム部ロープ(主巻ドラム＋引込ドラム）
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];											//ワイヤ掛け数
	pSimStat->lra.p = (
		pCraneStat->Cdr[ID_AHOIST][0] 														//全ロープ
		- pSimStat->db.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]						//d部ロープ
		- pSimStat->l_drum[ID_AHOIST]														//ドラム部ロープ
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];											//ワイヤ掛け数

	r0[ID_HOIST]	= pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - pSimStat->lrm.p;
	r0[ID_AHOIST]	= pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.p;
	

	//旋回は±180°で表現
	r0[ID_SLEW]		= pSimStat->nd[ID_SLEW].p * PI360*14/166;
	if (r0[ID_SLEW] > PI180)		r0[ID_SLEW] -= PI360;
	else if (r0[ID_SLEW] < -PI180)	r0[ID_SLEW] += PI360;
	else;
	
	r0[ID_GANTRY]	= pSimStat->l_drum[ID_GANTRY];
	r0[ID_BOOM_H]	= pspec->Lm * cos(pSimStat->th.p);

	vc.x = v0[ID_GANTRY]; vc.y = 0.0; vc.z = 0.0;		//クレーン中心位置
	rc.x = r0[ID_GANTRY]; rc.y = R0.y; rc.z = R0.z;		//クレーン中心位置
														
	//吊点部
	double cos_sl = cos(r0[ID_SLEW]);
	double sin_sl = sin(r0[ID_SLEW]);
	double cos_th = cos(pSimStat->th.p);
	double sin_th = sin(pSimStat->th.p);

	r.x = r0[ID_BOOM_H] * cos_sl + r0[ID_GANTRY];
	r.y = r0[ID_BOOM_H] * sin_sl;
	r.z = pspec->Hp + pspec->Lm * sin_th;

	v.x = v0[ID_BOOM_H] * cos_sl - r0[ID_BOOM_H] * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	v.y = v0[ID_BOOM_H] * sin_sl + r0[ID_BOOM_H] * v0[ID_SLEW] * cos_sl;
	v.z = pspec->Lm * pSimStat->th.v * cos_th;

	a.x = v0[ID_BOOM_H] * cos_sl - r0[ID_BOOM_H] * v0[ID_SLEW] * sin_sl + a0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * sin_sl + r0[ID_BOOM_H] * v0[ID_SLEW] * cos_sl;
	a.z = pspec->Lm * (pSimStat->th.a * cos_th - pSimStat->th.v * pSimStat->th.v * sin_th);


	double ar0 = def_spec.La * cos(pSimStat->th.p - def_spec.rad_Lm_La);
	r2.x = ar0 * cos_sl + r0[ID_GANTRY];
	r2.y = ar0 * sin_sl;
	r2.z = pspec->Hp + pspec->La * sin(pSimStat->th.p - def_spec.rad_Lm_La);

	v2.copy(v);
	a2.copy(a);

	//ロープ長セット　LOADオブジェクトから参照
	l_mh = pSimStat->lrm.p;
	l_ah = pSimStat->lra.p;

	//振れ周期セット
	pSimStat->w = sqrt(GA / l_mh);
	pSimStat->T = PI360/ pSimStat->w;
	pSimStat->wah = sqrt(GA / l_ah);
	pSimStat->Tah = PI360 / pSimStat->wah;
#endif
	return;
}
void CSimJC::init_crane(double _dt) {
#if 0	
	//計算パラメータ
	Lmb2 = pStruct->Lb * pStruct->Lb; //ブーム長の2乗
	LmbCosAdb = pspec->Lmb * cos(pspec->Alpa_db);
	LmLb = pspec->Lm * pspec->Lb;
	
	//r0は、各軸アブソコーダの値
	r0[ID_HOIST]	= SIM_INIT_MH;
	r0[ID_AHOIST]	= SIM_INIT_AH;
	r0[ID_GANTRY]	= SIM_INIT_X;
	r0[ID_SLEW]		= SIM_INIT_TH;
	r0[ID_BOOM_H]	= SIM_INIT_R;		//旋回半径は主巻吊点位置の半径とする

	//クレーン基準点の初期位置,速度
	rc.x = 10.0; rc.y = 0.0; rc.z = 0.0;
	vc.x = 0.0; vc.y = 0.0; vc.z = 0.0;

	//ドラム回転位置セット
	set_nbh_d_ph_th_from_r(r0[ID_BOOM_H]);												//旋回半径より引込ドラム回転量他をセット

	set_nmh_from_mh(r0[ID_HOIST]); 														//主巻揚程より主巻ドラム回転量他をセット

	set_nah_from_ah(r0[ID_AHOIST]); 													//補巻揚程より補巻ドラム回転量他をセット

	slw_rad_per_turn = PI360 * pspec->prm_nw[DRUM_ITEM_DIR][ID_SLEW] / pspec->Dttb;		//ピニオン（モータ）１回転での旋回角度変化量セット
	set_nsl_from_slr(r0[ID_SLEW]); 														//旋回位置からピニオン回転数をセット

	gnt_m_per_turn = PI180 * pspec->prm_nw[DRUM_ITEM_DIR][ID_GANTRY];					//車輪1回転での移動量セット
	set_ngt_from_gtm(r0[ID_GANTRY]);													//走行位置から車輪回転数セット

																						//吊点状態セット
	Vector3 _r(r0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r0[ID_GANTRY], r0[ID_BOOM_H] * sin(r0[ID_SLEW]), pspec->Lp + pspec->Lm * sin(pSimStat->th.p));
	Vector3 _v(0.0, 0.0, 0.0);
	init_mob(_dt, _r, _v);

	//ロープ長
	

	set_v_ref(0.0, 0.0, 0.0, 0.0, 0.0);	//初期速度指令値セット
	set_fex(0.0, 0.0, 0.0);			//初期外力セット
		
	//加速度一次遅れフィルタ時定数
	Tf[ID_HOIST]	= SIM_TF_HOIST;
	Tf[ID_BOOM_H]	= SIM_TF_BOOM_H;
	Tf[ID_SLEW]		= SIM_TF_SLEW;
	Tf[ID_GANTRY]	= SIM_TF_GANTRY;
	Tf[ID_AHOIST]	= SIM_TF_AHOIST;

	//計算用定数セット
	c_phb = (cal_Lb2Lp2 - pSimStat->d.p * pSimStat->d.p) / cal_2LbLp;
	c_ph = (cal_Lm2Lp2 - pSimStat->d.p * pSimStat->d.p) / cal_2LmLp;
	
	cal_Lm2Lp2 = pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp;
	cal_2LmLp = 2.0 * pspec->Lm * pspec->Lp;
	cal_Lb2Lp2 = pspec->Lb * pspec->Lb + pspec->Lp * pspec->Lp;
	cal_2LbLp = 2.0 * pspec->Lb * pspec->Lp;

	//荷重セット
	for (int i = 0; i < MOTION_ID_MAX; i++) {
		pSimStat->load[i].m = pSimStat->load[i].wx = pSimStat->load[i].dy = pSimStat->load[i].hz = 0.0;
	}

	pSimStat->load[ID_HOIST].m = pspec->Load0_mh;
	pSimStat->load[ID_AHOIST].m = pspec->Load0_ah;;
#endif
	return;
}
// 各モーションのブレーキ状態をセット
void CSimJC::update_break_status() {
	motion_brake[ID_HOIST]	= pPLC_IO->stat_mh.brake;
	motion_brake[ID_AHOIST]	= pPLC_IO->stat_ah.brake;
	motion_brake[ID_GANTRY]	= pPLC_IO->stat_gt.brake;
	motion_brake[ID_BOOM_H]	= pPLC_IO->stat_bh.brake;
	motion_brake[ID_SLEW]	= pPLC_IO->stat_sl.brake;
	return;
}

void CSimJC::set_nbh_d_ph_th_from_r(double r) {
#if 0
	pSimStat->th.p	= acos(r/pspec->Lm);					//半径は主巻吊点位置
	pSimStat->ph.p	= pspec->Php - pSimStat->th.p;
	pSimStat->d.p	= sqrt(pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp - 2.0 * pspec->Lm * pspec->Lp * cos(pSimStat->ph.p));
	pSimStat->db.p	= sqrt(pspec->Lb * pspec->Lb + pspec->Lp * pspec->Lp - 2.0 * pspec->Lb * pspec->Lp * cos(pSimStat->ph.p - pspec->Alpa_b ));
	pSimStat->th.v = pSimStat->ph.v = 0.0;
	pSimStat->th.a = pSimStat->ph.a = 0.0;


	//起伏ドラム巻取り量＝基準起伏ロープ長（ジブ部ロープ＋ドラム巻取り量の和）‐ジブ部分ロープ長さ
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Cdr[ID_BOOM_H][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H] * pSimStat->db.p;

	int i;
	//引込ドラムの層数
	for (i = 1; i < PLC_DRUM_LAYER_MAX-1; i++) {
		if (pSimStat->l_drum[ID_BOOM_H] < pCraneStat->Ldr[ID_BOOM_H][i]) break;
	}
	//起伏起伏ドラム層数
	pSimStat->i_layer[ID_BOOM_H] = i;																										//現在の層
	pSimStat->n_layer[ID_BOOM_H] = (pSimStat->l_drum[ID_BOOM_H] - pCraneStat->Ldr[ID_BOOM_H][i-1])/ pCraneStat->Cdr[ID_BOOM_H][i];			//現在の層の巻数
																																			//ドラム巻取り量
	//ドラム回転数
	pSimStat->nd[ID_BOOM_H].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H] * ((double)i - 1.0) + pSimStat->n_layer[ID_BOOM_H];				//引込ドラム部（溝数×（現在層数-1）＋現在層の巻き数
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum- pSimStat->nd[ID_BOOM_H].p;																	//主巻ドラム部（主巻引込巻き数の和‐引込部巻き数）

	//起伏主巻ドラム層数
	pSimStat->i_layer[ID_BHMH] = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]);								//現在層数-1（計算途中）
	pSimStat->n_layer[ID_BHMH] = pSimStat->nd[ID_BHMH].p - (double)pSimStat->i_layer[ID_BHMH] * pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH];		//層巻き数＝回転数-（現在層数-1）*溝数
	pSimStat->i_layer[ID_BHMH]++;																											//現在層数
	
	//起伏主巻ドラム巻取り量
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][pSimStat->i_layer[ID_BHMH] - 1] + pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->n_layer[ID_BHMH];
#endif
	return;
}
//旋回半径と主巻揚程から主巻ドラム回転数をセットする
void CSimJC::set_nmh_from_mh(double mh) {
#if 0
	//ロープ長
	double lrope = pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - mh;//主巻吊ロープ長＝ジブ軸高さ＋ジブ起伏高さ-揚程
	
	//主巻ドラム巻取り量 基準ロープ長（主巻、起伏ドラム巻取り量＋ジブ部＋ロープ長部の総和）-　ジブ部　- 起伏ドラム巻取 - ロープ長部
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Cdr[ID_HOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST] * pSimStat->d.p - pSimStat->l_drum[ID_BHMH] - lrope* pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_HOIST] < pCraneStat->Ldr[ID_HOIST][i]) break;
	}
	pSimStat->i_layer[ID_HOIST] = i;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->l_drum[ID_HOIST] - pCraneStat->Ldr[ID_HOIST][i - 1])/ pCraneStat->Cdr[ID_HOIST][i];

	//主巻ドラム回転数
	pSimStat->nd[ID_HOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_HOIST];
#endif
	return;

}
//dと補巻揚程から補巻ドラム回転数をセットする
void CSimJC::set_nah_from_ah(double ah) {
#if 0	
	//ロープ長
	double lrope = pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - ah;//補巻吊ロープ長＝ジブ軸高さ＋ジブ起伏高さ-揚程

	//補巻ドラム巻取り量 基準ロープ長（補巻ドラム巻取り量＋ジブ部＋ロープ長部の総和）-　ジブ部　- 起伏ドラム巻取 - ロープ長部
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Cdr[ID_AHOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST] * pSimStat->db.p  - lrope * pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_AHOIST] < pCraneStat->Ldr[ID_AHOIST][i]) break;
	}
	pSimStat->i_layer[ID_AHOIST] = i;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->l_drum[ID_AHOIST] - pCraneStat->Ldr[ID_AHOIST][i - 1]) / pCraneStat->Cdr[ID_AHOIST][i];

	//補巻ドラム回転数
	pSimStat->nd[ID_AHOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_AHOIST];
#endif
	return; 
} 
//旋回位置(rad)から旋回ピニオン回転数をセットする
void CSimJC::set_nsl_from_slr(double sl_rad) {
#if 0
	pSimStat->nd[ID_SLEW].p = sl_rad / PI360 *166.0/14.0; //166.0/14.0 TTB径/ピニオン径
	pSimStat->i_layer[ID_SLEW] = 1;
	pSimStat->n_layer[ID_SLEW] = sl_rad/PI360;

	pSimStat->l_drum[ID_SLEW] = pCraneStat->Cdr[ID_SLEW][0] * pSimStat->n_layer[ID_SLEW];//Cdr[0]ピニオン円周
#endif
	return; 
}
//走行位置から走行車輪回転数をセットする
void CSimJC::set_ngt_from_gtm(double gt_m) {
#if 0
	pSimStat->nd[ID_GANTRY].p = gt_m / pCraneStat->Cdr[ID_GANTRY][0];											//Cdr[0]車輪円周
	pSimStat->i_layer[ID_GANTRY] = 1;
	pSimStat->n_layer[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p;

	pSimStat->l_drum[ID_GANTRY] = pCraneStat->Cdr[ID_GANTRY][0] * pSimStat->n_layer[ID_GANTRY];//Cdr[0]車輪円周
#endif
	return;
}
//引込ドラム回転状態からd,th,d",th",d"",th""をセットする
void CSimJC::set_d_th_from_nbh() {

#if 0	
	//引込ドラム層状態
	pSimStat->i_layer[ID_BOOM_H] = (UINT32)(pSimStat->nd[ID_BOOM_H].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]) + 1;
	pSimStat->n_layer[ID_BOOM_H] = pSimStat->nd[ID_BOOM_H].p - pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]*(pSimStat->i_layer[ID_BOOM_H]-1);
	//起伏ドラム巻取り量	
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H] - 1] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	//ジブ部距離　（基準ロープ長-ドラム巻取り量）ロープ掛数
	pSimStat->db.p = (pCraneStat->Cdr[ID_BOOM_H][0] - pSimStat->l_drum[ID_BOOM_H])/ pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];
	pSimStat->d.p = sqrt(pSimStat->db.p* pSimStat->db.p + Lmb2-2.0 * pSimStat->db.p * LmbCosAdb);

	c_phb = (cal_Lb2Lp2 - pSimStat->db.p * pSimStat->db.p) / cal_2LbLp;
	c_ph = (cal_Lm2Lp2 - pSimStat->d.p * pSimStat->d.p) / cal_2LmLp;
	pSimStat->phb.p = acos(c_phb);					//φb
	pSimStat->ph.p = pSimStat->phb.p + pspec->Alpa_b;	//φ
	s_ph = sin(pSimStat->ph.p);
	pSimStat->th.p = pspec->Php - pSimStat->ph.p;	//θ
			
	double LbLpsinPh = pspec->Lb * pspec->Lp * s_ph;
	double Kw = pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]]/ pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];//ドラム周長/ワイヤ掛数
	
	pSimStat->db.v = -pSimStat->nd[ID_BOOM_H].v * Kw;///ワイヤ掛数;
	pSimStat->d.v = (pSimStat->db.p - LmbCosAdb) / LmLb * pSimStat->db.v;

	pSimStat->ph.v = pSimStat->d.p * pSimStat->d.v / LbLpsinPh;		//dφ/dt = dd'/LpLbsinφ
	pSimStat->th.v = -pSimStat->ph.v;								//-dφ/dt

	//加速度
	pSimStat->db.a = -pSimStat->nd[ID_BOOM_H].a * Kw;														//回転は引込方向が＋（＋回転→d縮）
	pSimStat->d.a = (pSimStat->db.v* pSimStat->db.v + (pSimStat->db.p - LmbCosAdb)*pSimStat->db.a)/LmLb;	//回転は引込方向が＋（＋回転→d縮）

	//pSimStat->ph.a = pSimStat->ph.v * (pSimStat->d.v / pSimStat->d.p + pSimStat->d.a/ pSimStat->d.v  - pSimStat->ph.v * c_ph / s_ph);
	pSimStat->ph.a = (pSimStat->d.v * pSimStat->d.v + pSimStat->d.p * pSimStat->d.a)/(pspec->Lm * pspec->Lp * s_ph)- pSimStat->ph.v* pSimStat->ph.v* c_ph / s_ph;
	pSimStat->th.a = -pSimStat->ph.a;
#endif
	return; 
} 
//引込ドラム状態をセットする
void  CSimJC::set_bh_layer() {
#if 0
	UINT32 i = (UINT32)(pSimStat->nd[ID_BOOM_H].p  / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]); //現在層数-1
	pSimStat->i_layer[ID_BOOM_H] = i + 1;
	pSimStat->n_layer[ID_BOOM_H] = (pCraneStat->Cdr[ID_BOOM_H][0] - pCraneStat->Ldr[ID_BOOM_H][i]) / pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[ID_BOOM_H][i] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
#endif
	return; 
}
//主巻ドラム回転状態をセットする
void  CSimJC::set_mh_layer(){
#if 0
	//引込主巻ドラム部
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum - pSimStat->nd[ID_BOOM_H].p;
	int i = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]); //現在層数-1

	pSimStat->n_layer[ID_BHMH] =  pSimStat->nd[ID_BHMH].p - (double)(i* pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]);
	pSimStat->i_layer[ID_BHMH] = i + 1;
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][i] + pSimStat->n_layer[ID_BHMH] * pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];

	//主巻ドラム部
	i = (UINT32)(pSimStat->nd[ID_HOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST]); //現在層数-1
	pSimStat->n_layer[ID_HOIST] = pSimStat->nd[ID_HOIST].p - (double)(i*pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST]);
	pSimStat->i_layer[ID_HOIST] = i + 1;
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Ldr[ID_HOIST][i] + pSimStat->n_layer[ID_HOIST] * pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];
#endif
	return; 
} 
//補巻ドラム回転状態とロープ長をセットする
void  CSimJC::set_ah_layer(){

#if 0
	//補巻ドラム部
	int i= (UINT32)(pSimStat->nd[ID_AHOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST]); //現在層数-1
	pSimStat->n_layer[ID_AHOIST] = pSimStat->nd[ID_AHOIST].p - (double)(i * pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST]);
	pSimStat->i_layer[ID_AHOIST] = i + 1;
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Ldr[ID_AHOIST][i] + pSimStat->n_layer[ID_AHOIST] * pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];
#endif
	return;
} 
//旋回ドラム回転状態をセットする
void  CSimJC::set_sl_layer(){
#if 0
	//旋回ピニオン部
	pSimStat->l_drum[ID_SLEW]	= pSimStat->nd[ID_SLEW].p * pCraneStat->Cdr[ID_SLEW][1];										//旋回移動量(m)
	pSimStat->i_layer[ID_SLEW]	= 1;// (UINT32)(pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0]);						//旋回回転数整数部 = 回転移動量/TTB円周
	pSimStat->n_layer[ID_SLEW]	= 0;// (pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0] - pSimStat->i_layer[ID_SLEW]);	//旋回回転数小数点以下
#endif
	return; 
} 
//走行ドラム回転状態をセットする
void  CSimJC::set_gt_layer(){
#if 0
	pSimStat->l_drum[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p * pCraneStat->Cdr[ID_GANTRY][1];										//移動量(m)
	pSimStat->i_layer[ID_GANTRY] = 1;// (UINT32)(pSimStat->nd[ID_GANTRY].p);																//走行車輪回転数部
	pSimStat->n_layer[ID_GANTRY] = 0;// (pSimStat->l_drum[ID_GANTRY] / pCraneStat->Cdr[ID_GANTRY][0] - pSimStat->i_layer[ID_GANTRY]);	//旋回回転数小数点以下
#endif
	return; 
} 


Vector3 CSimGC::A(Vector3& _r, Vector3& _v) {
	Vector3 vec3;
	return vec3;
}

Vector3 CSimOHC::A(Vector3& _r, Vector3& _v) {
	Vector3 vec3;
	return vec3;
}

/********************************************************************************/
/*      Load Object(吊荷）                                                      */
/********************************************************************************/

//吊荷位置の初期化
void CLoad ::init_mob(double _dt, Vector3& _r, Vector3& _v) {
	dt = _dt;
	r.copy(_r);
	v.copy(_v);
	return;
}

Vector3 CLoad::A(Vector3& r, Vector3& v) {
	Vector3 a;
	Vector3 L_;

	if (type == ID_AHOIST)
		L_ = L_.subVectors(r, pCrane->r2);
	else 					
		L_ = L_.subVectors(r, pCrane->r);

	double Sdivm = S() / m;

	a = L_.clone().multiplyScalor(Sdivm);
	a.z -= GA;

	double temp_d = L_.length();

	//計算誤差によるロープ長ずれ補正
	Vector3 hatL = L_.clone().normalize();
	// 補正ばね弾性力
	//Vector3 ak = hatL.clone().multiplyScalor(-compensationK * (pCrane->l_mh - L_.length()));

	Vector3 ak, v_;
	if (type == ID_AHOIST) {
		ak = hatL.clone().multiplyScalor(-compensationK * ( L_.length() - pCrane->l_ah));
		v_ = v_.subVectors(v, pCrane->v2);
	}
	else {
		ak = hatL.clone().multiplyScalor(-compensationK * (L_.length() - pCrane->l_mh));
		v_ = v_.subVectors(v, pCrane->v);
	}


	// 補正粘性抵抗力
	Vector3 agamma = hatL.clone().multiplyScalor(-compensationGamma * v_.dot(hatL));
	
	// 張力にひもの長さの補正力を加える
	a.add(ak).add(agamma);

	return a;
} //Model of acceleration

double  CLoad::S() { //Aの計算部の関係でS/Lとなっている。巻きの加速度分が追加されている。
	Vector3 v_ = v.clone().sub(pCrane->v);
	double v_abs2 = v_.lengthSq();
	Vector3 vectmp;
	Vector3 vecL; //= vectmp.subVectors(r, pCrane->r);

	if (type == ID_AHOIST) {
		vecL = vectmp.subVectors(r, pCrane->r2);
		return -m * (v_abs2 - pCrane->a.dot(vecL) - GA * vecL.z - (pCrane->a0[ID_AHOIST] * pCrane->l_ah + pCrane->v0[ID_AHOIST] * pCrane->v0[ID_AHOIST])) / (pCrane->l_ah * pCrane->l_ah);
	}
	else {
		vecL = vectmp.subVectors(r, pCrane->r);
		return -m * (v_abs2 - pCrane->a.dot(vecL) - GA * vecL.z - (pCrane->a0[ID_HOIST] * pCrane->l_mh + pCrane->v0[ID_HOIST] * pCrane->v0[ID_HOIST])) / (pCrane->l_mh * pCrane->l_mh);
	}
	return 0.0;
}

void CLoad::update_relative_vec() {//クレーン吊点との相対位置速度
	Vector3 vectmp;

	if (type == ID_AHOIST) {
		L = vectmp.subVectors(r, pCrane->r2);
		vL = vectmp.subVectors(v, pCrane->v2);
	}
	else {
		L = vectmp.subVectors(r, pCrane->r);
		vL = vectmp.subVectors(v, pCrane->v);
	}
	return;
}


