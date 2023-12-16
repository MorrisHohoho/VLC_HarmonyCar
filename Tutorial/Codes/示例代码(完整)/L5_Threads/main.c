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




#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_PORT_PWM1   1
#define IOT_PWM_PORT_PWM2   2
#define IOT_PWM_PORT_PWM3   3



#define PD_ADC_CHANNEL IOT_ADC_CHANNEL_5

void PDInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_11);
    IoSetFunc(IOT_IO_NAME_GPIO_11,IOT_IO_FUNC_GPIO_11_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_11,IOT_GPIO_DIR_IN);
    hi_io_set_pull(IOT_IO_NAME_GPIO_11,HI_IO_PULL_DOWN);
}
hi_float GetVlt(IotAdcChannelIndex ch)
{
     hi_u32 data;
    int ret = hi_adc_read((hi_adc_channel_index)ch, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0);
    if (ret != HI_ERR_SUCCESS)
    {
        printf("ADC Read Fail!\n");
    }
    hi_float vlt=0.0;
    vlt = hi_adc_convert_to_voltage(data);
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
       TaskMsleep(1000);
    }
}




void PWMInit()
{
    printf("PWMInit\r\n");
    IoTGpioInit(IOT_IO_NAME_GPIO_9);
    IoSetFunc(IOT_IO_NAME_GPIO_9,IOT_IO_FUNC_GPIO_9_PWM0_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9,IOT_GPIO_DIR_OUT);
    
    IoTPwmInit(IOT_PWM_PORT_PWM0);
    IoTPwmStop(IOT_PWM_PORT_PWM0);
}

void PWMTask(void)
{
    //初始化PWM模块
    PWMInit();
    unsigned short duty = 1;
    unsigned int freq = 65535;
    printf("start PWM!\r\n");
    while(1)
    {
        for(duty;duty<99;duty++)
        {
            IoTPwmStart(IOT_PWM_PORT_PWM0,duty,freq);
            TaskMsleep(50);
        }
        for(duty;duty>1;duty--)
        {
            IoTPwmStart(IOT_PWM_PORT_PWM0,duty,freq);
            TaskMsleep(50);
        }
    }

}



void demo(void)
{
    osThreadAttr_t attr;
    PDInit();
    PWMInit();
    
    IoTWatchDogDisable();

    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)PWMTask, NULL, &attr) == NULL)
    {
        printf("Falied to create PWMTask!\n");
    }

    attr.name = "ADCTack";
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)ADCTask, NULL, &attr) == NULL)
    {
        printf("Falied to create ADCTask!\n");
    }
}

APP_FEATURE_INIT(demo);
