#include"car_control.h"

#define GPIO0 0
#define GPIO1 1
#define GPIO9 9
#define GPIO10 10
#define GPIOFUNC 0
#define PWM_FREQ_FREQUENCY  (65535)

// #define PWM0 HI_IO_FUNC_GPIO_0_PWM3_OUT
// #define PWM1 HI_IO_FUNC_GPIO_1_PWM4_OUT
// #define PWM9 HI_IO_FUNC_GPIO_9_PWM0_OUT
// #define PWM10 HI_IO_FUNC_GPIO_10_PWM1_OUT

#define PWM0 HI_PWM_PORT_PWM3
#define PWM1 HI_PWM_PORT_PWM4
#define PWM9 HI_PWM_PORT_PWM0
#define PWM10 HI_PWM_PORT_PWM1

void car_gpio_init(void) {
    IoTGpioInit(IOT_IO_NAME_GPIO_0);
    IoTGpioInit(IOT_IO_NAME_GPIO_1);
    IoTGpioInit(IOT_IO_NAME_GPIO_9);
    IoTGpioInit(IOT_IO_NAME_GPIO_10);

    IoSetFunc(IOT_IO_NAME_GPIO_0,IOT_IO_FUNC_GPIO_0_PWM3_OUT);
    IoSetFunc(IOT_IO_NAME_GPIO_1,IOT_IO_FUNC_GPIO_1_PWM4_OUT);
    IoSetFunc(IOT_IO_NAME_GPIO_9,IOT_IO_FUNC_GPIO_9_PWM0_OUT);
    IoSetFunc(IOT_IO_NAME_GPIO_10,IOT_IO_FUNC_GPIO_10_PWM1_OUT);

    IoTGpioSetDir(IOT_IO_NAME_GPIO_0, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_1, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_10, IOT_GPIO_DIR_OUT);

    IoTPwmInit(PWM0);
    IoTPwmInit(PWM1);
    IoTPwmInit(PWM9);
    IoTPwmInit(PWM10);

    car_stop();
}

void gpio_control (unsigned int  gpio, IotGpioValue value) {
    // hi_io_set_func(gpio, GPIOFUNC);
    // IoTGpioSetDir(gpio, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(gpio, value);
}

void pwm_control(unsigned int gpio, unsigned int pwm_channel,int duty)
{
    // IoTGpioInit(gpio);
    // IoTGpioSetDir(gpio, IOT_GPIO_DIR_OUT);
    // IoSetFunc(gpio,5);

    // IoTPwmInit(pwm_channel);
    IoTPwmStart(pwm_channel,duty,PWM_FREQ_FREQUENCY);
}

void car_backward(void) {
    // gpio_control(GPIO0, IOT_GPIO_VALUE0);
    // gpio_control(GPIO1, IOT_GPIO_VALUE1);
    // gpio_control(GPIO9, IOT_GPIO_VALUE0);
    // gpio_control(GPIO10, IOT_GPIO_VALUE1);

    IoTPwmStart(PWM1,80,PWM_FREQ_FREQUENCY);
    IoTPwmStart(PWM10,73,PWM_FREQ_FREQUENCY);
}

void car_forward(void) {
    // // gpio_control(GPIO0, IOT_GPIO_VALUE1);
    // gpio_control(GPIO1, IOT_GPIO_VALUE0);
    // // gpio_control(GPIO9, IOT_GPIO_VALUE1);
    // gpio_control(GPIO10, IOT_GPIO_VALUE0);

    IoTPwmStart(PWM0,80,PWM_FREQ_FREQUENCY);
    IoTPwmStart(PWM9,73,PWM_FREQ_FREQUENCY);
}
void car_left(void) {
    // // gpio_control(GPIO0, IOT_GPIO_VALUE0);
    // pwm_control(GPIO0,PWM0,50);
    // gpio_control(GPIO1, IOT_GPIO_VALUE0);
    // // gpio_control(GPIO9, IOT_GPIO_VALUE1);
    // pwm_control(GPIO9,PWM9,60);
    // gpio_control(GPIO10, IOT_GPIO_VALUE0);

    IoTPwmStart(PWM0,65,PWM_FREQ_FREQUENCY);
    IoTPwmStart(PWM9,95,PWM_FREQ_FREQUENCY);
}

void car_right(void) {
    // // gpio_control(GPIO0, IOT_GPIO_VALUE1);
    //     pwm_control(GPIO0,PWM0,60);
    // gpio_control(GPIO1, IOT_GPIO_VALUE0);
    // // gpio_control(GPIO9, IOT_GPIO_VALUE0);
    //     pwm_control(GPIO9,PWM9,50);
    // gpio_control(GPIO10, IOT_GPIO_VALUE0);

    IoTPwmStart(PWM0,95,PWM_FREQ_FREQUENCY);
    IoTPwmStart(PWM9,65,PWM_FREQ_FREQUENCY);
}

void car_stop(void) {
    // gpio_control(GPIO0, IOT_GPIO_VALUE0);
    // gpio_control(GPIO1, IOT_GPIO_VALUE0);
    // gpio_control(GPIO9, IOT_GPIO_VALUE0);
    // gpio_control(GPIO10, IOT_GPIO_VALUE0);

    IoTPwmStop(PWM0);
    IoTPwmStop(PWM9);
    IoTPwmStop(PWM1);
    IoTPwmStop(PWM10);
}

hi_bool ControlCar(const char* cmd)
{
    if(strcmp(STOP,cmd)==0)
    {
        car_stop();
        return HI_TRUE;
    }
    else if(strcmp(FORWARD,cmd)==0)
    {
        car_stop();
        car_forward();
                return HI_TRUE;
    }
    else if(strcmp(BACKWARD,cmd)==0)
    {
        car_stop();
        car_backward();
                return HI_TRUE;
    }
    else if(strcmp(LEFT,cmd)==0)
    {
        car_stop();
        car_left();
                return HI_TRUE;
    }
    else if(strcmp(RIGHT,cmd)==0)
    {
        car_stop();
        car_right();
                return HI_TRUE;
    }
    else
        return HI_FALSE;
}
