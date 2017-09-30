#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "record.h"
#include "err_log.h"
#include "config/config.h"

int main(void)
{
	int ret = get_config_from_file();
	if(ret < 0)
	{
		err_log("Get config failed!\n");
		return -1;
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
