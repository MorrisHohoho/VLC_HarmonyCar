#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <hi_io.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_adc.h"
#include "iot_pwm.h"
#include "hi_adc.h"
#include "iot_watchdog.h"
#include "iot_errno.h"



/*Task 1: 正确配置ADC接口*/
void PDInit()
{

}



/*Task 2: 读取ADC值*/
hi_float GetVlt(IotAdcChannelIndex ch)
{
   
    hi_float vlt=0.0;

    return vlt;
}

void ADCTask()
{
    printf("I am adcTask!\n\r");
    hi_float vlt;

    while(1)
    {
       vlt=GetVlt(PD_ADC_CHANNEL);
       printf("%f\r\n",vlt);
    }
}

void demo(void)
{
    osThreadAttr_t attr;
    PDInit();

    IoTWatchDogDisable(); 
    attr.name = "ADCTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)ADCTask, NULL, &attr) == NULL)
    {
        printf("[key_demo] Falied to create KeyTask!\n");
    }
}

APP_FEATURE_INIT(demo);
