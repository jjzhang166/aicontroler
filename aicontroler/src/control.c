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
	pinMode(BEDROOM_LIGHT_PIN, OUTPUT);
	pinMode(SPEECH_WORK_STATE_PIN, OUTPUT);
	ISINIT = 1;
	write_bedroom_light_state(0);
	write_bdtts_light_state(0);
	write_speech_light_state(0);
	return 0;
}

/*
 * 卧室灯控制
 */
int write_bedroom_light_state(int state)
{
	if (ISINIT == 0)
	{
		err_log("Please setup wiringPi at first!\n");
		return -1;
	}
	if (state == LOW)
	{
		digitalWrite(BEDROOM_LIGHT_PIN, LOW);
	}
	else if (state == HIGH)
	{
		digitalWrite(BEDROOM_LIGHT_PIN, HIGH);
	}
	else
	{
		err_log("Unknow bedroom light state!\n");
	}
	return 0;
}

int write_speech_light_state(int state)
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
int write_bdtts_light_state(int state)
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

