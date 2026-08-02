#include "CAuxPol.h"
#include "CAuxEnv.h"
#include "resource.h"
#include "SmemAux.h"
#include "CCamera.h"
#include "SWYSENSOR_DEF.H"
#include "phisics.h"
#include "CHelper.h"
#include "CSwayShared.h"

extern std::vector<void*> VectpCTaskObj;    // TaskObjのポインタ
extern BC_TASK_ID         g_task_index;     // TaskObjのインデックス


//組み込み機能
extern int g_slbrk_enable;//旋回ブレーキ
extern int g_lanio_enable;//LANIO
extern int g_sway_sensor_enable;//振れセンサー
extern int g_gt_sensor_enable;//走行位置検出

// Swayセンサ関連
extern CTeliCamLib* pCamera;//GEカメラオブジェクトへのグローバルポインタ

// ***アプリケーション設定アクセスポインタ
extern PCONFIG_COMMON    gp_cnfg_common;        // 共通設定
extern PCONFIG_CAMERA    gp_cnfg_camera;        // カメラ設定
extern PCONFIG_MOUNTING  gp_cnfg_mounting;      // 取付寸法設定
extern PCONFIG_IMGPROC   gp_cnfg_imgprc;		// 画像処理条件設定

// ***アプリケーション情報アクセスポインタ  
extern PINFO_CLIENT_DATA gp_app_client;        // クライアント情報
extern PINFO_ADJUST_DATA gp_app_adjust;        // 調整情報
extern PINFO_IMGPRC_DATA gp_app_imgprc;        // 画像処理情報
extern PINFO_SYSTEM_DATA gp_app_system;        // システム情報

extern IMAGE_DATA g_img_src;

static CAuxEnv * pAuxEnvObj;				// CAuxEnvインスタンスのポインタ


int32_t CAuxPol::maintenance_mode = CODE_POL_MAINTE_OFF;

extern ST_DEVICE_CODE g_my_code;

ST_POL_MON1 CAuxPol::st_mon1;
ST_POL_MON2 CAuxPol::st_mon2;
ST_MOVE_AVE_DATA CAuxPol::m_move_avrg_data;	

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
	out_size = sizeof(ST_AUX_ENV_INF);
	if (OK_SHMEM != pEnvInfObj->create_smem(SMEM_AUX_ENV_INF_NAME, sizeof(ST_AUX_ENV_INF), MUTEX_AUX_ENV_INF_NAME)) {
		return(FALSE);
	}
	set_outbuf(pEnvInfObj->get_pMap());

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
	if (OK_SHMEM != pPolInfObj->create_smem(SMEM_AUX_POL_INF_NAME, sizeof(ST_AUX_POL_INF), MUTEX_AUX_POL_INF_NAME)) {
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

	//振れセンサ機能セットアップ
	if (g_sway_sensor_enable) {
		init_sway_sensor();

		g_img_src.data_mat = cv::imread("C:\/Work\/NonImg.bmp");
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

static double   coef_tg_size_w[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)];
static double   coef_tg_size_h[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)];

HRESULT CAuxPol::init_sway_sensor(){
	// 共有データ初期化
	for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
		gp_app_imgprc->target_data[idx].valid = FALSE;							// 検出状態
		gp_app_imgprc->target_data[idx].max_val = 0.0;							// 最大輝度
		for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
			gp_app_imgprc->target_data[idx].pos[axis] = 0.0;					// 検出位置[pixel]
		}
		gp_app_imgprc->target_data[idx].size			= 0;					// 検出サイズ
		gp_app_imgprc->target_data[idx].roi.x			= 0;					// ROI:x coordinate of the top-left corner
		gp_app_imgprc->target_data[idx].roi.y			= 0;					// ROI:y coordinate of the top-left corner
		gp_app_imgprc->target_data[idx].roi.width		= 0;					// ROI:width of the rectangle
		gp_app_imgprc->target_data[idx].roi.height		= 0;					// ROI:height of the rectangle

		//　検出ターゲット特徴評価計算用係数　ターゲット距離依存の検出PIXサイズ評価用
		// size_real:mm単位の実寸法(int)から1m距離での角度を求める→この値を距離で割ると視野上のターゲットの角度幅が求まる
		double size_w_rad1m = (double)(gp_app_imgprc->target_data[idx].size_real.width)	/ 1000.0;	//1m距離幅rad
		double size_h_rad1m = (double)(gp_app_imgprc->target_data[idx].size_real.height) / 1000.0;	//1m距離r高さrad
		double k = gp_cnfg_imgprc->PIXperRAD;//1RadあたりのPIX数

		//1mの距離でのPIX数(w(or h）/ 1.0m * k  評価時にはターゲット間距離で割る
		coef_tg_size_w[idx] = size_w_rad1m * k;//1m距離時の幅pix
		coef_tg_size_h[idx] = size_h_rad1m * k;//1m距離時の幅pix
	}

	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		gp_app_imgprc->sway_data[axis].target_pos = 0.0;   // ターゲット位置[pixel]
		gp_app_imgprc->sway_data[axis].target_tilt = 0.0;   // ターゲット傾き[pixel]
		gp_app_imgprc->sway_data[axis].sway_angle = 0.0;   // 振れ角[pixel]
		gp_app_imgprc->sway_data[axis].sway_speed = 0.0;   // 振れ速度[pixel/s]
		gp_app_imgprc->sway_data[axis].sway_zero = 0.0;   // 振れ中心[pixel]
	}
	gp_app_imgprc->target_size = 0.0;                                   // ターゲットサイズ(ターゲット検出データの平均)
	gp_app_imgprc->status = (uint32_t)(ENUM_PROCCESS_STATUS::DEFAULT);  // 検出状態
	gp_app_imgprc->img_fps = 0.0;                                       // フレームレート
	gp_app_imgprc->img_val = 0.0;                                       // 明度
	gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;                   // 自動露光コマンド(0:停止 1:Up -1:Down)
	gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val;            // 露光時間[us]
	
	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		gp_app_imgprc->sway_data[axis].sway_zero 
			= (gp_cnfg_camera->basis.roi[axis].offset+ gp_cnfg_camera->basis.roi[axis].size) * 0.5;  //振れ検出ROIの中心[pixel]
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

	// 振れ中心計測データ
	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		m_sway_zero_data.sway_min[axis] = static_cast<double>((gp_cnfg_camera->basis.roi[axis].offset
			+ gp_cnfg_camera->basis.roi[axis].size));            // 振れ角最小値
		m_sway_zero_data.sway_max[axis] = 0.0;                                         // 振れ角最大値
		m_sway_zero_data.sway_zero[axis] = gp_app_imgprc->sway_data[axis].sway_zero;    // 振れゼロ点
	}

	//サンプリング周期
	gp_app_system->sample_cycle = (double)inf.cycle_ms * 0.001; //サンプリング周期[s]


	//　ROI　Margin	設定用係数 (振れ角30°のときのPIXEL振幅)
	//  角周波数を掛けて30°振幅振れの振れ速度（PIX)振幅を評価する
	gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::X] = PI30 * gp_cnfg_common->pix1rad[(int)ENUM_AXIS::X];
	gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::Y] = PI30 * gp_cnfg_common->pix1rad[(int)ENUM_AXIS::Y];
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

