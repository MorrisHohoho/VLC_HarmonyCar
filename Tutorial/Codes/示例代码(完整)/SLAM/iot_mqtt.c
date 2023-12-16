/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - clarifications and/or documentation extension
 *******************************************************************************/

#include "MQTTClient.h"
#include "./wifi/wifi_connect.h"
#include "car_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_watchdog.h"
#include "iot_gpio.h"
#include "hi_time.h"
#include "hi_io.h"
#include "iot_gpio_ex.h"

#define MQTT_SERVERIP "111.1.111.111"
#define MQTT_SERVERPORT 1883
#define MQTT_CMD_TIMEOUT_MS 2000
#define MQTT_KEEP_ALIVE_MS 2000
#define MQTT_DELAY_2S 200
#define MQTT_DELAY_500_MS 50
#define MQTT_VERSION 3
#define MQTT_QOS 2
#define MQTT_TASK_STACK_SIZE (1024 * 10)
#define SENSOR_TASK_STACK_SIZE (1024 * 2)
#define SENSOR_TASK_PRIO 25

#define DELAY_US20 20
#define DELAY_MS10 10

#define MSGQUEUE_COUNT 32
#define MSGQUEUE_SIZE 10
osMessageQueueId_t mid_MsgQueue; // message queue id

static unsigned char sendBuf[1000];
static unsigned char readBuf[1000];

Network network;
MQTTClient client;
typedef struct
{
    int msg_type;
    char mes[64];
    float distance;
} app_msg_t;

void Hcsr04Init(void)
{
    /*
     * 设置超声波Echo为输入模式
     * 设置GPIO8功能（设置为GPIO功能）
     * Set ultrasonic echo as input mode
     * Set GPIO8 function (set as GPIO function)
     */
    IoSetFunc(IOT_IO_NAME_GPIO_8, IOT_IO_FUNC_GPIO_8_GPIO);
    /*
     * 设置GPIO8为输入方向
     * Set GPIO8 as the input direction
     */
    IoTGpioSetDir(IOT_IO_NAME_GPIO_8, IOT_GPIO_DIR_IN);
    /*
     * 设置GPIO7功能（设置为GPIO功能）
     * Set GPIO7 function (set as GPIO function)
     */
    IoSetFunc(IOT_IO_NAME_GPIO_7, IOT_IO_FUNC_GPIO_7_GPIO);
    /*
     * 设置GPIO7为输出方向
     * Set GPIO7 as the output direction
     */
    IoTGpioSetDir(IOT_IO_NAME_GPIO_7, IOT_GPIO_DIR_OUT);
}

float GetDistance(void)
{
    static unsigned long start_time = 0, time = 0;
    float distance = 0.0;
    IotGpioValue value = IOT_GPIO_VALUE0;
    unsigned int flag = 0;
    /*
     * 设置GPIO7输出低电平
     * 给trig发送至少10us的高电平脉冲，以触发传感器测距
     * Set GPIO7 to output direction
     * Send a high level pulse of at least 10us to the trig to trigger the range measurement of the sensor
     */
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE1);
    hi_udelay(DELAY_US20);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE0);
    /*
     * 计算与障碍物之间的距离
     * Calculate the distance from the obstacle
     */
    while (1)
    {
        /*
         * 获取GPIO8的输入电平状态
         * Get the input level status of GPIO8
         */
        IoTGpioGetInputVal(IOT_IO_NAME_GPIO_8, &value);
        /*
         * 判断GPIO8的输入电平是否为高电平并且flag为0
         * Judge whether the input level of GPIO8 is high and the flag is 0
         */
        if (value == IOT_GPIO_VALUE1 && flag == 0)
        {
            /*
             * 获取系统时间
             * get SysTime
             */
            start_time = hi_get_us();
            flag = 1;
        }
        /*
         * 判断GPIO8的输入电平是否为低电平并且flag为1
         * Judge whether the input level of GPIO8 is low and the flag is 1
         */
        if (value == IOT_GPIO_VALUE0 && flag == 1)
        {
            /*
             * 获取高电平持续时间
             * Get high level duration
             */
            time = hi_get_us() - start_time;
            break;
        }
    }
    /* 计算距离障碍物距离（340米/秒 转换为 0.034厘米/微秒, 2代表去来，两倍距离） */
    /* Calculate the distance from the obstacle */
    /* (340 m/s is converted to 0.034 cm/microsecond 2 represents going and coming, twice the distance) */
    distance = time * 0.034 / 2;
    //printf("distance is %0.2f cm\r\n", distance);
    return distance;
}

