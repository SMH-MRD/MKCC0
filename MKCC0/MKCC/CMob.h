#pragma once

#include "CVector3.h"
#include "COMMON_DEF.H"
#include "CCrane.h"
#include "CSpec.h"
#include "CSharedMem.h"
#include "SmemMain.H"


// 加速度一次遅れフィルタ時定数　double Tf[MOTION_ID_MAX];
#define SIM_TF_HOIST           0.2   //巻 　      
#define SIM_TF_GANTRY          0.2   //走行       
#define SIM_TF_TROLLY          0.2   //横行       
#define SIM_TF_BOOM_H          0.2   //引込       
#define SIM_TF_SLEW            0.2   //旋回
#define SIM_TF_AHOIST          0.2   //横行  

#define SIM_TRQ_FB_P100        1000  //トルクFB　100%値

#define SIM_VRESET_COUNT       1000 //速度指令0継続で速度を強制的に0にするカウント値

//Moving Objectクラス
class CMob
{
public:

    CMob();
    ~CMob();

    int id;         //機器ID
    int type;       //Mob種別
    double dt;      //計算時間間隔
    Vector3 a;      //加速度ベクトル
    Vector3 r;      //位置ベクトル
    Vector3 v;      //速度ベクトル
    Vector3 L;      //ロープベクトル(吊荷吊点相対ベクトル)
    Vector3 vL;     //ロープ速度ベクトル(吊荷吊点相対ベクトル)
    Vector3 fex;    //外力
    Vector3 dr;     //位置ベクトルの変化分
    Vector3 dv;     //速度ベクトルの変化分
    Vector3 R0;     //基準点

    double r0[MOTION_ID_MAX];                       //吊点　位置・角度(m, rad) 起伏は半径
    double v0[MOTION_ID_MAX];                       //吊点　速度・角速度(m/s, rad/s)
    double a0[MOTION_ID_MAX];                       //吊点　加速度・角加速度(m/s2, rad/s2)

    Vector3 rc;                                     //クレーン中心点の位置ベクトル
    Vector3 vc;                                     //クレーン中心点の速度ベクトル

    Vector3 r2;                                     //第2（補巻）吊点位置ベクトル
    Vector3 v2;                                     //第2（補巻）吊点速度ベクトル
    Vector3 a2;                                     //第2（補巻）吊点ベクトル

    double l_mh;                                    //巻ロープ長 m
    double l_ah;                                    //補巻巻ロープ長 m
     
    //加速度ベクトルを与えるメソッド　　継承先で再定義する
    virtual Vector3 A(Vector3& r, Vector3& v); 
    virtual void set_fex(double,double,double);         //外力
    virtual void set_dt(double);                        //計算時間間隔セット

    //速度ベクトルを与えるメソッド
    virtual Vector3 V(Vector3& r, Vector3& v);
    //時間発展を計算するメソッド
    virtual void timeEvolution();
    virtual void init_mob(double _dt, Vector3& _r, Vector3& _v) {
        dt = _dt;
        r.copy(_r);
        v.copy(_v);
        return;
    }
 
private:

};

//クレーンクラス
//r,vは、吊点の位置と座標

#define SIM_INIT_R                  40.0       //デフォルト旋回半径主巻
#define SIM_INIT_MHR                40.0       //デフォルト旋回半径主巻
#define SIM_INIT_MH                 50.0       //デフォルト主巻高さ
#define SIM_INIT_AH                 60.50      //デフォルト補巻高さ
#define SIM_INIT_TH                 0.0        //旋回初期値 rad
#define SIM_INIT_L                  9.8        //ロープ長初期値 m
#define SIM_INIT_X                  50.0       //走行初期値 m
#define SIM_INIT_M                  10000.0    //荷重初期値 kg

#define MOB_MODE_SIM                0
#define MOB_MODE_PLC                1


class CSimJC : public CMob
{
public:
	CSimJC(int _id);
    ~CSimJC();

    //各軸の仕様パラメータ
    LPST_STRUCTURE pStruct;
    //各軸の仕様パラメータ
    LPST_AXIS_SPEC pAxis_mh;
    LPST_AXIS_SPEC pAxis_bh;
    LPST_AXIS_SPEC pAxis_sl;
    LPST_AXIS_SPEC pAxis_gt;
    LPST_AXIS_SPEC pAxis_ah;

    LPST_SIMULATION_STATUS pSimStat;

    ST_SIM_LOAD M[MOTION_ID_MAX];                   //クレーン軸荷重（走行は全体荷重）
 
    int source_mode;
        
     double nv[MOTION_ID_MAX];                       //ドラム回転速度(%rps)
     double na[MOTION_ID_MAX];                       //ドラム回転加速度(%rps2)
     double nv_ref[MOTION_ID_MAX];                    //ドラム回転速度・角速度指令
     double na_ref[MOTION_ID_MAX];                    //ドラム加速度・角加速度指令

