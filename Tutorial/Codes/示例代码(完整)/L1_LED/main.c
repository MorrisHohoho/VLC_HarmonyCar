/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iot_gpio_ex.h" //再次封装部分GPIO接口，包含GPIO功能结构体定义

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"  //系统初始化
#include "cmsis_os2.h"  //cmsis_rtos2 标准接口

#include "iot_gpio.h"   //gpio外设接口


/**Task 1: 找到与Hi3861开发板连接的LED对应的GPIO接口**/
#define LED_GPIO IOT_IO_NAME_GPIO_9
/*end*/

/**Task 2: 查阅iot_gpio_ex_.h, 找到对应GPIO的功能结构体，并配置功能**/
#define LED_FUNC IOT_IO_FUNC_GPIO_9_GPIO
#define LED_DIR  IOT_GPIO_DIR_OUT
// or
// #define LED_DIR IOT_GPIO_DIR_IN
/*end*/

/*Task 3: 完成Blink函数*/
static void blink()
{
    while(1)
    {
   
        IoTGpioSetOutputVal(LED_GPIO, IOT_GPIO_VALUE1);
        TaskMsleep(1000);

        IoTGpioSetOutputVal(LED_GPIO, IOT_GPIO_VALUE0);
        TaskMsleep(1000);
    }
}


static void LedExampleEntry(void)
{
    // 初始化GPIO
    IoTGpioInit(LED_GPIO);
    // 设置GPIO功能
    IoSetFunc(LED_GPIO, LED_FUNC);
    // 设置GPIO方向
    IoTGpioSetDir(LED_GPIO, LED_DIR);

    // 控制LED灯打开
    IoTGpioSetOutputVal(LED_GPIO,IOT_GPIO_VALUE0);

    blink();
}

static void LedControlTask(void)
{
    osThreadAttr_t attr;

    attr.name = "LedCntrolDemo";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024; // 堆栈大小为1024 stack size 1024
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)LedExampleEntry, NULL, &attr) == NULL) {
        printf("[LedExample] Failed to create LedTask!\n");
    }
}

APP_FEATURE_INIT(LedControlTask);