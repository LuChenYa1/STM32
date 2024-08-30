#include "Emm_V5.h"
#include "usart.h"
//#include "main.h"

extern uint8_t TX_Uart1[50];
/**
  * @brief    位置模式
  * @param    addr：电机地址
  * @param    dir ：方向        ，0为CW，其余值为CCW
  * @param    vel ：速度(RPM)   ，范围0 - 5000RPM
  * @param    acc ：加速度      ，范围0 - 255，注意：0是直接启动
  * @param    clk ：脉冲数      ，范围0- (2^32 - 1)个
  * @param    raF ：相位/绝对标志，false为相对运动，true为绝对值运动
  * @param    snF ：多机同步标志 ，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Position_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, uint8_t raF, uint8_t snF)
{
	TX_Uart1[0] = addr;
	TX_Uart1[1] = 0xFD;
	TX_Uart1[2] = dir;
	TX_Uart1[3]  =  (uint8_t)(vel >> 8);         // 速度(RPM)高8位字节
    TX_Uart1[4]  =  (uint8_t)(vel >> 0);         // 速度(RPM)低8位字节 
	TX_Uart1[5]  =  acc;                         // 加速度，注意：0是直接启动
	TX_Uart1[6]  =  (uint8_t)(clk >> 24);        // 脉冲数(bit24 - bit31)
    TX_Uart1[7]  =  (uint8_t)(clk >> 16);        // 脉冲数(bit16 - bit23)
    TX_Uart1[8]  =  (uint8_t)(clk >> 8);         // 脉冲数(bit8  - bit15)
    TX_Uart1[9]  =  (uint8_t)(clk >> 0);         // 脉冲数(bit0  - bit7 )
	TX_Uart1[10] =  raF;                         // 相位/绝对标志，false为相对运动，true为绝对值运动
    TX_Uart1[11] =  snF;                         // 多机同步运动标志，false为不启用，true为启用
    TX_Uart1[12] =  0x6B;                        // 校验字节
	//发送命令
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 13);
}

/**
  * @brief    触发回零
  * @param    addr   ：电机地址
  * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  * @param    snF   ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Return_Origin(uint8_t addr, uint8_t o_mode, uint8_t snF)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x9A;                       // 功能码
  TX_Uart1[2] =  o_mode;                     // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  TX_Uart1[3] =  snF;                        // 多机同步运动标志，false为不启用，true为启用
  TX_Uart1[4] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    立即停止（所有控制模式都通用）
  * @param    addr  ：电机地址
  * @param    snF   ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Stop_Now(uint8_t addr, uint8_t snF)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0xFE;                       // 功能码
  TX_Uart1[2] =  0x98;                       // 辅助码
  TX_Uart1[3] =  snF;                        // 多机同步运动标志
  TX_Uart1[4] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    多机同步运动
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Synchronous_Action(uint8_t addr)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0xFF;                       // 功能码
  TX_Uart1[2] =  0x66;                       // 辅助码
  TX_Uart1[3] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    将当前位置清零
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Set_CurentPostion_Zero(uint8_t addr)
{
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x0A;                       // 功能码
  TX_Uart1[2] =  0x6D;                       // 辅助码
  TX_Uart1[3] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    使能信号控制
  * @param    addr  ：电机地址
  * @param    state ：使能状态     ，true为使能电机，false为关闭电机
  * @param    snF   ：多机同步标志 ，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_En_Control(uint8_t addr, uint8_t state, uint8_t snF)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0xF3;                       // 功能码
  TX_Uart1[2] =  0xAB;                       // 辅助码
  TX_Uart1[3] =  (uint8_t)state;             // 使能状态
  TX_Uart1[4] =  snF;                        // 多机同步运动标志
  TX_Uart1[5] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 6);
}

/**
  * @brief    解除堵转保护
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Reset_Clog_Pro(uint8_t addr)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x0E;                       // 功能码
  TX_Uart1[2] =  0x52;                       // 辅助码
  TX_Uart1[3] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    读取系统参数
  * @param    addr  ：电机地址
  * @param    s     ：系统参数类型
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
  uint8_t i = 0;
  
  // 装载命令
  TX_Uart1[i] = addr; ++i;                   // 地址

  switch(s)                             // 功能码
  {
    case S_VER  : TX_Uart1[i] = 0x1F; ++i; break;
    case S_RL   : TX_Uart1[i] = 0x20; ++i; break;
    case S_PID  : TX_Uart1[i] = 0x21; ++i; break;
    case S_VBUS : TX_Uart1[i] = 0x24; ++i; break;
    case S_CPHA : TX_Uart1[i] = 0x27; ++i; break;
    case S_ENCL : TX_Uart1[i] = 0x31; ++i; break;
    case S_TPOS : TX_Uart1[i] = 0x33; ++i; break;
    case S_VEL  : TX_Uart1[i] = 0x35; ++i; break;
    case S_CPOS : TX_Uart1[i] = 0x36; ++i; break;
    case S_PERR : TX_Uart1[i] = 0x37; ++i; break;
    case S_FLAG : TX_Uart1[i] = 0x3A; ++i; break;
    case S_ORG  : TX_Uart1[i] = 0x3B; ++i; break;
    case S_Conf : TX_Uart1[i] = 0x42; ++i; TX_Uart1[i] = 0x6C; ++i; break;
    case S_State: TX_Uart1[i] = 0x43; ++i; TX_Uart1[i] = 0x7A; ++i; break;
    default: break;
  }

  TX_Uart1[i] = 0x6B; ++i;                   // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, i);
}

/**
  * @brief    修改开环/闭环控制模式
  * @param    addr     ：电机地址
  * @param    svF      ：是否存储标志，false为不存储，true为存储
  * @param    ctrl_mode：控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, uint8_t svF, uint8_t ctrl_mode)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x46;                       // 功能码
  TX_Uart1[2] =  0x69;                       // 辅助码
  TX_Uart1[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
  TX_Uart1[4] =  ctrl_mode;                  // 控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
  TX_Uart1[5] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 6);
}

/**
  * @brief    速度模式
  * @param    addr：电机地址
  * @param    dir ：方向       ，0为CW，其余值为CCW
  * @param    vel ：速度       ，范围0 - 5000RPM
  * @param    acc ：加速度     ，范围0 - 255，注意：0是直接启动
  * @param    snF ：多机同步标志，false为不启用，true为启用
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint8_t snF)
{

  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0xF6;                       // 功能码
  TX_Uart1[2] =  dir;                        // 方向
  TX_Uart1[3] =  (uint8_t)(vel >> 8);        // 速度(RPM)高8位字节
  TX_Uart1[4] =  (uint8_t)(vel >> 0);        // 速度(RPM)低8位字节
  TX_Uart1[5] =  acc;                        // 加速度，注意：0是直接启动
  TX_Uart1[6] =  snF;                        // 多机同步运动标志
  TX_Uart1[7] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 8);
}



/**
  * @brief    设置单圈回零的零点位置
  * @param    addr  ：电机地址
  * @param    svF   ：是否存储标志，false为不存储，true为存储
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Set_O(uint8_t addr, uint8_t svF)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x93;                       // 功能码
  TX_Uart1[2] =  0x88;                       // 辅助码
  TX_Uart1[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
  TX_Uart1[4] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    修改回零参数
  * @param    addr  ：电机地址
  * @param    svF   ：是否存储标志，false为不存储，true为存储
  * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  * @param    o_dir  ：回零方向，0为CW，其余值为CCW
  * @param    o_vel  ：回零速度，单位：RPM（转/分钟）
  * @param    o_tm   ：回零超时时间，单位：毫秒
  * @param    sl_vel ：无限位碰撞回零检测转速，单位：RPM（转/分钟）
  * @param    sl_ma  ：无限位碰撞回零检测电流，单位：Ma（毫安）
  * @param    sl_ms  ：无限位碰撞回零检测时间，单位：Ms（毫秒）
  * @param    potF   ：上电自动触发回零，false为不使能，true为使能
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Modify_Params(uint8_t addr, uint8_t svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, uint8_t potF)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x4C;                       // 功能码
  TX_Uart1[2] =  0xAE;                       // 辅助码
  TX_Uart1[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
  TX_Uart1[4] =  o_mode;                     // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
  TX_Uart1[5] =  o_dir;                      // 回零方向
  TX_Uart1[6]  =  (uint8_t)(o_vel >> 8);     // 回零速度(RPM)高8位字节
  TX_Uart1[7]  =  (uint8_t)(o_vel >> 0);     // 回零速度(RPM)低8位字节 
  TX_Uart1[8]  =  (uint8_t)(o_tm >> 24);     // 回零超时时间(bit24 - bit31)
  TX_Uart1[9]  =  (uint8_t)(o_tm >> 16);     // 回零超时时间(bit16 - bit23)
  TX_Uart1[10] =  (uint8_t)(o_tm >> 8);      // 回零超时时间(bit8  - bit15)
  TX_Uart1[11] =  (uint8_t)(o_tm >> 0);      // 回零超时时间(bit0  - bit7 )
  TX_Uart1[12] =  (uint8_t)(sl_vel >> 8);    // 无限位碰撞回零检测转速(RPM)高8位字节
  TX_Uart1[13] =  (uint8_t)(sl_vel >> 0);    // 无限位碰撞回零检测转速(RPM)低8位字节 
  TX_Uart1[14] =  (uint8_t)(sl_ma >> 8);     // 无限位碰撞回零检测电流(Ma)高8位字节
  TX_Uart1[15] =  (uint8_t)(sl_ma >> 0);     // 无限位碰撞回零检测电流(Ma)低8位字节 
  TX_Uart1[16] =  (uint8_t)(sl_ms >> 8);     // 无限位碰撞回零检测时间(Ms)高8位字节
  TX_Uart1[17] =  (uint8_t)(sl_ms >> 0);     // 无限位碰撞回零检测时间(Ms)低8位字节
  TX_Uart1[18] =  potF;                      // 上电自动触发回零，false为不使能，true为使能
  TX_Uart1[19] =  0x6B;                      // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 20);
}

/**
  * @brief    强制中断并退出回零
  * @param    addr  ：电机地址
  * @retval   地址 + 功能码 + 命令状态 + 校验字节
  */
void Emm_V5_Origin_Interrupt(uint8_t addr)
{
  
  // 装载命令
  TX_Uart1[0] =  addr;                       // 地址
  TX_Uart1[1] =  0x9C;                       // 功能码
  TX_Uart1[2] =  0x48;                       // 辅助码
  TX_Uart1[3] =  0x6B;                       // 校验字节
  
  // 发送命令
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}
