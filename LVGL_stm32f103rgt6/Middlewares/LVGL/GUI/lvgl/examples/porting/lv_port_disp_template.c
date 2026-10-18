/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "../../lvgl.h"
/* ����lcd����ͷ�ļ� */
#include "./BSP/LCD/lcd.h"

/*********************
 *      DEFINES
 *********************/
#define USE_SRAM        0       /* ʹ���ⲿsramΪ1������Ϊ0 */
#ifdef USE_SRAM
#include "./MALLOC/malloc.h"
#endif

#define MY_DISP_HOR_RES (320)   /* ��Ļ���� */
#define MY_DISP_VER_RES (240)   /* ��Ļ�߶� */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/* ��ʾ�豸��ʼ������ */
static void disp_init(void);

/* ��ʾ�豸ˢ�º��� */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
/* GPU ��亯��(ʹ��GPUʱ����Ҫʵ��) */
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief       LCD���ٻ��ƺ���
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:Ҫ������ɫ
 * @retval      ��
 */
void lcd_draw_fast_rgb_color(int16_t sx, int16_t sy,int16_t ex, int16_t ey, uint16_t *color)
{
    uint16_t w = ex-sx+1;
    uint16_t h = ey-sy+1;

    lcd_set_window(sx, sy, w, h);
    uint32_t draw_size = w * h;
    lcd_write_ram_prepare();

    for(uint32_t i = 0; i < draw_size; i++)
    {
        lcd_wr_data(color[i]);
    }
}

/**
 * @brief       ��ʼ����ע����ʾ�豸
 * @param       ��
 * @retval      ��
 */
void lv_port_disp_init(void)
{
    /*-------------------------
     * ��ʼ����ʾ�豸
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * ����һ����ͼ������
     *----------------------------*/

    /**
     * LVGL ��Ҫһ����������������С����
     * �����������������ݻ�ͨ����ʾ�豸�� `flush_cb`(��ʾ�豸ˢ�º���) ���Ƶ���ʾ�豸��
     * ����������Ĵ�С��Ҫ������ʾ�豸һ�еĴ�С
     *
     * ������3�л�������:
     * 1. ��������:
     *      LVGL �Ὣ��ʾ�豸�����ݻ��Ƶ����������д����ʾ�豸��
     *
     * 2. ˫������:
     *      LVGL �Ὣ��ʾ�豸�����ݻ��Ƶ�����һ����������������д����ʾ�豸��
     *      ��Ҫʹ�� DMA ��Ҫ��ʾ����ʾ�豸������д�뻺������
     *      �����ݴӵ�һ������������ʱ������ʹ LVGL �ܹ�����Ļ����һ���ֻ��Ƶ���һ����������
     *      ����ʹ����Ⱦ��ˢ�¿��Բ���ִ�С�
     *
     * 3. ȫ�ߴ�˫������
     *      ����������Ļ��С��ȫ�ߴ绺�������������� disp_drv.full_refresh = 1��
     *      ������LVGL��ʼ���� 'flush_cb' ����ʽ�ṩ������Ⱦ��Ļ����ֻ�����֡�������ĵ�ַ��
     */

    /* ��������ʾ��) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
#if USE_SRAM
    static lv_color_t buf_1 = mymalloc(SRAMEX, MY_DISP_HOR_RES * MY_DISP_VER_RES);              /* ���û������Ĵ�СΪ��Ļ��ȫ�ߴ��С */
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES);     /* ��ʼ����ʾ������ */
#else
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                                              /* ���û������Ĵ�СΪ 10 ����Ļ�Ĵ�С */
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);                  /* ��ʼ����ʾ������ */
#endif

    /* ˫������ʾ��) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                                            /* ���û������Ĵ�СΪ 10 ����Ļ�Ĵ�С */
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                                            /* ������һ���������Ĵ�СΪ 10 ����Ļ�Ĵ�С */
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);             /* ��ʼ����ʾ������ */

    /* ȫ�ߴ�˫������ʾ��) �������������� disp_drv.full_refresh = 1 */
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];                               /* ����һ��ȫ�ߴ�Ļ����� */
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];                               /* ������һ��ȫ�ߴ�Ļ����� */
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_HOR_RES * MY_DISP_VER_RES);/* ��ʼ����ʾ������ */

    /*-----------------------------------
     * �� LVGL ��ע����ʾ�豸
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /* ��ʾ�豸�������� */
    lv_disp_drv_init(&disp_drv);                    /* ��ʼ��ΪĬ��ֵ */

    /* ����������ʾ�豸�ĺ���  */

    /* ������ʾ�豸�ķֱ���
     * ����Ϊ����������ԭ�ӵĶ����Ļ�������˶�̬��ȡ�ķ�ʽ��
     * ��ʵ����Ŀ�У�ͨ����ʹ�õ���Ļ��С�ǹ̶��ģ���˿���ֱ������Ϊ��Ļ�Ĵ�С */
    disp_drv.hor_res = lcddev.width;
    disp_drv.ver_res = lcddev.height;

    /* �����������������ݸ��Ƶ���ʾ�豸 */
    disp_drv.flush_cb = disp_flush;

    /* ������ʾ������ */
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /* ȫ�ߴ�˫������ʾ��)*/
    //disp_drv.full_refresh = 1

    /* �������GPU����ʹ����ɫ����ڴ�����
     * ע�⣬������� lv_conf.h ��ʹ�� LVGL ����֧�ֵ� GPUs
     * ��������в�ͬ�� GPU����ô����ʹ������ص������� */
    //disp_drv.gpu_fill_cb = gpu_fill;

    /* ע����ʾ�豸 */
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief       ��ʼ����ʾ�豸�ͱ�Ҫ����Χ�豸
 * @param       ��
 * @retval      ��
 */
