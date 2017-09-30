/*
 * time.h
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

#ifndef SRC_RECTIME_H_
#define SRC_RECTIME_H_

#include <time.h>

time_t start_time_ms();
time_t start_time_s();
float get_time_difference_ms(time_t startTime);
float get_time_difference_s(time_t startTime);

#endif /* SRC_RECTIME_H_ */