static IMAGE_DATA img_src;
static cv::Mat    img_roi; // 切抜き画像
static cv::Mat    img_hsv;
static cv::Mat    img_hsv_bin;
static cv::Mat    img_mask[(uint32_t)(ENUM_IMAGE_MASK::E_MAX)];
static cv::Mat    lut;

int CAuxPol::input() {
	if (g_sway_sensor_enable) {
		//カメラ‐ターゲット間距離（クライアントからの情報）
		if (gp_app_adjust->target_distance_fixed)	gp_app_adjust->target_distance = 30.0;
		else 										gp_app_adjust->target_distance = pCsInf->msg_client.body.d[0];

		if (gp_app_adjust->target_distance != 0.0) {
			double w = sqrt(GA/gp_app_adjust->target_distance);
			gp_app_adjust->w[(int)ENUM_AXIS::X] = gp_app_adjust->w[(int)ENUM_AXIS::Y] = w;			//振れ角周波数
			gp_app_adjust->T[(int)ENUM_AXIS::X] = gp_app_adjust->T[(int)ENUM_AXIS::Y] = PI360/w;    //振れ周期
		}
		else {
			gp_app_adjust->target_distance != GA;
			gp_app_adjust->w[(int)ENUM_AXIS::X] = gp_app_adjust->w[(int)ENUM_AXIS::Y] = 1.0;		//振れ角周波数
			gp_app_adjust->T[(int)ENUM_AXIS::X] = gp_app_adjust->T[(int)ENUM_AXIS::Y] = 1.0;		//振れ周期
		}
		
		// 画像取込み
		uint32_t img_valid = get_opencv_image();

		(img_valid & (uint32_t)(ENUM_IMAGE_STATUS::ENABLED)) ?	
			(gp_app_imgprc->status |= (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) :   // 画像処理状態:画像データ有効
			(gp_app_imgprc->status &= (~(uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE))); // 画像処理状態:画像データ無効
		
		gp_app_imgprc->img_fps = g_img_src.fps;   // フレームレート[fps]
	}

	return S_OK;
}

static bool chk_flg = FALSE;

int CAuxPol::parse() {
	cv::Scalar mean_val;    //読込画素データの平均値
	uint32_t   width = 0;
	uint32_t   height = 0;
	uint32_t   mask_low[(uint32_t)(ENUM_HSV_MODEL::E_MAX)];
	uint32_t   mask_upp[(uint32_t)(ENUM_HSV_MODEL::E_MAX)];
	BOOL       ret = FALSE;
	std::vector<cv::Mat> planes;

	if (g_sway_sensor_enable) {
	// 検出処理
#pragma region PROCESS_TAGET
		if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) {
			//----------------------------------------------------------------------------
			// 画像色をBGR→HSVに変換
			if (!gp_cnfg_imgprc->roi.valid) {//ROI処理無効選択時
				cv::cvtColor(g_img_src.data_mat, img_hsv, cv::COLOR_BGR2HSV);
			}

			//----------------------------------------------------------------------------
			// 各チャンネルごとに2値化(LUT変換)し、3チャンネル全てのANDを取り、マスク画像を作成する
#pragma region CREATE_MASK_IMAGE

			lut = cv::Mat(256, 1, CV_8UC3); // LUT:Look Up Table
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {            //Mask画像1,2
				PTARGET_DATA ptarget_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ

				//マスクのvalidは、iniファイルで選択されていたらtrue
				if (!gp_cnfg_imgprc->mask[idx].valid) {
					ptarget_data->max_val = 0.0; // 最大輝度
					continue;
				}

				// ROIの範囲(長方形)を設定する
				if (gp_cnfg_imgprc->roi.valid) {//roi処理有効
					// * (x, y, width, height)で指定
					if (ptarget_data->valid) {
						// ROIの振れ角速度移動補償値計算
						if (gp_app_adjust->target_distance != 0.0) {//カメラーターゲット間距離　!=0
							//30°振れ角度振幅x2
							ptarget_data->size_roi_spd_margin.width = (int)(gp_app_adjust->w[(int)ENUM_AXIS::X]) * gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::X];
							ptarget_data->size_roi_spd_margin.height = (int)(gp_app_adjust->w[(int)ENUM_AXIS::Y]) * gp_app_adjust->coef_roi_margin[(int)ENUM_AXIS::Y];
						}
						else {
							ptarget_data->size_roi_spd_margin.width = ptarget_data->size_roi_spd_margin.height = SWAY_SENSOR_ROI_MIN_W;
						}

						// 水平方向　ROIの範囲を画面からはみ出さないように開始位置決定
						{
							int32_t roi_size = (int32_t)(ptarget_data->size_expected.width)
								+ ptarget_data->size_roi_spd_margin.width;
							if ((roi_size <= 0) || (roi_size > (int32_t)(g_img_src.width))) {
								roi_size = g_img_src.width;
							}
							else if (roi_size < SWAY_SENSOR_ROI_MIN_W) {
								//サイズ下限リミット
								roi_size = SWAY_SENSOR_ROI_MIN_W;
							}
							else;

							int32_t tmp_val = (int32_t)((double)roi_size / 2.0 + 0.5); //roi sizeの半分

							if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) - tmp_val) <= 0) {
								ptarget_data->roi.x = 0;
							}
							else if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) + tmp_val) > (int32_t)(g_img_src.width)) {
								ptarget_data->roi.x = g_img_src.width - roi_size;
							}
							else {
								ptarget_data->roi.x = (int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::X)]) - tmp_val;
							}
							ptarget_data->roi.width = roi_size;
						}

						// 垂直方向　ROIの範囲を画面からはみ出さないように開始位置決定
						{
							//int32_t roi_size = (int32_t)(static_cast<double>(target_data->size) * gp_cnfg_imgprc->roi.scale)
							int32_t roi_size = (int32_t)(ptarget_data->size_expected.height)
								+ ptarget_data->size_roi_spd_margin.height;
							if ((roi_size <= 0) || (roi_size > (int32_t)(g_img_src.height))) {
								roi_size = g_img_src.height;
							}
							else if (roi_size < SWAY_SENSOR_ROI_MIN_H) {
								//サイズ下限リミット
								roi_size = SWAY_SENSOR_ROI_MIN_H;
							}
							else;
#if 1
							int32_t tmp_val = (int32_t)((static_cast<double>(roi_size) / 2.0) + 0.5);
#else
							int32_t tmp_val = roi_size;
#endif
							if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) - tmp_val) <= 0) {
								ptarget_data->roi.y = 0;
							}
							else if (((int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) + tmp_val) > (int32_t)(g_img_src.height)) {
								ptarget_data->roi.y = g_img_src.height - roi_size;
							}
							else {
								ptarget_data->roi.y = (int32_t)(ptarget_data->pos[(uint32_t)(ENUM_AXIS::Y)]) - tmp_val;
							}
							ptarget_data->roi.height = roi_size;
						}
						ptarget_data->range_over_count = SWAY_SENSOR_RANGE_OVER_COUNT;
					}   // if (target_data->valid)
#if 0
					else if (target_data->range_over_count > 0) {//レンジオーバーでROI保持
						target_data->range_over_count--;
						if (target_data->range_over_count <= 0)target_data->range_over_count = 0;
						gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_ROI_KEEP;
					}
