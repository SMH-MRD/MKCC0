#pragma once

#include <windows.h>
#include "..\Common\COMMON_DEF.h"
#include "..\Common\phisics.h"

typedef struct tagMklogItemUnit {
	INT32 code;
	double d100;
	WCHAR title[16];
	INT16(*func)(double d);//入力値を100%値に正規化する関数
}ST_MKLOG_ITEM_UNIT, *LPST_LOG_MKITEM_UNIT;

#define N_MKLOG_ITEM_DB			256

#define MKLOG_ITEM_TYPE_CHECK	0xF800
#define MKLOG_ITEM_TYPE_ANALOG	0x0000
#define MKLOG_ITEM_TYPE_DIO16	0x8000
#define MKLOG_ITEM_TYPE_BIT		0xC000
#define MKLOG_ITEM_TYPE_MENTE	0xF800

//### COMMON 0x0-0x1
#define MKLOG_ITEM_TIME				0 
#define MKLOG_ITEM_EMPTY			0 
#define MKLOG_ITEM_COUNT			1 
//### ANALOG 0x02-0xBF
#define MKLOG_ITEM_MH_POS_FB		2 
#define MKLOG_ITEM_BH_POS_FB		3 
#define MKLOG_ITEM_SL_POS_FB		4 
#define MKLOG_ITEM_AH_POS_FB		5 
#define MKLOG_ITEM_MH_SPD_FB		6 
#define MKLOG_ITEM_BH_SPD_FB		7 
#define MKLOG_ITEM_SL_SPD_FB		8 
#define MKLOG_ITEM_AH_SPD_FB		9 
#define MKLOG_ITEM_GT_SPD_FB		10 
#define MKLOG_ITEM_MH_SPD_REF		11
#define MKLOG_ITEM_BH_SPD_REF		12
#define MKLOG_ITEM_SL_SPD_REF		13
#define MKLOG_ITEM_AH_SPD_REF		14
#define MKLOG_ITEM_GT_SPD_REF		15
#define MKLOG_ITEM_SWY1_TH_X		16
#define MKLOG_ITEM_SWY1_TH_Y		17
#define MKLOG_ITEM_SWY1_DTH_X		18
#define MKLOG_ITEM_SWY1_DTH_Y		19
#define MKLOG_ITEM_SWY1_TIL_X		20
#define MKLOG_ITEM_SWY1_TIL_Y		21
#define MKLOG_ITEM_SWY1_DTIL_X		22
#define MKLOG_ITEM_SWY1_DTIL_Y		23
#define MKLOG_ITEM_SWY1_TX			24
#define MKLOG_ITEM_SWY1_TY			25
#define MKLOG_ITEM_SWY1_DTHWX		26
#define MKLOG_ITEM_SWY1_DTHWY		27
#define MKLOG_ITEM_SWY1_AMP_X		28
#define MKLOG_ITEM_SWY1_AMP_Y		29
#define MKLOG_ITEM_SWY1_PH_X		30
#define MKLOG_ITEM_SWY1_PH_Y		31
#define MKLOG_ITEM_MHL				32
//NOTCH
#define MKLOG_ITEM_NOTCH_COM_MH		50
#define MKLOG_ITEM_NOTCH_COM_GT		51
#define MKLOG_ITEM_NOTCH_COM_BH		52
#define MKLOG_ITEM_NOTCH_COM_SL		53
#define MKLOG_ITEM_NOTCH_COM_AH		54
//MENTENANCE
#define MKLOG_ITEM_SIN_1HZ		    124	//CHECK用SINEカーブ
#define MKLOG_ITEM_COS_1HZ			125	//CHECK用COSカーブ
#define MKLOG_ITEM_SIN_1HZ01HZ	    126	//CHECK用SINEカーブ振幅変調あり
#define MKLOG_ITEM_COS_1HZ01HZ		127//CHECK用COSカーブ振幅変調あり
//16bit DIO
#define MKLOG_ITEM_OPECAB_DI00		128
#define MKLOG_ITEM_OPECAB_DI01		129
#define MKLOG_ITEM_OPECAB_DI02		130
#define MKLOG_ITEM_OPECAB_DI03		131
#define MKLOG_ITEM_OPECAB_DI04		132
#define MKLOG_ITEM_OPECAB_DI05		133
#define MKLOG_ITEM_OPECAB_DI06		134
#define MKLOG_ITEM_OPECAB_DI07		135
#define MKLOG_ITEM_OPECAB_DI08		136
#define MKLOG_ITEM_OPECAB_DI09		137
#define MKLOG_ITEM_OPECAB_DI10		138
#define MKLOG_ITEM_OPECAB_DI11		139
#define MKLOG_ITEM_OPECAB_DI12		140
#define MKLOG_ITEM_OPECAB_DI13		141
#define MKLOG_ITEM_OPECAB_DI14		142
#define MKLOG_ITEM_OPECAB_DI15		143

