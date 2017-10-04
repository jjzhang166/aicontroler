/*
 * control.h
 *
 *  Created on: 2017年9月12日
 *      Author: cheetah
 */

#ifndef SRC_CONTROL_H_
#define SRC_CONTROL_H_

#include "err_log.h"

/*
 * 初始化wiringpi
 */
int init_wiringpi();

/*
 * 设置某个pin为输出模式，并设置pin电平
 * 参数：pin wiringpi gpio编号
 *      state pin电平 0 或者 1
 */
int set_output_pin_state(int pin,int state);

/*
 * 或者某个GPIO的电平
 * 参数：pin wiringpi GPIO编号
 */
int get_inpput_pin_value(int pin);

/*
 * 设置语音识别状态指示灯
 */
int set_bdtts_light_state(int state);

/*
 * 设置录音状态指示灯
 */
int set_speech_light_state(int state);

#endif /* SRC_CONTROL_H_ */