#endif
					else {
						ptarget_data->roi.x = 0;
						ptarget_data->roi.y = 0;
						ptarget_data->roi.width = g_img_src.width;
						ptarget_data->roi.height = g_img_src.height;
					}   // if (target_data->valid) else

					// 部分画像を生成
					// * 部分画像とその元画像は共通の画像データを参照するため、
					//   部分画像に変更を加えると、元画像も変更される。
					img_roi = g_img_src.data_mat(ptarget_data->roi);

					// ####### 画像色をBGR→HSVに変換 ###########
					cv::cvtColor(img_roi, img_hsv, cv::COLOR_BGR2HSV);
					gp_app_imgprc->mean_hsv = cv::mean(img_hsv);        //各チャンネルの平均値　現在未使用

				} // ROI処理有効モード　if (gp_cnfg_imgprc->roi.valid > 0)
				else {
					ptarget_data->roi.x = 0;
					ptarget_data->roi.y = 0;
					ptarget_data->roi.width = g_img_src.width;
					ptarget_data->roi.height = g_img_src.height;
				}   // if (gp_cnfg_imgprc->roi.valid > 0) else

				//ターゲット検出予定角度幅（実寸法/ターゲットとの距離）
				ptarget_data->size_expected.width = static_cast<int>(coef_tg_size_w[idx] / gp_app_adjust->target_distance);
				ptarget_data->size_expected.height = static_cast<int>(coef_tg_size_h[idx] / gp_app_adjust->target_distance);

				// 3チャンネルのLUT:Look Up Table 作成
				for (uint32_t i = 0; i < (uint32_t)(ENUM_HSV_MODEL::E_MAX); i++) {
					if (i == (uint32_t)(ENUM_HSV_MODEL::V)) {
#if 0
						if (target_data->valid) {
							mask_low[i] = 0; // HSVマスク判定値(下限)
						}
						else {//ターゲット未検出時は全体輝度の平均以下はノイズとしてカット
							mask_low[i] = (uint32_t)(gp_app_imgprc->mean_hsv(i)); // HSVマスク判定値(下限)
						}
#endif
						mask_low[i] = gp_cnfg_imgprc->mask[idx].hsv_l[i]; // HSVマスク判定値(下限)

						mask_upp[i] = gp_cnfg_imgprc->mask[idx].hsv_u[i]; // HSVマスク判定値(上限)
					}
					else {
						mask_low[i] = gp_cnfg_imgprc->mask[idx].hsv_l[i]; // HSVマスク判定値(下限)
						mask_upp[i] = gp_cnfg_imgprc->mask[idx].hsv_u[i]; // HSVマスク判定値(上限)
					}
				}
				// LTUテーブル作成　256の配列にそのインデックスの輝度が0か255を入れる
				for (uint32_t i = 0; i < 256; i++) {
					//上限値　下限値が個別設定になっているので上限設定<下限設定となっている時がある
					for (uint32_t k = 0; k < (uint32_t)(ENUM_HSV_MODEL::E_MAX); k++) {
						if (mask_low[k] <= mask_upp[k]) {                                                   //下限値<=上限値　
							((mask_low[k] <= i) && (i <= mask_upp[k])) ? lut.data[i * lut.step + k] = 255 : //⇒　下限値 <= i and i<=上限値で255(ON)
								lut.data[i * lut.step + k] = 0;
						}
						else {                                                                              //下限値 >=上限値(Hの0付近用）) 
							((i <= mask_upp[k]) || (mask_low[k] <= i)) ? lut.data[i * lut.step + k] = 255 : //⇒　下限値 <= i or i <= 上限値で255
								lut.data[i * lut.step + k] = 0;
						}
					}
				}

				// チャンネルごとのLUT変換(各チャンネルごとに2値化処理)
				//img_hsvはroiサイズになっている
				cv::LUT(img_hsv, lut, img_hsv_bin);//LUT:Look Up Table

				// マスク画像の作成
				cv::split(img_hsv_bin, planes); // チャンネルごとに2値化された画像をそれぞれのチャンネルに分解する


				cv::bitwise_and(planes[(uint32_t)(ENUM_HSV_MODEL::H)], planes[(uint32_t)(ENUM_HSV_MODEL::V)], img_mask[idx]);
				cv::bitwise_and(img_mask[idx], planes[(uint32_t)(ENUM_HSV_MODEL::S)], img_mask[idx]);

				// 最大輝度抽出
				//img_hsvはroiサイズになっている
				cv::split(img_hsv, planes);
				// Vチャンネルの最大値を取り込む
				cv::minMaxLoc(planes[(uint32_t)(ENUM_HSV_MODEL::V)], NULL, &ptarget_data->max_val);

			}   // for (UINT idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)

#pragma endregion CREATE_MASK_IMAGE

		 //----------------------------------------------------------------------------
		// ノイズ除去
		// ガウスフィルタ 未使用
#if 0
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
				if (gp_cnfg_imgprc->mask[idx].valid) {
					cv::GaussianBlur(img_mask[idx], img_mask[idx], cv::Size(5, 5), 0, 0);
				}
			}
#endif       
			// ゴマ塩（Opening or 中央値）
#pragma region NOISE_CUT_1
			switch (gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].type) {
				case (uint32_t)(ENUM_NOISE_FILTER1::MEDIAN) :     // 中央値フィルター
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {
							cv::medianBlur(img_mask[idx],
								img_mask[idx],
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);
						}
					}
				break;
				case (uint32_t)(ENUM_NOISE_FILTER1::OPENNING) :   // オープニング処理(縮小→拡大)
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {
							//                  cv::morphologyEx(img_mask[idx],
							//                                   img_mask[idx],
							//                                   MORPH_OPEN,
							//                                   cv::Mat(),
							//                                   cv::Point(-1,-1),
							//                                   m_imgprcparam.filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);
							cv::erode(img_mask[idx],
								img_mask[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);    // 収縮
							cv::dilate(img_mask[idx],
								img_mask[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_1)].val);   // 膨張
						}

					}
				break;
				default:
					break;
			}
#pragma endregion NOISE_CUT_1
#if 1
			// 穴埋め（Opening or 中央値）
#pragma region NOISE_CUT_2
			switch (gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].type) {
				case (uint32_t)(ENUM_NOISE_FILTER2::CLOSING) :    // クロージング処理(拡大→縮小)
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {
							//                  cv::morphologyEx(img_mask[idx],
							//                                   img_mask[idx],
							//                                   MORPH_CLOSE,
							//                                   cv::Mat(),
							//                                   cv::Point(-1,-1),
							//                                   gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);
							cv::dilate(img_mask[idx],
								img_mask[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);   // 膨張
							cv::erode(img_mask[idx],
								img_mask[idx],
								cv::Mat(),
								cv::Point(-1, -1),
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);    // 収縮
						}
					}
				break;

				case (uint32_t)(ENUM_NOISE_FILTER2::MEDIAN) :     // 中央値フィルター
					for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
						if (gp_cnfg_imgprc->mask[idx].valid) {
							cv::medianBlur(img_mask[idx],
								img_mask[idx],
								gp_cnfg_imgprc->filter[(uint32_t)(ENUM_NOISE_FILTER::FILTER_2)].val);
						}
					}
				break;

				default:
					break;
			}
