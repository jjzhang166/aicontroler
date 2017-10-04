/*
 * control.c
 *
 *  Created on: 2017年9月12日
 *      Author: cheetah
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>

#include "config.h"
#include "control.h"
#include "err_log.h"

int ISINIT = 0;

int init_wiringpi()
{
	int ret = wiringPiSetup();
	if (ret != 0)
	{
		err_log("Setup wiringPi failed!\n");
		return ret;
	}
	pinMode(BDTTS_WORK_STATE_PIN, OUTPUT);   //设置语音识别状态指示灯为ＯＵＴ
	pinMode(SPEECH_WORK_STATE_PIN, OUTPUT);
	ISINIT = 1;
	set_bdtts_light_state(0);
	set_speech_light_state(0);
	return 0;
}

int set_speech_light_state(int state)
{
	if (ISINIT == 0)
	{
		err_log("Please setup wiringPi at first!\n");
		return -1;
	}
	if (state == LOW)
	{
		digitalWrite(SPEECH_WORK_STATE_PIN, LOW);
	}
	else if (state == HIGH)
	{
		digitalWrite(SPEECH_WORK_STATE_PIN, HIGH);
	}
	else
	{
		err_log("Unknow bedroom light state!\n");
	}
	return 0;
}

/*
 * 语音识别指示灯
 */
int set_bdtts_light_state(int state)
{
	if (ISINIT == 0)
	{
		err_log("Please setup wiringPi at first!\n");
		return -1;
	}
	if (state == LOW)
	{
		digitalWrite(BDTTS_WORK_STATE_PIN, LOW);
	}
	else if (state == HIGH)
	{
		digitalWrite(BDTTS_WORK_STATE_PIN, HIGH);
	}
	else
	{
		err_log("Unknow bedroom light state!\n");
	}
	return 0;
}

/*
 * 设置某个gpio为输出模式，并设置电平
 */
int set_output_pin_state(int pin,int state)
{
	if((state == 0 || state == 1) && pin >= 0)
	{
		pinMode(pin,OUTPUT);
		digitalWrite(pin,state);
	}
	else
	{
		err_log("Please set right state for gpio(0 or 1)!\n");
		return -1;
	}
	return 0;
}

/*
 * 设置某个pin为输入模式，并读取pin值
 */
int get_inpput_pin_value(int pin)
{
	if(pin < 0)
	{
		err_log("Please set a right gpio pin number!\n");
		return -1;
	}
	else
	{
		pinMode(pin,INPUT);
		int read = digitalRead(pin);
		return read;
	}
	return -1;
}

