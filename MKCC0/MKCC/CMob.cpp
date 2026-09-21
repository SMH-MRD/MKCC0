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
//ルンゲクッタ方による時間発展
void CMob::timeEvolution() {

	Vector3 v1 = V(r, v);
	Vector3 a1 = A(r, v);

	Vector3 _v1 = Vector3(r.x + v1.x * dt / 2.0, r.y + v1.y * dt / 2.0, r.z + v1.z * dt / 2.0);//半歩進んだ位置のr(_v1となっているのでv1と勘違いした）
	Vector3 _a1 = Vector3(v.x + a1.x * dt / 2.0, v.y + a1.y * dt / 2.0, v.z + a1.z * dt / 2.0);//半歩進んだ位置のv(_a1となっているのでa1と勘違いした）
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



	//dr.x = dt * v.x;
	//dr.y = dt * v.y;
	//dr.z = dt * v.z;

	dr.x = dt / 6.0 * (v1.x + 2.0 * v2.x + 2.0 * v3.x + v4.x);
	dr.y = dt / 6.0 * (v1.y + 2.0 * v2.y + 2.0 * v3.y + v4.y);
	dr.z = dt / 6.0 * (v1.z + 2.0 * v2.z + 2.0 * v3.z + v4.z);

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
	pAxis_mh = pCrane->get_axis_mh();
	pAxis_bh = pCrane->get_axis_bh();
	pAxis_sl = pCrane->get_axis_sl();
	pAxis_gt = pCrane->get_axis_gt();
	pAxis_ah = pCrane->get_axis_ah();
}
CSimJC::~CSimJC() {}

//軸加速度
void CSimJC::Ac() {	//加速度計算
	for (int i = 0; i < SIM_N_AXIS; i++) {
	//## ドラム加速指令計算
		//速度指令に未達(ドラム回転速度　指令>FB）
		if ((motion_brake[i] == false) && 0) {//ブレーキ閉
			na_ref[i] = 0.0;
		}
		else if ((nv_ref[i] - nv[i]) > accdec_cut_spd_range[i]) {
			if (nv_ref[i] > 0.0) na_ref[i] = nacc_spec[i];			//正転加速指令
			else				 na_ref[i] = ndec_spec[i];			//逆転減速指令
		}
		//速度指令に未達(ドラム回転速度 指令<FB) 
		else if ((nv_ref[i] - nv[i]) < -accdec_cut_spd_range[i]) {
			if (nv_ref[i] > 0.0) na_ref[i] = -ndec_spec[i];			//正転減速指令
			else				 na_ref[i] = -nacc_spec[i];			//逆転加速指令
		}
		//速度指令に到達
		else {
			na_ref[i] = 0.0;
		}

		//極限停止
		if ((na_ref[i] > 0.0) && (is_fwd_endstop[i])) na_ref[i] = 0.0;
		if ((na_ref[i] < 0.0) && (is_rev_endstop[i])) na_ref[i] = 0.0;

	//## ドラム加速度FB計算　当面指令に対して一次遅れフィルタを入れる形で計算
		//一次遅れフィルタ式　Yk = (dt*Xk+Tf*Yk-1)/(dt+Tf)
	
		na[i] = (dt * na_ref[i] + Tf[i] * na[i]) / (dt + Tf[i]);
		//0リミット
		if ((na[i] < 0.00001) && (na[i] > -0.00001))	na[i] = 0.0;
	}
	return;
}

