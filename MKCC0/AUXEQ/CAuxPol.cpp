#include "CAuxPol.h"
#include "CAuxEnv.h"
#include "resource.h"
#include "CCamera.h"
#include "SWYSENSOR_DEF.H"
#include "phisics.h"
#include "CHelper.h"
#include "CSwayShared.h"
#include <mutex>

std::mutex auxpol_mtx;//ロックガード用無ミューテックス
extern std::vector<void*> VectpCTaskObj;    // TaskObjのポインタ
extern BC_TASK_ID         g_task_index;     // TaskObjのインデックス

//組み込み機能
extern int g_slbrk_enable;//旋回ブレーキ
extern int g_lanio_enable;//LANIO
extern int g_sway_sensor_enable;//振れセンサー
extern int g_gt_sensor_enable;//走行位置検出

// Swayセンサ関連
extern CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ
extern CSwayShared* pSwaySharedObj;

// ***アプリケーション設定アクセスポインタ
extern PCONFIG_COMMON    gp_cnfg_common;        // 共通設定
extern PCONFIG_CAMERA    gp_cnfg_camera;        // カメラ設定
extern PCONFIG_MOUNTING  gp_cnfg_mounting;      // 取付寸法設定
extern PCONFIG_IMGPROC   gp_cnfg_imgprc;		// 画像処理条件設定

// ***アプリケーション情報アクセスポインタ  
extern PINFO_IMGBUF_DATA gp_app_imgbuf[(uint32_t)(ENUM_IMAGE::E_MAX)];
extern PINFO_CLIENT_DATA gp_app_client;        // クライアント情報
extern PINFO_ADJUST_DATA gp_app_adjust;        // 調整情報
extern PINFO_IMGPRC_DATA gp_app_imgprc;        // 画像処理情報
extern PINFO_SYSTEM_DATA gp_app_system;        // システム情報

extern IMAGE_DATA g_img_src_work;
extern ST_DEVICE_CODE g_my_code;

static CAuxEnv * pAuxEnvObj;					// CAuxEnvインスタンスのポインタ

int32_t CAuxPol::maintenance_mode = CODE_POL_MAINTE_OFF;
int32_t CAuxPol::disp_mode = CODE_POL_MAINTE_OFF;

ST_POL_MON1 CAuxPol::st_mon1;
ST_POL_MON2 CAuxPol::st_mon2;
ST_MOVE_AVE_DATA CAuxPol::m_move_avrg_data;	
ST_SWAY_WORK CAuxPol::st_sway_work;

//共有メモリ参照用定義
extern CSharedMem* pEnvInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pCsInfObj;
extern CSharedMem* pScadInfObj;
extern CSharedMem* pPolInfObj;

static LPST_AUX_ENV_INF		pEnvInf;
static LPST_AUX_CS_INF		pCsInf;
static LPST_AUX_AGENT_INF	pAgentInf;
static LPST_AUX_SCAD_INF	pScadInf;
static LPST_AUX_POL_INF		pPolInf;


/****************************************************************************/
/*   デフォルト関数											                    */
/****************************************************************************/
CAuxPol::CAuxPol() {
}
CAuxPol::~CAuxPol() {
}

HRESULT CAuxPol::initialize(LPVOID lpParam) {

	HRESULT hr = S_OK;
	//### 出力用共有メモリ取得
	out_size = sizeof(ST_AUX_POL_INF);
	if (OK_SHMEM != pPolInfObj->create_smem(SMEM_AUX_POL_INF_NAME, sizeof(ST_AUX_POL_INF), MUTEX_AUX_POL_INF_NAME)) {
		return(FALSE);
	}
	pPolInf = (LPST_AUX_POL_INF)pPolInfObj->get_pMap();
	set_outbuf(pPolInfObj->get_pMap());

	//### 入力用共有メモリ取得
	if (OK_SHMEM != pCsInfObj->create_smem(SMEM_AUX_CS_INF_NAME, sizeof(ST_AUX_CS_INF), MUTEX_AUX_CS_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AUX_AGENT_INF_NAME, sizeof(ST_AUX_AGENT_INF), MUTEX_AUX_AGENT_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pScadInfObj->create_smem(SMEM_AUX_SCAD_INF_NAME, sizeof(ST_AUX_SCAD_INF), MUTEX_AUX_SCAD_INF_NAME)) {
		return(FALSE);
	}
	if (OK_SHMEM != pEnvInfObj->create_smem(SMEM_AUX_ENV_INF_NAME, sizeof(ST_AUX_ENV_INF), MUTEX_AUX_ENV_INF_NAME)) {
		return(FALSE);
	}

	pEnvInf = (LPST_AUX_ENV_INF)(pEnvInfObj->get_pMap());
	pAgentInf = (LPST_AUX_AGENT_INF)(pAgentInfObj->get_pMap());
	pCsInf = (LPST_AUX_CS_INF)pCsInfObj->get_pMap();

	if ((pEnvInf == NULL) || (pAgentInf == NULL) || (pCsInf == NULL))
		hr = S_FALSE;

	if (hr == S_FALSE) {
		wos.str(L""); wos << L"Initialize : SMEM NG"; msg2listview(wos.str());
		return hr;
	};

	//EnvInfの初期化完了待ち
	while(pEnvInf->initialized != L_ON) {
		Sleep(100);
	}

	//振れセンサ機能セットアップ
	if (g_sway_sensor_enable) {
		init_sway_sensor();
		//ダミー画像書き込み
		g_img_src_work.data_mat = cv::imread("C:\/Work\/NonImg.bmp");

		for (int i = 0; i < (int)ENUM_AXIS::E_MAX; i++) {
			st_sway_work.peak_chk_flg[i] = POL_CODE_P2P_WAIT_F_PEAK;
			st_sway_work.sway_peak_f[i] = 0.0;
			st_sway_work.sway_peak_r[i] = 0.0;
		}
	}

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
	set_item_chk_txt();
	set_panel_tip_txt();
	//モニタ2 CB状態セット	
	if (st_mon2.hwnd_mon != NULL)
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_CHECKED, 0L);
	else
		SendMessage(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);

	return hr;
}

static double   coef_tg_size_w[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)][(uint32_t)(ENUM_AXIS::E_MAX)];
static double   coef_tg_size_h[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)][(uint32_t)(ENUM_AXIS::E_MAX)];

static IMAGE_DATA img_src;
static cv::Mat    img_roi; // 切抜き画像
static cv::Mat    img_hsv_roi;
static cv::Mat    img_hsv_bin_roi;
static cv::Mat    img_mask_roi[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)];
static cv::Mat    lut;

HRESULT CAuxPol::init_sway_sensor(){
	// 画像処理用画像データ初期化(起動直後のエラー回避用)
	cv::Mat init_img(2048, 1536, CV_8UC3, cv::Scalar(255, 0, 0));
	for (int idx = 0; idx < (int)(ENUM_IMAGE::E_MAX); idx++) {
		pSwaySharedObj->set_app_info_data(idx, init_img);
	}

	// 共有データ初期化

	//サンプリング周期
	gp_app_system->sample_cycle = (double)inf.cycle_ms * 0.001; //サンプリング周期[s]

	for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
		gp_app_imgprc->target_data[idx].valid = FALSE;							// 検出状態
		gp_app_imgprc->target_data[idx].max_val = 0.0;							// 最大輝度
		gp_app_imgprc->target_data[idx].size			= 0;					// 検出サイズ
		gp_app_imgprc->target_data[idx].roi.x			= 0;					// ROI:x coordinate of the top-left corner
		gp_app_imgprc->target_data[idx].roi.y			= 0;					// ROI:y coordinate of the top-left corner
		gp_app_imgprc->target_data[idx].roi.width		= 0;					// ROI:width of the rectangle
		gp_app_imgprc->target_data[idx].roi.height		= 0;					// ROI:height of the rectangle

		for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
			gp_app_imgprc->target_data[idx].pos[axis] = 0.0;					// 検出位置[pixel]
		}
	}

	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		gp_app_imgprc->sway_data[axis].p	= 0.0;	// 振れ角[pixel]
		gp_app_imgprc->sway_data[axis].til	= 0.0;	// ターゲット傾き[pixel]
		gp_app_imgprc->sway_data[axis].v	= 0.0;	// 振れ速度[pixel/s]
		gp_app_imgprc->sway_data[axis].a	= 0.0;	// 振れ加速度[pixel/s]
		gp_app_imgprc->sway_data[axis].p0	= 0.0;	// 振れ中心[pixel]
	}

	gp_app_imgprc->target_size = 0.0;                                   // ターゲットサイズ(ターゲット検出データの平均)
	gp_app_imgprc->status = (uint32_t)(ENUM_PROCCESS_STATUS::DEFAULT);  // 検出状態
	gp_app_imgprc->img_fps = 0.0;                                       // フレームレート
	gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;                   // 自動露光コマンド(0:停止 1:Up -1:Down)
	gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val;            // 露光時間[us]
	gp_app_imgprc->fc = SWAY_SENSOR_CUTOFF_HZ;							// 振れ検出カットオフ周波数

	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		gp_app_imgprc->sway_data[axis].p0 
			= (gp_cnfg_camera->basis.roi[axis].offset+ gp_cnfg_camera->basis.roi[axis].size) * 0.5;  //振れ検出ROIの中心[pixel]

		//一次遅れフィルタ係数
		gp_app_imgprc->sway_data[axis].tau = 1.0 / (PI360 * gp_app_imgprc->fc);//時定数
		gp_app_imgprc->sway_data[axis].alpha = gp_app_system->sample_cycle / (gp_app_imgprc->sway_data[axis].tau + gp_app_system->sample_cycle);
		gp_app_imgprc->sway_data[axis].g = GA;

		//速度,加速度計算用バッファ初期化
		while (gp_app_imgprc->sway_data[axis].p_history.size() < PRM_SWAY_P_HIST_SIZE) {
			gp_app_imgprc->sway_data[axis].p_history.push_back(0.0);
		}
		while (gp_app_imgprc->sway_data[axis].p_history.size() < PRM_SWAY_V_HIST_SIZE) {
			gp_app_imgprc->sway_data[axis].v_history.push_back(0.0);
		}
	}

	//----------------------------------------------------------------------------
	// メンバー変数の初期化
	// 輝度移動平均データ初期化
	PST_MOVE_AVE_DATA pmove_avrg_data = &m_move_avrg_data;
	ZeroMemory(pmove_avrg_data->data, sizeof(pmove_avrg_data->data)); // データ
	pmove_avrg_data->wptr		= 0;		// 書込みポインタ
	pmove_avrg_data->data_count = 0;		// データ数
	pmove_avrg_data->total_val	= 0;		// 輝度積算
	pmove_avrg_data->max_val	= 0.0;		// 最大輝度(移動平均後)
			
	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		// 振れ中心計測データ
		m_sway_zero_data.sway_min[axis]		= (double)((gp_cnfg_camera->basis.roi[axis].offset+ gp_cnfg_camera->basis.roi[axis].size));            // 振れ角最小値
		m_sway_zero_data.sway_max[axis]		= 0.0;									// 振れ角最大値
		m_sway_zero_data.sway_zero[axis]	= gp_app_imgprc->sway_data[axis].p0;    // 振れゼロ点

		//　ROI　Margin	設定用係数 (振れ角30°のときのPIXEL振幅)
		//  角周波数を掛けて30°振幅（PIX)のスキャン変化最大値を評価する
		gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::X] = PI45 * gp_cnfg_common->PIXperRAD[(int)ENUM_AXIS::X] * gp_app_system->sample_cycle;
		gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::Y] = PI45 * gp_cnfg_common->PIXperRAD[(int)ENUM_AXIS::Y] * gp_app_system->sample_cycle;

		//振れ加速度の異常値排除用リミット値(振れ振幅45°ロープ長GAの時(ω=1.0)の角加速度振幅値
		st_sway_work.sway_acc_chk_limit[(int)ENUM_AXIS::X] = PI45 * gp_cnfg_common->PIXperRAD[(int)ENUM_AXIS::X] * 1.0 * 1.0;
		st_sway_work.sway_acc_chk_limit[(int)ENUM_AXIS::Y] = PI45 * gp_cnfg_common->PIXperRAD[(int)ENUM_AXIS::Y] * 1.0 * 1.0;

		//振れ速度0判定リミット値(振れ振幅0.1°ロープ長GA*の時(ω=1.0)の角加速度振幅値
		st_sway_work.sway_acc_peak_chk_limit[(int)ENUM_AXIS::X] = 0.0;
		st_sway_work.sway_acc_peak_chk_limit[(int)ENUM_AXIS::Y] = 0.0;

		for (int idx = 0; idx < (int)(ENUM_IMAGE_MASK::E_MAX); idx++) {
			// PIX単位ターゲットサイズ計算用係数　この値を距離で割るとターゲットのPIXELサイズ期待値が算出される
			// (size_real(mm)単位/1000 / 距離)rad   * pix/rad
			coef_tg_size_w[idx][axis] = (double)gp_app_imgprc->target_data[idx].size_real.width * gp_cnfg_common->PIXperRAD[axis];
			coef_tg_size_w[idx][axis] /= 1000.0;	//mm→m変換
			coef_tg_size_h[idx][axis] = (double)gp_app_imgprc->target_data[idx].size_real.height * gp_cnfg_common->PIXperRAD[axis];
			coef_tg_size_h[idx][axis] /= 1000.0;	//mm→m変換
		}
	}
	// LUT Table 初期化
	lut = cv::Mat(256, 1, CV_8UC3); // LUT:Look Up Table　縦に256画素、横に1画素の、縦に細長い3チャンネル（カラー）画像


	return S_OK;
}

