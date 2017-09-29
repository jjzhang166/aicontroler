/*
 * control.h
 *
 *  Created on: 2017年9月12日
 *      Author: cheetah
 */

#ifndef SRC_CONTROL_H_
#define SRC_CONTROL_H_

#include "err_log.h"

int init_wiringpi();
int write_bedroom_light_state(int state);
int write_bdtts_light_state(int state);
int write_speech_light_state(int state);

#endif /* SRC_CONTROL_H_ */