#endif
#pragma endregion NOISE_CUT_2
			//----------------------------------------------------------------------------
			// 画像処理
#if 1
#pragma region IMAGE_PROC
			double  pos_x, pos_y;
			gp_app_imgprc->exps_ctrl_mode |= EXPOSURE_CONTROL_RESET_STEP;
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
				PTARGET_DATA target_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ
				if (gp_cnfg_imgprc->mask[idx].valid) {//マスク画像有効(iniファイル設定値）
					// 輪郭抽出(一番外側の白の輪郭のみを取得)
					cv::findContours(img_mask[idx], gp_app_imgprc->contours[idx], cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

					// 重心検出　!!ここでtarget_data->valid をセット
					if (gp_cnfg_imgprc->imgprc == (uint32_t)ENUM_COG::BOUND_RECT) {//proc_center_gravity2で処理
						pos_x = 0.0;
						pos_y = 0.0;
						target_data->valid = proc_center_gravity2(gp_app_imgprc->contours[idx],
							&pos_x,
							&pos_y,
							&target_data->size,
							&target_data->size_expected,
							&target_data->size_detected); // 検出状態
					}
					else {
						pos_x = 0.0;
						pos_y = 0.0;
						target_data->valid = proc_center_gravity(gp_app_imgprc->contours[idx],
							&pos_x,
							&pos_y,
							&target_data->size,
							gp_cnfg_imgprc->imgprc); // 検出状態
					}
#if 0//LPF無
					target_data->pos[(uint32_t)(ENUM_AXIS::X)] = pos_x + target_data->roi.x; // 検出位置X[pixel]
					target_data->pos[(uint32_t)(ENUM_AXIS::Y)] = pos_y + target_data->roi.y; // 検出位置Y[pixel]
#else//LPF有
					double temp_x = pos_x + (double)target_data->roi.x - target_data->pos[(uint32_t)(ENUM_AXIS::X)];
					double temp_y = pos_y + (double)target_data->roi.y - target_data->pos[(uint32_t)(ENUM_AXIS::Y)];
					double chk_limit_x = target_data->size_roi_spd_margin.width, chk_limit_y = target_data->size_roi_spd_margin.height;

					bool is_pos_jump = false;
					if ((temp_x > chk_limit_x) || (-temp_x > chk_limit_x) || (temp_y > chk_limit_y) || (-temp_y > chk_limit_y))is_pos_jump = true;

					//      if (is_pos_jump){
					if (is_pos_jump) {
						target_data->pos[(uint32_t)(ENUM_AXIS::X)] = (pos_x + (double)target_data->roi.x); // 検出位置X[pixel]
						target_data->pos[(uint32_t)(ENUM_AXIS::Y)] = (pos_y + (double)target_data->roi.y); // 検出位置X[pixel]
					}
					else {
						target_data->pos[(uint32_t)(ENUM_AXIS::X)] =
							SWAY_SENSOR_LPF_K1 * target_data->pos[(uint32_t)(ENUM_AXIS::X)]
							+ SWAY_SENSOR_LPF_K2 * (pos_x + target_data->roi.x); // 検出位置X[pixel]

						target_data->pos[(uint32_t)(ENUM_AXIS::Y)] =
							SWAY_SENSOR_LPF_K1 * target_data->pos[(uint32_t)(ENUM_AXIS::Y)]
							+ SWAY_SENSOR_LPF_K2 * (pos_y + target_data->roi.y); // 検出位置Y[pixel]
					}

					//いずれかのターゲット検出でROI　MODEでシャッターコントロール
					if (target_data->valid) {
						gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_ROI_MODE;
						target_data->range_over_count = SWAY_SENSOR__RANGE_OVER_COUNT_LIMIT;
					}

#endif
				}
				else {
					target_data->valid = FALSE;  // 検出状態
					target_data->pos[(uint32_t)(ENUM_AXIS::X)] = 0.0;    // 検出位置X[pixel]
					target_data->pos[(uint32_t)(ENUM_AXIS::Y)] = 0.0;    // 検出位置Y[pixel]
					target_data->size = 0;
					target_data->range_over_count = 0;
				}
			}   // for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)

#pragma endregion IMAGE_PROC
#endif
		}   // if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE))
		else {
			//----------------------------------------------------------------------------
			// マスク画像を作成する
#pragma region CREATE_MASK_IMAGE
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
				PTARGET_DATA target_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ
				// ROIの範囲(長方形)を設定する
				if (gp_cnfg_imgprc->roi.valid) {
					// * (x, y, width, height)で指定
					target_data->roi.x = 0;
					target_data->roi.y = 0;
					target_data->roi.width = g_img_src.width;
					target_data->roi.height = g_img_src.height;

					// 部分画像を生成
					// * 部分画像とその元画像は共通の画像データを参照するため、
					//   部分画像に変更を加えると、元画像も変更される。
					if (g_img_src.data_mat.data != nullptr) {
						img_roi = g_img_src.data_mat(target_data->roi);
						// 画像色をBGR→HSVに変換
						cv::cvtColor(img_roi, img_hsv, cv::COLOR_BGR2HSV);
					}
				}
				else {
					target_data->roi.x = 0;
					target_data->roi.y = 0;
					target_data->roi.width = g_img_src.width;
					target_data->roi.height = g_img_src.height;
				}

				gp_app_imgprc->exps_ctrl_mode |= EXPOSURE_CONTROL_RESET_STEP;

				img_hsv.copyTo(img_mask[idx]);
			}   // for (UINT idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++)
#pragma endregion CREATE_MASK_IMAGE

		//----------------------------------------------------------------------------
		// 画像処理
#pragma region IMAGE_PROC
			for (uint32_t idx = 0; idx < (uint32_t)(ENUM_IMAGE_MASK::E_MAX); idx++) {
				PTARGET_DATA target_data = &gp_app_imgprc->target_data[idx];    // ターゲット検出データ
				target_data->valid = FALSE;  // 検出状態
				target_data->max_val = 0.0;    // 最大輝度
				target_data->pos[(uint32_t)(ENUM_AXIS::X)] = 0.0;    // 検出位置X[pixel]
				target_data->pos[(uint32_t)(ENUM_AXIS::Y)] = 0.0;    // 検出位置Y[pixel]
				target_data->size = 0;      // 検出サイズ
				target_data->roi.x = 0;      // ROI:x coordinate of the top-left corner
				target_data->roi.y = 0;      // ROI:y coordinate of the top-left corner
				target_data->roi.width = 0;      // ROI:width of the rectangle
				target_data->roi.height = 0;      // ROI:height of the rectangle
			}
#pragma endregion IMAGE_PROC
		}   // if (gp_app_imgprc->status & (uint32_t)(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) else
#pragma endregion PROCESS_TAGET
	// 画像保存
#pragma region PUT_IMAGE
		// マスク画像1
		if (gp_cnfg_imgprc->mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)].valid) {
			CSwayShared::set_app_info_data((uint32_t)(ENUM_IMAGE::MASK_1),
				img_mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_1)]);
		}
		// マスク画像2
		if (gp_cnfg_imgprc->mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)].valid) {
			CSwayShared::set_app_info_data(
				(uint32_t)(ENUM_IMAGE::MASK_2),
				img_mask[(uint32_t)(ENUM_IMAGE_MASK::MASK_2)]);
		}
		// 処理画像
		CSwayShared::set_app_info_data((uint32_t)(ENUM_IMAGE::PROCESS), g_img_src.data_mat);
