/*
 * config.c
 *
 *  Created on: 2017年9月29日
 *      Author: cheetah
 *      https://github.com/haipome/ini
 */
#include "../config/config.h"

#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <mcheck.h>

#include "../config/ini.h"

int get_config_from_file()
{
	/* $ export MALLOC_TRACE=malloc.log */
	mtrace();
	ini_t *conf = ini_load("config.ini");
	if (conf == NULL)
	{
		return -1;
	}
	//api_set
	ini_read_str(conf, "api_set", "baidu_apikey", &(api_set.baidu_apikey), "");
	ini_read_str(conf, "api_set", "baidu_cuid", &(api_set.baidu_cuid), "");
	ini_read_str(conf, "api_set", "baidu_secretkey", &(api_set.baidu_secretkey), "");
	ini_read_str(conf, "api_set", "tuling_key", &(api_set.tuling_key), "");
	ini_read_str(conf, "api_set", "tuling_uid", &(api_set.tuling_uid), "");
	//base_set
	ini_read_int(conf, "base_set", "simple_rate", &(base_set.simple_rate), 16000);
	ini_read_int(conf, "base_set", "bit_depth", &(base_set.bit_depth), 16);
	ini_read_int(conf, "base_set", "channels", &(base_set.channels), 1);
	ini_read_int(conf, "base_set", "default_record_time", &(base_set.default_record_time), 8);
	ini_read_int(conf, "base_set", "save_last_frame", &(base_set.save_last_frame), 60);
	ini_read_int(conf, "base_set", "voice_threshold", &(base_set.voice_threshold), 400);
	ini_read_int(conf, "base_set", "voice_silence_count", &(base_set.voice_silence_count), 150);
	ini_read_int(conf, "base_set", "voice_max_count", &(base_set.voice_max_count), 20);
	ini_read_int(conf, "base_set", "test_envir_time", &(base_set.test_envir_time), 600);
	ini_read_int(conf, "base_set", "remove_enterface_time", &(base_set.remove_enterface_time), 30);
	ini_read_int(conf, "base_set", "default_bdtts_per", &(base_set.default_bdtts_per), 0);
	ini_read_int(conf, "base_set", "default_bdtts_vol", &(base_set.default_bdtts_vol), 15);
	ini_read_str(conf, "base_set", "default_play_device", &(base_set.default_play_device), "default");
	ini_read_str(conf, "base_set", "default_record_device", &(base_set.default_record_device), "default");
	//other_set
	ini_read_int(conf, "other_set", "is_raspi", &(other_set.is_raspi), 0);
	ini_read_int(conf, "other_set", "mic_soundamp_factor", &(other_set.mic_soundamp_factor), 0);
	ini_read_int(conf, "other_set", "is_save_tts_data", &(other_set.is_save_tts_data), 0);
	ini_read_str(conf, "other_set", "save_tts_data_name", &(other_set.save_tts_data_name), "default");
	ini_read_int(conf, "other_set", "is_save_record_data", &(other_set.is_save_record_data),0);
	ini_read_str(conf, "other_set", "save_record_data_name", &(other_set.save_record_data_name), "default");
	ini_read_int(conf, "other_set", "mp3_play_method", &(other_set.mp3_play_method),0);
	//gpio_set
	ini_read_int(conf, "gpio_set", "record_work_state_pin", &(gpio_set.record_work_state_pin),0);
	ini_read_int(conf, "gpio_set", "speech_work_state_pin", &(gpio_set.speech_work_state_pin),0);
	ini_read_int(conf, "gpio_set", "bedroom_light_pin", &(gpio_set.bedroom_light_pin),0);
	//free
	ini_free(conf);

	if(base_set.default_bdtts_per < 0 || base_set.default_bdtts_per >4)
	{
		base_set.default_bdtts_per = 0;
	}
	if(base_set.default_bdtts_vol <= 0 || base_set.default_bdtts_vol >15)
	{
		base_set.default_bdtts_vol = 10;
	}
	if(other_set.is_raspi == 1 && !strcmp(base_set.default_record_device,"default"))
	{
		base_set.default_record_device = "plughw:1,0";
	}
	return 0;
}

int save_config_to_file()
{
	return 0;
}
