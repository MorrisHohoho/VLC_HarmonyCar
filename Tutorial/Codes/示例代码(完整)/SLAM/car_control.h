#ifndef __APP_DEMO_ROBOT_CAR_H__
#define __APP_DEMO_ROBOT_CAR_H__

#include "iot_gpio_ex.h"

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"
#include "iot_pwm.h"
#include "hi_pwm.h"


/*
* Do not use 000 or 111, because when no message is transmitted
* the default voltage jumps between 0 and 3.3V, which might be 000 or 111.
*/

#define STOP "stop"
#define FORWARD "forward"
#define LEFT   "left"
#define RIGHT  "right"
#define BACKWARD "backward"

void car_gpio_init();


void car_backward();
void car_forward();
void car_left();
void car_right();
void car_stop();

hi_bool ControlCar(const char* cmd);

#endif