HRESULT CAuxPol::routine_work(void* pObj) {
	if (inf.total_act % 20 == 0) {
		wos.str(L""); wos << inf.status << L":" << std::setfill(L'0') << std::setw(4) << inf.act_time;
		msg2host(wos.str());
	}

	input();
	parse();
	output();
	return S_OK;
}

int CAuxPol::input() {
	if (g_sway_sensor_enable) {

		std::lock_guard<std::mutex> lock(auxpol_mtx);//ロックガード

		//カメラ‐ターゲット間距離（クライアントからの情報）
		if (gp_app_adjust->host_source_mode == SWAY_CAL_BASE_SET_BY_HOST) {
				gp_app_adjust->target_distance = pCsInf->msg_client.body.tg_distance;
		}
		else if (gp_app_adjust->host_source_mode == SWAY_CAL_BASE_SET_BY_DEFAULT) {
			gp_app_adjust->target_distance = POL_PRM_TG_DIST_DEFAULT;
		}
		else {//SWAY_CAL_BASE_SET_BY_MANUAL;
			gp_app_adjust->target_distance;			//SCADAのモニタウィンドウのスライダーコントロールでセット
		}
		if (gp_app_adjust->target_distance < POL_PRM_TG_DIST_DEFAULT) gp_app_adjust->target_distance = POL_PRM_TG_DIST_DEFAULT;

		//振れ周期等
		for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
			if (gp_app_adjust->host_source_mode == SWAY_CAL_BASE_SET_BY_HOST) {
				gp_app_imgprc->sway_data[axis].T = pCsInf->msg_client.body.T[axis];
				gp_app_imgprc->sway_data[axis].w = pCsInf->msg_client.body.w[axis];
			}
			else if (gp_app_adjust->host_source_mode == SWAY_CAL_BASE_SET_BY_DEFAULT) {
				gp_app_imgprc->sway_data[axis].T = POL_PRM_T_DEFAULT;
				gp_app_imgprc->sway_data[axis].w = POL_PRM_W_DEFAULT;
			}
			else {//SWAY_CAL_BASE_SET_BY_MANUAL;
				gp_app_imgprc->sway_data[axis].w = sqrt(gp_app_imgprc->sway_data[axis].g/ gp_app_adjust->target_distance);
				gp_app_imgprc->sway_data[axis].T = PI360 / gp_app_imgprc->sway_data[axis].w;
			}
			gp_app_imgprc->sway_data[axis].w2 = gp_app_imgprc->sway_data[axis].w * gp_app_imgprc->sway_data[axis].w;

			//このタスクが振れ周期を見る時のカウント数
			st_sway_work.sway_T_task_count[axis] = (int)(gp_app_imgprc->sway_data[axis].T / ((double)inf.cycle_ms / 1000.0));
		}

		//ターゲット検出予定角度幅PIX（実寸法/ターゲットとの距離）
		for (int idx = 0; idx < (int)(ENUM_IMAGE_MASK::E_MAX); idx++) {
			gp_app_imgprc->target_data[idx].size_expected.width		= (int)(coef_tg_size_w[idx][(int)ENUM_AXIS::X] / gp_app_adjust->target_distance);
			gp_app_imgprc->target_data[idx].size_expected.height	= (int)(coef_tg_size_h[idx][(int)ENUM_AXIS::Y] / gp_app_adjust->target_distance);
		}
		
		// 画像取込み　g_img_src_work.data_mat g_img_src_work.data_bgr
		uint32_t img_valid = get_opencv_image();

		(img_valid & (uint32_t)(ENUM_IMAGE_STATUS::ENABLED)) ?	
			(gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) :				// 画像処理状態:画像データ有効
			(gp_app_imgprc->status &= (~(uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)));			// 画像処理状態:画像データ無効
		
		gp_app_imgprc->img_fps = g_img_src_work.fps;   // フレームレート[fps]
	}

	return S_OK;
}

static bool chk_flg = FALSE;
static int count_invalid_img = 0;

