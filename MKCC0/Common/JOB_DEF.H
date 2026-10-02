#pragma once

/**********************************************************************************************************/
//JOB ITEM
/**********************************************************************************************************/
//JOB関連

#define N_JOB_LIST					3		//JOB LIST登録数
#define N_JOB_REGIST_MAX			10		//JOB登録最大数
#define N_JOB_TYPE_MAX				128		//登録可能JOBのタイプ数
#define N_JOB_TARGET_MAX			10
#define N_JOB_OPTION_MAX			10
#define N_JOB_COMMAND_MAX			10		//JOBを構成するコマンド最大数
#define N_JOB_PARAM_I16				8		//JOBに設定可能なINT16パラメータ数  


//JOB TYPE CODE
#define ID_JOBTYPE_JOB				0		//JOB Type index番号
#define ID_JOBTYPE_SEMI				1		//SEMIAUTO Type index番号
#define ID_JOBTYPE_ANTISWAY			2		//FB ANTISWAY Type index番号
#define ID_JOBTYPE_GBC_TEST0		256		//GBC TEST用のタイプ識別
#define ID_JOBTYPE_GBC_TEST1		257		//GBC TEST用のジョブ内容識別
#define ID_JOBTYPE_END				-1		//JOB CODEリストの末尾識別用CODE

//COMMAND PATTERN
#define COM_PTN_ORDINARY			 0x00000000  //通常
#define COM_PTN_1SHOT_AS			 0x00000001  //ワンショット振止
#define COM_PTN_HALF_T_AS			 0x00000002  //半周期振止
#define COM_PTN_2SHOT_AS			 0x00000004  //インチング振れ止め
#define COM_PTN_2SHOT_MOVE0			 0x00000008  //インチング移動(振れ抑制無し）
#define COM_PTN_2SHOT_MOVE_PLUS		 0x00000010  //インチング移動(振れ抑制有）
#define COM_PTN_FULL_T				 0x00000020  //1周期振止


#define MODE_PC_CTRL					0x00000001
#define MODE_ANTISWAY					0x00010000
#define MODE_RMOTE_PANEL				0x00000100

#define ID_JOBIO_COMTYPE_PAUSE			0
#define ID_JOBIO_COMTYPE_PICK			1
#define ID_JOBIO_COMTYPE_GRND			2
#define ID_JOBIO_COMTYPE_PARK_POS		3	//停止位置指定移動
#define ID_JOBIO_COMTYPE_PARK_TIME		4	//移動時間指定移動
#define ID_JOBIO_COMTYPE_AS_1SHOT		5	//1インチング振れ止め
#define ID_JOBIO_COMTYPE_AS_2SHOT		6	//2インチング振れ止め
#define ID_JOBIO_COMTYPE_POS_2SHOT		7	//2インチング位置合わせ

//レシピ　Type
#define CTR_TYPE_WAIT_TIME					0x0000	//待機（時間経過待ち）
#define CTR_TYPE_WAIT_POS_HST				0x0101	//巻到達待ち
#define CTR_TYPE_WAIT_POS_GNT				0x0102	//走行到達待ち
#define CTR_TYPE_WAIT_POS_TRY				0x0104	//横行到達待ち
#define CTR_TYPE_WAIT_POS_BH				0x0108	//引込到達待ち
#define CTR_TYPE_WAIT_POS_SLW				0x0110	//旋回到達待ち
#define CTR_TYPE_WAIT_LAND					0x0120	//着床待ち
#define CTR_TYPE_WAIT_PH_1SHOT				0x0201	//位相待ち ONE SHOT
#define CTR_TYPE_WAIT_PH_2SHOT_FIRST		0x0202	//2INCH初回位相待ち
#define CTR_TYPE_WAIT_PH_2SHOT_SECOND		0x0204	//位相待ち TWO INCH　2回目
#define CTR_TYPE_VOUT_TIME					0x1000  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_V						0x1001  //ステップ速度　速度到達完了
#define CTR_TYPE_VOUT_POS					0x1002  //ステップ速度　位置到達完了
#define CTR_TYPE_VOUT_PHASE     			0x1004  //ステップ速度　位相到達完了
#define CTR_TYPE_VOUT_LAND					0x1008  //ステップ速度　着床完了
#define CTR_TYPE_VOUT_TIME_1SHOT			0x1010  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_TIME_2SHOT1			0x1020  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_TIME_2SHOT2			0x1040  //ステップ速度　時間完了
#define CTR_TYPE_AOUT_TIME					0x2000  //加速速度　時間完了
#define CTR_TYPE_AOUT_V						0x2001  //加速速度　速度到達完了
#define CTR_TYPE_AOUT_POS					0x2002  //加速速度　位置到達完了
#define CTR_TYPE_AOUT_PHASE     			0x2004  //加速速度　位相到達完了
#define CTR_TYPE_AOUT_LAND					0x2008  //加速速度　着床完了
#define CTR_TYPE_FINE_POS					0x8001	//微小位置合わせ
#define CTR_TYPE_SMART_SLOW_DOWN			0x8010	//スマートスローダウン