#pragma endregion PUT_IMAGE
	// 振れ検出処理
#pragma region SWAY_PROC
		proc_sway();
#pragma endregion SWAY_PROC
	// シャッタコントロール
#pragma region EXPOSURE_CONTROL
		set_expstime();
#pragma endregion EXPOSURE_CONTROL
	// 制御PCとのIF CHECK　MODE
		if (maintenance_mode == CODE_POL_MAINTE_COMCHECK)   proc_comchk_mode();   
	}
	return S_OK;
}
int CAuxPol::output() {          //出力処理




	return S_OK;
}
int CAuxPol::close() {
	return 0;
}

uint32_t CAuxPol::get_opencv_image(void)
{
	//----------------------------------------------------------------------------
	// 画像データ取得(画像ファイル)
	if (gp_cnfg_common->img_source_camera != (uint32_t)ENUM_GRAB_IMAGE::GRAB_CAMERA) {	//画像ファイル読込
		g_img_src.data_mat = cv::imread(CStrHelper::conv_string(gp_cnfg_common->img_source_fname));
		if (g_img_src.data_mat.data != NULL) {
			g_img_src.status |= (uint32_t)ENUM_IMAGE_STATUS::ENABLED;					// 画像ステータス:画像有効
			g_img_src.width = g_img_src.data_mat.cols;									// 画像サイズ(水平画素) [pixel]
			g_img_src.height = g_img_src.data_mat.rows;									// 画像サイズ(垂直画素) [pixel]
			g_img_src.fps = gp_cnfg_camera->basis.framerate;							// 画像フレームレート[fps]
		}
		else {
			g_img_src.status &= (~(uint32_t)ENUM_IMAGE_STATUS::ENABLED);					// 画像ステータス:画像有効
			g_img_src.width = gp_cnfg_camera->basis.roi[(uint32_t)ENUM_AXIS::X].size;   // 画像サイズ(水平画素) [pixel]
			g_img_src.height = gp_cnfg_camera->basis.roi[(uint32_t)ENUM_AXIS::Y].size;	// 画像サイズ(垂直画素) [pixel]
			g_img_src.fps = 0.0;                                                      // 画像フレームレート[fps]
		}
	}
	else {
		// 画像データ取得(カメラ)
		if (g_img_src.data_bgr != NULL) {
			if ((pCamera != NULL) &&
				(!(gp_app_system->status & (uint32_t)(ENUM_SYSTEM_STATUS::CAMERA_RESET_RUN)))) { // カメラ再接続中
				// 画像情報を取得
				if ((pCamera->get_image(g_img_src.data_bgr) >= 0) &&
					(pCamera->get_image_size(&g_img_src.width, &g_img_src.height) >= 0) &&
					(pCamera->get_image_fps(&g_img_src.fps) >= 0)) {
					g_img_src.status |= ((uint32_t)(ENUM_IMAGE_STATUS::ENABLED));            // 画像ステータス:画像有効
				}
				else {
					g_img_src.status &= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));						// 画像ステータス:画像有効
					g_img_src.width = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;			// 画像サイズ(水平画素) [pixel]
					g_img_src.height = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;		// 画像サイズ(垂直画素) [pixel]
					g_img_src.fps = 0.0;                                                                // 画像フレームレート [fps]
					ZeroMemory(g_img_src.data_bgr, (sizeof(uint8_t) * IMAGE_SIZE * IMAGE_FORMAT_SIZE)); // 画像データバッファのポインタ(BGR 24bit)
				}
			}
			else {
				g_img_src.status &= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));							// 画像ステータス:画像有効
				g_img_src.width = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;				// 画像サイズ(水平画素) [pixel]
				g_img_src.height = gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;			// 画像サイズ(垂直画素) [pixel]
				g_img_src.fps = 0.0;                                                                    // 画像フレームレート [fps]
				ZeroMemory(g_img_src.data_bgr, (sizeof(uint8_t) * IMAGE_SIZE * IMAGE_FORMAT_SIZE));     // 画像データバッファのポインタ(BGR 24bit)
			}

			if (g_img_src.status & (uint32_t)ENUM_IMAGE_STATUS::ENABLED) {
				// OpenCV画像への変換
				g_img_src.data_mat = cv::Mat(
					g_img_src.height,
					g_img_src.width,
					CV_8UC3,
					g_img_src.data_bgr
				);    // 画像データ(OpenCV変換画像)    
			}
		}   // if (g_img_src.data_bgr != NULL)
		else {
			g_img_src.status	&= (~(uint32_t)(ENUM_IMAGE_STATUS::ENABLED));				// 画像ステータス:画像有効
			g_img_src.width		= gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::X)].size;	// 画像サイズ(水平画素) [pixel]
			g_img_src.height	= gp_cnfg_camera->basis.roi[(uint32_t)(ENUM_AXIS::Y)].size;	// 画像サイズ(垂直画素) [pixel]
			g_img_src.fps		= 0.0;														// 画像フレームレート[fps]
		} 
	}
	return g_img_src.status;
}

