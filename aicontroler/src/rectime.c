/*
 * time.c
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

#include "rectime.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

time_t end_time_s();
time_t end_time_ms();

/*
 * 开始时间
 */
time_t start_time_s()
{
	time_t t_start;
	t_start = time(NULL);  //!< 单位为s
	return t_start;
}

time_t start_time_ms()
{
	time_t t_start;
	t_start = clock();  //单位为ms
	return t_start;
}

/*
 * 结束时间 单位s
 */
time_t end_time_s()
{
	time_t t_end;
	t_end = time(NULL);
	return t_end;
}

/*
 * 结束时间 单位s
 */
time_t end_time_ms()
{
	time_t t_end;
	t_end = clock();
	return t_end;
}

/*
 * 更具开始时间计算时间差 返回值单位为：秒
 */
float get_time_difference_s(time_t startTime)
{
	time_t endtime = end_time_s();
	float t = difftime(endtime, startTime);
	return t;
}

/*
 * 更具开始时间计算时间差 返回值单位为：毫秒
 */
float get_time_difference_ms(time_t startTime)
{
	time_t endtime = end_time_ms();
	float t = difftime(endtime, startTime);
	return t;
}
