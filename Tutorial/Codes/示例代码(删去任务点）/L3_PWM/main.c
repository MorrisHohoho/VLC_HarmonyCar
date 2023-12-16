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

/*
    GA12-N20 直流减速电机的相关API接口
    左轮：IB:GPIO5, IA: GPIO6
    右轮：IB:GPIO9, IA: GPIO10
*/
#include <stdio.h>
#include <unistd.h>

#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "iot_pwm.h"
#include "hi_time.h"

#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_PORT_PWM1   1
#define IOT_PWM_PORT_PWM2   2
#define IOT_PWM_PORT_PWM3   3




/*Task 1: 配置PWM*/
void PWMInit()
{

}


/*Task 2: 改变PWM占空比，实现呼吸灯效果*/
void PWMTask(void)
{
  

}

void PWMEntry(void)
{
    osThreadAttr_t attr;
    printf("PWMTask\r\n");
    IoTWatchDogDisable();
    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; // 堆栈大小为1024*5,stack size 1024*5
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)PWMTask, NULL, &attr) == NULL) {
        printf("Failed to create PWMTask!\n");
    }

    printf("PWMTask\r\n");
}

APP_FEATURE_INIT(PWMEntry);