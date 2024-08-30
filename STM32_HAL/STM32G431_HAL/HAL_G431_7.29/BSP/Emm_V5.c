#include "Emm_V5.h"
#include "usart.h"
//#include "main.h"

extern uint8_t TX_Uart1[50];
/**
  * @brief    λ��ģʽ
  * @param    addr�������ַ
  * @param    dir ������        ��0ΪCW������ֵΪCCW
  * @param    vel ���ٶ�(RPM)   ����Χ0 - 5000RPM
  * @param    acc �����ٶ�      ����Χ0 - 255��ע�⣺0��ֱ������
  * @param    clk ��������      ����Χ0- (2^32 - 1)��
  * @param    raF ����λ/���Ա�־��falseΪ����˶���trueΪ����ֵ�˶�
  * @param    snF �����ͬ����־ ��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Position_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, uint8_t raF, uint8_t snF)
{
	TX_Uart1[0] = addr;
	TX_Uart1[1] = 0xFD;
	TX_Uart1[2] = dir;
	TX_Uart1[3]  =  (uint8_t)(vel >> 8);         // �ٶ�(RPM)��8λ�ֽ�
    TX_Uart1[4]  =  (uint8_t)(vel >> 0);         // �ٶ�(RPM)��8λ�ֽ� 
	TX_Uart1[5]  =  acc;                         // ���ٶȣ�ע�⣺0��ֱ������
	TX_Uart1[6]  =  (uint8_t)(clk >> 24);        // ������(bit24 - bit31)
    TX_Uart1[7]  =  (uint8_t)(clk >> 16);        // ������(bit16 - bit23)
    TX_Uart1[8]  =  (uint8_t)(clk >> 8);         // ������(bit8  - bit15)
    TX_Uart1[9]  =  (uint8_t)(clk >> 0);         // ������(bit0  - bit7 )
	TX_Uart1[10] =  raF;                         // ��λ/���Ա�־��falseΪ����˶���trueΪ����ֵ�˶�
    TX_Uart1[11] =  snF;                         // ���ͬ���˶���־��falseΪ�����ã�trueΪ����
    TX_Uart1[12] =  0x6B;                        // У���ֽ�
	//��������
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 13);
}

/**
  * @brief    ��������
  * @param    addr   �������ַ
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Return_Origin(uint8_t addr, uint8_t o_mode, uint8_t snF)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x9A;                       // ������
  TX_Uart1[2] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  TX_Uart1[3] =  snF;                        // ���ͬ���˶���־��falseΪ�����ã�trueΪ����
  TX_Uart1[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    ����ֹͣ�����п���ģʽ��ͨ�ã�
  * @param    addr  �������ַ
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Stop_Now(uint8_t addr, uint8_t snF)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0xFE;                       // ������
  TX_Uart1[2] =  0x98;                       // ������
  TX_Uart1[3] =  snF;                        // ���ͬ���˶���־
  TX_Uart1[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    ���ͬ���˶�
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Synchronous_Action(uint8_t addr)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0xFF;                       // ������
  TX_Uart1[2] =  0x66;                       // ������
  TX_Uart1[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    ����ǰλ������
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Set_CurentPostion_Zero(uint8_t addr)
{
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x0A;                       // ������
  TX_Uart1[2] =  0x6D;                       // ������
  TX_Uart1[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    ʹ���źſ���
  * @param    addr  �������ַ
  * @param    state ��ʹ��״̬     ��trueΪʹ�ܵ����falseΪ�رյ��
  * @param    snF   �����ͬ����־ ��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_En_Control(uint8_t addr, uint8_t state, uint8_t snF)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0xF3;                       // ������
  TX_Uart1[2] =  0xAB;                       // ������
  TX_Uart1[3] =  (uint8_t)state;             // ʹ��״̬
  TX_Uart1[4] =  snF;                        // ���ͬ���˶���־
  TX_Uart1[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 6);
}

/**
  * @brief    �����ת����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Reset_Clog_Pro(uint8_t addr)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x0E;                       // ������
  TX_Uart1[2] =  0x52;                       // ������
  TX_Uart1[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}

/**
  * @brief    ��ȡϵͳ����
  * @param    addr  �������ַ
  * @param    s     ��ϵͳ��������
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
  uint8_t i = 0;
  
  // װ������
  TX_Uart1[i] = addr; ++i;                   // ��ַ

  switch(s)                             // ������
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

  TX_Uart1[i] = 0x6B; ++i;                   // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, i);
}

/**
  * @brief    �޸Ŀ���/�ջ�����ģʽ
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    ctrl_mode������ģʽ����Ӧ��Ļ�ϵ�P_Pul�˵�����0�ǹر������������ţ�1�ǿ���ģʽ��2�Ǳջ�ģʽ��3����En�˿ڸ���Ϊ��Ȧ��λ�����������ţ�Dir�˿ڸ���Ϊ��λ����ߵ�ƽ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, uint8_t svF, uint8_t ctrl_mode)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x46;                       // ������
  TX_Uart1[2] =  0x69;                       // ������
  TX_Uart1[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  TX_Uart1[4] =  ctrl_mode;                  // ����ģʽ����Ӧ��Ļ�ϵ�P_Pul�˵�����0�ǹر������������ţ�1�ǿ���ģʽ��2�Ǳջ�ģʽ��3����En�˿ڸ���Ϊ��Ȧ��λ�����������ţ�Dir�˿ڸ���Ϊ��λ����ߵ�ƽ����
  TX_Uart1[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 6);
}

/**
  * @brief    �ٶ�ģʽ
  * @param    addr�������ַ
  * @param    dir ������       ��0ΪCW������ֵΪCCW
  * @param    vel ���ٶ�       ����Χ0 - 5000RPM
  * @param    acc �����ٶ�     ����Χ0 - 255��ע�⣺0��ֱ������
  * @param    snF �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint8_t snF)
{

  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0xF6;                       // ������
  TX_Uart1[2] =  dir;                        // ����
  TX_Uart1[3] =  (uint8_t)(vel >> 8);        // �ٶ�(RPM)��8λ�ֽ�
  TX_Uart1[4] =  (uint8_t)(vel >> 0);        // �ٶ�(RPM)��8λ�ֽ�
  TX_Uart1[5] =  acc;                        // ���ٶȣ�ע�⣺0��ֱ������
  TX_Uart1[6] =  snF;                        // ���ͬ���˶���־
  TX_Uart1[7] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 8);
}



/**
  * @brief    ���õ�Ȧ��������λ��
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Origin_Set_O(uint8_t addr, uint8_t svF)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x93;                       // ������
  TX_Uart1[2] =  0x88;                       // ������
  TX_Uart1[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  TX_Uart1[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 5);
}

/**
  * @brief    �޸Ļ������
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    o_dir  �����㷽��0ΪCW������ֵΪCCW
  * @param    o_vel  �������ٶȣ���λ��RPM��ת/���ӣ�
  * @param    o_tm   �����㳬ʱʱ�䣬��λ������
  * @param    sl_vel ������λ��ײ������ת�٣���λ��RPM��ת/���ӣ�
  * @param    sl_ma  ������λ��ײ�������������λ��Ma��������
  * @param    sl_ms  ������λ��ײ������ʱ�䣬��λ��Ms�����룩
  * @param    potF   ���ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Origin_Modify_Params(uint8_t addr, uint8_t svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, uint8_t potF)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x4C;                       // ������
  TX_Uart1[2] =  0xAE;                       // ������
  TX_Uart1[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  TX_Uart1[4] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  TX_Uart1[5] =  o_dir;                      // ���㷽��
  TX_Uart1[6]  =  (uint8_t)(o_vel >> 8);     // �����ٶ�(RPM)��8λ�ֽ�
  TX_Uart1[7]  =  (uint8_t)(o_vel >> 0);     // �����ٶ�(RPM)��8λ�ֽ� 
  TX_Uart1[8]  =  (uint8_t)(o_tm >> 24);     // ���㳬ʱʱ��(bit24 - bit31)
  TX_Uart1[9]  =  (uint8_t)(o_tm >> 16);     // ���㳬ʱʱ��(bit16 - bit23)
  TX_Uart1[10] =  (uint8_t)(o_tm >> 8);      // ���㳬ʱʱ��(bit8  - bit15)
  TX_Uart1[11] =  (uint8_t)(o_tm >> 0);      // ���㳬ʱʱ��(bit0  - bit7 )
  TX_Uart1[12] =  (uint8_t)(sl_vel >> 8);    // ����λ��ײ������ת��(RPM)��8λ�ֽ�
  TX_Uart1[13] =  (uint8_t)(sl_vel >> 0);    // ����λ��ײ������ת��(RPM)��8λ�ֽ� 
  TX_Uart1[14] =  (uint8_t)(sl_ma >> 8);     // ����λ��ײ���������(Ma)��8λ�ֽ�
  TX_Uart1[15] =  (uint8_t)(sl_ma >> 0);     // ����λ��ײ���������(Ma)��8λ�ֽ� 
  TX_Uart1[16] =  (uint8_t)(sl_ms >> 8);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  TX_Uart1[17] =  (uint8_t)(sl_ms >> 0);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  TX_Uart1[18] =  potF;                      // �ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  TX_Uart1[19] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 20);
}

/**
  * @brief    ǿ���жϲ��˳�����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void Emm_V5_Origin_Interrupt(uint8_t addr)
{
  
  // װ������
  TX_Uart1[0] =  addr;                       // ��ַ
  TX_Uart1[1] =  0x9C;                       // ������
  TX_Uart1[2] =  0x48;                       // ������
  TX_Uart1[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)TX_Uart1, 4);
}