void messageArrived(MessageData *data)
{
    int ret = 0;
    // app_msg_t *app_msg;
    // printf("Message arrived on topic %.*s: %.*s\n", data->topicName->lenstring.len,
    //        data->topicName->lenstring.data, data->message->payloadlen, data->message->payload);

    char tmp[16];
    memset(tmp, 0, 16);
    strncpy(tmp, data->message->payload, data->message->payloadlen);

    
    ControlCar(tmp);

    // app_msg = malloc(sizeof(app_msg_t));
    // if (app_msg != NULL)
    // {
    //     app_msg->msg_type = 0;
    //     memset(app_msg->mes, 0, 64);
    //     strcpy(app_msg->mes, data->message->payload);
    //     ret = osMessageQueuePut(mid_MsgQueue, &app_msg, 0, 0);
    //     if (ret != osOK)
    //     {
    //         printf("Failed to put message in queue\n");
    //         free(app_msg);
    //     }
    // }
}

void DealMsg(char *msg)
{
    printf("%s\n\r",msg);
    ControlCar(msg);
}

void ReportMsg(float distance)
{
    int count = 0;
    int rc = 0;
    MQTTMessage message;
    char payload[30];
    message.qos = MQTT_QOS;
    message.retained = 0;
    message.payload = payload;
    (void)sprintf_s(payload, sizeof(payload), "%d", (int)distance);
    message.payloadlen = strlen(payload);

    if ((rc = MQTTPublish(&client, "distance", &message)) != 0)
    {
        printf("Return code from MQTT publish is %d\n", rc);
        NetworkDisconnect(&network);
        MQTTDisconnect(&client);
    }
    osDelay(10);
}

static void MQTTDemoTask(void)
{
    WifiConnect("IPhone 11", "gldhvgirfg4i");
    // WifiConnect("HackerSpace", "iloveSCU");
    printf("Starting ...\n");
    int rc, count = 0;

    NetworkInit(&network);
    printf("NetworkConnect  ...\n");

    NetworkConnect(&network, MQTT_SERVERIP, MQTT_SERVERPORT);
    printf("MQTTClientInit  ...\n");
    MQTTClientInit(&client, &network, MQTT_CMD_TIMEOUT_MS, sendBuf, sizeof(sendBuf), readBuf, sizeof(readBuf));

    MQTTString clientId = MQTTString_initializer;
    clientId.cstring = "bearpi";

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID = clientId;
    data.willFlag = 0;
    data.MQTTVersion = MQTT_VERSION;
    data.keepAliveInterval = MQTT_KEEP_ALIVE_MS;
    data.cleansession = 1;

    printf("MQTTConnect  ...\n");
    rc = MQTTConnect(&client, &data);
    if (rc != 0)
    {
        printf("MQTTConnect: %d\n", rc);
        NetworkDisconnect(&network);
        MQTTDisconnect(&client);
        osDelay(MQTT_DELAY_2S);
    }

    printf("MQTTSubscribe  ...\n");
    rc = MQTTSubscribe(&client, "carcontrol", MQTT_QOS, messageArrived);
    if (rc != 0)
    {
        printf("MQTTSubscribe: %d\n", rc);
        osDelay(MQTT_DELAY_2S);
    }
    app_msg_t *app_msg;
    while (1)
    {
        app_msg = NULL;
        (void)osMessageQueueGet(mid_MsgQueue, (void **)&app_msg, NULL, 0U);
        if (app_msg != NULL)
        {
            switch (app_msg->msg_type)
            {
            case 0:
                DealMsg(&app_msg->mes);
                break;
            case 1:
                ReportMsg(app_msg->distance);
                break;
            default:
                break;
            }
            free(app_msg);
        }
    }
}

static void SensorTaskEntry(void)
{
    app_msg_t *app_msg;
    int ret = 0;
    float distance;

    Hcsr04Init();
    car_gpio_init();

    while (1)
    {
        app_msg = malloc(sizeof(app_msg_t));
        app_msg->msg_type = 1;
        distance = GetDistance();
        app_msg->distance = distance;

        ret = osMessageQueuePut(mid_MsgQueue, &app_msg, 0U, 0U);
        if (ret != 0)
        {
            printf("osMessageQueuePut failed\n");
            free(app_msg);
        }
        osDelay(50);
    }
}

static void MQTTDemo(void)
{
    IoTWatchDogDisable();
    osThreadAttr_t attr;
    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_COUNT, MSGQUEUE_SIZE, NULL);
    if (mid_MsgQueue == NULL)
    {
        printf("Failed to create Message Queue!\n");
    }

    attr.name = "MQTTDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = MQTT_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)MQTTDemoTask, NULL, &attr) == NULL)
    {
        printf("[MQTT_Demo] Failed to create MQTTDemoTask!\n");
    }

    attr.stack_size = SENSOR_TASK_STACK_SIZE;
    attr.priority = SENSOR_TASK_PRIO;
    attr.name = "SensorTaskEntry";
    if (osThreadNew((osThreadFunc_t)SensorTaskEntry, NULL, &attr) == NULL)
    {
        printf("Failed to create SensorTaskEntry!\n");
    }
}

APP_FEATURE_INIT(MQTTDemo);