int CAuxPol::parse() {
	cv::Scalar mean_val;    //読込画素データの平均値
	uint32_t   width = 0;
	uint32_t   height = 0;
	uint32_t   mask_low[(uint32_t)(ENUM_HSV_MODEL::E_MAX)];
	uint32_t   mask_upp[(uint32_t)(ENUM_HSV_MODEL::E_MAX)];
	BOOL       ret = FALSE;
	std::vector<cv::Mat> planes;

	if (g_sway_sensor_enable) {
		std::lock_guard<std::mutex> lock(auxpol_mtx);//ロックガード


		if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) {

			count_invalid_img = 0;//画像異常カウントクリア

		//# 検出処理
			//ROI処理無効選択時
			if (!gp_cnfg_imgprc->roi.valid) {
				cv::cvtColor(g_img_src_work.data_mat, img_hsv_roi, cv::COLOR_BGR2HSV);//元mat画像を直接hsv画像に変換
			}
#pragma region マスク画像の生成 各チャンネルごとに2値化し3チャンネル全てのANDを取りマスク画像を作成する
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) { //Mask画像1(赤色ターゲット）,2(緑色ターゲット）を生成	

				PTARGET_DATA ptarget_data = &gp_app_imgprc->target_data[idx];// ターゲット検出データ

				if (!gp_cnfg_imgprc->mask[idx].valid) {		//マスクの validは、初期設定でセット　基本 true有効
					ptarget_data->max_val = 0.0; continue;  // マスク処理無効なので最大輝度　0でスルー
				}

				// ## ROIの範囲(長方形)を設定する (x, y, width, height)で指定
				if (gp_cnfg_imgprc->roi.valid) {			//roiの validは、初期設定でセット　基本 true有効処理有効
					if (ptarget_data->valid) {				//target_dataの validは、重心演算の正常完了でターゲット位置が求まっていたらセット
						// ROIの振れ角速度移動補償値計算
						if (gp_app_adjust->target_distance != 0.0) {//カメラーターゲット間距離の情報アリ→30°振れ角速度振幅(角周波数×振れ角PIXEL単位振幅）×2　
							ptarget_data->size_roi_spd_margin.width		= (int)(gp_app_imgprc->sway_data[(int)ENUM_AXIS::X].w * gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::X]) * 2;
							ptarget_data->size_roi_spd_margin.height	= (int)(gp_app_imgprc->sway_data[(int)ENUM_AXIS::Y].w * gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::Y]) * 2;
						}
					
						// 水平方向　ROIの範囲を画面からはみ出さないように開始位置決定
						{	//roiの幅は、ターゲットの期待サイズ＋振れ角速度補償値で決定する
							int32_t roi_size = (int32_t)(ptarget_data->size_expected.width) + ptarget_data->size_roi_spd_margin.width;
							if ((roi_size <= 0) || (roi_size > (int32_t)(g_img_src_work.width))) {
								roi_size = g_img_src_work.width;//サイズ計算値が範囲外の場合は、画面幅に設定
							};
							if (roi_size < SWAY_SENSOR_ROI_MIN_W) {
								roi_size = SWAY_SENSOR_ROI_MIN_W;//サイズ下限リミット
							};
							int32_t tmp_val = roi_size / 2; //roi sizeの半分
							if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) - tmp_val) <= 0) {
								ptarget_data->roi.x = 0;//ROIの開始位置が画面左端より左に行かないようにする
							}
							else if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) + tmp_val) > (int32_t)(g_img_src_work.width)) {
								ptarget_data->roi.x = g_img_src_work.width - roi_size;//ROIの開始位置が画面右端より右に行かないようにする
							}
							else {
								ptarget_data->roi.x = (int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) - tmp_val;
							}
							ptarget_data->roi.width = roi_size;
						}
						// 垂直方向　ROIの範囲を画面からはみ出さないように開始位置決定
						{//roiの高さは、ターゲットの期待サイズ＋振れ角速度補償値で決定する
							int32_t roi_size = (int32_t)(ptarget_data->size_expected.height) + ptarget_data->size_roi_spd_margin.height;
							if ((roi_size <= 0) || (roi_size > (int32_t)(g_img_src_work.height))) {
								roi_size = g_img_src_work.height;//サイズ計算値が範囲外の場合は、画面高さに設定
							};
							if (roi_size < SWAY_SENSOR_ROI_MIN_H) {
								roi_size = SWAY_SENSOR_ROI_MIN_H;//サイズ下限リミット
							};
							int32_t tmp_val = roi_size / 2;
							if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) - tmp_val) <= 0) {
								ptarget_data->roi.y = 0;//ROIの開始位置が画面上端より上に行かないようにする
							}
							else if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) + tmp_val) > (int32_t)(g_img_src_work.height)) {
								ptarget_data->roi.y = g_img_src_work.height - roi_size;//ROIの開始位置が画面下端より下に行かないようにする
							}
							else {
								ptarget_data->roi.y = (int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) - tmp_val;
							}
							ptarget_data->roi.height = roi_size;
						}
						ptarget_data->range_over_count = SWAY_SENSOR_RANGE_OVER_COUNT;
					}   // if (target_data->valid)
					else {//重心演算の未完了でターゲット位置が求まっていない場合は、ROIを画面全体に設定する
						ptarget_data->roi.x = 0; ptarget_data->roi.y = 0;
						ptarget_data->roi.width = g_img_src_work.width;	ptarget_data->roi.height = g_img_src_work.height;
					}   // if (target_data->valid) else
				} // ROI処理有効モード　if (gp_cnfg_imgprc->roi.valid)
				else {//ROI処理無効モードでROIを画面全体に設定する
					ptarget_data->roi.x = 0; ptarget_data->roi.y = 0;
					ptarget_data->roi.width = g_img_src_work.width;	ptarget_data->roi.height = g_img_src_work.height;
				}   // if (gp_cnfg_imgprc->roi.valid) else

				// ## HSV変換した部分画像を生成
				//　!! 部分画像とその元画像は共通の画像データを参照するため 部分画像に変更を加えると元画像も変更される。
				img_roi = g_img_src_work.data_mat(ptarget_data->roi);
				cv::cvtColor(img_roi, img_hsv_roi, cv::COLOR_BGR2HSV); // 画像色をBGR→HSVに変換画像をセット

				// ## 3チャンネルのLUT:Look Up Table 作成
					//gp_app_imgprc->mean_hsv = cv::mean(img_hsv_roi);        //各チャンネルの平均値　現在未使用
					//マスキングフィルタ値セット
				for (uint32_t i = 0; i < (uint32_t)(ENUM_HSV_MODEL::E_MAX); i++) {//閾値はSCAD画面のスライダで設定できる様にするのでここでセット
					if (i == (uint32_t)(ENUM_HSV_MODEL::V)) {// Vチャンネルは、調整するかも・・・しれない
						mask_low[i] = gp_cnfg_imgprc->mask[idx].hsv_l[i]; // HSVマスク判定値(下限)
						mask_upp[i] = gp_cnfg_imgprc->mask[idx].hsv_u[i]; // HSVマスク判定値(上限)
					}
					else {
						mask_low[i] = gp_cnfg_imgprc->mask[idx].hsv_l[i]; // HSVマスク判定値(下限)
						mask_upp[i] = gp_cnfg_imgprc->mask[idx].hsv_u[i]; // HSVマスク判定値(上限)
					}
				}
				// LTUテーブル作成　256の配列にそのインデックスの輝度が0か255を入れ２値化する
				for (uint32_t i = 0; i < 256; i++) {
					//上限値　下限値が個別設定になっているので上限設定<下限設定となっている時がある
					for (uint32_t k = 0; k < (uint32_t)(ENUM_HSV_MODEL::E_MAX); k++) {
						if (mask_low[k] <= mask_upp[k]) { //下限値<=上限値　
							((mask_low[k] <= i) && (i <= mask_upp[k])) ? lut.data[i * lut.step + k] = 255 : //⇒　下限値 <= i and i<=上限値で255(ON) : iが上下限値の間にある時
								lut.data[i * lut.step + k] = 0;    //⇒　iが上下限値の間に無い時
						}
						else {//下限値 >上限値 @下限値 >=上限値(Hの0付近用）) 
							((i <= mask_upp[k]) || (mask_low[k] <= i)) ? lut.data[i * lut.step + k] = 255 : //⇒　下限値 <= i or i <= 上限値で255
								lut.data[i * lut.step + k] = 0;
						}
					}
				}

				// ## チャンネルごとのLUT変換(各チャンネルごとに2値化処理) img_hsv_roiはroiサイズになっている
					//LUT:Look Up Table 入力画素値がlutのインデックスとなり、lutの値が出力画素値となる。
					// 256の配列にそのインデックスの輝度が0か255を入れるので、各チャンネルごとに2値化される
				cv::LUT(img_hsv_roi, lut, img_hsv_bin_roi);

				// ## マスク画像の作成
				cv::split(img_hsv_bin_roi, planes); // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する
				//HチャンネルとVチャンネルのANDを取り、さらにSチャンネルのANDを取ることで、H,S,V全ての条件を満たす画素のみが255となるマスク画像を作成する
				cv::bitwise_and(planes[(uint32_t)(ENUM_HSV_MODEL::H)], planes[(uint32_t)(ENUM_HSV_MODEL::V)], img_mask_roi[idx]);
				cv::bitwise_and(img_mask_roi[idx], planes[(uint32_t)(ENUM_HSV_MODEL::S)], img_mask_roi[idx]);

				// ## ROI内最大輝度抽出
				cv::split(img_hsv_roi, planes);// チャンネルごとに2値化前のHSV画像をそれぞれのチャンネルに分解する
				//void cv::minMaxLoc(
				//	cv::InputArray src,       // 入力行列（単一チャンネルのみ）
				//	double* minVal,           // 最小値を格納する変数へのポインタ（不要なら NULL または nullptr）
				//	double* maxVal = 0,       // 最大値を格納する変数へのポインタ（不要なら nullptr）
				//	cv::Point* minLoc = 0,    // 最小値の座標を格納する構造体へのポインタ（不要なら nullptr）
				//	cv::Point* maxLoc = 0,    // 最大値の座標を格納する構造体へのポインタ（不要なら nullptr）
				//	cv::InputArray mask = cv::noArray() // 探索範囲を限定するマスク画像（オプション）
				//);
				cv::minMaxLoc(planes[(uint32_t)(ENUM_HSV_MODEL::V)], NULL, &ptarget_data->max_val);// Vチャンネルの最大値を取り込む
			}   // for (UINT idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)
#pragma endregion MASK CREATE


#pragma region ノイズ除去

#pragma region ゴマ塩除去（Closing or 中央値 or GAUSS or MORPHOLOGY）
			switch (gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].type) {
				case (uint32_t)(ENUM_NOISE_FILTER1::MEDIAN) :
					//中央値フィルター::ガウスぼかし（cv::GaussianBlur）が画像全体をなめらかにぼかすのに対し、
					// メディアンフィルタは「エッジ（輪郭）をくっきり残したまま、ノイズだけを強力に除去する」という強い個性を持っています。
					// 特に、画像にポツポツと現れる「ごま塩ノイズ（Salt - and -pepper noise）」の除去に絶大な効果を発揮します。
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {//マスクの validは、初期設定でセット　基本 true有効

							//void cv::medianBlur(
							//	cv::InputArray src, // 入力画像（1, 3, 4チャンネル対応。型は CV_8U, CV_16U, CV_32F 等。※チャンネル数により制限あり）
							//	cv::OutputArray dst, // 出力画像（入力画像と同じサイズ・同じ型）
							//	int ksize            // カーネルサイズ（正の奇数。1より大きい奇数: 3, 5, 7, 9...）
							//);
							cv::medianBlur(img_mask_roi[idx], img_mask_roi[idx], gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);
						}
					}
				break;
				case (uint32_t)(ENUM_NOISE_FILTER1::OPENNING) :   // オープニング処理(縮小→拡大)
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {//マスクの validは、初期設定でセット　基本 true有効
	
							//# 収縮と膨張の組み合わせでオープニング処理を行う
							
							//void cv::erode(
							//	cv::InputArray src,       // 入力画像（通常は二値化画像）
							//	cv::OutputArray dst,      // 出力画像（サイズと型は同じ）
							//	cv::InputArray kernel,    // 構造要素（カーネル）
							//	cv::Point anchor = cv::Point(-1, -1), // アンカー位置（デフォルトは中心）
							//	int iterations = 1,       // 実行回数（繰り返すほど白が削れて細くなる）
							//	int borderType = cv::BORDER_CONSTANT,
							//	const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue()
							//);
							cv::erode(img_mask_roi[idx],// 収縮:白い領域を「削る（周囲を黒にする）」処理です。
								img_mask_roi[idx],
								cv::Mat(),			// カーネル（構造要素）を指定しない場合は、3x3の正方形のカーネルが使用されます。
								cv::Point(-1, -1),	// アンカー位置 デフォルト=中心
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val//実行回数（繰り返すほど白が削れて細くなる）
							);    // 収縮
							
							//void cv::dilate(
							//	cv::InputArray src,       // 入力画像（通常は二値化画像）
							//	cv::OutputArray dst,      // 出力画像（サイズと型は同じ）
							//	cv::InputArray kernel,    // 構造要素（カーネル）
							//	cv::Point anchor = cv::Point(-1, -1), // アンカー位置（デフォルトは中心）
							//	int iterations = 1,       // 実行回数（繰り返すほど白が太くなる）
							//	int borderType = cv::BORDER_CONSTANT,
							//	const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue()
							//);
							cv::dilate(img_mask_roi[idx],//膨張：白い領域を「太らせる（周囲を白にする）」処理です。収縮で削られた白い領域を元の大きさに戻すために使用されます。
								img_mask_roi[idx],
								cv::Mat(),// カーネル（構造要素）を指定しない場合は、3x3の正方形のカーネルが使用されます。
								cv::Point(-1, -1),// アンカー位置 デフォルト=中心
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val//実行回数（繰り返すほど白が太くなる）
							);   // 膨張
						}
					}
				break;
				case (uint32_t)(ENUM_NOISE_FILTER2::GAUSS) :// ガウスフィルタ
				{
					//画像にガウスぼかし（Gaussian Blur）を適用する関数です。
					//画像のノイズ除去や、輪郭抽出（エッジ検出）の事前処理として、画像処理の中で最も頻繁に使われる平滑化（ぼかし）手法の一つです。
					//void cv::GaussianBlur(
					//	cv::InputArray src,     // 入力画像（チャンネル数は任意、型は CV_8U, CV_16U, CV_16S, CV_32F, CV_64F）
					//	cv::OutputArray dst,    // 出力画像（入力画像と同じサイズ・同じ型）
					//	cv::Size ksize,         // カーネルサイズ（ぼかす範囲。幅と高さ。必ず正の奇数）
					//	double sigmaX,          // X方向の標準偏差（ぼかしの強さ）
					//	double sigmaY = 0,      // Y方向の標準偏差（デフォルト0ならsigmaXと同じになる）
					//	int borderType = cv::BORDER_DEFAULT // 画像の端の処理方法
					//);	
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						cv::GaussianBlur(img_mask_roi[idx], img_mask_roi[idx], cv::Size(5, 5), 0, 0);
					}
				}
				break;
				case (uint32_t)(ENUM_NOISE_FILTER2::MORPHOLOGY) :// モノフォロジー

					// 主に二値化（白黒）画像に対して使用され、「ノイズ（小さな点）の除去」「破線の結合」「穴埋め」「輪郭の抽出」など、
					// 形状の特徴を補正・強調する際に非常に強力な威力を発揮します。
					//void cv::morphologyEx(
					//	cv::InputArray src,       // 入力画像（チャンネル数は任意、通常は二値化画像）
					//	cv::OutputArray dst,      // 出力画像（入力画像と同じサイズ・同じ型）
					//	int op,                   // モルフォロジー演算の種類（後述）
					//	cv::InputArray kernel,    // 構造要素（カーネル）。cv::getStructuringElementで作成
					//	cv::Point anchor = cv::Point(-1, -1), // アンカー位置（デフォルトは中心）
					//	int iterations = 1,       // 実行回数（繰り返すほど効果が強まる）
					//	int borderType = cv::BORDER_CONSTANT, // 画像の端の処理方法
					//	const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue()
					//);
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						cv::morphologyEx(img_mask_roi[idx], img_mask_roi[idx], cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);
					}
				break;

				default:
					break;
			}
