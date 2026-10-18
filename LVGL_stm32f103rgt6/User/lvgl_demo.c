#include "lvgl_demo.h"
#include "./BSP/LED/led.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./BSP/KEY/key.h"

#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "stdint.h"
#include "stdio.h"


static lv_point_t line_points1[]={
    {0,65},
    {320,65}
};

static lv_point_t line_points2[]={
    {0,175},
    {320,175}
};
static lv_style_t style;                                 //Ŀ������꣺X,Y
uint8_t tmp_buf_tx[32]={0}; 
uint8_t tmp_buf_rx[16]={0}; 
uint16_t joy_x;
uint16_t joy_y;
uint16_t battery_adc;

int x;  //����Ŀ������
int z;  //����Ŀ������

uint8_t rc_bat;       /* 遥控器电量: 本地ADC测量 (battery_percent) */
uint8_t chassis_bat;  /* 底盘电量: NRF24L01接收 ('P'报文) */

uint8_t auto_switch;      //�Զ�ģʽ�ź�
uint8_t munual_switch;    //�ֶ�ģʽ�ź�
uint8_t stop;             //��ͣ�ź�

static uint32_t power_tick=0;

extern uint8_t munual_control;

lv_obj_t *table1=NULL;
lv_obj_t *table2=NULL;

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO     1           /* �������ȼ� */
#define START_STK_SIZE      128         /* �����ջ��С */
TaskHandle_t StartTask_Handler;         /* ������ */
void start_task(void *pvParameters);    /* ������ */

/* LV_DEMO_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define LV_DEMO_TASK_PRIO   3           /* �������ȼ� */
#define LV_DEMO_STK_SIZE    1024        /* �����ջ��С */
TaskHandle_t LV_DEMOTask_Handler;       /* ������ */
void lv_demo_task(void *pvParameters);  /* ������ */

/* NRF_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define NRF_TASK_PRIO       2           /* �������ȼ� */
#define NRF_STK_SIZE        128         /* �����ջ��С */
TaskHandle_t NRFTask_Handler;           /* ������ */
void nrf_task(void *pvParameters);      /* ������ */

/* LED_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define LED_TASK_PRIO       4           /* �������ȼ� */
#define LED_STK_SIZE        128         /* �����ջ��С */
TaskHandle_t LEDTask_Handler;           /* ������ */
void led_task(void *pvParameters);      /* ������ */
/******************************************************************************************************/

/**
 * @brief       lvgl_demo��ں���
 * @param       ��
 * @retval      ��
 */
void lvgl_demo(void)
{
    lv_init();                                          /* lvglϵͳ��ʼ�� */
    lv_port_disp_init();                                /* lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ��� */
    lv_port_indev_init();                               /* lvgl����ӿڳ�ʼ��,����lv_init()�ĺ��� */

    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */

    vTaskStartScheduler();                              /* ����������� */
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    pvParameters = pvParameters;
    
    taskENTER_CRITICAL();           /* �����ٽ��� */

    /* ����LVGL���� */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);

		/* NRF24L01���� */
    xTaskCreate((TaskFunction_t )nrf_task,
                (const char*    )"nrf_task",
                (uint16_t       )NRF_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )NRF_TASK_PRIO,
                (TaskHandle_t*  )&NRFTask_Handler);
								
								
    /* LED�������� */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);

    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
}

