#include "./BSP/SWITCH/switch.h"


/**
 * @brief       初始化switch1相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void swith1_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    Switch1_GPIO_CLK_ENABLE();                              /* Switch1时钟使能 */


    gpio_init_struct.Pin = Switch1_GPIO_PIN;                /* SWITCH1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                  /* 下拉 */
    
    HAL_GPIO_Init(Switch1_GPIO_PORT, &gpio_init_struct);       /* 初始化LED0引脚 */
	
	  HAL_GPIO_WritePin(Switch1_GPIO_PORT, Switch1_GPIO_PIN, GPIO_PIN_RESET);
}

