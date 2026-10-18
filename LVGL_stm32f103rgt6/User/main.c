#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SWITCH/switch.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./BSP/TIMER/btim.h"

#include "./MALLOC/malloc.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/TOUCH/touch.h"



/* LVGL */
#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "lvgl_demo.h"


int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    usmart_dev.init(72);                /* 初始化USMART */
    led_init();                         /* 初始化LED */
		swith1_init();                      /* 初始化SWITCH1 */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
		adc_init();                         /* 初始化ADC1 */
		nrf24l01_init();                    /* 初始化2.4G */
		btim_timx_int_init(99,7199);        /* 定时器初始化 */
	
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    
    lvgl_demo();                        /* 运行FreeRTOS例程 */
}