/**
 * @brief       LVGL��������
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void lv_demo_task(void *pvParameters)
{
    pvParameters = pvParameters;
    
	 //������
	lv_obj_t *line1 = lv_line_create(lv_scr_act());
    lv_obj_t *line2 = lv_line_create(lv_scr_act());
    lv_line_set_points(line1,line_points1,2);
    lv_line_set_points(line2,line_points2,2);

	//ת����ʾ�ַ�
    lv_obj_t *label1 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label1,&lv_font_montserrat_48,LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label1,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
	lv_obj_align(label1,LV_ALIGN_TOP_MID,0,10);
		
	//ֹͣ��ʾ�ַ�
	lv_obj_t *label3 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label3,&lv_font_montserrat_48,LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label3,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
	lv_obj_align(label3,LV_ALIGN_BOTTOM_MID,0,-10);
	
    //����С����test1
    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(label2,"test1");
    lv_obj_set_pos(label2,0,75);

    //��������
    lv_style_init(&style);
    lv_style_set_pad_top(&style, 10);     // �����ϱ߾�
    lv_style_set_pad_bottom(&style, 10);  // �����±߾�
	  table1 = lv_table_create(lv_scr_act());   //Ŀ������꣺X,Y
    table2 = lv_table_create(lv_scr_act());    //�ٶȣ�����
    lv_table_set_row_cnt(table1,1);                      //��1����Ϊһ������
    lv_table_set_col_cnt(table1,2);
    lv_table_set_row_cnt(table2,1);                      //��������Ϊ����һ��
    lv_table_set_col_cnt(table2,2);
    lv_table_set_col_width(table1,1,120);
    lv_table_set_col_width(table1,0,120);
    lv_table_set_col_width(table2,0,120);
    lv_table_set_col_width(table2,1,120);
    lv_obj_add_style(table1, &style, LV_PART_ITEMS);  // LV_PART_ITEMS ��ʾӦ�õ���Ԫ��
    lv_obj_add_style(table2, &style, LV_PART_ITEMS);  // LV_PART_ITEMS ��ʾӦ�õ���Ԫ��
    lv_obj_set_pos(table1,0,95);
    lv_obj_set_pos(table2,0,130);
    lv_table_set_cell_value_fmt(table1,0,0,"X : %d mm",0);
    lv_table_set_cell_value_fmt(table1,0,1,"Y : %d mm",0);
	lv_table_set_cell_value_fmt(table2,0,0,"RC : %d %%",0);
	lv_table_set_cell_value_fmt(table2,0,1,"CHAS : %d %%",0);
	  



	//����2.4G��ʾ������ʾ
	lv_obj_t *label4 = lv_label_create(lv_scr_act());
	lv_label_set_text(label4," ");
	lv_obj_set_pos(label4,0,210);

	/* ===== 监控页 page2: 右键进入, 左键返回控制页 ===== */
	lv_obj_t *page2 = lv_obj_create(lv_scr_act());
	lv_obj_set_size(page2, 320, 240);
	lv_obj_set_pos(page2, 0, 0);
	lv_obj_set_style_bg_color(page2, lv_color_hex(0x1e1e2e), 0);
	lv_obj_set_style_bg_opa(page2, LV_OPA_COVER, 0);
	lv_obj_set_style_border_width(page2, 0, 0);
	lv_obj_set_style_pad_all(page2, 12, 0);
	lv_obj_clear_flag(page2, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);              /* 初始隐藏, 先显示控制页 */

	lv_obj_t *mon_title = lv_label_create(page2);
	lv_label_set_text(mon_title, "SYSTEM MONITOR");
	lv_obj_set_style_text_color(mon_title, lv_color_hex(0x7cffb2), 0);
	lv_obj_align(mon_title, LV_ALIGN_TOP_MID, 0, 0);

	/* 遥控器电量 - 圆弧仪表 */
	lv_obj_t *arc_rc = lv_arc_create(page2);
	lv_obj_set_size(arc_rc, 120, 120);
	lv_obj_align(arc_rc, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_arc_set_range(arc_rc, 0, 100);
	lv_arc_set_value(arc_rc, 0);
	lv_obj_clear_flag(arc_rc, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_remove_style(arc_rc, NULL, LV_PART_KNOB);         /* 去掉旋钮, 当进度环用 */

	lv_obj_t *arc_rc_label = lv_label_create(arc_rc);
	lv_label_set_text(arc_rc_label, "RC\n0%");
	lv_obj_align(arc_rc_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_text_align(arc_rc_label, LV_TEXT_ALIGN_CENTER, 0);

	/* 底盘电量 - 进度条 */
	lv_obj_t *bar_base = lv_bar_create(page2);
	lv_obj_set_size(bar_base, 150, 18);
	lv_obj_align(bar_base, LV_ALIGN_TOP_RIGHT, 0, 35);
	lv_bar_set_range(bar_base, 0, 100);

	lv_obj_t *bar_base_label = lv_label_create(page2);
	lv_label_set_text(bar_base_label, "BASE : 0%");
	lv_obj_align_to(bar_base_label, bar_base, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	lv_obj_set_style_text_color(bar_base_label, lv_color_hex(0xcdd6f4), 0);

	/* 状态信息: 模式 / 急停 / 运行时间 */
	lv_obj_t *mon_info = lv_label_create(page2);
	lv_label_set_text(mon_info, "MODE : --\nSTATE : --\nTIME : 00:00:00");
	lv_obj_align(mon_info, LV_ALIGN_TOP_RIGHT, 0, 90);
	lv_obj_set_style_text_color(mon_info, lv_color_hex(0xcdd6f4), 0);
	lv_obj_set_style_text_line_space(mon_info, 6, 0);

	/* 电池图标(状态栏风格, 显示遥控电量, 低电量变红) */
	lv_obj_t *batt_icon = lv_obj_create(page2);
	lv_obj_set_size(batt_icon, 46, 22);
	lv_obj_align(batt_icon, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_set_style_bg_opa(batt_icon, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(batt_icon, 0, 0);
	lv_obj_set_style_pad_all(batt_icon, 0, 0);
	lv_obj_clear_flag(batt_icon, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *batt_body = lv_obj_create(batt_icon);
	lv_obj_set_size(batt_body, 40, 20);
	lv_obj_align(batt_body, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_style_radius(batt_body, 3, 0);
	lv_obj_set_style_bg_opa(batt_body, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_color(batt_body, lv_color_hex(0xcdd6f4), 0);
	lv_obj_set_style_border_width(batt_body, 2, 0);
	lv_obj_set_style_pad_all(batt_body, 2, 0);
	lv_obj_clear_flag(batt_body, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *batt_tip = lv_obj_create(batt_icon);
	lv_obj_set_size(batt_tip, 3, 8);
	lv_obj_align_to(batt_tip, batt_body, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
	lv_obj_set_style_radius(batt_tip, 1, 0);
	lv_obj_set_style_bg_color(batt_tip, lv_color_hex(0xcdd6f4), 0);
	lv_obj_set_style_bg_opa(batt_tip, LV_OPA_COVER, 0);
	lv_obj_set_style_border_width(batt_tip, 0, 0);

	lv_obj_t *batt_fill = lv_obj_create(batt_body);
	lv_obj_set_size(batt_fill, 10, 12);
	lv_obj_align(batt_fill, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_style_radius(batt_fill, 1, 0);
	lv_obj_set_style_bg_color(batt_fill, lv_color_hex(0x7cffb2), 0);
	lv_obj_set_style_bg_opa(batt_fill, LV_OPA_COVER, 0);
	lv_obj_set_style_border_width(batt_fill, 0, 0);
		
	if(nrf24l01_check())	//���NRF24L01�Ƿ���λ.	
	{
		lv_label_set_text(label4,"NRF24L01 Error");
		lv_obj_set_style_text_color(label4,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
		vTaskDelay(100);
	 }
    while(1)
    {

		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
		{
			lv_label_set_text(label1,"AUTO");
			auto_switch = 1;
			munual_switch = 0;
			 
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12))
		{
			lv_label_set_text(label1,"MANUAL");
			auto_switch = 0;
			munual_switch = 1 ;
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8))
		{
			stop = 1;
			lv_label_set_text(label3,"STOP");
			lv_obj_set_style_text_color(label3,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
		}
		else
		{
			stop = 0;
			lv_label_set_text(label3,"GO");
			lv_obj_set_style_text_color(label3,lv_color_hex(0x2E2E2E),LV_STATE_DEFAULT);
		}
		
		if(HAL_GetTick()-power_tick>500)
		{
				rc_bat=battery_percent();
				power_tick=HAL_GetTick();
		}


    lv_table_set_cell_value_fmt(table2,0,0,"RC : %d %%",rc_bat);
    lv_table_set_cell_value_fmt(table2,0,1,"CHAS : %d %%",chassis_bat);
		
		lv_table_set_cell_value_fmt(table1,0,0,"X : %d mm",x);
        lv_table_set_cell_value_fmt(table1,0,1,"Y : %d mm",z);

		/* 左右按键翻页 (while循环50ms间隔, 天然消抖; key_release做按下边沿检测) */
		static uint8_t key_release = 1;
		if(key_release && KEY_RIGHT == 0)
		{
			key_release = 0;
			lv_obj_clear_flag(page2, LV_OBJ_FLAG_HIDDEN);     /* 右键: 进入监控页 */
		}
		else if(key_release && KEY_LEFT == 0)
		{
			key_release = 0;
			lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);       /* 左键: 返回控制页 */
		}
		else if(KEY_RIGHT == 1 && KEY_LEFT == 1) key_release = 1;

		/* 监控页可见时刷新数据 */
		if(!lv_obj_has_flag(page2, LV_OBJ_FLAG_HIDDEN))
		{
			lv_arc_set_value(arc_rc, rc_bat);
			lv_label_set_text_fmt(arc_rc_label, "RC\n%d%%", rc_bat);
			lv_bar_set_value(bar_base, chassis_bat, LV_ANIM_OFF);
			lv_label_set_text_fmt(bar_base_label, "BASE : %d%%", chassis_bat);

			const char *pmode = auto_switch ? "AUTO" : (munual_switch ? "MANUAL" : "IDLE");
			const char *pstate = stop ? "STOP" : "GO";
			uint32_t sec = HAL_GetTick() / 1000;
			lv_label_set_text_fmt(mon_info, "MODE : %s\nSTATE : %s\nTIME : %02d:%02d:%02d",
				pmode, pstate, (int)(sec/3600), (int)((sec%3600)/60), (int)(sec%60));

			/* 电池图标: 按遥控电量填充并变色 (>50绿 / 20~50黄 / <20红) */
			{
				int bw = 32 * rc_bat / 100;
				if(bw < 1 && rc_bat > 0) bw = 1;
				lv_obj_set_width(batt_fill, bw);
				lv_color_t bc = (rc_bat > 50) ? lv_color_hex(0x7cffb2)
					: (rc_bat > 20 ? lv_color_hex(0xf9e2af) : lv_color_hex(0xf38ba8));
				lv_obj_set_style_bg_color(batt_fill, bc, 0);
			}
		}
        lv_timer_handler(); /* LVGL��ʱ�� */
        vTaskDelay(50);
    }
}

/**
 * @brief       nrf_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void nrf_task(void *pvParameters)
{
    pvParameters = pvParameters;
	
    while(1)
    {
			nrf24l01_rx_mode();	
			nrf24l01_rx_packet(tmp_buf_rx);
			
			if((munual_switch||auto_switch) && munual_control)
			{
				 // ��ȡadcֵ��ת��Ϊ�ַ���
				joy_x = adc_get_result(3);
				joy_y = adc_get_result(2);
				if(adc_get_result(3)>3900)             joy_x = 1;         //ǰ��
				else if(adc_get_result(3)<2000)        joy_x = 2;         //���� 
				else                                   joy_x = 0;         //X����ֹ
				if(adc_get_result(2)>3900)             joy_y = 1;         //��ת
				else if(adc_get_result(2)<2000)        joy_y = 2;         //��ת 
				else                                   joy_y = 0;         //Y����ֹ
				
				sprintf((char *)tmp_buf_tx, "X:%4d Y:%4d %d %d %d", joy_x, joy_y, auto_switch, munual_switch, stop); // ��ʽ�� ADC ֵΪ�ַ���
				nrf24l01_tx_mode();
				nrf24l01_tx_packet(tmp_buf_tx);
				munual_control=0;
				
					nrf24l01_rx_mode();	
					nrf24l01_rx_packet(tmp_buf_rx);
			}
			
			if(tmp_buf_rx[0]=='P')
			{
					int vp=0, ip=1;
					while(ip<16 && tmp_buf_rx[ip]>='0' && tmp_buf_rx[ip]<='9')
					{ vp = vp*10 + (tmp_buf_rx[ip]-'0'); ip++; }
					if(vp>100) vp=100;
					chassis_bat = (uint8_t)vp;
			}

			if(tmp_buf_rx[0]=='X')
			{
					parse_buffer(
							tmp_buf_rx,
							&x,
							&z
					);
			 }
			
			 vTaskDelay(10);
		}
}

/**
 * @brief       parse_buffer
 * @param       uint8_t tmp_buf_rx[]:����Ļ�������
 * @param       int *x: ���պ�������
 * @param       int *z: ������������
 * @retval      ��
 */
int parse_buffer(uint8_t tmp_buf_rx[], int *x, int *z) {
    // ���ָ���Ƿ�Ϊ��
    if (tmp_buf_rx == NULL || x == NULL || z == NULL) {
        return -1; // ������Ч
    }

    // ��ʼ��x��zΪ��Чֵ
    *x = 0;
    *z = 0;

    // ������������
    if (tmp_buf_rx[1] == '-') {
        // ��鲻ͬλ�õ�Z�ַ�
        if (tmp_buf_rx[3] == 'Z') {
            *x = -(tmp_buf_rx[2] - '0');
            if (tmp_buf_rx[8] == ' ' && tmp_buf_rx[7] != ' ') {
                *z = (tmp_buf_rx[4] - '0') * 1000 + (tmp_buf_rx[5] - '0') * 100 + (tmp_buf_rx[6] - '0') * 10 + (tmp_buf_rx[7] - '0') * 1;
            } else if (tmp_buf_rx[7] == ' ') {
                *z = (tmp_buf_rx[4] - '0') * 100 + (tmp_buf_rx[5] - '0') * 10 + (tmp_buf_rx[6] - '0') * 1 + (tmp_buf_rx[7] - '0') * 0;
            }
        }

        if (tmp_buf_rx[4] == 'Z') {
            *x = -((tmp_buf_rx[2] - '0') * 10 + (tmp_buf_rx[3] - '0'));
            if (tmp_buf_rx[9] == ' ' && tmp_buf_rx[8] != ' ') {
                *z = (tmp_buf_rx[5] - '0') * 1000 + (tmp_buf_rx[6] - '0') * 100 + (tmp_buf_rx[7] - '0') * 10 + (tmp_buf_rx[8] - '0') * 1;
            } else if (tmp_buf_rx[8] == ' ') {
                *z = (tmp_buf_rx[5] - '0') * 100 + (tmp_buf_rx[6] - '0') * 10 + (tmp_buf_rx[7] - '0') * 1 + (tmp_buf_rx[8] - '0') * 0;
            }
        }

        if (tmp_buf_rx[5] == 'Z') {
            *x = -((tmp_buf_rx[2] - '0') * 100 + (tmp_buf_rx[3] - '0') * 10 + (tmp_buf_rx[4] - '0'));
            if (tmp_buf_rx[10] == ' ' && tmp_buf_rx[9] != ' ') {
                *z = (tmp_buf_rx[6] - '0') * 1000 + (tmp_buf_rx[7] - '0') * 100 + (tmp_buf_rx[8] - '0') * 10 + (tmp_buf_rx[9] - '0') * 1;
            } else if (tmp_buf_rx[9] == ' ') {
                *z = (tmp_buf_rx[6] - '0') * 100 + (tmp_buf_rx[7] - '0') * 10 + (tmp_buf_rx[8] - '0') * 1 + (tmp_buf_rx[9] - '0') * 0;
            }
        }
    }
    // ������������
    else {
        if (tmp_buf_rx[2] == 'Z') {
            *x = tmp_buf_rx[1] - '0';
            if (tmp_buf_rx[7] == ' ' && tmp_buf_rx[6] != ' ') {
                *z = (tmp_buf_rx[3] - '0') * 1000 + (tmp_buf_rx[4] - '0') * 100 + (tmp_buf_rx[5] - '0') * 10 + (tmp_buf_rx[6] - '0') * 1;
            } else if (tmp_buf_rx[6] == ' ') {
                *z = (tmp_buf_rx[3] - '0') * 100 + (tmp_buf_rx[4] - '0') * 10 + (tmp_buf_rx[5] - '0') * 1 + (tmp_buf_rx[6] - '0') * 0;
            }
        }

        if (tmp_buf_rx[3] == 'Z') {
            *x = (tmp_buf_rx[1] - '0') * 10 + (tmp_buf_rx[2] - '0');
            if (tmp_buf_rx[8] == ' ' && tmp_buf_rx[7] != ' ') {
                *z = (tmp_buf_rx[4] - '0') * 1000 + (tmp_buf_rx[5] - '0') * 100 + (tmp_buf_rx[6] - '0') * 10 + (tmp_buf_rx[7] - '0') * 1;
            } else if (tmp_buf_rx[7] == ' ') {
                *z = (tmp_buf_rx[4] - '0') * 100 + (tmp_buf_rx[5] - '0') * 10 + (tmp_buf_rx[6] - '0') * 1 + (tmp_buf_rx[7] - '0') * 0;
            }
        }

        if (tmp_buf_rx[4] == 'Z') {
            *x = (tmp_buf_rx[1] - '0') * 100 + (tmp_buf_rx[2] - '0') * 10 + (tmp_buf_rx[3] - '0');
            if (tmp_buf_rx[9] == ' ' && tmp_buf_rx[8] != ' ') {
                *z = (tmp_buf_rx[5] - '0') * 1000 + (tmp_buf_rx[6] - '0') * 100 + (tmp_buf_rx[7] - '0') * 10 + (tmp_buf_rx[8] - '0') * 1;
            } else if (tmp_buf_rx[8] == ' ') {
                *z = (tmp_buf_rx[5] - '0') * 100 + (tmp_buf_rx[6] - '0') * 10 + (tmp_buf_rx[7] - '0') * 1 + (tmp_buf_rx[8] - '0') * 0;
            }
        }
    }

    return 0; // �����ɹ�
}

/**
 * @brief       led_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void led_task(void *pvParameters)
{
    pvParameters = pvParameters;
    
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(1000);
    }
}


