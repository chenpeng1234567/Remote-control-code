#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "string.h"

/* ��ͨ��ADC�ɼ� */
ADC_HandleTypeDef g_adc_handle;   /* ADC��� */

/********************************************************************/
/**
 * @brief       ADC��ʼ������
 *   @note      ������֧��ADC1/ADC2����ͨ��, ���ǲ�֧��ADC3
 *              ����ʹ��12λ����, ADC����ʱ��=12M, ת��ʱ��Ϊ: �������� + 12.5��ADC����
 *              ��������������: 239.5, ��ת��ʱ�� = 252 ��ADC���� = 21us
 * @param       ��
 * @retval      ��
 */
void adc_init(void)
{
    RCC_PeriphCLKInitTypeDef adc_clkinit;

    ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* IO��ʱ��ʹ�� */
    ADC_ADCX_CHY_CLK_ENABLE();      /* ADCʱ��ʹ�� */

    adc_clkinit.PeriphClockSelection = RCC_PERIPHCLK_ADC;   /* ADC����ʱ�� */
    adc_clkinit.AdcClockSelection = RCC_ADCPCLK2_DIV6;      /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clkinit);                /* ����ADCʱ�� */

    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* �Ҷ��� */
    g_adc_handle.Init.ScanConvMode = DISABLE;                /* ��ɨ��ģʽ */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* �ر�����ת�� */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* 1��ת���ڹ��������� Ҳ����ֻת����������1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* ��ֹ����������ģʽ */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* ����������ͨ����Ϊ0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* �������� */
    HAL_ADC_Init(&g_adc_handle);                             /* ��ʼ�� */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* У׼ADC */
}

/**
 * @brief       ADC�ײ��������������ã�ʱ��ʹ��
                �˺����ᱻHAL_ADC_Init()����
 * @param       hadc:ADC���
 * @retval      ��
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    ADC_ADCX_CHY_CLK_ENABLE();      /* ʹ��ADCxʱ�� */
    ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* ����GPIOʱ�� */

    /* AD�ɼ�����ģʽ����,ģ������ */
    GPIO_Initure.Pin = ADC_ADCX_CHY_GPIO_PIN;        
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_NOPULL;     
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &GPIO_Initure);
}

/**
 * @brief       ����ADCͨ������ʱ��
 * @param       adcx : adc���ָ��,ADC_HandleTypeDef
 * @param       ch   : ͨ����, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: ����ʱ��  0~7, ��Ӧ��ϵΪ:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5��ADCʱ������        ADC_SAMPLETIME_7CYCLES_5, 7.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5��ADCʱ������     ADC_SAMPLETIME_28CYCLES_5, 28.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5��ADCʱ������       ADC_SAMPLETIME_55CYCLES_5, 55.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5��ADCʱ������       ADC_SAMPLETIME_239CYCLES_5, 239.5��ADCʱ������
 * @param       rank: ��ͨ���ɼ�ʱ��Ҫ���õĲɼ����,
                �����㶨��channle1��rank=1��channle2 ��rank=2��
                ��ô��Ӧ����DMA����ռ�ı�������AdcDMA[0] ��i��channle1��ת�������AdcDMA[1]����ͨ��2��ת������� 
                ��ͨ��DMA����Ϊ ADC_REGULAR_RANK_1
 *   @arg       ���1~16��ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      ��
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime)
{
    /* ���ö�ӦADCͨ�� */
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;
    adc_channel.Rank = rank;
    adc_channel.SamplingTime = stime;
    HAL_ADC_ConfigChannel( adc_handle, &adc_channel);    /* ����ADCX��ͨ��0(��ӦPA0��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch: ͨ��ֵ 0~17��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      ��
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* ����ͨ�������кͲ���ʱ�� */
    HAL_ADC_Start(&g_adc_handle);                            /* ����ADC */

    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* ��ѯת�� */

    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* �������һ��ADC1�������ת����� */
    
}

/**
 * @brief       ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param       ch      : ͨ����, 0~17
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* ��ȡtimes������ */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* ����ƽ��ֵ */
}

uint16_t battery_adc_read(void)
{
    return adc_get_result_average(
        ADC_CHANNEL_0,
        20
    );
}

uint8_t battery_percent(void)
{

    float voltage;
    /* ADC samples the 5V regulated rail (1:2 divider). That rail is regulated, so
       this is a brown-out monitor, NOT true battery state-of-charge:
       >=4.8V => 100%, 4.0~4.8V => linear, <=4.0V => 0%.
       For real battery %, move the divider to the 3S battery side with ~1:4 ratio. */


    uint16_t adc=
    battery_adc_read();


    voltage =
    adc * 3.3f /4095.0f *2;


    int percent;


		percent =
		(int)((voltage-4.0f)
		/
		(4.8f-4.0f)
		*100);


		if(percent>100)
				percent=100;


		if(percent<0)
				percent=0;


		return (uint8_t)percent;
}