//BIT 0xC0-0xFF
#define MKLOG_ITEM_CONTROL_SOURCE	192
#define MKLOG_ITEM_CONTROL_ESTOP	193
#define MKLOG_ITEM_GRIP_SWITCH		194


#define MKLOG_ITEM_EVENT			253	//EVENT
//MENTENANCE
#define MKLOG_ITEM_ONOFF2HZ			254	//2HzONOFF
#define MKLOG_ITEM_ONOFF10HZ		255	//2HzONOFF


typedef struct ST_MKLOG_DB {
	ST_MKLOG_ITEM_UNIT item[N_MKLOG_ITEM_DB] =
	{
//		  CODE		100%		Title			*func
		{ 0x0000,	0.0,		L"NA",			NULL },//000 MKLOG_ITEM_TIME		  
		{ 0x0001,	0.0,		L"COUNT",		NULL },//001 MKLOG_ITEM_COUNT   
		{ 0x0002,	80.0,		L"MH_POS_FB  ",	NULL },//002 MKLOG_ITEM_MH_POS_FB   
		{ 0x0003,	60.0 ,		L"BH_POS_FB  ",	NULL },//003 MKLOG_ITEM_BH_POS_FB   
		{ 0x0004,	360.0 ,		L"SL_POS_FB  ",	NULL },//004 MKLOG_ITEM_SL_POS_FB   
		{ 0x0005,	80.0 ,		L"AH_POS_FB  ",	NULL },//005 MKLOG_ITEM_AH_POS_FB   
		{ 0x0006,	2250.0,		L"MH_SPD_FB  ",	NULL },//006 MKLOG_ITEM_MH_SPD_FB   
		{ 0x0007,	1750.0,		L"BH_SPD_FB  ",	NULL },//007 MKLOG_ITEM_BH_SPD_FB   
		{ 0x0008,	1750.0,		L"SL_SPD_FB  ",	NULL },//008 MKLOG_ITEM_SL_SPD_FB   
		{ 0x0009,	4000.0,		L"AH_SPD_FB  ",	NULL },//009 MKLOG_ITEM_AH_SPD_FB   
		{ 0x000A,	4000.0,		L"GT_SPD_FB  ",	NULL },//010 MKLOG_ITEM_GT_SPD_FB   
		{ 0x000B,	3000.0,		L"MH_SPD_REF ",	NULL },//011 MKLOG_ITEM_MH_SPD_REF  
		{ 0x000C,	1000.0,		L"BH_SPD_REF ",	NULL },//012 MKLOG_ITEM_BH_SPD_REF  
		{ 0x000D,	1000.0,		L"SL_SPD_REF ",	NULL },//013 MKLOG_ITEM_SL_SPD_REF  
		{ 0x000E,	4000.0,		L"AH_SPD_REF ",	NULL },//014 MKLOG_ITEM_AH_SPD_REF  
		{ 0x000F,	4000.0,		L"GT_SPD_REF ",	NULL },//015 MKLOG_ITEM_GT_SPD_REF 
		{ 0x0010,	PI5,		L"SW1RADX    ",	NULL },//016 MKLOG_ITEM_SWY1_TH_X
		{ 0x0011,	PI5,		L"SW1RADY    ",	NULL },//017 MKLOG_ITEM_SWY1_TH_Y	
		{ 0x0012,	PI5 * 1.0,	L"SW1DRADX   ",	NULL },//018 MKLOG_ITEM_SWY1_DTH_X	Aω ω=√(g/l）=√(9.8/9.8）
		{ 0x0013,	PI5 * 1.0,	L"SW1DRADY   ",	NULL },//019 MKLOG_ITEM_SWY1_DTH_Y	Aω ω=√(g/l）=√(9.8/9.8）
		{ 0x0014,	0.087,		L"SW1TILX    ",	NULL },//020 MKLOG_ITEM_SWY1_TIL_X	5deg/100%
		{ 0x0015,	0.087,		L"SW1TILY    ",	NULL },//021 MKLOG_ITEM_SWY1_TIL_Y	5deg/100%
		{ 0x0016,	0.0547,		L"SW1DTILX   ",	NULL },//022 MKLOG_ITEM_SWY1_DTIL_X	5deg * 2π/10秒　/100%
		{ 0x0017,	0.0547,		L"SW1DTILY   ",	NULL },//023 MKLOG_ITEM_SWY1_DTIL_Y	π/100/s/100%
		{ 0x0018,	30.0,		L"SW1T       ",	NULL },//024 MKLOG_ITEM_SWY1_T		
		{ 0x0019,	PI5,		L"SW1DX/W    ",	NULL },//025 MKLOG_ITEM_SWY1_DTHWX	dθ/dt/ω　π/10/100%
		{ 0x001a,	PI5,		L"SW1DY/W    ",	NULL },//026 MKLOG_ITEM_SWY1_DTHWY	dθ/dt/ω　π/10/100%
		{ 0x001b,	PI5,		L"SW1AMP_X   ",	NULL },//027 MKLOG_ITEM_SWY1_AMP_X	
		{ 0x001c,	PI5,		L"SW1AMP_Y   ",	NULL },//028 MKLOG_ITEM_SWY1_AMP_Y	
		{ 0x001d,	PI180,		L"SW1PH_X    ",	NULL },//029 MKLOG_ITEM_SWY1_PH_X	
		{ 0x001e,	PI180,		L"SW1PH_Y    ",	NULL },//030 MKLOG_ITEM_SWY1_PH_Y	
		{ 0x001f,	100.0,		L"MH L"		  ,	NULL },//031
		{ 0,		0.0,		L"-",			NULL },//032
		{ 0,		0.0,		L"-",			NULL },//033
		{ 0,		0.0,		L"-",			NULL },//034
		{ 0,		0.0,		L"-",			NULL },//035
		{ 0,		0.0,		L"-",			NULL },//036
		{ 0,		0.0,		L"-",			NULL },//037
		{ 0,		0.0,		L"-",			NULL },//038
		{ 0,		0.0,		L"-",			NULL },//039
		{ 0,		0.0,		L"-",			NULL },//040
		{ 0,		0.0,		L"-",			NULL },//041
		{ 0,		0.0,		L"-",			NULL },//042
		{ 0,		0.0,		L"-",			NULL },//043
		{ 0,		0.0,		L"-",			NULL },//044
		{ 0,		0.0,		L"-",			NULL },//045
		{ 0,		0.0,		L"-",			NULL },//046
		{ 0,		0.0,		L"-",			NULL },//047
		{ 0,		0.0,		L"-",			NULL },//048
		{ 0,		0.0,		L"-",			NULL },//049
		{ 0x0032,	4.0,		L"MH_NOTCH_REF",NULL },//050 MKLOG_ITEM_NOTCH_COM_MH
		{ 0x0033,	4.0,		L"GT_NOTCH_REF",NULL },//051 MKLOG_ITEM_NOTCH_COM_GT
		{ 0x0034,	4.0,		L"BH_NOTCH_REF",NULL },//052 MKLOG_ITEM_NOTCH_COM_BH
		{ 0x0035,	4.0,		L"SW_NOTCH_REF",NULL },//053 MKLOG_ITEM_NOTCH_COM_SL
		{ 0x0036,	4.0,		L"AH_NOTCH_REF",NULL },//054 MKLOG_ITEM_NOTCH_COM_AH
		{ 0,		0.0,		L"-",			NULL },//055
		{ 0,		0.0,		L"-",			NULL },//056
		{ 0,		0.0,		L"-",			NULL },//057
		{ 0,		0.0,		L"-",			NULL },//058
		{ 0,		0.0,		L"-",			NULL },//059
		{ 0,		0.0,		L"-",			NULL },//060
		{ 0,		0.0,		L"-",			NULL },//061
		{ 0,		0.0,		L"-",			NULL },//062
		{ 0,		0.0,		L"-",			NULL },//063
		{ 0,		0.0,		L"-",			NULL },//064
		{ 0,		0.0,		L"-",			NULL },//065
		{ 0,		0.0,		L"-",			NULL },//066
		{ 0,		0.0,		L"-",			NULL },//067
		{ 0,		0.0,		L"-",			NULL },//068
		{ 0,		0.0,		L"-",			NULL },//069
		{ 0,		0.0,		L"-",			NULL },//070
		{ 0,		0.0,		L"-",			NULL },//071
		{ 0,		0.0,		L"-",			NULL },//072
		{ 0,		0.0,		L"-",			NULL },//073
		{ 0,		0.0,		L"-",			NULL },//074
		{ 0,		0.0,		L"-",			NULL },//075
		{ 0,		0.0,		L"-",			NULL },//076
		{ 0,		0.0,		L"-",			NULL },//077
		{ 0,		0.0,		L"-",			NULL },//078
		{ 0,		0.0,		L"-",			NULL },//079
		{ 0,		0.0,		L"-",			NULL },//080
		{ 0,		0.0,		L"-",			NULL },//081
		{ 0,		0.0,		L"-",			NULL },//082
		{ 0,		0.0,		L"-",			NULL },//083
		{ 0,		0.0,		L"-",			NULL },//084
		{ 0,		0.0,		L"-",			NULL },//085
		{ 0,		0.0,		L"-",			NULL },//086
		{ 0,		0.0,		L"-",			NULL },//087
		{ 0,		0.0,		L"-",			NULL },//088
		{ 0,		0.0,		L"-",			NULL },//089
		{ 0,		0.0,		L"-",			NULL },//090
		{ 0,		0.0,		L"-",			NULL },//091
		{ 0,		0.0,		L"-",			NULL },//092
		{ 0,		0.0,		L"-",			NULL },//093
		{ 0,		0.0,		L"-",			NULL },//094
		{ 0,		0.0,		L"-",			NULL },//095
		{ 0,		0.0,		L"-",			NULL },//096
		{ 0,		0.0,		L"-",			NULL },//097
		{ 0,		0.0,		L"-",			NULL },//098
		{ 0,		0.0,		L"-",			NULL },//099
		{ 0,		0.0,		L"-",			NULL },//100
		{ 0,		0.0,		L"-",			NULL },//101
		{ 0,		0.0,		L"-",			NULL },//102
		{ 0,		0.0,		L"-",			NULL },//103
		{ 0,		0.0,		L"-",			NULL },//104
		{ 0,		0.0,		L"-",			NULL },//105
		{ 0,		0.0,		L"-",			NULL },//106
		{ 0,		0.0,		L"-",			NULL },//107
		{ 0,		0.0,		L"-",			NULL },//108
		{ 0,		0.0,		L"-",			NULL },//109
		{ 0,		0.0,		L"-",			NULL },//110
		{ 0,		0.0,		L"-",			NULL },//111
		{ 0,		0.0,		L"-",			NULL },//112
		{ 0,		0.0,		L"-",			NULL },//113
		{ 0,		0.0,		L"-",			NULL },//114
		{ 0,		0.0,		L"-",			NULL },//115
		{ 0,		0.0,		L"-",			NULL },//116
		{ 0,		0.0,		L"-",			NULL },//117
		{ 0,		0.0,		L"-",			NULL },//118
		{ 0,		0.0,		L"-",			NULL },//119
		{ 0,		0.0,		L"-",			NULL },//120
		{ 0,		0.0,		L"-",			NULL },//121
		{ 0,		0.0,		L"-",			NULL },//122
		{ 0,		0.0,		L"-",			NULL },//123
		{ 0x007C,	10000.0,	L"SIN1HZ",		NULL },//124
		{ 0x007D,	10000.0,	L"COS1HZ",		NULL },//125
		{ 0x007E,	10000.0,	L"SIN1HZ05",	NULL },//126
		{ 0x007F,	10000.0,	L"COS1HZ05",	NULL },//127
		{ 0x0880,	0.0,		L"OPECAB_DI00",	NULL },//128 MKLOG_ITEM_OPECAB_DI00 
		{ 0x0881,	0.0,		L"OPECAB_DI01",	NULL },//129 MKLOG_ITEM_OPECAB_DI01 
		{ 0x0882,	0.0,		L"OPECAB_DI02",	NULL },//130 MKLOG_ITEM_OPECAB_DI02 
		{ 0x0883,	0.0,		L"OPECAB_DI03",	NULL },//131 MKLOG_ITEM_OPECAB_DI03 
		{ 0x0884,	0.0,		L"OPECAB_DI04",	NULL },//132 MKLOG_ITEM_OPECAB_DI04 
		{ 0x0885,	0.0,		L"OPECAB_DI05",	NULL },//133 MKLOG_ITEM_OPECAB_DI05 
		{ 0x0886,	0.0,		L"OPECAB_DI06",	NULL },//134 MKLOG_ITEM_OPECAB_DI06 
		{ 0x0887,	0.0,		L"OPECAB_DI07",	NULL },//135 MKLOG_ITEM_OPECAB_DI07 
		{ 0x0888,	0.0,		L"OPECAB_DI08",	NULL },//136 MKLOG_ITEM_OPECAB_DI08 
		{ 0x0889,	0.0,		L"OPECAB_DI09",	NULL },//137 MKLOG_ITEM_OPECAB_DI09 
		{ 0x088A,	0.0,		L"OPECAB_DI10",	NULL },//138 MKLOG_ITEM_OPECAB_DI10 
		{ 0x088B,	0.0,		L"OPECAB_DI11",	NULL },//139 MKLOG_ITEM_OPECAB_DI11 
		{ 0x088C,	0.0,		L"OPECAB_DI12",	NULL },//140 MKLOG_ITEM_OPECAB_DI12 
		{ 0x088D,	0.0,		L"OPECAB_DI13",	NULL },//141 MKLOG_ITEM_OPECAB_DI13 
		{ 0x088E,	0.0,		L"OPECAB_DI14",	NULL },//142 MKLOG_ITEM_OPECAB_DI14 
		{ 0x088F,	0.0,		L"OPECAB_DI15",	NULL },//143 MKLOG_ITEM_OPECAB_DI15 
		{ 0x0890,	0.0,		L"-",			NULL },//144
		{ 0x0891,	0.0,		L"-",			NULL },//145
		{ 0x0892,	0.0,		L"-",			NULL },//146
		{ 0x0893,	0.0,		L"-",			NULL },//147
		{ 0x0894,	0.0,		L"-",			NULL },//148
		{ 0x0895,	0.0,		L"-",			NULL },//149
		{ 0x0896,	0.0,		L"-",			NULL },//150
		{ 0x0897,	0.0,		L"-",			NULL },//151
		{ 0x0898,	0.0,		L"-",			NULL },//152
		{ 0x0899,	0.0,		L"-",			NULL },//153
		{ 0x089A,	0.0,		L"-",			NULL },//154
		{ 0x089B,	0.0,		L"-",			NULL },//155
		{ 0x089C,	0.0,		L"-",			NULL },//156
		{ 0x089D,	0.0,		L"-",			NULL },//157
		{ 0x089E,	0.0,		L"-",			NULL },//158
		{ 0x089F,	0.0,		L"-",			NULL },//159
		{ 0x08A0,	0.0,		L"-",			NULL },//160
		{ 0x08A1,	0.0,		L"-",			NULL },//161
		{ 0x08A2,	0.0,		L"-",			NULL },//162
		{ 0x08A3,	0.0,		L"-",			NULL },//163
		{ 0x08A4,	0.0,		L"-",			NULL },//164
		{ 0x08A5,	0.0,		L"-",			NULL },//165
		{ 0x08A6,	0.0,		L"-",			NULL },//166
		{ 0x08A7,	0.0,		L"-",			NULL },//167
		{ 0x08A8,	0.0,		L"-",			NULL },//168
		{ 0x08A9,	0.0,		L"-",			NULL },//169
		{ 0x08AA,	0.0,		L"-",			NULL },//170
		{ 0x08AB,	0.0,		L"-",			NULL },//171
		{ 0x08AC,	0.0,		L"-",			NULL },//172
		{ 0x08AD,	0.0,		L"-",			NULL },//173
		{ 0x08AE,	0.0,		L"-",			NULL },//174
		{ 0x08AF,	0.0,		L"-",			NULL },//175
		{ 0x08B0,	0.0,		L"-",			NULL },//176
		{ 0x08B1,	0.0,		L"-",			NULL },//177
		{ 0x08B2,	0.0,		L"-",			NULL },//178
		{ 0x08B3,	0.0,		L"-",			NULL },//179
		{ 0x08B4,	0.0,		L"-",			NULL },//180
		{ 0x08B5,	0.0,		L"-",			NULL },//181
		{ 0x08B6,	0.0,		L"-",			NULL },//182
		{ 0x08B7,	0.0,		L"-",			NULL },//183
		{ 0x08B8,	0.0,		L"-",			NULL },//184
		{ 0x08B9,	0.0,		L"-",			NULL },//185
		{ 0x08BA,	0.0,		L"-",			NULL },//186
		{ 0x08BB,	0.0,		L"-",			NULL },//187
		{ 0x08BC,	0.0,		L"-",			NULL },//188
		{ 0x08BD,	0.0,		L"-",			NULL },//189
		{ 0x08BE,	0.0,		L"-",			NULL },//190
		{ 0x08BF,	0.0,		L"-",			NULL },//191
		//BIT 100%値⇒1000.0でグラフの10%で表示(10000が100%表示なので）
		{ 0xC0C0,	800.0,		L"CSOURCE  ",	NULL },//192 MKLOG_ITEM_CONTROL_SOURCE	
		{ 0xC0C1,	800.0,		L"ESTOP    ",	NULL },//193 MKLOG_ITEM_CONTROL_ESTOP
		{ 0xC0C2,	800.0,		L"AutoActSW",	NULL },//194 MKLOG_ITEM_AUTO_ACTIVE_SWITCH
		{ 0xC0C3,	0.0,		L"-",			NULL },//195
		{ 0xC0C4,	0.0,		L"-",			NULL },//196
		{ 0xC0C5,	0.0,		L"-",			NULL },//197
		{ 0xC0C6,	0.0,		L"-",			NULL },//198
		{ 0xC0C7,	0.0,		L"-",			NULL },//199
		{ 0xC0C8,	0.0,		L"-",			NULL },//200
		{ 0xC0C9,	0.0,		L"-",			NULL },//201
		{ 0xC0CA,	0.0,		L"-",			NULL },//202
		{ 0xC0CB,	0.0,		L"-",			NULL },//203
		{ 0xC0CC,	0.0,		L"-",			NULL },//204
		{ 0xC0CD,	0.0,		L"-",			NULL },//205
		{ 0xC0CE,	0.0,		L"-",			NULL },//206
		{ 0xC0CF,	0.0,		L"-",			NULL },//207
		{ 0xC0D0,	0.0,		L"-",			NULL },//208
		{ 0xC0D1,	0.0,		L"-",			NULL },//209
		{ 0xC0D2,	0.0,		L"-",			NULL },//210
		{ 0xC0D3,	0.0,		L"-",			NULL },//211
		{ 0xC0D4,	0.0,		L"-",			NULL },//212
		{ 0xC0D5,	0.0,		L"-",			NULL },//213
		{ 0xC0D6,	0.0,		L"-",			NULL },//214
		{ 0xC0D7,	0.0,		L"-",			NULL },//215
		{ 0xC0D8,	0.0,		L"-",			NULL },//216
		{ 0xC0D9,	0.0,		L"-",			NULL },//217
		{ 0xC0DA,	0.0,		L"-",			NULL },//218
		{ 0xC0DB,	0.0,		L"-",			NULL },//219
		{ 0xC0DC,	0.0,		L"-",			NULL },//220
		{ 0xC0DD,	0.0,		L"-",			NULL },//221
		{ 0xC0DE,	0.0,		L"-",			NULL },//222
		{ 0xC0DF,	0.0,		L"-",			NULL },//223
		{ 0xC0E0,	0.0,		L"-",			NULL },//224
		{ 0xC0E1,	0.0,		L"-",			NULL },//225
		{ 0xC0E2,	0.0,		L"-",			NULL },//226
		{ 0xC0E3,	0.0,		L"-",			NULL },//227
		{ 0xC0E4,	0.0,		L"-",			NULL },//228
		{ 0xC0E5,	0.0,		L"-",			NULL },//229
		{ 0xC0E6,	0.0,		L"-",			NULL },//230
		{ 0xC0E7,	0.0,		L"-",			NULL },//231
		{ 0xC0E8,	0.0,		L"-",			NULL },//232
		{ 0xC0E9,	0.0,		L"-",			NULL },//233
		{ 0xC0EA,	0.0,		L"-",			NULL },//234
		{ 0xC0EB,	0.0,		L"-",			NULL },//235
		{ 0xC0EC,	0.0,		L"-",			NULL },//236
		{ 0xC0ED,	0.0,		L"-",			NULL },//237
		{ 0xC0EE,	0.0,		L"-",			NULL },//238
		{ 0xC0EF,	0.0,		L"-",			NULL },//239
		{ 0xC0F0,	0.0,		L"-",			NULL },//240
		{ 0xC0F1,	0.0,		L"-",			NULL },//241
		{ 0xC0F2,	0.0,		L"-",			NULL },//242
		{ 0xC0F3,	0.0,		L"-",			NULL },//243
		{ 0xC0F4,	0.0,		L"-",			NULL },//244
		{ 0xC0F5,	0.0,		L"-",			NULL },//245
		{ 0xC0F6,	0.0,		L"-",			NULL },//246
		{ 0xC0F7,	0.0,		L"-",			NULL },//247
		{ 0xC0F8,	0.0,		L"-",			NULL },//248
		{ 0xC0F9,	0.0,		L"-",			NULL },//249
		{ 0xC0FA,	0.0,		L"-",			NULL },//250
		{ 0xC0FB,	0.0,		L"-",			NULL },//251
		{ 0xC0FC,	0.0,		L"-",			NULL },//252
		{ 0xC0FD,	0.0,		L"EVENT",		NULL },//253 MKLOG_ITEM_EVENT
		{ 0xC0FE,	800.0,		L"ONOF2Hz",		NULL },//254
		{ 0xC0FF,	800.0,		L"ONOF10Hz",	NULL },//255
	};
}ST_MKLOG_DB, *LPST_MKLOG_DB;
