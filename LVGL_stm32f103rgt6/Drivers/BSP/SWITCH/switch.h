#ifndef _SWITCH_H
#define _SWITCH_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */
//PA11 手动
//PA12 自动
//PA8  急停

#define Switch1_GPIO_PORT                  GPIOA
#define Switch1_GPIO_PIN                   GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_8
#define Switch1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PA口时钟使能 */


/******************************************************************************************/
/* 外部接口函数*/
void swith1_init(void);                                                                            /* 初始化 */

#endif
