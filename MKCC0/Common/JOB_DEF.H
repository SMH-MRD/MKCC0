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
#define N_JOB_COMMAND_MAX			10			//　JOBを構成するコマンド最大数


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


#define MODE_PC_CTRL		0x00000001
#define MODE_ANTISWAY		0x00010000
#define MODE_RMOTE_PANEL	0x00000100

#define ID_JOBIO_COMTYPE_PAUSE				0
#define ID_JOBIO_COMTYPE_PICK				1
#define ID_JOBIO_COMTYPE_GRND				2
#define ID_JOBIO_COMTYPE_PARK				3
#define ID_JOBIO_COMTYPE_ANTISWAY			4

#define CODE_JOBIO_COMTYPE_ANTISWAY			-1