/// @brief 重心検出
/// @param
/// @return
/// @note
BOOL CAuxPol::proc_center_gravity(std::vector<std::vector<cv::Point>> contours, double* outPosX, double* outPosY, int* outTgtSize, UINT sel)
{
	BOOL    ret = FALSE;
	double  pos_x = 0.0;
	double  pos_y = 0.0;
	int32_t target_size = 0;

	switch (sel) {
	case static_cast<uint32_t>(ENUM_COG::MAX_CONTOUR_AREA):
#pragma region COG_ALGORITHM1
		//----------------------------------------------------------------------------
		// 重心位置算出アルゴリズム(最大輪郭面積)
	{
		BOOL     find = FALSE;
		size_t   count = 0;
		double   max_area = 0;
		size_t   max_area_contour = 0;
		cv::Rect roi;
		for (size_t i = 0; i < contours.size(); i++) {
			double area = contourArea(contours.at(i));
			if (max_area < area) {
				max_area = area;
				max_area_contour = i;
				find = TRUE;
			}
		}

		if (find) {
			count = contours.at(max_area_contour).size();
			for (size_t i = 0; i < count; i++) {
				pos_x += contours.at(max_area_contour).at(i).x;
				pos_y += contours.at(max_area_contour).at(i).y;
			}
			if (count > 0) {
				pos_x /= count;
				pos_y /= count;
				roi = cv::boundingRect(contours[max_area_contour]);
				target_size = (roi.width > roi.height) ? (roi.width + 1) : (roi.height + 1);

				ret = TRUE;
			}
		}
	}
#pragma endregion COG_ALGORITHM1
	break;

	case static_cast<uint32_t>(ENUM_COG::MAX_CONTOUR_LENGTH):
#pragma region COG_ALGORITHM2
		//----------------------------------------------------------------------------
		// 重心位置算出アルゴリズム(最大輪郭長)
	{
		BOOL     find = FALSE;
		size_t   max_size = 0;
		size_t   max_id = 0;
		cv::Rect roi;
		if (contours.size() > 0) {
			for (size_t i = 0; i < contours.size(); i++) {
				if (contours[i].size() > max_size) {
					max_size = contours[i].size();
					max_id = i;
					find = TRUE;
				}
			}
			if (find) {
				cv::Moments mu = moments(contours[max_id]);
				if (mu.m00 > 0.0) {
					pos_x = mu.m10 / mu.m00;
					pos_y = mu.m01 / mu.m00;
					roi = cv::boundingRect(contours[max_id]);
					target_size = (roi.width > roi.height) ? (roi.width + 1) : (roi.height + 1);

					ret = TRUE;
				}
			}
		}
	}
#pragma endregion COG_ALGORITHM2
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
	*outTgtSize = target_size;

	return ret;
}

/// @brief 重心検出 Type2
/// @param
/// @return
/// @note
BOOL CAuxPol::proc_center_gravity2(std::vector<std::vector<cv::Point>> contours, double* outPosX, double* outPosY, int* outTgtSize, cv::Size2i* size_expected, cv::Size2i* size_detected)
{

	BOOL    ret = FALSE;
	double  pos_x = 0.0;
	double  pos_y = 0.0;
	int32_t target_size = 0;

	//----------------------------------------------------------------------------
	// 重心位置算出アルゴリズム(最大輪郭面積)
	BOOL     find = FALSE;
	size_t   count = 0;
	double   max_area = 0, size_chk_high, size_chk_low;
	size_t   max_area_contour = 0;

	size_t   area_detected_max = 0;
	cv::Rect roi;

	//ロープ長から期待されるターゲットの検出サイズ
	//検出サイズ上限　寸法が2倍以下
	size_chk_high = (double)(size_expected->width) * 4.0;
	//検出サイズ下限　寸法が1/8倍以上
   // size_chk_low = (double)(size_expected->width)/2.0;
	size_chk_low = (double)(size_expected->width) / 4.0;
	//サイズ範囲許容値

	for (size_t i = 0; i < contours.size(); i++) {
		double area = contourArea(contours.at(i));

		//輪郭の長方形枠を求め縦横比
		roi = cv::boundingRect(contours[i]);

		//サイズ及び縦横比が許容値内で検出　面積最大のものを検出areaと判定
		if ((max_area < area) && (roi.height < size_chk_high) && (roi.width < size_chk_high) && (roi.height > size_chk_low) && (roi.width > size_chk_low)) {
			max_area = area;
			max_area_contour = i;

			find = TRUE;
		}
	}

	if (find) {
		roi = cv::boundingRect(contours[max_area_contour]);
		//       target_size = (roi.width > roi.height) ? (roi.width + 1) : (roi.height + 1);
			   //ターゲットサイズは面積で評価20241105
		target_size = roi.width * roi.height;
		pos_x = double(roi.x + roi.width / 2);
		pos_y = double(roi.y + roi.height / 2);
		ret = TRUE;
	}

	if (isnan(pos_x) || isnan(pos_y)) {
		pos_x = 0.0;
		pos_y = 0.0;

		ret = FALSE;
	}
	*outPosX = pos_x;
	*outPosY = pos_y;
	*outTgtSize = target_size;

	return ret;
}

/// @brief 振れ検出処理
/// @param
/// @return
/// @note
void CAuxPol::proc_sway(void)
{
	//----------------------------------------------------------------------------
	// ターゲット検出(ターゲット検出データの中心)
	//ターゲット1,2共にマスク設定有効
	if ((gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid) &&
		(gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid)) {
		if ((gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid) &&
			(gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid)) {
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[axis].target_pos = (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].pos[axis]
					+ gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].pos[axis]) * 0.5;  // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[axis].target_tilt = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].pos[axis]
					- gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].pos[axis];          // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].size
				+ gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].size) * 0.5;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE);    // 状態
		}
		else if (gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid) {
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[axis].target_pos = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].pos[axis];  // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[axis].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].size;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE);    // 状態
		}
		else if (gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid) {
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[axis].target_pos = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].pos[axis];  // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[axis].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].size;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE);    // 状態
		}
		else {
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_pos = (double)(CAM1_SPEC_PIXEL_H) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_pos = (double)(CAM1_SPEC_PIXEL_V) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_tilt = 0.0;   // ターゲット傾き[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = 0.0;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status &= (~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)); // TARGET_ENABLEクリア
		}
	}

	//ターゲット1のみマスク設定有効
	else if (gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid) {
		if (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].valid) { //ターゲット1有効
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[axis].target_pos = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].pos[axis];  // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[axis].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].size;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE);    // 状態
		}
		else {                                                                                  //ターゲット1無効
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_pos = (double)(CAM1_SPEC_PIXEL_H) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_pos = (double)(CAM1_SPEC_PIXEL_V) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_tilt = 0.0;   // ターゲット傾き[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = 0.0;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status &= (~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)); // TARGET_ENABLEクリア
		}
	}

	//ターゲット2のみマスク設定有効
	else if (gp_cnfg_imgprc->mask[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid) {
		if (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].valid) {   //ターゲット2有効
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[axis].target_pos = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].pos[axis];  // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[axis].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].size;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE);    // 状態
		}
		else {                                                                                  //ターゲット2無効
			for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_pos = (double)(CAM1_SPEC_PIXEL_H) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_pos = (double)(CAM1_SPEC_PIXEL_V) / 2.0;   // ターゲット位置[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_tilt = 0.0;   // ターゲット傾き[pixel]
				gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_tilt = 0.0;   // ターゲット傾き[pixel]
			}
			gp_app_imgprc->target_size = 0.0;   // ターゲットサイズ(ターゲット検出データの平均)
			gp_app_imgprc->status &= (~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)); // TARGET_ENABLEクリア
		}
	}

	//共にマスク設定無効
	else {
		for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
			gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_pos = (double)(CAM1_SPEC_PIXEL_H) / 2.0;   // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_pos = (double)(CAM1_SPEC_PIXEL_V) / 2.0;   // ターゲット位置[pixel]
			gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)].target_tilt = 0.0;   // ターゲット傾き[pixel]
			gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)].target_tilt = 0.0;   // ターゲット傾き[pixel]
		}
		gp_app_imgprc->target_size = 0.0;   // ターゲットサイズ(ターゲット検出データの平均)
		gp_app_imgprc->status &= (~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)); // TARGET_ENABLEクリア
	}

	//----------------------------------------------------------------------------
	// 振れ検出
	double dt = gp_app_system->sample_cycle;   // タスク実行周期[s]
	for (uint32_t axis = 0; axis < (uint32_t)(ENUM_AXIS::E_MAX); axis++) {
		PSWAY_DATA sway_data = &gp_app_imgprc->sway_data[axis];   // 振れ検出データ(ターゲット検出データの中心)
		
		double     last_sway_angle = sway_data->sway_angle;       // 振れ角(前回値)[pixel]
		if (gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)) {
			sway_data->sway_angle = sway_data->sway_zero - sway_data->target_pos;   // 振れ角[pixel]

			double sway_speed = (sway_data->sway_angle - last_sway_angle) / dt; // 振れ速度[pixel/s]
			double temp_spd = sway_data->sway_speed - sway_speed;
			double chk_limit = gp_app_adjust->coef_roi_margin[axis] * gp_app_adjust->w[axis];

			if ((temp_spd > chk_limit) || (-temp_spd > chk_limit)) {
				;//前回値保持
			}
			else {
				sway_data->sway_speed = SWAY_SENSOR_LPF_K1 * sway_data->sway_speed + SWAY_SENSOR_LPF_K2 * sway_speed; // フィルタ
			}
		}
		else {
			sway_data->sway_angle = 0.0;    // 振れ角[pixel]
			sway_data->sway_speed = 0.0;    // 振れ速度[pixel/s]
		}
	}

	//----------------------------------------------------------------------------
	// 振れゼロ点設定処理