#pragma endregion NOISE_CUT_1
			//
#pragma region 穴埋め（Opening or 中央値 or GAUSS or MORPHOLOGY）
			switch (gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].type) {
				case (uint32_t)(ENUM_NOISE_FILTER2::CLOSING) :    // クロージング処理(拡大→縮小)
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {//マスクの validは、初期設定でセット　基本 true有効
							//# モルフォロジー変換 未使用

							// 主に二値化（白黒）画像に対して使用され、「ノイズ（小さな点）の除去」「破線の結合」「穴埋め」「輪郭の抽出」など、
							// 形状の特徴を補正・強調する際に非常に強力な威力を発揮します。
							//void cv::morphologyEx(
							//	cv::InputArray src,       // 入力画像（チャンネル数は任意、通常は二値化画像）
							//	cv::OutputArray dst,      // 出力画像（入力画像と同じサイズ・同じ型）
							//	int op,                   // モルフォロジー演算の種類（後述）
							//	cv::InputArray kernel,    // 構造要素（カーネル）。cv::getStructuringElementで作成
							//	cv::Point anchor = cv::Point(-1, -1), // アンカー位置（デフォルトは中心）
							//	int iterations = 1,       // 実行回数（繰り返すほど効果が強まる）
							//	int borderType = cv::BORDER_CONSTANT, // 画像の端の処理方法
							//	const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue()
							//);
							// cv::morphologyEx(img_mask_roi[idx],img_mask_roi[idx],MORPH_OPEN,cv::Mat(), cv::Point(-1,-1), m_imgprcparam.filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);


							cv::dilate(img_mask_roi[idx],
								img_mask_roi[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);   // 膨張
							cv::erode(img_mask_roi[idx],
								img_mask_roi[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);    // 収縮
						}
					}
				break;

				case (uint32_t)(ENUM_NOISE_FILTER2::MEDIAN) :     // 中央値フィルター

					//中央値フィルタのval(kernel size)が偶数だとmedianBlur()でクラッシュする
					if (gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val%2==0)
						gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val++;

					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {
							cv::medianBlur(img_mask_roi[idx],
								img_mask_roi[idx],
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);
						}
					}

				break;

				case (uint32_t)(ENUM_NOISE_FILTER2::GAUSS):    // ガウスフィルタ

					//画像にガウスぼかし（Gaussian Blur）を適用する関数です。
					//画像のノイズ除去や、輪郭抽出（エッジ検出）の事前処理として、画像処理の中で最も頻繁に使われる平滑化（ぼかし）手法の一つです。
					//void cv::GaussianBlur(
					//	cv::InputArray src,     // 入力画像（チャンネル数は任意、型は CV_8U, CV_16U, CV_16S, CV_32F, CV_64F）
					//	cv::OutputArray dst,    // 出力画像（入力画像と同じサイズ・同じ型）
					//	cv::Size ksize,         // カーネルサイズ（ぼかす範囲。幅と高さ。必ず正の奇数）
					//	double sigmaX,          // X方向の標準偏差（ぼかしの強さ）
					//	double sigmaY = 0,      // Y方向の標準偏差（デフォルト0ならsigmaXと同じになる）
					//	int borderType = cv::BORDER_DEFAULT // 画像の端の処理方法
					//);	
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
							cv::GaussianBlur(img_mask_roi[idx], img_mask_roi[idx], cv::Size(5, 5), 0, 0);
					}

				break;
				case (uint32_t)(ENUM_NOISE_FILTER2::MORPHOLOGY) :    // モノフォロジー

					// 主に二値化（白黒）画像に対して使用され、「ノイズ（小さな点）の除去」「破線の結合」「穴埋め」「輪郭の抽出」など、
					// 形状の特徴を補正・強調する際に非常に強力な威力を発揮します。
					//void cv::morphologyEx(
					//	cv::InputArray src,       // 入力画像（チャンネル数は任意、通常は二値化画像）
					//	cv::OutputArray dst,      // 出力画像（入力画像と同じサイズ・同じ型）
					//	int op,                   // モルフォロジー演算の種類（後述）
					//	cv::InputArray kernel,    // 構造要素（カーネル）。cv::getStructuringElementで作成
					//	cv::Point anchor = cv::Point(-1, -1), // アンカー位置（デフォルトは中心）
					//	int iterations = 1,       // 実行回数（繰り返すほど効果が強まる）
					//	int borderType = cv::BORDER_CONSTANT, // 画像の端の処理方法
					//	const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue()
					//);
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						cv::morphologyEx(img_mask_roi[idx], img_mask_roi[idx], cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);
					}
				break;

				default:
					break;
			}
#pragma endregion NOISE_CUT_2

#pragma endregion NOISE_CUT

#pragma region ターゲット検出
			double  pos_x, pos_y;
//			gp_app_imgprc->exps_ctrl_mode |= EXPOSURE_CONTROL_RESET_STEP;

			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
				PTARGET_DATA ptarget_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ
				if (gp_cnfg_imgprc->mask[idx].valid) {//マスク画像有効(iniファイル設定値）

					//## 輪郭抽出(一番外側の白の輪郭のみ(入れ子構造は見ない）を取得)

					//void cv::findContours(
					//	cv::InputArray image,               // 入力画像（8ビット・1チャンネルの二値化画像）
					//	cv::OutputArrayOfArrays contours,   // 検出された輪郭の保存先（std::vector<std::vector<cv::Point>>）
					//	cv::OutputArray hierarchy,          // 輪郭の階層構造（親子関係）の保存先（std::vector<cv::Vec4i>）
					//	int mode,                           // 輪郭抽出モード（後述）
					//	int method,                         // 輪郭の近似手法（後述）
					//	cv::Point offset = cv::Point()      // すべての輪郭点をシフトさせるオフセット（通常は省略）
					//);
					cv::findContours(
						img_mask_roi[idx], 
						gp_app_imgprc->contours[idx], 
						cv::RETR_EXTERNAL,	///最も外側の輪郭だけを抽出します
						cv::CHAIN_APPROX_NONE//輪郭のすべての点を愚直に保存します  CHAIN_APPROX_SIMPLE: 水平・垂直・斜めの直線を圧縮し、端点（頂点）だけを残します
					);

			//## 重心検出　!!ここでtarget_data->valid をセット
					if (gp_cnfg_imgprc->imgprc == (uint32_t)ENUM_COG::BOUND_RECT) {//proc_center_gravity2で処理 サイズチェック有
						pos_x = 0.0;pos_y = 0.0;
						ptarget_data->valid = proc_center_gravity2(
							gp_app_imgprc->contours[idx],	// 解析用入力データ
							&pos_x,							// 検出位置出力バッファ
							&pos_y,							// 検出位置出力バッファ
							&ptarget_data->size,			// 検出サイズ出力バッファ(面積)
							&ptarget_data->size_expected,	// 期待サイズ入力バッファ
							&ptarget_data->size_detected	// 期待サイズ入力バッファ
						); // 検出状態
					}
					else {
						ptarget_data->valid = false;
						//proc_center_gravity()は使わない
						//pos_x = 0.0;
						//pos_y = 0.0;
						//ptarget_data->valid = proc_center_gravity(
						//	gp_app_imgprc->contours[idx],
						//	&pos_x,
						//	&pos_y,
						//	&ptarget_data->size,// 検出サイズ出力バッファ(長辺)
						//	gp_cnfg_imgprc->imgprc
						//); // 検出状態
					}

					double pos_x_now = pos_x + (double)ptarget_data->roi.x;
					double pos_y_now = pos_y + (double)ptarget_data->roi.y;

					ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)] = pos_x_now;
					ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)] = pos_y_now;

					// 速度検出部でフィルタを掛けるので以下削除20260817
					
					//double dx = pos_x_now - ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)];
					//double dy = pos_y_now - ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)];
					//
					////size_roi_spd_marginは角速度単位なのでスキャンタイムを掛ける
					//double chk_limit_x = ptarget_data->size_roi_spd_margin.width	* inf.cycle_ms / 1000 ;
					//double chk_limit_y = ptarget_data->size_roi_spd_margin.height	* inf.cycle_ms / 1000;
					//
					//bool is_pos_jump = false;//ターゲット位置ジャンプ検出フラグ
					//if ((dx > chk_limit_x) || (-dx > chk_limit_x) || (dy > chk_limit_y) || (-dy > chk_limit_y))
					//	is_pos_jump = true;
					//
					//if (is_pos_jump) {//ターゲット位置ジャンプ検出時は、フィルタをかけずに現在の位置をそのままセットする
					//
					//}
					//else {//ジャンプ無しではフィルタをかける（後段の速度計算の為）
					//	ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)] =
					//			SWAY_SENSOR_LPF_K1 * ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]//前回値
					//		+	SWAY_SENSOR_LPF_K2 * pos_x_now;									
					//	ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)] =
					//			SWAY_SENSOR_LPF_K1 * ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]//前回値
					//		+	SWAY_SENSOR_LPF_K2 * pos_y_now; 
					//}

					//いずれかのターゲット検出でROI　MODEでシャッターコントロール有効、
					if (ptarget_data->valid) {
						gp_app_imgprc->exps_ctrl_mode	= EXPOSURE_CONTROL_ROI_MODE;
						ptarget_data->range_over_count	= SWAY_SENSOR_RANGE_OVER_COUNT_LIMIT;
					}
				}
				else {//if (gp_cnfg_imgprc->mask[idx].valid) {//マスク画像有効(iniファイル設定値）
					ptarget_data->valid = FALSE;  // 検出状態
					ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)] = 0.0;    // 検出位置X[pixel]
					ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)] = 0.0;    // 検出位置Y[pixel]
					ptarget_data->size = 0;
					ptarget_data->range_over_count = 0;
				}
			}   // for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)
#pragma endregion DETECT TARGET



		}// if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE))
		else {

			count_invalid_img++;//画像異常カウント
			//レンジオーバーフラグセット
			st_sway_work.sway_target_range_over[(int)ENUM_AXIS::X] = st_sway_work.sway_target_range_over[(int)ENUM_AXIS::Y] = L_ON;

			// 振れ1/4周期までは前回値保持,　それ以上でマスク画像　画像処理データをクリアする	
			if ((count_invalid_img > st_sway_work.sway_T_task_count[(int)ENUM_AXIS::X] / 4) || (count_invalid_img > st_sway_work.sway_T_task_count[(int)ENUM_AXIS::Y] / 4)) {

				for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
					PTARGET_DATA target_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ
					// 受信画像全体をROIとして設定する
					target_data->roi.x = 0;	target_data->roi.y = 0;
					target_data->roi.width = g_img_src_work.width;	target_data->roi.height = g_img_src_work.height;
					target_data->valid = FALSE;							// 検出状態
					target_data->max_val = 0.0;							// 最大輝度
					target_data->pos[(uint32_t)(ENUM_AXIS::X)] = 0.0;   // 検出位置X[pixel]
					target_data->pos[(uint32_t)(ENUM_AXIS::Y)] = 0.0;   // 検出位置Y[pixel]
					target_data->size = 0;      // 検出サイズ				
					if ((gp_cnfg_imgprc->roi.valid) && (g_img_src_work.data_mat.data != nullptr)) {//ROI有効モード
						g_img_src_work.data_mat.copyTo(img_roi);
						cv::cvtColor(img_roi, img_hsv_roi, cv::COLOR_BGR2HSV);// 画像色をBGR→HSVに変換
					}
					gp_app_imgprc->exps_ctrl_mode |= EXPOSURE_CONTROL_RESET_STEP;
					img_hsv_roi.copyTo(img_mask_roi[idx]);

				}   // for (UINT idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)
			}
		}   // if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) else
