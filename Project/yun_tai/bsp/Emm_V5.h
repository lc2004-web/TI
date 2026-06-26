#ifndef __EMM_V5_H
#define __EMM_V5_H

#include "usart.h"
#include "delay.h"

/**********************************************************
*** Emm_V5.0 closed-loop stepper driver library
*** Author: ZHANGDATOU
*** Technical Support: Zhangdatou Closed-loop Servo
*** Taobao Store: https://zhangdatou.taobao.com
*** CSDN Blog: https://blog.csdn.net/zhangdatou666
*** QQ Group: 262438510
**********************************************************/

#define                 ABS(x)                          ((x) > 0 ? (x) : -(x))

typedef enum {
    S_VBUS  = 5,    // Read bus voltage
    S_CBUS  = 6,    // Read bus current
    S_CPHA  = 7,    // Read phase current
    S_ENCO  = 8,    // Read encoder raw value
    S_CLKC  = 9,    // Read real-time pulse count
    S_ENCL  = 10,   // Read encoder linear calibration value
    S_CLKI  = 11,   // Read input pulse count
    S_TPOS  = 12,   // Read target position
    S_SPOS  = 13,   // Read real-time set target position
    S_VEL   = 14,   // Read real-time velocity
    S_CPOS  = 15,   // Read real-time position
    S_PERR  = 16,   // Read position error
    S_VBAT  = 17,   // Read coil/battery voltage (Y42)
    S_TEMP  = 18,   // Read real-time temperature (Y42)
    S_FLAG  = 19,   // Read motor status flags
    S_OFLAG = 20,   // Read origin status flags
    S_OAF   = 21,   // Read motor + origin status flags (Y42)
    S_PIN   = 22,   // Read input pin status (Y42)
}SysParams_t;

#define     MMCL_LEN        512
extern __IO uint16_t MMCL_count, MMCL_cmd[MMCL_LEN];

/**********************************************************
*** Single command functions (Y42)
**********************************************************/
/**********************************************************
*** Calibration functions
**********************************************************/
void Emm_V5_Trig_Encoder_Cal(uint8_t addr);
void Emm_V5_Reset_Motor(uint8_t addr);
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr);
void Emm_V5_Reset_Clog_Pro(uint8_t addr);
void Emm_V5_Restore_Motor(uint8_t addr);

/**********************************************************
*** Motion control functions
**********************************************************/
void Emm_V5_Multi_Motor_Cmd(uint8_t addr);
void Emm_V5_En_Control(uint8_t addr, bool state, bool snF);
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, uint8_t raF, bool snF);
void Emm_V5_Set_QPos_Params(uint8_t addr, uint16_t vel, uint8_t acc, uint8_t raF, bool snF);
void Emm_V5_QPos_Control(uint8_t addr, int32_t clk);
void Emm_V5_Stop_Now(uint8_t addr, bool snF);
void Emm_V5_Synchronous_motion(uint8_t addr);

/**********************************************************
*** Origin/Homing functions
**********************************************************/
void Emm_V5_Origin_Set_O(uint8_t addr, bool svF);
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF);
void Emm_V5_Origin_Interrupt(uint8_t addr);
void Emm_V5_Origin_Read_Params(uint8_t addr);
void Emm_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
void X_V2_Origin_Read_SL_RP(uint8_t addr);
void X_V2_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp);

/**********************************************************
*** System parameter read functions
**********************************************************/
void Emm_V5_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms);
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s);

/**********************************************************
*** Parameter read/write functions
**********************************************************/
void Emm_V5_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id);
void Emm_V5_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep);
void Emm_V5_Modify_PDFlag(uint8_t addr, bool pdf);
void Emm_V5_Read_Opt_Param_Sta(uint8_t addr);
void Emm_V5_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype);
void Emm_V5_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype);
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode);
void Emm_V5_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir);
void Emm_V5_Modify_Lock_Btn(uint8_t addr, bool svF, bool lockbtn);
void Emm_V5_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel);
void Emm_V5_Modify_OM_ma(uint8_t addr, bool svF, uint16_t om_ma);
void Emm_V5_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA);
void Emm_V5_Read_PID_Params(uint8_t addr);
void Emm_V5_Modify_PID_Params(uint8_t addr, bool svF, uint32_t kp, uint32_t ki, uint32_t kd);
void Emm_V5_Read_DMX512_Params(uint8_t addr);
void Emm_V5_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step);
void Emm_V5_Read_Pos_Window(uint8_t addr);
void Emm_V5_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw);
void Emm_V5_Read_Otocp(uint8_t addr);
void Emm_V5_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms);
void Emm_V5_Read_Heart_Protect(uint8_t addr);
void Emm_V5_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp);
void Emm_V5_Read_Integral_Limit(uint8_t addr);
void Emm_V5_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il);

/**********************************************************
*** System state read functions
**********************************************************/
void Emm_V5_Read_System_State_Params(uint8_t addr);
void Emm_V5_Read_Motor_Conf_Params(uint8_t addr);

/**********************************************************
*** Multi-Motor Command List (MMCL) functions
*** Append commands to buffer, flush with Multi_Motor_Cmd
**********************************************************/
/**********************************************************
*** Calibration functions (MMCL)
**********************************************************/
void Emm_V5_MMCL_Trig_Encoder_Cal(uint8_t addr);
void Emm_V5_MMCL_Reset_Motor(uint8_t addr);
void Emm_V5_MMCL_Reset_CurPos_To_Zero(uint8_t addr);
void Emm_V5_MMCL_Reset_Clog_Pro(uint8_t addr);
void Emm_V5_MMCL_Restore_Motor(uint8_t addr);

/**********************************************************
*** Motion control functions (MMCL)
**********************************************************/
void Emm_V5_MMCL_En_Control(uint8_t addr, bool state, bool snF);
void Emm_V5_MMCL_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);
void Emm_V5_MMCL_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, uint8_t raF, bool snF);
void Emm_V5_MMCL_Set_QPos_Params(uint8_t addr, uint16_t vel, uint8_t acc, uint8_t raF, bool snF);
void Emm_V5_MMCL_QPos_Control(uint8_t addr, int32_t clk);
void Emm_V5_MMCL_Stop_Now(uint8_t addr, bool snF);
void Emm_V5_MMCL_Synchronous_motion(uint8_t addr);

/**********************************************************
*** Origin functions (MMCL)
**********************************************************/
void Emm_V5_MMCL_Origin_Set_O(uint8_t addr, bool svF);
void Emm_V5_MMCL_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF);
void Emm_V5_MMCL_Origin_Interrupt(uint8_t addr);
void Emm_V5_MMCL_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
void X_V2_MMCL_Origin_Read_SL_RP(uint8_t addr);
void X_V2_MMCL_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp);

/**********************************************************
*** System parameter read functions (MMCL)
**********************************************************/
void Emm_V5_MMCL_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms);
void Emm_V5_MMCL_Read_Sys_Params(uint8_t addr, SysParams_t s);

#endif