#if 0
	for (uint32_t axis = 0; axis < static_cast<uint32_t>(ENUM_AXIS::E_MAX); axis++) {
		PSWAY_DATA sway_data = &gp_app_imgprc->sway_data[axis];   // 振れ検出データ(ターゲット検出データの中心)
		sway_data->sway_zero = get_sway_zero(axis); // 振れ中心[pixel]
	}
#else
	get_sway_zero(); // 振れ中心[pixel]
#endif
	return;
}

/// @brief 振れゼロ点設定処理
/// @param
/// @return
/// @note
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
	span_msec = static_cast<uint32_t>(span_usec / 1000);    // 時間の間隔[msec]

	//ゼロセット実行中フラグセット
	if (gp_app_client->command & static_cast<uint32_t>(SW_CLIENT_COM_CAMERA1_0SET)) { // クライアント0セットコマンドフラグON

		if (!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) {//MCC 0セット進行中　OFF

			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN);     //MCC 0セット進行中フラグセット
			wosmsg.str(L""); wosmsg << L"<Information>Running sway zero setting";
			msg2listview(wosmsg.str()); 
		}
	}

	if (((gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_RUN)) ||        //0セット進行中　ON
		(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) &&    //MCC 0セット進行中　ON
		!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED))) {  //0セット完了フラグ　ON

		if (gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE)) {       //ターゲット検出中　ON
			if (span_msec >= 0) {                               //計測継続時間 > 0
				if (span_msec < gp_cnfg_imgprc->swayzeroset_time) {   //計測設定時間経過していない
					if (sway_data->target_pos < m_sway_zero_data.sway_min[idx]) { //最小値更新
						m_sway_zero_data.sway_min[idx] = sway_data->target_pos;
					}
					if (m_sway_zero_data.sway_max[idx] < sway_data->target_pos) { //最大値更新
						m_sway_zero_data.sway_max[idx] = sway_data->target_pos;
					}
				}
				else {
					m_sway_zero_data.sway_zero[idx] = (m_sway_zero_data.sway_min[idx] + m_sway_zero_data.sway_max[idx]) * 0.5;
					gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED);  // 
					QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
					wosmsg.str(L""); wosmsg << L"<Information>Completed  sway zero setting";
					msg2listview(wosmsg.str());
				}
			}
			else {
				QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
			}
		}
		else {//ターゲット検出中でない　計測用カウンターホールド値セット　0セット完了フラグ　ON
			gp_app_imgprc->status |= static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED);   // 
			QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
			wosmsg.str(L""); wosmsg << L"<Error>Failed to sway zero setting";
			msg2listview(wosmsg.str());
		}
	}
	else {  //0セット進行中でない　計測用カウンターホールド値セット
		QueryPerformanceCounter(&m_sway_zero_data.time_counter);    // 計測時間用パフォーマンスカウンター
	}

	if (gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED)) {      //0セット完了状態

		gp_app_imgprc->status &= ~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_RUN);            //0セット進行中クリア

		if (!(gp_app_client->command & static_cast<uint32_t>(SW_CLIENT_COM_CAMERA1_0SET))) {            //カメラ1の0セットコマンドクリア済

			gp_app_imgprc->status &= ~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN);    //MCC 0セット進行中クリア
		}

		if (!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_RUN)) &&     //0セット進行中でない
			!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_MCC_RUN))) { //0セット進行中でない

			gp_app_imgprc->status &= (~static_cast<uint32_t>(ENUM_PROCCESS_STATUS::ZERO_SET_COMPLETED));//0セット完了状態クリア
		}
	}

	return m_sway_zero_data.sway_zero[idx];
}

//NEW LOGIC 0転　常時更新
static int sway0_counter = 0;
double CAuxPol::get_sway_zero()
{
	PSWAY_DATA    sway_data_x = &gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::X)];   // 振れ検出データ(ターゲット検出データの中心)
	PSWAY_DATA    sway_data_y = &gp_app_imgprc->sway_data[static_cast<uint32_t>(ENUM_AXIS::Y)];   // 振れ検出データ(ターゲット検出データの中心)

	std::wstring  msg;

	sway0_counter--;
	if (sway0_counter < 0) {

		sway0_counter = (int)(gp_app_adjust->T[0] / gp_app_system->sample_cycle);

		if (!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::TARGET_ENABLE))) {
			sway_data_x->sway_zero = (double)(CAM1_SPEC_PIXEL_H / 2);
			sway_data_y->sway_zero = (double)(CAM1_SPEC_PIXEL_V / 2);
		}
		else {
			sway_data_x->sway_zero = (m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::X)] + m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::X)]) * 0.5;
			sway_data_y->sway_zero = (m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::Y)] + m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::Y)]) * 0.5;
		}
		//検出最大値,最小値リセット
		m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::X)] = CAM1_SPEC_PIXEL_H;
		m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::Y)] = CAM1_SPEC_PIXEL_V;
		m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::X)] = m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::Y)] = 0;
	}
	else {
		//検出最大値,最小値更新
		if (sway_data_x->target_pos < m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::X)]) { //最小値更新
			m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::X)] = sway_data_x->target_pos;
		}
		if (m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::X)] < sway_data_x->target_pos) { //最大値更新
			m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::X)] = sway_data_x->target_pos;
		}

		if (sway_data_y->target_pos < m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::Y)]) { //最小値更新
			m_sway_zero_data.sway_min[static_cast<uint32_t>(ENUM_AXIS::Y)] = sway_data_y->target_pos;
		}
		if (m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::Y)] < sway_data_y->target_pos) { //最大値更新
			m_sway_zero_data.sway_max[static_cast<uint32_t>(ENUM_AXIS::Y)] = sway_data_y->target_pos;
		}
	}

	return 0.0;
}

/// @brief シャッタコントロール
/// @param
/// @return
/// @note

