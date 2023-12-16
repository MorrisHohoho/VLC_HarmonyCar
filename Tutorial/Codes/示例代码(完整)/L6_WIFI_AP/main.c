
#include "ap_mode.h"
#include "iot_gpio_ex.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_stdlib.h"
#include "hi_time.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "iot_errno.h"

#include <stdio.h>
#include <unistd.h>


volatile extern bool  led_flg = false;

void *wifi_config_thread(const char *arg)
{
    arg = arg;
    printf("wifi_start_softap \r\n");
    wifi_start_softap();

    osThreadExit();
    return NULL;
}

void GpioInit()
{
    IoSetFunc(IOT_IO_NAME_GPIO_9,IOT_IO_FUNC_GPIO_9_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9,IOT_GPIO_DIR_OUT);
}

void LedTask()
{
    while(1)
    {
        if(led_flg == 0)
        {
            IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_9, IOT_GPIO_VALUE1);
        }
        else
        {
            IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_9, IOT_GPIO_VALUE0);
        }
    }
}



void wifi_config_entry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    GpioInit();
    attr.name = "wifi_config_thread";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = 36;

    if (osThreadNew((osThreadFunc_t)wifi_config_thread, NULL, &attr) == NULL) {
        printf("[LedExample] Falied to create LedTask!\n");
    }

    attr.name = "LED_TASK";
    attr.priority = osPriorityNormal;
    if(osThreadNew((osThreadFunc_t)LedTask, NULL, &attr) == NULL)
    {
        printf("[LedExample] Falied to create LedTask!\n");
    }
}


APP_FEATURE_INIT(wifi_config_entry);