#pragma region 画像保存 共有データに生成画像を格納する
		// マスク画像1
		if (gp_cnfg_imgprc->mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].valid) {
			CSwayShared::set_app_info_data((uint32_t)(ENUM_IMAGE::MASK_1),img_mask_roi[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)]);
		}
		// マスク画像2
		if (gp_cnfg_imgprc->mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].valid) {
			CSwayShared::set_app_info_data((uint32_t)(ENUM_IMAGE::MASK_2),img_mask_roi[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)]);
		}
		// 処理画像
		CSwayShared::set_app_info_data((uint32_t)(ENUM_IMAGE::PROCESS), g_img_src_work.data_mat);
#pragma endregion PUT_IMAGE
	
#pragma region ターゲット検出位置セット
		//# 検出ターゲット位置(X,Y)セット
		bool is_target1_valid	= gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].valid;
		bool is_target2_valid	= gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].valid;
		bool is_target12_valid	= is_target1_valid * is_target2_valid;

		double pos_tg_x1 = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].pos[(uint32_t)(ENUM_AXIS::X)];
		double pos_tg_y1 = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].pos[(uint32_t)(ENUM_AXIS::Y)];
		double pos_tg_x2 = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].pos[(uint32_t)(ENUM_AXIS::X)];
		double pos_tg_y2 = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].pos[(uint32_t)(ENUM_AXIS::Y)];

		//レンジオーバーフラグクリア
		st_sway_work.sway_target_range_over[(int)ENUM_AXIS::X] = st_sway_work.sway_target_range_over[(int)ENUM_AXIS::Y] = L_OFF;

		if ((gp_app_adjust->mask_mode == SWAY_SENSOR_MASK_MODE_12) && (is_target12_valid)) {//ターゲット1,2共に有効
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].p	= (pos_tg_x1 + pos_tg_x2) * 0.5;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].til	= pos_tg_x1 - pos_tg_x2;
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].p	= (pos_tg_y1 + pos_tg_y2) * 0.5;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].til	= pos_tg_y1 - pos_tg_y2;

			gp_app_imgprc->target_size = (
				(double)gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].size
			  + (double)gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].size
				) * 0.5;											 // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE); // TARGET_ENABLEクリア
		}
		else if ((gp_app_adjust->mask_mode & SWAY_SENSOR_MASK_MODE_1) && (is_target1_valid)) {//ターゲット1のみ有効
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].p	= pos_tg_x1;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].til	= 0.0;
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].p	= pos_tg_y1;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].til	= 0.0;

			gp_app_imgprc->target_size = (double)gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].size;											 // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE); // TARGET_ENABLEクリア
		}
		else if ((gp_app_adjust->mask_mode & SWAY_SENSOR_MASK_MODE_2) && (is_target2_valid)) {//ターゲット2のみ有効
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].p	= pos_tg_x2;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].til	= 0.0;
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].p	= pos_tg_y2;  // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].til	= 0.0;

			gp_app_imgprc->target_size = (double)gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].size;											 // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE); // TARGET_ENABLEクリア
		}
		else {//マスク設定無効 IDLE
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].p = (double)(CAM1_SPEC_PIXEL_H_OFFSET);   // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].p = (double)(CAM1_SPEC_PIXEL_V_OFFSET);   // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::X)].til = 0.0;   // ターゲット傾き[pixel]
			gp_app_imgprc->sway_data[(uint32_t)(ENUM_AXIS::Y)].til = 0.0;   // ターゲット傾き[pixel]

			gp_app_imgprc->target_size = 0.0;											 // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status &= (~(uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE)); // TARGET_ENABLEクリア

			st_sway_work.sway_target_range_over[(int)ENUM_AXIS::X] = st_sway_work.sway_target_range_over[(int)ENUM_AXIS::Y] = L_ON;
		}

#pragma endregion SET TARGET POS

		// 制御PCとのIF CHECK　MODE デバッグ用　検出処理なしで強制的に振れデータをセット
		if (maintenance_mode == CODE_POL_MAINTE_DBG_OVERRIDE)   proc_dbg_override();

#pragma region 振れ検出処理
		for (int i = 0; i < (int)ENUM_AXIS::E_MAX; i++) {
			proc_sway(i); 
		}
		
#pragma endregion SWAY_PROC

#pragma region シャッタコントロール
		set_expstime();
#pragma endregion EXPOSURE_CONTROL

	}//if (g_sway_sensor_enable)
	return S_OK;
}
int CAuxPol::output() {          //出力処理
	return S_OK;
}
int CAuxPol::close() {
	return 0;
}

/// <summary>
/// 作業用の画像バッファに画像を読み込む(g_img_src_work)
/// 設定により保存済 bmpファイルかカメラ映像を選択取り込み
/// </summary>
/// <scenario>
/// 画像ファイル選択時：
/// 　　指定したファイルを直接matに読み込む　g_img_src_work.data_mat
/// カメラ画像時:
/// 　　カメラから画像を取得し、g_img_src_work.data_bgrに格納し、OpenCVのMatに変換してg_img_src_work.data_matに格納する
/// </scenario>
/// <param name=""></param>
/// <returns></returns>
uint32_t CAuxPol::get_opencv_image(void)
{
	//----------------------------------------------------------------------------
	// 画像データ取得(画像ファイル)
	if (gp_cnfg_common->img_source_camera != (uint32_t)ENUM_GRAB_IMAGE::GRAB_CAMERA) {	//画像ファイル読込
		g_img_src_work.data_mat = cv::imread(CStrHelper::conv_string(gp_cnfg_common->img_source_fname));
		if (g_img_src_work.data_mat.data != NULL) {
			g_img_src_work.status |= (uint32_t)ENUM_IMAGE_STATUS::ENABLED;					// 画像ステータス:画像有効
			g_img_src_work.width = g_img_src_work.data_mat.cols;							// 画像サイズ(水平画素) [pixel]
			g_img_src_work.height = g_img_src_work.data_mat.rows;							// 画像サイズ(垂直画素) [pixel]
			g_img_src_work.fps = gp_cnfg_camera->basis.framerate;							// 画像フレームレート[fps]
		}
		else {
			g_img_src_work.status &= (~(uint32_t)ENUM_IMAGE_STATUS::ENABLED);				// 画像ステータス:画像有効
			g_img_src_work.width = gp_cnfg_camera->basis.roi[(uint32_t)ENUM_AXIS::X].size;  // 画像サイズ(水平画素) [pixel]
			g_img_src_work.height = gp_cnfg_camera->basis.roi[(uint32_t)ENUM_AXIS::Y].size;	// 画像サイズ(垂直画素) [pixel]
			g_img_src_work.fps = 0.0;                                                       // 画像フレームレート[fps]
		}
	}
	else {
		// 画像データ取得(カメラ)
		if (g_img_src_work.data_bgr != NULL) {//作業用画像バッファのポインタ有効
			if ((pCamera != NULL) &&(!(gp_app_system->status & (uint32_t)(ENUM_SYSTEM_STATUS::CAMERA_RESET_RUN)))) { // カメラ再接続中
				// 画像情報を取得
				if ((pCamera->get_image(g_img_src_work.data_bgr) >= 0) &&									// 画像データ取得 0:成功、-:失敗
					(pCamera->get_image_size(&g_img_src_work.width, &g_img_src_work.height) >= 0) &&		// 画像サイズ取得 0:成功、-:失敗
					(pCamera->get_image_fps(&g_img_src_work.fps) >= 0)) {
					g_img_src_work.status |= ((uint32_t)(ENUM_IMAGE_STATUS::ENABLED));            // 画像ステータス:画像有効
				}
				else {
					g_img_src_work.status &= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));						// 画像ステータス:画像有効
					g_img_src_work.width = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;			// 画像サイズ(水平画素) [pixel]
					g_img_src_work.height = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;		// 画像サイズ(垂直画素) [pixel]
					g_img_src_work.fps = 0.0;                                                                // 画像フレームレート [fps]
					ZeroMemory(g_img_src_work.data_bgr, (sizeof(uint8_t) * IMAGE_SIZE * IMAGE_FORMAT_SIZE)); // 画像データバッファのポインタ(BGR 24bit)
				}
			}
			else {
				g_img_src_work.status &= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));							// 画像ステータス:画像有効
				g_img_src_work.width = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;			// 画像サイズ(水平画素) [pixel]
				g_img_src_work.height = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;			// 画像サイズ(垂直画素) [pixel]
				g_img_src_work.fps = 0.0;                                                                   // 画像フレームレート [fps]
				ZeroMemory(g_img_src_work.data_bgr, (sizeof(uint8_t) * IMAGE_SIZE * IMAGE_FORMAT_SIZE));    // 画像データバッファのポインタ(BGR 24bit)
			}

			if (g_img_src_work.status & (uint32_t)ENUM_IMAGE_STATUS::ENABLED) {
				// OpenCV画像への変換
				g_img_src_work.data_mat = cv::Mat(
					g_img_src_work.height,
					g_img_src_work.width,
					CV_8UC3,
					g_img_src_work.data_bgr
				);    // 画像データ(OpenCV変換画像)    
			}
		}   // if (g_img_src_work.data_bgr != NULL)
		else {
			g_img_src_work.status	&= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));				// 画像ステータス:画像有効
			g_img_src_work.width		= gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;	// 画像サイズ(水平画素) [pixel]
			g_img_src_work.height	= gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;	// 画像サイズ(垂直画素) [pixel]
			g_img_src_work.fps		= 0.0;														// 画像フレームレート[fps]
		} 
	}
	return g_img_src_work.status;
}