static bool was_over_expose = false;
static bool was_under_expose = false;
static double exps_time_upper_limit;
static double exps_time_lower_limit;
void CAuxPol::set_expstime()
{
	if (!gp_cnfg_common->img_source_camera) {
		return;
	}

	if (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].max_val >
		gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].max_val) {
		gp_app_imgprc->exps_chk_brightness = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].max_val;
	}
	else {
		gp_app_imgprc->exps_chk_brightness = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].max_val;
	}

	//----------------------------------------------------------------------------
	// シャッターコントロール禁止判定
	// 備考：画像入力異常またはシャッターコントロール固定で禁止
	if (!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) ||
		!(gp_cnfg_camera->expstime.auto_control)) {
		gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;
		gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val;

		ZeroMemory(m_move_avrg_data.data, sizeof(m_move_avrg_data.data));
		m_move_avrg_data.wptr = 0;
		m_move_avrg_data.data_count = 0;
		m_move_avrg_data.total_val = 0;
		m_move_avrg_data.max_val = 0.0f;

		was_over_expose = false;
		was_under_expose = false;

		//  gp_app_imgprc->exps_chk_brightness = 0;
	}   // if (!(gp_app_imgprc->status & static_cast<uint32_t>(ENUM_PROCCESS_STATUS::IMAGE_ENABLE)) || !(gp_cnfg_camera->expstime.auto_control))
	else {

#if 1  //NEW LOGIC

		if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_ROI_MODE) {

			//一旦ターゲットを検出したら、最大輝度が下がらない限り露光時間キープ
			if (gp_app_imgprc->exps_chk_brightness <= 180.0) {
				gp_app_imgprc->exps_time += 1.0 * (255.0 - gp_app_imgprc->exps_chk_brightness);
				was_under_expose = true;
			}
			else if ((gp_app_imgprc->exps_chk_brightness >= 254.0) && (was_under_expose == false)) {
				gp_app_imgprc->exps_time -= gp_app_imgprc->exps_chk_brightness * 0.01;
				was_over_expose = true;
				was_under_expose = false;
			}
			else {
				was_over_expose = false;
				was_under_expose = true;
			}
		}
		else if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_ROI_KEEP) {
			//レンジオーバー用　露光時間キープ
			gp_app_imgprc->exps_time = gp_app_imgprc->exps_time;
		}
		else {//初期化ステップ

			if (gp_app_imgprc->exps_ctrl_mode == EXPOSURE_CONTROL_RESET_STEP) {
				//              gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_min;
				gp_app_imgprc->exps_time = 1000;
				exps_time_lower_limit = gp_cnfg_camera->expstime.val_min;
				exps_time_upper_limit = gp_cnfg_camera->expstime.val_max;
				was_over_expose = false;
				gp_app_imgprc->exps_step_count = EXPOSURE_CONTROL_STEP_COUNT;
			}

			int is_wait_step = gp_app_imgprc->exps_ctrl_mode & 0x00000001;//奇数ステップは待機
			if (is_wait_step) {//待機ステップ
				gp_app_imgprc->exps_step_count--;
				if (gp_app_imgprc->exps_step_count < 0)
					gp_app_imgprc->exps_ctrl_mode++;        //次ステップへ
			}
			else {
				if (gp_app_imgprc->exps_chk_brightness >= 254.0) {//ハレーション

					exps_time_upper_limit = gp_app_imgprc->exps_time;

					if (was_under_expose) {
						gp_app_imgprc->exps_time -= (exps_time_upper_limit - exps_time_lower_limit) / 2;
					}
					else {
						if ((gp_app_imgprc->exps_time / 2) < exps_time_lower_limit)
							gp_app_imgprc->exps_time -= (exps_time_upper_limit - exps_time_lower_limit) / 2;
						else
							gp_app_imgprc->exps_time /= 2;
					}
					was_over_expose = true;
					was_under_expose = false;
				}
				else {
					exps_time_lower_limit = gp_app_imgprc->exps_time;
					if (gp_app_imgprc->exps_chk_brightness < 220.0) {
						if (was_over_expose) {
							gp_app_imgprc->exps_time = gp_app_imgprc->exps_time + (exps_time_upper_limit - exps_time_lower_limit) / 2;
						}
						else {
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

#else
		//----------------------------------------------------------------------------
		// 輝度移動平均
#pragma region BrightnessMoveAverage
		double max_val;
		if (gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].max_val >
			gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].max_val) {
			max_val = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_2)].max_val;
		}
		else {
			max_val = gp_app_imgprc->target_data[static_cast<uint32_t>(ENUM_IMAGE_MASK::MASK_1)].max_val;
		}
		gp_app_imgprc->img_val = CHelper::moving_average(max_val, &m_move_avrg_data, MOVE_AVERAGE_COUNT);
#pragma endregion BrightnessMoveAverage

		//----------------------------------------------------------------------------
		switch (gp_app_imgprc->exps_mode) {
		case EXPOSURE_CONTROL_LIGHT:    // 明るく
			if (gp_app_imgprc->img_val > gp_cnfg_camera->expstime.auto_stop_l) {
				gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;
			}
			break;

		case EXPOSURE_CONTROL_DARKEN:   // 暗く
			if (gp_app_imgprc->img_val < gp_cnfg_camera->expstime.auto_stop_h) {
				gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_HOLD;
			}
			break;

		default:                        // Hold
			if (gp_app_imgprc->img_val > gp_cnfg_camera->expstime.auto_start_h) {
				gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_DARKEN;
			}
			else if (gp_app_imgprc->img_val < gp_cnfg_camera->expstime.auto_start_l) {
				gp_app_imgprc->exps_mode = EXPOSURE_CONTROL_LIGHT;
			}
			else {
				;
			}
			break;
		}   // switch (gp_app_imgprc->exps_mode)

		//----------------------------------------------------------------------------
		switch (gp_app_imgprc->exps_mode) {
		case EXPOSURE_CONTROL_LIGHT:    // 明るく
			if ((gp_app_imgprc->exps_time += gp_cnfg_camera->expstime.auto_rate) > gp_cnfg_camera->expstime.val_max) {
				gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_max;
			}
			break;

		case EXPOSURE_CONTROL_DARKEN:   // 暗く
			if ((gp_app_imgprc->exps_time -= gp_cnfg_camera->expstime.auto_rate) < gp_cnfg_camera->expstime.val_min) {
				gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_min;
			}
			break;

		default:
			break;
		}	// switch(gp_app_imgprc->exps_mode)

#endif
		if (gp_app_imgprc->exps_ctrl_mode > EXPOSURE_CONTROL_RESET_STEP_FIN) gp_app_imgprc->exps_ctrl_mode = EXPOSURE_CONTROL_RESET_STEP;
		if (gp_app_imgprc->exps_time > gp_cnfg_camera->expstime.val_max) gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_max;
		if (gp_app_imgprc->exps_time < gp_cnfg_camera->expstime.val_min) gp_app_imgprc->exps_time = gp_cnfg_camera->expstime.val_min;

	}   // else

	return;
}

static uint32_t pol_counter = 0;
void CAuxPol::proc_comchk_mode() {

	double cycleTs = 10.0;
	uint32_t cycle_count = (uint32_t)(cycleTs * 1000.0) / inf.cycle_ms;

	double wt = PI360 / cycleTs * (double)((pol_counter % cycle_count) * inf.cycle_ms) / 1000.0;

	gp_app_imgprc->sway_data[(int)ENUM_AXIS::X].sway_angle = 1000.0 * sin(wt);

	pol_counter++;
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

		case IDC_TASK_ITEM_CHECK1:
		case IDC_TASK_ITEM_CHECK2:
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
		wstr = L"ｶﾒﾗ0点設定";
		SetWindowText(GetDlgItem(inf.hwnd_opepane, IDC_TASK_ITEM_CHECK1), wstr.c_str());
		wstr = L"COMCHK";
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

