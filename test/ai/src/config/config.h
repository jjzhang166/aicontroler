/*
 * config.h
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

struct api_parameter{
	char *baidu_cuid;
	char *baidu_apikey;
	char *baidu_secretkey;
	char *tuling_key;
	char *tuling_uid;
};

struct base_parameter
{
	int simple_rate;
	int bit_depth;
	int channels;
	int default_record_time;
	int save_last_frame;
	int voice_threshold;
	int voice_silence_count;
	int voice_max_count;
	int test_envir_time;
	int remove_enterface_time;
	int default_bdtts_per;
	int default_bdtts_vol;
	char *default_play_device;
	char *default_record_device;
};

struct other_parameter
{
	int is_save_tts_data;
	char *save_tts_data_name;
	int is_save_record_data;
	char *save_record_data_name;
	int mp3_play_method;
};

struct gpio_parameter
{
	int record_work_state_pin;
	int speech_work_state_pin;
	int bedroom_light_pin;
};

struct api_parameter api_set;
struct base_parameter base_set;
struct other_parameter other_set;
struct gpio_parameter gpio_set;

int get_config_from_file();
int save_config_to_file();

#endif /* SRC_CONFIG_H_ */
