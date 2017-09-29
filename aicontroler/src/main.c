#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "record.h"
#include "err_log.h"
#include "control.h"

int main(void)
{
	int ret = init_wiringpi();
	if (ret < 0)
	{
		err_log("WiringPi setup failed!\n");
		return -2;
	}
	//开启语音识别
	ret = init_record();
	if (ret)
	{
		err_log("Error start recording: %d\n", ret);
		return ret;
	}
	return 0;
}