#if 0
/// <summary>
/// 重心位置算出アルゴリズム(最大輪郭面積)
/// </summary>
/// <param name="contours"></param>
/// <param name="outPosX">重心位置X座標検出結果出力バッファ</param>
/// <param name="outPosY">重心位置Y座標検出結果出力バッファ</param>
/// <param name="outTgtSize">検出ターゲット面積出力バッファ</param>
/// <param name="sel">検出アルゴリズム選択</param>
/// <returns></returns>
BOOL CAuxPol::proc_center_gravity(std::vector<std::vector<cv::Point>> contours, double* outPosX, double* outPosY, int* outTgtSize, UINT sel)
{
	BOOL    ret = FALSE;
	double  pos_x = 0.0,pos_y = 0.0;
	int32_t target_size = 0;

	switch (sel) {
		case (uint32_t)(ENUM_COG::MAX_CONTOUR_AREA) : {//最大輪郭面積をターゲットとする
		BOOL     find = FALSE;
		size_t   count = 0,max_area_contour = 0;
		double   max_area = 0;
		cv::Rect roi;

		// 最大面積の輪郭を求める
		for (size_t i = 0; i < contours.size(); i++) {
			double area = contourArea(contours.at(i));
			if (max_area < area) {
				max_area = area;
				max_area_contour = i;
				find = TRUE;
			}
		}

		if (find) {
			count = contours.at(max_area_contour).size();//輪郭の要素数（点の数）を取得
			//輪郭要素の座標を合計して平均を求めることで重心位置を算出する
			for (size_t i = 0; i < count; i++) {
				pos_x += contours.at(max_area_contour).at(i).x;
				pos_y += contours.at(max_area_contour).at(i).y;
			}
			if (count > 0) {
				pos_x /= count;	pos_y /= count;
				roi = cv::boundingRect(contours[max_area_contour]);
				// ターゲットサイズは輪郭の外接矩形の長辺をターゲットサイズとする
				target_size = (roi.width > roi.height) ? (roi.width + 1) : (roi.height + 1);

				ret = TRUE;
			}
		}
	}
	break;

	case (uint32_t)(ENUM_COG::MAX_CONTOUR_LENGTH):
		//----------------------------------------------------------------------------
		// 重心位置算出アルゴリズム(最大輪郭長)
	{
		BOOL     find = FALSE;
		size_t   max_size = 0;
		size_t   max_id = 0;
		cv::Rect roi;
		if (contours.size() > 0) {
			for (size_t i = 0; i < contours.size(); i++) {
				{//輪郭の要素数（点の数）最大の輪郭を取得
				if (contours[i].size() > max_size) 
					max_size = contours[i].size();
					max_id = i;
					find = TRUE;
				}
			}
			if (find) {
				cv::Moments mu = moments(contours[max_id]);
				//•m00 は領域の面積（輝度合計）、重心は cx = m10/m00, cy = m01/m00 で計算できるため
				// 重心検出によく使われます（コード中の mu.m10 / mu.m00 と同じ）。
				if (mu.m00 > 0.0) {//重心位置算出
					pos_x = mu.m10 / mu.m00;
					pos_y = mu.m01 / mu.m00;
					roi = cv::boundingRect(contours[max_id]);
					target_size = (roi.width > roi.height) ? (roi.width + 1) : (roi.height + 1);

					ret = TRUE;
				}
			}
		}
	}
	break;

	default:
		break;

	}   // switch (sel)

	if (isnan(pos_x) || isnan(pos_y)) {
		pos_x = 0.0;
		pos_y = 0.0;

		ret = FALSE;
	}
	*outPosX = pos_x;
	*outPosY = pos_y;
	*outTgtSize = target_size;//検出エリアの長辺をターゲットサイズとする

	return ret;
}
#endif
/// <summary>
/// ターゲット検出(ターゲット検出データの中心)　輪郭の最大面積を検出（有効サイズチェック有）し、重心位置を算出する
/// </summary>
/// <param name="contours">		解析用入力データ		</param>
/// <param name="outPosX">		検出位置出力バッファ	</param>
/// <param name="outPosY">		検出位置出力バッファ	</param>
/// <param name="outTgtSize">	検出サイズ出力バッファ	</param>
/// <param name="size_expected">期待サイズ入力バッファ	</param>
/// <param name="size_detected">期待サイズ入力バッファ	</param>
/// <scenario>
/// 重心位置算出アルゴリズム(最大輪郭面積)
/// </scenario>
/// <returns></returns>
BOOL CAuxPol::proc_center_gravity2(std::vector<std::vector<cv::Point>> contours, double* outPosX, double* outPosY, int* outTgtSize, cv::Size2i* size_expected, cv::Size2i* size_detected)
{
	BOOL    ret = FALSE,find = FALSE;
	double  pos_x = 0.0, pos_y = 0.0,max_area = 0, size_chk_high, size_chk_low;
	int32_t target_size = 0;
	size_t   count = 0,max_area_contour,area_detected_max = 0;
	cv::Rect roi;

	//#　ターゲット判定閾値
	size_chk_high	= (double)(size_expected->width) * 4.0;//ロープ長から期待される検出サイズ上限　想定寸法4倍以下
   	size_chk_low	= (double)(size_expected->width) / 4.0;//ロープ長から期待される検出サイズ下限　想定寸法1/4倍以上

	for (size_t i = 0; i < contours.size(); i++) {
		//#　指定した輪郭（点列）から 面積（PIXELスカラー値）を返す
		double area = contourArea(contours.at(i));	
		//# 輪郭の長方形枠を求める
		roi = cv::boundingRect(contours[i]);		
		//# サイズ及び縦横比が許容値内で検出　面積最大のものを検出areaと判定
		if (
			(max_area < area) &&			// 面積最大
			(roi.height < size_chk_high) &&	// 縦サイズ上限以下 
			(roi.width < size_chk_high) &&  // 横サイズ上限以下
			(roi.height > size_chk_low) &&  // 縦サイズ下限以上 
			(roi.width > size_chk_low)      // 横サイズ下限以上
			) {//面積最大のものを検出
			max_area = area;
			max_area_contour = i;
			find = TRUE;
		}
	}
	//# 検出結果出力
	if (find) {
		roi = cv::boundingRect(contours[max_area_contour]);
	   //ターゲットサイズは面積で評価
		target_size = roi.width * roi.height;
		pos_x = double(roi.x + roi.width / 2);
		pos_y = double(roi.y + roi.height / 2);
		ret = TRUE;
	}

	if (isnan(pos_x) || isnan(pos_y)) {//isnan(非数)は検出されないようにする
		pos_x = 0.0;pos_y = 0.0;
		ret = FALSE;
	}

	//検出内容出力
	*outPosX = pos_x;
	*outPosY = pos_y;
	*outTgtSize = target_size;
	size_detected->width = roi.width;
	size_detected->height = roi.height;

	return ret;
}

/// <summary>
/// マスク処理で検出したターゲットの位置からターゲット位置と傾きを算出する
/// </summary>
/// <scenario>
/// ターゲット検出(ターゲット検出データの中心)
/// </scenario>
/// <param name=""></param>


void CAuxPol::proc_sway(int idx)
{
	//----------------------------------------------------------------------------
	// 振れ検出
	// !!! 振れpはAcos(ωt)をベースとする
	double dt = gp_app_system->sample_cycle;										// タスク実行周期[s]
	PSWAY_DATA psway_data = &gp_app_imgprc->sway_data[idx];							// 検出結果出力用構造体設定

	//一次遅れフィルタの遅れ理論値 -atan(f：振れ周波数/fc：カットオフ周波数)　
	double  expected_delay_ph = atan(psway_data->tau * psway_data->w);
	// double  expected_delay_ph = (psway_data->tau + gp_app_system->sample_cycle) * psway_data->w;

	size_t np = psway_data->p_history.size();
	size_t nv = psway_data->v_history.size();

	//振れ角履歴バッファ更新　速度演算用　
	psway_data->p_history.push_back(psway_data->p);		// 振れ角今回値は前段のターゲット検出部で導出済のものをセット
	if (psway_data->p_history.size() > PRM_SWAY_P_HIST_SIZE) {
		psway_data->p_history.erase(psway_data->p_history.begin());
	};

	//	if ((gp_app_imgprc->status & (uint32_t)ENUM_PROCCESS_STATUS::TARGET_ENABLE) && (psway_data->w > 0.0)){
	if (psway_data->w > 0.0){//振れ周期値有
		//振れ速度現在値(フィルタ適用前）
		double speed_now = (psway_data->p_history[np - 1] - psway_data->p_history[np - 2]) / dt;

		//振れ加速度現在値(フィルタ適用前）
		//double speed_last = (psway_data->p_history[np - 2] - psway_data->p_history[np - 3]) / dt;
		//2スキャンで評価してみる
		double acc_now = (speed_now - psway_data->v_history[nv - 2]) / (dt*2);

		//フィルタ処理
		double chk_limit = st_sway_work.sway_acc_chk_limit[idx];
		//加速度評価値が閾値内でフィルタ処理　範囲外は前回値保持

		//一時遅れフィルタの基本式　output = α×input + (1-α）× previous_output
		//サンプリング周期T=40msec　カットオフ周波数fc=2Hz →　時定数τ=1/2πfc≒0.08 α=T/(τ+T) = 0.33
		psway_data->v = psway_data->alpha * speed_now + (1 - psway_data->alpha) * psway_data->v; // フィルタ有
		psway_data->a = psway_data->alpha * acc_now + (1 - psway_data->alpha) * psway_data->a;	// フィルタ有
		psway_data->vw = psway_data->v / psway_data->w;						// v/w
		psway_data->aw2 = psway_data->a / (psway_data->w * psway_data->w);	// a/w/w

		//レンジオーバーで無い時
		if (st_sway_work.sway_target_range_over[idx] == L_OFF) {
			psway_data->amp_cal = sqrt(psway_data->vw * psway_data->vw + psway_data->aw2 * psway_data->aw2);
			psway_data->ph_cal = atan2(psway_data->v * psway_data->w, psway_data->a);
			//位相の検出遅れ分を補正(実物は遅れ分位相が進んでいる）
			psway_data->ph_cal += expected_delay_ph;
			if (psway_data->ph_cal > PI180) psway_data->ph_cal -= PI360;//位相は±πで表現する
		}

		// p2pロジックで振れゼロ点,振幅,位相を求める
		psway_data->amp_p2p = (st_sway_work.sway_peak_f[idx] - st_sway_work.sway_peak_r[idx]) / 2.0;	//p2pの値から振幅を求める
		psway_data->p0 = psway_data->amp_p2p + st_sway_work.sway_peak_r[idx];				//0点

		psway_data->ph_time -= gp_app_system->sample_cycle * psway_data->w;		//時間ベース位相を前回値からサンプルサイクル分進める

		if (psway_data->ph_time < -PI180) psway_data->ph_time += PI360;//位相は±πで表現する

		if ((psway_data->v_history[nv - 3] > 0.0) && (psway_data->v < 0.0)) {//振れ角速度符号変化(3scan前と比較）
			st_sway_work.sway_peak_f[idx] = psway_data->p;				//振れ加速度－で振れ＋peak
			psway_data->ph_time = 0.0 - expected_delay_ph;
			st_sway_work.peak_chk_flg[idx] = POL_CODE_P2P_WAIT_R_PEAK;	//リバース側待ちに切替
			st_sway_work.sway_peak_chk_count[idx] = 0;
		}
		else if ((psway_data->v_history[nv - 3] < 0.0) && (psway_data->v > 0.0)) {
			st_sway_work.sway_peak_r[idx] = psway_data->p;				//振れ加速度+で振れ-peak
			psway_data->ph_time = PI180 - expected_delay_ph;
			st_sway_work.peak_chk_flg[idx] = POL_CODE_P2P_WAIT_F_PEAK;	//フォワード側待ちに切替
			st_sway_work.sway_peak_chk_count[idx] = 0;
		}
		else {
			st_sway_work.sway_peak_chk_count[idx]++;
			if (st_sway_work.sway_peak_chk_count[idx] > st_sway_work.sway_T_task_count[idx]) {
				st_sway_work.peak_chk_flg[idx] = POL_CODE_P2P_WAIT_STOP;
				st_sway_work.sway_peak_f[idx] = psway_data->p;
				st_sway_work.sway_peak_r[idx] = psway_data->p;
				psway_data->ph_time = 0.0;
			}

			if (st_sway_work.peak_chk_flg[idx] == POL_CODE_P2P_WAIT_R_PEAK) {
				if (st_sway_work.sway_peak_r[idx] > psway_data->p)
					st_sway_work.sway_peak_r[idx] = psway_data->p;
			}
			if (st_sway_work.peak_chk_flg[idx] == POL_CODE_P2P_WAIT_F_PEAK) {
				if (st_sway_work.sway_peak_f[idx] < psway_data->p)
					st_sway_work.sway_peak_f[idx] = psway_data->p;
			}
		}
	}
	else {
		psway_data->v = psway_data->a = psway_data->vw = psway_data->aw2 = 0.0;
	}

	//振れ角s速度履歴バッファ更新　加速度,振幅,位相演算用　
	psway_data->v_history.push_back(psway_data->v);		// 振れ角今回値は前段のターゲット検出部で導出済のものをセット
	if (psway_data->v_history.size() > PRM_SWAY_V_HIST_SIZE) {
		psway_data->v_history.erase(psway_data->v_history.begin());
	};

	if (disp_mode == CODE_POL_MAINTE_DISP_P2P) {
		psway_data->amp_disp = psway_data->amp_p2p;
		psway_data->ph_disp = psway_data->ph_time;
	}
	else {
		psway_data->amp_disp = psway_data->amp_cal;
		psway_data->ph_disp = psway_data->ph_cal;
	}
	//----------------------------------------------------------------------------
	return;
}

