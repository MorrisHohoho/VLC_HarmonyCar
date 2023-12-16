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



void demo(void)
{
    osThreadAttr_t attr;
    attr.name = "Task1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Task1, NULL, &attr) == NULL)
    {
        printf("Falied to create Task!\n");
    }

    attr.name = "Tack2";
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Task2, NULL, &attr) == NULL)
    {
        printf("Falied to create Task!\n");
    }


APP_FEATURE_INIT(demo);
