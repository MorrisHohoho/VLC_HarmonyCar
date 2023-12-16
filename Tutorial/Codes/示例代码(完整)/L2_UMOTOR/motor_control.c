#include "motor_control.h"

#include "iot_gpio_ex.h"
#include "iot_gpio.h"

//Task 3: 查阅硬件手册，正确初始化电机使用的gpio口
void MotorInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_0);
    IoTGpioInit(IOT_IO_NAME_GPIO_1);
    IoTGpioInit(IOT_IO_NAME_GPIO_9);
    IoTGpioInit(IOT_IO_NAME_GPIO_10);

    IoSetFunc(IOT_IO_NAME_GPIO_0,IOT_IO_FUNC_GPIO_0_GPIO);
    IoSetFunc(IOT_IO_NAME_GPIO_1,IOT_IO_FUNC_GPIO_1_GPIO);
    IoSetFunc(IOT_IO_NAME_GPIO_9,IOT_IO_FUNC_GPIO_9_GPIO);
    IoSetFunc(IOT_IO_NAME_GPIO_10,IOT_IO_FUNC_GPIO_10_GPIO);

    IoTGpioSetDir(IOT_IO_NAME_GPIO_0,IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_1,IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9,IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_10,IOT_GPIO_DIR_OUT);
}


//Task 4: 实现前进
void CarForward()
{
    CarStop();
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_0,IOT_GPIO_VALUE1);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_9,IOT_GPIO_VALUE1);
}


//Task 4: 实现刹车
void CarStop()
{
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_0,IOT_GPIO_VALUE0);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_1,IOT_GPIO_VALUE0);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_9,IOT_GPIO_VALUE0);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_10,IOT_GPIO_VALUE0);
}