static void disp_init(void)
{
    /*You code here*/
    lcd_init();         /* ��ʼ��LCD */
    lcd_display_dir(1); /* ���ú��� */
}

/**
 * @brief       ���ڲ�������������ˢ�µ���ʾ���ϵ��ض�����
 *   @note      ����ʹ�� DMA �����κ�Ӳ���ں�̨����ִ���������
 *              ���ǣ���Ҫ��ˢ����ɺ���ú��� 'lv_disp_flush_ready()'
 *
 * @param       disp_drv    : ��ʾ�豸
 *   @arg       area        : Ҫˢ�µ����򣬰����������εĶԽ�����
 *   @arg       color_p     : ��ɫ����
 *
 * @retval      ��
 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /* LVGL �ٷ�������һ�����ˢ����Ļ�����ӣ������Ч�������Ч�� */

//    int32_t x;
//    int32_t y;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            /*Put a pixel to the display. For example:*/
//            /*put_px(x, y, *color_p)*/
//            color_p++;
//        }
//    }

//    /* ��ָ�����������ָ����ɫ�� */
//    lcd_color_fill(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);
    lcd_draw_fast_rgb_color(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

    /* ��Ҫ!!!
     * ֪ͨͼ�ο⣬�Ѿ�ˢ������� */
    lv_disp_flush_ready(disp_drv);
}

/* ��ѡ: GPU �ӿ� */

/* ������ MCU ��Ӳ�������� (GPU) ��ô�����ʹ������Ϊ�ڴ������ɫ */
/**
 * @brief       ʹ�� GPU ������ɫ���
 *   @note      ���� MCU ��Ӳ�������� (GPU),��ô����������Ϊ�ڴ������ɫ���
 *
 * @param       disp_drv    : ��ʾ�豸
 *   @arg       dest_buf    : Ŀ�껺����
 *   @arg       dest_width  : Ŀ�껺�����Ŀ���
 *   @arg       fill_area   : ��������
 *   @arg       color       : ��ɫ����
 *
 * @retval      ��
 */
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