#if 0
/// <summary>
/// 振れゼロ点設定処理(調整時に計測後固定タイプ（不採用）
/// </summary>
/// <scenario>
/// 
/// </scenario>
/// <param name="idx"></param>
/// <returns></returns>
double CAuxPol::get_sway_zero(uint32_t idx)
{
	PSWAY_DATA    sway_data = &gp_app_imgprc->sway_data[idx];   // 振れ検出データ(ターゲット検出データの中心)
	LARGE_INTEGER frequency;                                    // システムの周波数
	LARGE_INTEGER cur_count;                                    // 現在のカウント数
	LONGLONG      span_usec;                                    // 時間の間隔[usec]
	uint32_t      span_msec;                                    // 時間の間隔[msec]
	std::wostringstream  wosmsg;

	QueryPerformanceFrequency(&frequency);  // システムの周波数
	QueryPerformanceCounter(&cur_count);    // 現在のカウント数
	span_usec = ((cur_count.QuadPart - m_sway_zero_data.time_counter.QuadPart) * 1000000L) / frequency.QuadPart;    // 時間の間隔[usec]
	span_msec = (uint32_t)(span_usec / 1000);    // 時間の間隔[msec]

	//ゼロセット実行中フラグセット
	if (gp_app_client->command & (uint32_t)(SW_CLIENT_COM_CAMERA1_0SET)) { // クライアント0セットコマンドフラグON

		if (!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) {//MCC 0セット進行中　OFF

			gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN);     //MCC 0セット進行中フラグセット
			wosmsg.str(L""); wosmsg << L"<Information>Running sway p0 setting";
			msg2listview(wosmsg.str()); 
		}
	}

	if (((gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_RUN)) ||        //0セット進行中　ON
		(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) &&    //MCC 0セット進行中　ON
		!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED))) {  //0セット完了フラグ　ON

		if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE)) {       //ターゲット検出中　ON
			if (span_msec >= 0) {                               //計測継続時間 > 0
				if (span_msec < gp_cnfg_imgprc->swayzeroset_time) {   //計測設定時間経過していない
					if (sway_data->tg_pos < m_sway_zero_data.sway_min[idx]) { //最小値更新
						m_sway_zero_data.sway_min[idx] = sway_data->tg_pos;
					}
					if (m_sway_zero_data.sway_max[idx] < sway_data->tg_pos) { //最大値更新
						m_sway_zero_data.sway_max[idx] = sway_data->tg_pos;
					}
				}
				else {
					m_sway_zero_data.sway_zero[idx] = (m_sway_zero_data.sway_min[idx] + m_sway_zero_data.sway_max[idx]) * 0.5;
					gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED);  // 
					QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
					wosmsg.str(L""); wosmsg << L"<Information>Completed  sway p0 setting";
					msg2listview(wosmsg.str());
				}
			}
			else {
				QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
			}
		}
		else {//ターゲット検出中でない　計測用カウンターホールド値セット　0セット完了フラグ　ON
			gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED);   // 
			QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
			wosmsg.str(L""); wosmsg << L"<Error>Failed to sway p0 setting";
			msg2listview(wosmsg.str());
		}
	}
	else {  //0セット進行中でない　計測用カウンターホールド値セット
		QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
	}

	if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED)) {      //0セット完了状態

		gp_app_imgprc->status &= ~(uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_RUN);            //0セット進行中クリア

		if (!(gp_app_client->command & (uint32_t)(SW_CLIENT_COM_CAMERA1_0SET))) {            //カメラ1の0セットコマンドクリア済

			gp_app_imgprc->status &= ~(uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN);    //MCC 0セット進行中クリア
		}

		if (!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_RUN)) &&     //0セット進行中でない
			!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) { //0セット進行中でない

			gp_app_imgprc->status &= (~(uint32_t)(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED));//0セット完了状態クリア
		}
	}

	return m_sway_zero_data.sway_zero[idx];
}
#endif

static int sway0_counter[(uint32_t)(ENUM_AXIS::E_MAX)] = { 0,0 };

/// <summary>
/// 振れゼロ点設定処理(NEW LOGIC 0点　常時更新
/// (未使用：振れ速度符号変化を見る方式に変更
/// </summary>
/// <scenario>
/// 振れ周期間のPeak to Peakを常時計測して更新
/// </scenario>
/// <param name="idx"></param>
/// <returns></returns>
#if 0
double CAuxPol::get_sway_p2p(int idx)
{
	if (--sway0_counter[idx] < 0) {//0点更新処理

		sway0_counter[idx] = (int)(gp_app_imgprc->sway_data[idx].T / gp_app_system->sample_cycle);

		if (!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::TARGET_ENABLE))) {//ターゲット検出が無効の時
			//画面中心
			gp_app_imgprc->sway_data[idx].p0 = (double)(gp_cnfg_common->full_pix[idx] / 2);
		}
		else {
			//最大最小値の中点		
			gp_app_imgprc->sway_data[idx].p0 = (m_sway_zero_data.sway_min[idx] + m_sway_zero_data.sway_max[idx]) * 0.5;
		}
		//検出最大値,最小値リセット
		m_sway_zero_data.sway_min[idx] = gp_cnfg_common->full_pix[idx];
		m_sway_zero_data.sway_max[idx] = 0;
	}
	else {//検出最大値,最小値更新
		if (gp_app_imgprc->sway_data[idx].p < m_sway_zero_data.sway_min[idx]) { //最小値更新
			m_sway_zero_data.sway_min[idx] = gp_app_imgprc->sway_data[idx].p;
		}
		if (gp_app_imgprc->sway_data[idx].p > m_sway_zero_data.sway_max[idx]) { //最大値更新
			m_sway_zero_data.sway_max[idx] = gp_app_imgprc->sway_data[idx].p;
		}
	}
	return 0.0;
}
#endif
static bool was_over_expose		= false;		//輝度が上限設定値を超えたことがあるかどうかのフラグ
static bool was_under_expose	= false;	//輝度が下限設定値を超えたことがあるかどうかのフラグ
static double exps_time_upper_limit;
static double exps_time_lower_limit;

