#pragma once
#include "CSpec.h"
#include "CPlc.h"
#include "CFaults.h"
#include "CVector3.h"

typedef struct _ST_CRANE_INFO {
	int crane_id;
	int crane_type;
}ST_CRANE_INFO, * LPST_CRANE_INFO;

class CCrane
{
private:
	int crane_id;
public:
	
	/// <summary>
	/// コンストラクタ　
	/// plc_rbuf,plc_wbuf (MCC用）PLCとの通信バッファのアドレス,(OTE用）MCCとの通信バッファのアドレス
	/// </summary>
	/// <param name="_crane_id"></param>
	/// <param name="plc_rbuf"></param>
	/// <param name="plc_wbuf"></param>
	CCrane(int _crane_id, INT16* plc_rbuf, INT16* plc_wbuf);
	CCrane(int _crane_id);
	virtual ~CCrane();

	CSpec* pSpec;
	CPlc* pPlc;
	CFaults* pFlt;
	ST_CRANE_INFO st_crane_inf;

	int		get_id() { return crane_id; };
	void	set_crane_type();
	void	update_vect_lp_load();								//吊荷の吊点からの相対ベクトル
	double	S();												//Rope tension(張力計算）
	void	init_crane(double t, Vector3& r, Vector3& v);

	//PLC通信バッファ
	LPST_JC_PLC_IO_R get_plc_rif()	{ if (pPlc != NULL) return &(pPlc->un_plc_io_rif.JC);	return NULL;}
	LPST_JC_PLC_IO_W get_plc_wif()	{ if (pPlc != NULL) return &(pPlc->un_plc_io_wif.JC);	return NULL;}
	
	//仕様定義構造体
	LPST_STRUCTURE	 get_st_struct(){ if (pSpec != NULL)return &pSpec->st_struct;	return NULL;}
	LPST_AXIS_SPEC	 get_axis_mh()	{ if (pSpec != NULL)return &pSpec->axis_spec[ID_HOIST];		return NULL;}
	LPST_AUTO_SPEC	 get_auto_mh()	{ if (pSpec != NULL)return &pSpec->auto_spec[ID_HOIST];		return NULL;}
	LPST_REMOTE_SPEC get_rmt_mh()	{ if (pSpec != NULL)return &pSpec->rmt_spec[ID_HOIST];		return NULL;}
	LPST_AXIS_SPEC	 get_axis_bh()	{ if (pSpec != NULL)return &pSpec->axis_spec[ID_BOOM_H];	return NULL;}
	LPST_AUTO_SPEC	 get_auto_bh()	{ if (pSpec != NULL)return &pSpec->auto_spec[ID_BOOM_H];	return NULL;}
	LPST_REMOTE_SPEC get_rmt_bh()	{ if (pSpec != NULL)return &pSpec->rmt_spec[ID_BOOM_H];		return NULL;}
	LPST_AXIS_SPEC	 get_axis_sl()	{ if (pSpec != NULL)return &pSpec->axis_spec[ID_SLEW];		return NULL;}
	LPST_AUTO_SPEC	 get_auto_sl()	{ if (pSpec != NULL)return &pSpec->auto_spec[ID_SLEW];		return NULL;}
	LPST_REMOTE_SPEC get_rmt_sl()	{ if (pSpec != NULL)return &pSpec->rmt_spec[ID_SLEW];		return NULL;}
	LPST_AXIS_SPEC	 get_axis_gt()	{ if (pSpec != NULL)return &pSpec->axis_spec[ID_GANTRY];	return NULL;}
	LPST_AUTO_SPEC	 get_auto_gt()	{ if (pSpec != NULL)return &pSpec->auto_spec[ID_GANTRY];	return NULL;}
	LPST_REMOTE_SPEC get_rmt_gt()	{ if (pSpec != NULL)return &pSpec->rmt_spec[ID_GANTRY];		return NULL;}
	LPST_AXIS_SPEC	 get_axis_ah()	{ if (pSpec != NULL)return &pSpec->axis_spec[ID_AHOIST];	return NULL;}
	LPST_AUTO_SPEC	 get_auto_ah()	{ if (pSpec != NULL)return &pSpec->auto_spec[ID_AHOIST];	return NULL;}
	LPST_REMOTE_SPEC get_rmt_ah()	{ if (pSpec != NULL)return &pSpec->rmt_spec[ID_AHOIST];		return NULL;}
};