/*** コマンド種類 ***/
#define COM_TYPE_MASK			0x0F00      
#define COM_TYPE_PICK			0x0100
#define COM_TYPE_GRND			0x0200
#define COM_TYPE_PARK			0x0400
#define COM_TYPE_FROM_TO		0x0800
#define COM_TYPE_NON 			0x0000

/*** 状態種類 ***/
#define STAT_MASK               0x00FF      //ステータス部マスク
#define STAT_NA					0x0000      //適用不可
#define STAT_MANUAL				0x0000      //適用不可
#define STAT_STANDBY			0x0001      //スタンバイ
#define STAT_TRIGED             0x0002      //起動済実行待ち
#define STAT_ACTIVE             0x0004      //実行中報告
#define STAT_SUSPENDED          0x0008      //一時停止報告

#define STAT_ABOTED             0x0120      //キャンセル
#define STAT_END				0x0020      //正常完了
#define STAT_ABNORMAL_END       0x8020      //異常完了
#define STAT_SEQ_ERR_END		0x2020      //シーケンス異常完了

#define STAT_REQ_WAIT           0x0080      //要求待ち
#define STAT_POSITIVE			0x1000		//OK
#define STAT_SUCCEED			0x1000		//成功
#define STAT_ACK				0x1001		//正常受付
#define STAT_NAK				0x8000		//NG
#define STAT_TIME_OVER			0x8010		//タイムオーバー

#define STAT_LOGICAL_ERROR		0x8004      //整合性異常
#define STAT_CODE_ERROR			0x8008      //適合コード無し

/*** ジョブ,コマンドステータス ***/
//auto_ongoing用
#define AUTO_TYPE_JOB_MASK				0x7000      //JOB種別部マスク
#define AUTO_TYPE_SEL_MASK				0xf000      //自動種別部マスク
#define AUTO_TYPE_MANUAL				0x0000
#define AUTO_TYPE_JOB					0x1000      //種別JOB
#define AUTO_TYPE_SEMIAUTO				0x2000      //種別半自動
#define AUTO_TYPE_OPERATION				0x4000      //種別クレーン操作
#define AUTO_TYPE_FB_ANTI_SWAY			0x8000
//antisway_on_going用
#define ANTISWAY_ALL_MANUAL				0x00000000      //振れ止め無し
#define ANTISWAY_BH_ACTIVE				0x00000001      //引込振止実行中
#define ANTISWAY_BH_PAUSED				0x00000002      //引込振止一時停止
#define ANTISWAY_BH_COMPLETE			0x00000008      //引込振止完了
#define ANTISWAY_SLEW_ACTIVE			0x00000010      //旋回振止実行
#define ANTISWAY_SLEW_PAUSED			0x00000020      //旋回振止一時停止
#define ANTISWAY_SLEW_COMPLETE			0x00000080      //旋回振止完了 
//axis_status用
#define AG_AXIS_STAT_FB0				0x000000001	 //停止中
#define AG_AXIS_STAT_PC_ENABLE			0x800000000	 //PC指令可
#define AG_AXIS_STAT_AUTO_ENABLE		0x100000000	 //自動可
#define AG_AXIS_STAT_ANTISWAY_ENABLE	0x200000000	 //振れ止め可