/// <summary>
/// シャッタコントロール
/// </summary>
/// <scenario>
/// 
/// </scenario>
static int exps_hold_count = 0;
void CAuxPol::set_expstime()
{
	if ((!gp_cnfg_common->img_source_camera)|| (gp_app_imgprc->exps_mode == EXPOSURE_CONTROL_MANUAL)){
		//# 画像入力がカメラでない（カメラ:1 画像データ:0）か　手動設定モード(SCADAのウィンドウで設定）でreturn　
			gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_RESET_STEP;
		return;
	}
	
	if (!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE))) {//% シャッターコントロール禁止判定 （画像入力異常でHOLD)
		gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;
		exps_hold_count++;
		if ((exps_hold_count > st_sway_work.sway_T_task_count[(int)ENUM_AXIS::X]*2 ) || (exps_hold_count > st_sway_work.sway_T_task_count[(int)ENUM_AXIS::Y]*2)) {
			//振れ2周期以上ホールド継続でリセットステップへ移行
			exps_hold_count = 0;
			gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val;		
			gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_RESET_STEP;
		}

		ZeroMemory(m_move_avrg_data.data, sizeof(m_move_avrg_data.data));//移動平均データリセット
		m_move_avrg_data.wptr = 0;m_move_avrg_data.data_count = 0;m_move_avrg_data.total_val = 0;m_move_avrg_data.max_val = 0.0f;
		was_over_expose = false;
		was_under_expose = false;
	}   // if (!(gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) || !(gp_cnfg_camera->expstime.auto_control))
	else {
		//# ２つのマスク処理後画像の最大輝度のうち大きい方を評価対象輝度にセット
		if (gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].max_val > gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].max_val) {
			gp_app_imgprc->exps_chk_brightness = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].max_val;
		}
		else {
			gp_app_imgprc->exps_chk_brightness = gp_app_imgprc->target_data[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].max_val;
		}

		//輝度が閾値範囲外の時シャッタコントロール実施
		if ((gp_app_imgprc->exps_chk_brightness < gp_cnfg_camera->expstime.auto_start_l) ||
			(gp_app_imgprc->exps_chk_brightness > gp_cnfg_camera->expstime.auto_start_h)) {
			gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_AUTO;
		}
		else return;
		
		if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_ROI_MODE) {
			//一旦ターゲットを検出したら、最大輝度が下がらない限り露光時間キープ
			if (gp_app_imgprc->exps_chk_brightness <= gp_cnfg_camera->expstime.auto_start_l) { // 最大輝度が閾値以下の場合、露光時間を増やす
				// 輝度に比例して露光時間を増やす 
				gp_app_imgprc->exps_time += 1.0 * (255.0 - gp_app_imgprc->exps_chk_brightness);
				was_under_expose = true;
			}
			else if ((gp_app_imgprc->exps_chk_brightness >= gp_cnfg_camera->expstime.auto_start_h) && (was_under_expose == false)) {// 最大輝度が閾値以上の場合、露光時間を減らす
				// 輝度に比例して露光時間を減らす 
				gp_app_imgprc->exps_time -= gp_app_imgprc->exps_chk_brightness * 0.1;
				was_over_expose = true;
				was_under_expose = false;
			}
			else {//最大輝度が下限閾値以上かつ上限閾値以下の場合、露光時間をキープ
				was_over_expose = false;
				//was_under_expose = true;//露光時間を減らす方向は下限閾値まで落ち込まない限りキープ;
				was_under_expose = false;//露光時間を減らす方向は下限閾値まで落ち込まない限りキープ;
			}
		}
		else if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_HOLD) {
			//レンジオーバー用　露光時間キープ
			//gp_app_imgprc->exps_time = gp_app_imgprc->exps_time;
		}
		else {//初期化ステップ
			if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_RESET_STEP) {//リセット0ステップ
				gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val;
				//初期化ステップでの上下限値セット
				exps_time_lower_limit = gp_cnfg_camera->expstime.val_min;
				exps_time_upper_limit = gp_cnfg_camera->expstime.val_max;
				was_over_expose = false;
				gp_app_imgprc->exps_step_count = EXPOSURE_CONTROL_STEP_COUNT;//待機待ち用カウントセット
			}
			// exps_ctrl_modeは、コードRESET_STEP 0100 →　RESET_STEP_FIN　011Fまで32ステップまで実施
			// 偶数ステップは露光時間を変更する実処理ステップ、
			// 奇数ステップは待機ステップで exps_step_countは待機ステップのカウントダウン用
			// シャッター速度を変更した後、次のステップに進む前に待機することで、カメラの露光時間が安定するまでの時間を確保する
			//　メインルーチンからROIモードにセットされるまで継続
	
			int is_wait_step = gp_app_imgprc->exps_ctrl_mode & 0x00000001;//奇数ステップは待機
			if (is_wait_step) {//待機ステップ
				gp_app_imgprc->exps_step_count--;
				if (gp_app_imgprc->exps_step_count < 0) //待機ステップ完了　次ステップへ
					gp_app_imgprc->exps_ctrl_mode++;       
			}
			else {
				if (gp_app_imgprc->exps_chk_brightness >= 254.0) {//ハレーション
					//現在の露光時間を上限値とする
					exps_time_upper_limit = gp_app_imgprc->exps_time;

					if (was_under_expose) {
						gp_app_imgprc->exps_time -= (exps_time_upper_limit - exps_time_lower_limit) / 2;
					}
					else {
						gp_app_imgprc->exps_time -= (exps_time_upper_limit - exps_time_lower_limit) / 2;//レンジ範囲の半分のスパンで補正

						if (gp_app_imgprc->exps_time < exps_time_lower_limit)gp_app_imgprc->exps_time < exps_time_lower_limit;
					}
					was_over_expose = true;		//過剰露光フラグON
					was_under_expose = false;	//過少露光フラグOFF
				}
				else {
					exps_time_lower_limit = gp_app_imgprc->exps_time;//最大輝度以下なので現在値を下限値に設定
					if (gp_app_imgprc->exps_chk_brightness < 220.0) {
						if (was_over_expose) {//前の設定ステップは過剰露光からの補正だった
							gp_app_imgprc->exps_time = gp_app_imgprc->exps_time + (exps_time_upper_limit - exps_time_lower_limit) / 2;//レンジ範囲の半分のスパンで補正
						}
						else {//前の設定ステップは過剰露光でなかった
							if ((gp_app_imgprc->exps_time * 2) > exps_time_upper_limit) {
								gp_app_imgprc->exps_time = gp_app_imgprc->exps_time + (exps_time_upper_limit - exps_time_lower_limit) / 2;
							}
							else {
								gp_app_imgprc->exps_time = gp_app_imgprc->exps_time * 2;
							}
						}
					}
					was_over_expose = false;
					was_under_expose = true;
				}
				gp_app_imgprc->exps_ctrl_mode++;
				gp_app_imgprc->exps_step_count = EXPOSURE_CONTROL_STEP_COUNT;
			}
		}
		
		if (gp_app_imgprc->exps_ctrl_mode > EXPOSURE_CONTROL_RESET_STEP_FIN) {//リセットステップ完了で通常モード
			gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_ROI_MODE;
		}

		//上下限リミット処理
		if (gp_app_imgprc->exps_time > gp_cnfg_camera->expstime.val_max) gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_max;
		if (gp_app_imgprc->exps_time < gp_cnfg_camera->expstime.val_min) gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_min;

	}   // else

	return;
}


uint32_t pol_counter = 0;

static double ph_dbug[2] = { 0.0, PI90 };
static double p0_dbug[2] = { 1000.0, 500.0 };
static double amp_dbug[2] = { 100.0, 50.0 };
/// <summary>
/// デバッグ用　検出処理なしで強制的にターゲット検出位置をセット
/// </summary>
void CAuxPol::proc_dbg_override() {

	PSWAY_DATA psway_data;
	for (int i = 0; i < (int)ENUM_AXIS::E_MAX; i++) {
		psway_data = &gp_app_imgprc->sway_data[i];
		ph_dbug[i] += gp_app_system->sample_cycle * psway_data->w;
		gp_app_imgprc->sway_data[i].p = amp_dbug[i] * sin(ph_dbug[i]) + p0_dbug[i];
	} 
	return;
}

/****************************************************************************/
/*   モニタウィンドウ									                    */
/****************************************************************************/
LRESULT CALLBACK CAuxPol::Mon1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon1.hctrl[POL_ID_MON1_STATIC_1] = CreateWindowW(TEXT("STATIC"), st_mon1.text[POL_ID_MON1_STATIC_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_mon1.pt[POL_ID_MON1_STATIC_1].x, st_mon1.pt[POL_ID_MON1_STATIC_1].y,
			st_mon1.sz[POL_ID_MON1_STATIC_1].cx, st_mon1.sz[POL_ID_MON1_STATIC_1].cy,
			hWnd, (HMENU)(POL_ID_MON1_CTRL_BASE + POL_ID_MON1_STATIC_1), hInst, NULL);

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

static wostringstream mon2wos;
LRESULT CALLBACK CAuxPol::Mon2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		//STATIC,LABEL
		for (int i = POL_ID_MON2_STATIC_MSG; i <= POL_ID_MON2_STATIC_MSG; i++) {
			st_mon2.hctrl[i] = CreateWindowW(TEXT("STATIC"), st_mon2.text[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_mon2.pt[i].x, st_mon2.pt[i].y, st_mon2.sz[i].cx, st_mon2.sz[i].cy,
				hWnd, (HMENU)(POL_ID_MON2_CTRL_BASE + i), hInst, NULL);
		}

		SetTimer(hWnd, POL_ID_MON2_TIMER, POL_PRM_MON2_TIMER_MS, NULL);

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wp);

		int _Id = wmId - POL_ID_MON2_CTRL_BASE;
		// 選択されたメニューの解析:
		switch (_Id)
		{

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
		KillTimer(hWnd, POL_ID_MON2_TIMER);
		st_mon2.hwnd_mon = NULL;
	}break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return S_OK;
}
HWND CAuxPol::open_monitor_wnd(HWND h_parent_wnd, int id) {

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

		st_mon1.hwnd_mon = CreateWindowW(TEXT("POL_MON1"), TEXT("POL_MON1"), WS_OVERLAPPEDWINDOW,
			POL_MON1_WND_X, POL_MON1_WND_Y, POL_MON1_WND_W, POL_MON1_WND_H,
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
		wcex.lpszMenuName = TEXT("POL_MON2");
		wcex.lpszClassName = TEXT("POL_MON2");
		wcex.hIconSm = NULL;

		ATOM fb = RegisterClassExW(&wcex);

		st_mon2.hwnd_mon = CreateWindowW(TEXT("POL_MON2"), TEXT("POL_MON2"), WS_OVERLAPPEDWINDOW,
			POL_MON2_WND_X, POL_MON2_WND_Y, POL_MON2_WND_W, POL_MON2_WND_H,
			h_parent_wnd, nullptr, hInst, nullptr);

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
void CAuxPol::close_monitor_wnd(int id) {
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
void CAuxPol::show_monitor_wnd(int id) {
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
void CAuxPol::hide_monitor_wnd(int id) {
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
LRESULT CALLBACK CAuxPol::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_USER_TASK_REQ: {
		if (HIWORD(wp) == WM_USER_WPH_OPEN_IF_WND) {
			if (lp == BC_ID_MON1) st_mon1.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
			if (lp == BC_ID_MON2) st_mon2.hwnd_mon = open_monitor_wnd(hDlg, (int)lp);
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
			case IDC_TASK_FUNC_RADIO1: {
				if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED) {
					maintenance_mode = CODE_POL_MAINTE_DBG_OVERRIDE;
					gp_app_adjust->host_source_mode = SWAY_CAL_BASE_SET_BY_MANUAL;

				}
				else {
					maintenance_mode = CODE_POL_MAINTE_OFF;
					gp_app_adjust->host_source_mode = SWAY_CAL_BASE_SET_BY_HOST;
				}
			}break;
			default:break;
			}
		}break;
		case IDC_TASK_ITEM_CHECK2: {
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1: {
				if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED) {
					disp_mode = CODE_POL_MAINTE_DISP_P2P;
				}
				else {
					disp_mode = CODE_POL_MAINTE_OFF;
				}
			}break;
			default:break;
			}
		}break;
		case IDC_TASK_ITEM_CHECK3:
		case IDC_TASK_ITEM_CHECK4:
		case IDC_TASK_ITEM_CHECK5:
		{
			if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED)
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = true;
			else
				inf.panel_act_chk[inf.panel_func_id - IDC_TASK_FUNC_RADIO1][LOWORD(wp) - IDC_TASK_ITEM_CHECK1] = false;
		}break;
		case IDC_TASK_ITEM_CHECK6:
		{
			switch (inf.panel_func_id) {
			case IDC_TASK_FUNC_RADIO1: {
				if (IsDlgButtonChecked(hDlg, LOWORD(wp)) == BST_CHECKED) {
					SendMessage(inf.hwnd_parent, WM_USER_AUX_DISP_CAMERA_CHK, WP_CODE_IMSHOW_SHOW, 0);
				}
				else {
					SendMessage(inf.hwnd_parent, WM_USER_AUX_DISP_CAMERA_CHK, WP_CODE_IMSHOW_CLOSE, 0);
				}
			}break;
			default:break;
			}

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
void CAuxPol::msg2listview(wstring wstr) {

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
void CAuxPol::set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6) {
	wstring wstr;
	wstr += std::to_wstring(p1); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT1), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p2); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT2), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p3); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT3), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p4); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT4), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p5); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT5), wstr.c_str()); wstr.clear();
	wstr += std::to_wstring(p6); SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_EDIT6), wstr.c_str());
}
//タブパネルのStaticテキストを設定
void CAuxPol::set_panel_tip_txt() {
	wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1:
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
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
void CAuxPol::set_func_pb_txt() {
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO1, L"Sway");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO2, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO3, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO4, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO5, L"-");
	SetDlgItemText(inf.hwnd_opepane, IDC_TASK_FUNC_RADIO6, L"-");
	return;
}
//タブパネルのItem chkテキストを設定
void CAuxPol::set_item_chk_txt() {
	wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"DbgMode";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), wstr.c_str());
		wstr = L"DispP2P";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK2), wstr.c_str());
		wstr = L"-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK3), wstr.c_str());
		wstr = L"-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK4), wstr.c_str());
		wstr = L"-";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK5), wstr.c_str());
		wstr = L"imshow";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK6), wstr.c_str());
	}
	case IDC_TASK_FUNC_RADIO2:
	case IDC_TASK_FUNC_RADIO3:
	case IDC_TASK_FUNC_RADIO4:
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