     double nacc_spec[MOTION_ID_MAX];
     double ndec_spec[MOTION_ID_MAX];


    bool is_fwd_endstop[MOTION_ID_MAX];             //正転極限判定
    bool is_rev_endstop[MOTION_ID_MAX];             //逆転極限判定
 
    double trq_fb[MOTION_ID_MAX];                   //モータートルクFB
    bool motion_brake[MOTION_ID_MAX];               //ブレーキ開閉状態
      
    void init_crane(int crane_id); 
    void get_crane_status(LPST_CRANE_STAT pstat, LPST_CC_PLC_IO pplcio);
    void timeEvolution();                           //時間発展を計算するメソッド
    void set_mode(int _mode) { source_mode = _mode;return; }

private:
    int v_reset_count[MOTION_ID_MAX];
    double brk_elaped_time[MOTION_ID_MAX];          //ブレーキ開放経過時間
    double Tf[MOTION_ID_MAX];                       //一次遅れフィルタ時定数

    Vector3 A(Vector3& _r, Vector3& _v);            //吊点加速度計算（旋回、引込方向をxy方向に変換
    void Ac();                                      //クレーン加速度計算 SIM mode, PLC mode

    double accdec_cut_spd_range[MOTION_ID_MAX];     //加減速指令を0にする速度指令とFBの差の範囲
 };


class CSimGC : public CMob
{
public:
    CSimGC(int _id) { id = _id; };
    ~CSimGC() {};

    //各軸の仕様パラメータ
    LPST_STRUCTURE pStruct;
    //各軸の仕様パラメータ
    LPST_AXIS_SPEC pAxis_mh;
    LPST_AXIS_SPEC pAxis_bh;
    LPST_AXIS_SPEC pAxis_sl;
    LPST_AXIS_SPEC pAxis_gt;
    LPST_AXIS_SPEC pAxis_ah;

    LPST_CRANE_STAT pCraneStat;
    LPST_CC_PLC_IO  pPLC_IO;
    LPST_SPEC pspec;
    LPST_SIMULATION_STATUS pSimStat;

    double M;                                       //クレーン全体質量　Kg
    void init_crane(int crane_id);                              

private:
    double brk_elaped_time[MOTION_ID_MAX];          //ブレーキ開放経過時間
    double Tf[MOTION_ID_MAX];                       //加速度一時遅れ

    Vector3 A(Vector3& _r, Vector3& _v);            //吊点加速度計算（旋回、引込方向をxy方向に変換
    void Ac();                                      //クレーン加速度計算 SIM mode, PLC mode

    double accdec_cut_spd_range[MOTION_ID_MAX];     //加減速指令を0にする速度指令とFBの差の範囲
};

class CSimOHC : public CMob
{
public:
    CSimOHC(int _id) { id = _id; };
    ~CSimOHC() {};

    //各軸の仕様パラメータ
    LPST_STRUCTURE pStruct;
    //各軸の仕様パラメータ
    LPST_AXIS_SPEC pAxis_mh;
    LPST_AXIS_SPEC pAxis_bh;
    LPST_AXIS_SPEC pAxis_sl;
    LPST_AXIS_SPEC pAxis_gt;
    LPST_AXIS_SPEC pAxis_ah;

    LPST_CRANE_STAT pCraneStat;
    LPST_CC_PLC_IO  pPLC_IO;
    LPST_SIMULATION_STATUS pSimStat;

    double M;                                       //クレーン全体質量　Kg
    void init_crane(double _dt);

private:
    double brk_elaped_time[MOTION_ID_MAX];          //ブレーキ開放経過時間
    double Tf[MOTION_ID_MAX];                       //加速度一時遅れ

    Vector3 A(Vector3& _r, Vector3& _v);            //吊点加速度計算（旋回、引込方向をxy方向に変換
    void Ac();                                      //クレーン加速度計算 SIM mode, PLC mode

    double accdec_cut_spd_range[MOTION_ID_MAX];     //加減速指令を0にする速度指令とFBの差の範囲
};


//計算誤差吸収処理　紐長さ補正力＝補正ばね弾性力＋補正粘性抵抗力
#define compensationK 0.5                           //紐長さ補正弾性係数
#define compensationGamma 0.5                       //紐長さ粘性係数

//吊荷クラス
class CLoad : public CMob
{
public:
    CLoad() { M.m = 10000.0; pMobBase = NULL; };
    ~CLoad() {};

    void init_load(int id);
    void update_relative_vec();         //吊点との相対ベクトル更新
    Vector3 A(Vector3& r, Vector3& v);  //Model of acceleration
    double S();	//Rope tension

    CMob * pMobBase;//接続クレーン

    ST_SIM_LOAD M; //吊荷質量Kg,サイズ
         
    int type;                   //吊荷のタイプ
 
    int set_m(double _m) { M.m = _m; return(0); }
    int set_crane(CMob* _pMobBase) { pMobBase =_pMobBase; return(0); }
    int set_type(int _type) { type = _type; return(type); }

private:

};




