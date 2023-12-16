#include "iot_gpio_ex.h"
#include "hcsr04.h"
#include "motor_control.h"

#include <stdio.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_watchdog.h"
#include "iot_gpio.h"
#include "hi_time.h"
#include "hi_io.h"

static void DeviceInit()
{
    Hcsr04Init();
    MotorInit();
}


//Task 5: 实现根据测距结果控制电机
static void Hcsr04SampleTask()
{

}


void Hcsr04SampleEntry(void)
{
    osThreadAttr_t attr;
    DeviceInit();
    IoTWatchDogDisable();
    attr.name = "Hcsr04SampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; // 堆栈大小为1024*5 stack size 5*1024
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Hcsr04SampleTask, NULL, &attr) == NULL) {
        printf("[Hcsr04SampleTask] Failed to create Hcsr04SampleTask!\n");
    }
}
APP_FEATURE_INIT(Hcsr04SampleEntry);