/// <summary>
/// 
/// </summary>
/// <param name="_r"></param>
/// <param name="_v"></param>
/// <returns></returns>
Vector3 CSimJC::A(Vector3& _r, Vector3& _v) {
	Vector3 vec3;	
	return vec3;
}
void CSimJC::timeEvolution() {
	//クレーン部
	//ドラム加速度計算
	Ac();

//### ドライブ制御関連
	//トルク・速度　FB計算
	for (int i = 0; i < SIM_N_AXIS; i++) {
		//トルクFB
		if (na_ref[i] != 0) {//加速指令!=0
			//トルクFB
			if (motion_brake[i] == false) {//ブレーキ閉
				trq_fb[i] = SIM_TRQ_FB_P100 * 0.3;	//30%トルク指令
			}
			else {//ブレーキ開
				trq_fb[i] = SIM_TRQ_FB_P100 * 0.8;	//80%トルク指令
			}
		}
		else {
			trq_fb[i] = 0;	//停止時は速度0
		}

		//速度FB(オイラー法）
		nv[i] += na[i] * dt;
		if (v_reset_count[i] >= SIM_VRESET_COUNT) nv[i] = 0.0;	//速度指令0が一定時間以上で強制的に0

		//チェックカウント処理
		if (nv_ref[i] != 0.0) {
			v_reset_count[i] = 0;
		}
		else {
			if (v_reset_count[i] >= SIM_VRESET_COUNT) v_reset_count[i] = SIM_VRESET_COUNT;
			else v_reset_count[i]++;
		}
	}
	return;
}
void CSimJC::init_crane(int crane_id) {

	//ドラム加速度仕様
	nacc_spec[ID_HOIST]	= pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Ta0;
	ndec_spec[ID_HOIST]	= pAxis_mh->Rpm_rated / 60.0 / pAxis_mh->Td0;
	nacc_spec[ID_BOOM_H] = pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Ta0;
	ndec_spec[ID_BOOM_H] = pAxis_bh->Rpm_rated / 60.0 / pAxis_bh->Td0;
	nacc_spec[ID_SLEW]	= pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Ta0;
	ndec_spec[ID_SLEW]	= pAxis_sl->Rpm_rated / 60.0 / pAxis_sl->Td0;
	nacc_spec[ID_GANTRY] = pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Ta0;
	ndec_spec[ID_GANTRY] = pAxis_gt->Rpm_rated / 60.0 / pAxis_gt->Td0;
	nacc_spec[ID_AHOIST] = pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Ta0;
	ndec_spec[ID_AHOIST] = pAxis_ah->Rpm_rated / 60.0 / pAxis_ah->Td0;

	//0速とみなす速度上限（ドラム回転速度）
	accdec_cut_spd_range[ID_HOIST] = 0.005 * pAxis_mh->Rpm_rated / 60.0 / (pCrane->get_axis_mh()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_BOOM_H] = 0.005 * pAxis_bh->Rpm_rated / 60.0 / (pCrane->get_axis_bh()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_SLEW] = 0.005 * pAxis_sl->Rpm_rated / 60.0 / (pCrane->get_axis_sl()->Gear_ratio);	//0.5%
	accdec_cut_spd_range[ID_GANTRY] = 0.005 * pAxis_gt->Rpm_rated / 60.0 / (pCrane->get_axis_gt()->Gear_ratio);	//0.5%	
	accdec_cut_spd_range[ID_AHOIST] = 0.005 * pAxis_ah->Rpm_rated / 60.0 / (pCrane->get_axis_ah()->Gear_ratio);	//0.5%

	for (int i = 0; i < SIM_N_AXIS; i++) {
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		Tf[i] = 1.0;
		brk_elaped_time[i] = 0.0;
		a0[i] = 0.0;
		v0[i] = 0.0;
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		trq_fb[i] = 0.0;    //モータートルクFB
		motion_brake[i] = false;
	}
	source_mode = MOB_MODE_SIM;
	//加速度一次遅れフィルタ時定数
	Tf[ID_HOIST]	= SIM_TF_HOIST;
	Tf[ID_BOOM_H]	= SIM_TF_BOOM_H;
	Tf[ID_SLEW]		= SIM_TF_SLEW;
	Tf[ID_GANTRY]	= SIM_TF_GANTRY;
	Tf[ID_AHOIST]	= SIM_TF_AHOIST;

	//軸負荷セット
	for (int i = 0; i < MOTION_ID_MAX; i++) {
		M[i].m = M[i].wx = M[i].dy = M[i].hz = 0.0;
	}
	return;
}

void CSimJC::get_crane_status(LPST_CRANE_STAT pstat, LPST_CC_PLC_IO pplc) {

	for (int i = 0; i < SIM_N_AXIS; i++) {
		//速度指令値　取り込みPLC 指令は100%→1.0　ドラム回転速度指令でセット
		nv_ref[i] = (double)pplc->stat_axis[i].v_ref * pCrane->pSpec->axis_spec[i].Rpm_rated / 60000.0;//定格rpmの100%が1000→RPSに変換→1000*60で割る
		if (i == ID_SLEW) {//旋回は油圧ブレーキ
			if (pplc->stat_axis[i].brake)	motion_brake[i] = false; //PLC信号 ONでブレーキ閉
			else							motion_brake[i] = true;
		}
		else {
			if (pplc->stat_axis[i].brake)	motion_brake[i] = true; //PLC信号 ONでブレーキ開
			else							motion_brake[i] = false;
		}

	}

	pSimStat->th = pstat->bh_th;
	pSimStat->ph = pstat->sl_ph;
	pSimStat->d	 = pstat->d;
	pSimStat->lrm = pstat->mhl;

	pLoad->l_mh = pstat->mhl.p;//ロープ長

	r.copy(pstat->r);
	v.copy(pstat->v);
	a.copy(pstat->a);



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
void CLoad ::init_load(int id) {
	M.m = pCrane->pSpec->st_struct.Whook;
	return;
}

Vector3 CLoad::A(Vector3& r, Vector3& v) {

	Vector3 L_ = L_.subVectors(r, pMobBase->r);		//吊荷と吊点の相対ベクトル（直行座標）

	double Sdivm = S() / M.m;	//張力/吊荷質量 a=F/m

	Vector3 a = L_.clone().multiplyScalor(Sdivm);
	a.z -= GA;
	
	//計算誤差によるロープ長ずれ補正
	Vector3 hatL = L_.clone().normalize();
	// 補正ばね弾性力
	Vector3 ak = hatL.clone().multiplyScalor(-compensationK * (l_mh - L_.length()));
	Vector3	v_ = v_.subVectors(v, pMobBase->v);
	// 補正粘性抵抗力
	Vector3 agamma = hatL.clone().multiplyScalor(-compensationGamma * v_.dot(hatL));
			
	// 張力にひもの長さの補正力を加える
	a.add(ak).add(agamma);

	return a;
} //Model of acceleration

double  CLoad::S() { //Aの計算部の関係でS/Lとなっている。巻きの加速度分が追加されている。

	Vector3 v_ = v.clone().sub(pMobBase->v);	//吊荷-吊点間相対速度ベクトル
	double v_abs2 = v_.lengthSq();				//吊荷-吊点間相対速度ベクトル距離の2乗
	Vector3 vecL = vecL.subVectors(r, pMobBase->r);


	return  -M.m * (v_abs2 - pMobBase->a.dot(vecL) - GA * vecL.z ) / (l_mh * l_mh);
}

void CLoad::update_relative_vec() {//クレーン吊点との相対位置速度
	L	= L.subVectors(r, pMobBase->r);
	vL	= vL.subVectors(v, pMobBase->v);
	return;
}


