#include "CCcScad.h"
#include "resource.h"

extern CCrane* pCrane;

//*************************************************************************************************************
// 
// SCADAタスク　Logデータ処理関連関数
// 
// 
//*************************************************************************************************************
    //LOG記録データセット用関数
INT16 CScada::empty(double d100) { return 0; }
//位置FB
INT16 CScada::pos_fb_MH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_HOIST].pos_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::pos_fb_GT(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_GANTRY].pos_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::pos_fb_BH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_BOOM_H].pos_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::pos_fb_SL(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_SLEW].pos_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::pos_fb_AH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_AHOIST].pos_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}

//速度FB
INT16 CScada::v_fb_MH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_HOIST].v_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::v_fb_GT(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_GANTRY].v_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::v_fb_BH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_BOOM_H].v_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::v_fb_SL(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_SLEW].v_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}
INT16 CScada::v_fb_AH(double d100) {
    if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_AHOIST].v_fb / d100 * MKLOG_PARAM_DATA_100PER_D);
}

INT16 CScada::v_ref_MH(double d100) { if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_HOIST].v_ref / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::v_ref_GT(double d100) { if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_GANTRY].v_ref / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::v_ref_BH(double d100) { if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_BOOM_H].v_ref / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::v_ref_SL(double d100) { if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_SLEW].v_ref / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::v_ref_AH(double d100) { if (d100 == 0.0)return 0; return INT16(pPLC_IO->stat_axis[ID_AHOIST].v_ref / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::swy1_TH_X(double d100) { if (d100 == 0.0)return 0; return  INT16((pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].p - 1024) / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1_TH_Y(double d100) { if (d100 == 0.0)return 0; return  INT16((pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].p - 768)/ d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1DTH_X(double d100) { if (d100 == 0.0)return 0; return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].v / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1DTH_Y(double d100) { if (d100 == 0.0)return 0; return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].v / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::swy1DTHW_X(double d100) { if (d100 == 0.0)return 0; return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].vw / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1DTHW_Y(double d100) { if (d100 == 0.0)return 0; return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].vw / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::swy1_Tx(double d100) { if (d100 == 0.0)       return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].T / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1_Ty(double d100) { if (d100 == 0.0)       return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].T / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYAMP_CAL_X(double d100) { if (d100 == 0.0)  return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].amp_cal / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYAMP_CAL_Y(double d100) { if (d100 == 0.0)  return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].amp_cal / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYps_cal_X(double d100) { if (d100 == 0.0)   return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].ps_cal / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYps_cal_Y(double d100) { if (d100 == 0.0)   return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].ps_cal / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::swy1SWYAMP_P2P_X(double d100) { if (d100 == 0.0)  return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].amp_p2p / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYAMP_P2P_Y(double d100) { if (d100 == 0.0)  return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].amp_p2p / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYps_time_X(double d100) { if (d100 == 0.0)   return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::X].ps_time / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::swy1SWYps_time_Y(double d100) { if (d100 == 0.0)   return 0;   return  INT16(pAUX_CS_Inf->msg_server.body.sway_data[(int)ENUM_AXIS::Y].ps_time / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::l_rope_mh(double d100) { if (d100 == 0.0)return 0; return INT16(pEnv_Inf->l_mh / d100 * MKLOG_PARAM_DATA_100PER_D); }
INT16 CScada::l_rope_ah(double d100) { if (d100 == 0.0)return 0; return INT16(pEnv_Inf->l_ah / d100 * MKLOG_PARAM_DATA_100PER_D); }

INT16 CScada::plc_fault(double dindex)  { return 0; return INT16(pPLC_IO->pflt_buf[(int)dindex]); }
INT16 CScada::plc_cab_bi(double dindex) { return 0; return INT16(pPLC_IO->pcab_bi[(int)dindex]); }

INT16 CScada::control_source(double d100) { if (pPLC_IO->ctrl_source) return 0; else return (INT16)d100; }
INT16 CScada::e_stop(double d100) { if (pPLC_IO->e_stop_pb_active) return (INT16)d100; else return 0; }
INT16 CScada::auto_active_sw(double d100) { if (pEnv_Inf->auto_active_sw) return (INT16)d100; else return 0; }
INT16 CScada::slbrk_pswitch(double d100) { if (pPLC_IO->stat_axis[ID_SLEW].brake) return (INT16)d100; else return 0; }

INT16 CScada::notch_com_mh(double d100) { return  pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::notch_mh] * MKLOG_PARAM_DATA_100PER_D / (INT16)d100;
}  //notch(-4～4）*d100　d100=5.0をデフォルト
INT16 CScada::notch_com_gt(double d100) { return  pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::notch_gt] * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_com_bh(double d100) { return  pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::notch_bh] * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_com_sl(double d100) { return  pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::notch_sl] * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_com_ah(double d100) { return  pOTE_Inf->st_msg_ote_u_rcv.body.st.pnl_ctrl[OTE_PNL_CTRLS::notch_ah] * MKLOG_PARAM_DATA_100PER_D / (INT16)d100 ; }

INT16 CScada::notch_fb_mh(double d100) { return pPLC_IO->stat_axis[ID_HOIST].notch_ref * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }  //notch(-4～4）*d100　d100=4.0をデフォルト
INT16 CScada::notch_fb_gt(double d100) { return pPLC_IO->stat_axis[ID_GANTRY].notch_ref * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_fb_bh(double d100) { return pPLC_IO->stat_axis[ID_BOOM_H].notch_ref * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_fb_sl(double d100) { return pPLC_IO->stat_axis[ID_SLEW].notch_ref * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }
INT16 CScada::notch_fb_ah(double d100) { return pPLC_IO->stat_axis[ID_AHOIST].notch_ref * MKLOG_PARAM_DATA_100PER_D / (INT16)d100; }

INT16 CScada::onoff2hz(double d100) { if ((systime.wSecond % 4) == 0) return(INT16)d100; else if (systime.wSecond % 2 == 0)return(INT16)-d100; else; return 0; }
INT16 CScada::onoff10hz(double d100) { if ((systime.wSecond % 10) < 5) return(INT16)d100; return 0; }

INT16 CScada::sin1hz(double d100) { return (INT16)(d100 * sin((double)systime.wMilliseconds / 1000.0 * PI360)); }
INT16 CScada::cos1hz(double d100) { return (INT16)(d100 * cos((double)systime.wMilliseconds / 1000.0 * PI360 + PI45)); }
INT16 CScada::sin1hz01hz(double d100) {
    double th = systime.wMilliseconds / 1000.0 * PI360;
    double thm = double(log_elapse_count[MKLOG_ID_TYPE_EVENT] % 400) / 400.0 * PI360;
    return (INT16)(d100 * sin(th) + 0.5 * d100 * cos(thm));
}
INT16 CScada::cos1hz01hz(double d100) {
    double th = systime.wMilliseconds / 1000.0 * PI360;
    double thm = double(log_elapse_count[MKLOG_ID_TYPE_EVENT] % 400) / 400.0 * PI360;
    return (INT16)(d100 * cos(th) + 0.5 * d100 * cos(thm));
}