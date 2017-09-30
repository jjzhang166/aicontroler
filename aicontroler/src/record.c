/*
 * record.c
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

// Use the newer ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <alsa/asoundlib.h>

#include "writefile.h"
#include "config.h"
#include "record.h"
#include "rectime.h"
#include "ai.h"
#include "control.h"

/*
 * 存放录音数据Buffer最大长度，这个选项最好不要调整
 */
#define MAX_WAV_SIZE  6000000

int _COUNT = 0;                 //记录总帧数
char _OLD_VOICE_BUFFER[MAX_WAV_SIZE];
int _OLD_VOICE_TOTAL_COUNT = 0;
int _OLD_VOICE_TEMP_COUNT = 0;
char *device = (char *) DEFAULT_RECORD_DEVICE;  //录音设备

//计算一次采样的音频声音大小
int pcm_db_average(char* ptr, size_t size);
//开始录音
int start_record(struct WaveHeader *hdr, int envl);
//存放录音之前的数据
int old_voice_data(char *buffer, int count, char **oldbuffer, int *oldcount);
//获取当前时间
int now_time(char **gotime);
//测试混案件音量大小
int test_envirment_volume(struct WaveHeader *hdr);

struct WaveHeader *generic_wav_header(uint32_t sample_rate, uint16_t bit_depth,
		uint16_t channels)
{
	struct WaveHeader *hdr;
	hdr = malloc(sizeof(*hdr));
	if (!hdr)
		return NULL;
	memcpy(&hdr->RIFF_marker, "RIFF", 4);
	memcpy(&hdr->filetype_header, "WAVE", 4);
	memcpy(&hdr->format_marker, "fmt ", 4);
	hdr->data_header_length = 16;
	hdr->format_type = 1;
	hdr->number_of_channels = channels;
	hdr->sample_rate = sample_rate;
	hdr->bytes_per_second = sample_rate * channels * bit_depth / 8;
	hdr->bytes_per_frame = channels * bit_depth / 8;
	hdr->bits_per_sample = bit_depth;
	return hdr;
}

/*
 * 获取一帧音频的平均采样大小　　参考
 * http://blog.csdn.net/ownWell/article/details/8114121
 * http://blog.csdn.net/freeze_z/article/details/44310245
 */
int pcm_db_average(char* ptr, size_t size)
{
	int ndb = 0;
	short int value;
	int i;
	long v = 0;
	for (i = 0; i < size; i += 2)
	{
		memcpy((char*) &value, ptr + i, 1); //低位采样
		memcpy((char*) &value + 1, ptr + i + 1, 1);
		v += abs(value);
	}
	v = v / (size / 2);
	if (v != 0)
	{
		ndb = v;
	}
	return ndb;
}

/*
 * 记录开始录音之前的帧数
 */
int old_voice_data(char *buffer, int count, char **oldbuffer, int *oldcount)
{
	if (count != 0 && (buffer != NULL || !strcmp(buffer, "")))
	{
		if (_OLD_VOICE_TEMP_COUNT < SAVE_LAST_FRAME)
		{
			int j = 0;
			if (_OLD_VOICE_TOTAL_COUNT + count > MAX_WAV_SIZE)
			{
				err_log("Out of max wav size!");
				return -1;
			}
			for (int i = _OLD_VOICE_TOTAL_COUNT;
					i < _OLD_VOICE_TOTAL_COUNT + count; i++)
			{
				_OLD_VOICE_BUFFER[i] = buffer[j];
				j++;
			}
			_OLD_VOICE_TOTAL_COUNT = _OLD_VOICE_TOTAL_COUNT + count;
			_OLD_VOICE_TEMP_COUNT++;
		}
		else
		{
			int j = 0;
			if (_OLD_VOICE_TOTAL_COUNT + count > MAX_WAV_SIZE)
			{
				err_log("Out of max wav size!");
			}
			else
			{
				int temp_count = _OLD_VOICE_TOTAL_COUNT - count;
				char *temp_buffer = (char *) calloc(temp_count, sizeof(char));
				memcpy(temp_buffer, &_OLD_VOICE_BUFFER[count], temp_count);
				memset(_OLD_VOICE_BUFFER, 0, MAX_WAV_SIZE);
				memcpy(_OLD_VOICE_BUFFER, temp_buffer, temp_count);
				for (int i = temp_count; i < temp_count + count; i++)
				{
					_OLD_VOICE_BUFFER[i] = buffer[j];
					j++;
				}
				free(temp_buffer);
			}
		}
	}
	if(*oldbuffer != NULL)
	{
		//重新申请足够的内存空间
		*oldbuffer = (char *) calloc(_OLD_VOICE_TOTAL_COUNT, sizeof(char));
		//复制最后的数据到内存空间里面
		memcpy(*oldbuffer, _OLD_VOICE_BUFFER, _OLD_VOICE_TOTAL_COUNT);
		*oldcount = _OLD_VOICE_TOTAL_COUNT;
	}
	return 0;
}

/*
 * 获取当前系统时间
 */
int now_time(char **gotime)
{
	time_t t;
	struct tm *pt;
	char pc[20] = { 0 }, hou[2] = { 0 }, min[2] = { 0 }, sec[2] = { 0 };
	time(&t);
	pt = localtime(&t);
	if (pt->tm_hour < 10)
	{
		sprintf(hou, "0%d", pt->tm_hour);
	}
	else
	{
		sprintf(hou, "%d", pt->tm_hour);
	}
	if (pt->tm_min < 10)
	{
		sprintf(min, "0%d", pt->tm_min);
	}
	else
	{
		sprintf(min, "%d", pt->tm_min);
	}
	if (pt->tm_sec < 10)
	{
		sprintf(sec, "0%d", pt->tm_sec);
	}
	else
	{
		sprintf(sec, "%d", pt->tm_sec);
	}
	sprintf(pc, "%d-%d-%d %s:%s:%s", pt->tm_year + 1900, pt->tm_mon + 1,
			pt->tm_mday, hou, min, sec);
	*gotime = (char *) calloc(20, sizeof(char));
	strcpy(*gotime, pc);
	return 0;
}

/*
 * 录音
 */
int start_record(struct WaveHeader *hdr, int envl)
{
	err_log("\n初始化录音设备...");
	int err, size, dir, work = 0, count = 0, sum = 0, state = 0, vladd = 0;
	long tempi = 0;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int sampleRate = hdr->sample_rate;
	snd_pcm_uframes_t frames = 64;
	char *buffer,*nowTime;
	char totalBuff[MAX_WAV_SIZE] = { 0 };
	time_t starttimes = start_time_s();
	//pcm数据最大不会超过32767，所以大于323767没有意义了
	//其实接近于32767也没有意义，谁也不会在那种环境下做语音识别是吧
	if (envl + VOICE_THRESHOLD >= 32767)
	{
		vladd = 32767 - envl;
	}
	else
	{
		vladd = VOICE_THRESHOLD;
	}
	/* Open PCM device for recording (capture). */
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0);
	if (err)
	{
		err_log("Unable to open PCM device: %s\n", snd_strerror(err));
		return err;
	}
	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);
	/* Fill it in with default values. */
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0)
	{
		err_log("snd_pcm_hw_params_any error,%s\n", snd_strerror(err));
		exit(1);
	}
	/* ### Set the desired hardware parameters. ### */
	/* Interleaved mode */
	err = snd_pcm_hw_params_set_access(handle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err)
	{
		err_log("Error setting interleaved mode: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	/* Signed 16-bit little-endian format */
	switch ((hdr->bits_per_sample) / 8)
	{
	case 1:
		err = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U8);
		break;
	case 2:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S16_LE);
		break;
	case 3:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S24_LE);
		break;
	default:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S16_LE);
	}
	if (err)
	{
		err_log("Error setting format: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	/* set channels (stereo) */
	err = snd_pcm_hw_params_set_channels(handle, params,
			hdr->number_of_channels);
	if (err)
	{
		err_log("Error setting channels: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	/* there need 1600 */
	sampleRate = hdr->sample_rate;
	err = snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, &dir);
	if (err)
	{
		err_log("Error setting sampling rate (%d): %s\n", sampleRate,
				snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	hdr->sample_rate = sampleRate;
	/* Set period size*/
	err = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
	if (err)
	{
		err_log("Error setting period size: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	/* Write the parameters to the driver */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0)
	{
		err_log("Unable to set HW parameters: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	/* Use a buffer large enough to hold one period */
	err = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	if (err)
	{
		err_log("Error retrieving period size: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return err;
	}
	size = frames * hdr->bits_per_sample / 8 * hdr->number_of_channels;
	buffer = (char *) malloc(size);
	if (!buffer)
	{
		err_log("Buffer error.\n");
		snd_pcm_close(handle);
		return -1;
	}
	err = snd_pcm_hw_params_get_period_time(params, &sampleRate, &dir);
	if (err)
	{
		err_log("Error retrieving period time: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		free(buffer);
		return err;
	}
	err_log("完成!\n");
	while (1)
	{
		err = snd_pcm_readi(handle, buffer, frames);
		if (err == -EPIPE)
		{
			/* EPIPE means overrun */
			err_log("Overrun occurred\n");
			snd_pcm_prepare(handle);
			break;
		}
		else if (err < 0)
		{
			err_log("Error from read: %s\n", snd_strerror(err));
			snd_pcm_close(handle);
			free(buffer);
			return err;
		}
		else if (err != (int) frames)
		{
			err_log("short read, read %d frames\n", err);
			snd_pcm_close(handle);
			free(buffer);
			return err;
		}
		int avr = pcm_db_average(buffer, size);
		//我也不晓得在ｎａｎｏｐｉ上面会有这个东东
		if (avr == 32768 && tempi < REMOVE_ENTERFACE_TIME)
		{
			continue;
		}
		tempi++;
		if (tempi < REMOVE_ENTERFACE_TIME)
		{
			continue;
		}
		//err_log("AVR:%d\n",avr);
		//连续帧的音量都高于阀值
		if ((avr >= envl + vladd) && !work)
		{
			sum = sum + avr;
			if (sum / VOICE_MAX_COUNT > envl + vladd)
			{
				state = 1;
				sum = 0;
			}
			count = 0;
		}
		else if (work && (avr >= envl + vladd))
		{
			state = 1;
			count = 0;
			sum = 0;
		}
		else if (work && (avr < envl + vladd))
		{
			//在录音，但是音量低于阀值 就是没说话了
			count++;
			if (count >= VOICE_SILENCE_COUNT)
			{   //多少帧的音量都没有超过阀值
				state = 0;
				count = 0;
			}
			sum = 0;
		}
		else
		{
			sum = 0;
			state = 0;
			work = 0;
		}
		avr = 0;
		if (state && !work)
		{   //记录第一帧和之前的几帧数据(就是开始录音)
			err_log("录音中...\n");
			write_speech_light_state(1);   //开启录音状态指示灯
			starttimes = start_time_s();   //记录开始录音的时间
			int j = 0, lastcount = 0;
			char *lastBuffer = (char *) calloc(1, sizeof(char));
			old_voice_data(NULL, 0, &lastBuffer, &lastcount);  //将开始录音之前的数据拷贝到数组当中（取）
			memcpy(totalBuff, lastBuffer, lastcount);
			_COUNT = 0 + lastcount;  //开始计数_COUNT
			if (_COUNT > MAX_WAV_SIZE)
			{
				err_log("Out if max wav size!\n");
				break;
			}
			for (int i = _COUNT; i < _COUNT + size; i++)
			{
				totalBuff[i] = buffer[j];
				j++;
			}
			_COUNT = _COUNT + size;
			work = 1;
			free(lastBuffer);
		}
		else if (state && work)
		{
			int j = 0;
			if (_COUNT + size > MAX_WAV_SIZE)
			{
				err_log("Out if max wav size!\n");
				break;
			}
			for (int i = _COUNT; i < _COUNT + size; i++)
			{
				totalBuff[i] = buffer[j];
				j++;
			}
			_COUNT = _COUNT + size;
			float t = get_time_difference_s(starttimes);
			if (t > DEFAULT_RECORD_TIME)
			{
				err_log("超出时间，跳出！\n");
				break;
			}
		}
		else if (!state && work)
		{
			int j = 0;
			if (_COUNT + size > MAX_WAV_SIZE)
			{
				err_log("Out of max wav size!");
				break;
			}
			for (int i = _COUNT; i < _COUNT + size; i++)
			{
				totalBuff[i] = buffer[j];
				j++;
			}
			_COUNT = _COUNT + size;
			work = 0;
			break;
		}
		else
		{
			//记录没录音之前的采样数据(存)
			char *final = NULL;
			int ret = old_voice_data(buffer, size, &final, 0);
			if (ret < 0)
			{
				err_log("The final data get failed!");
			}
		}
		memset(buffer, 0, size);
	}
	free(buffer);
	//必须先关闭PCM，后面语音识别之后播放识别内容需要用到。
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	write_speech_light_state(0);  //关闭录音状态指示灯
	//是否保存录音音频文件
	if (SAVE_RECORD_DATA)
	{
		write_wav_file(SAVE_WAV_FILE_NAME, totalBuff, _COUNT);
	}
	//开始调用语音识别
	speech_record(totalBuff, _COUNT);
	//清空相关缓冲区
	memset(totalBuff, 0, MAX_WAV_SIZE);
	memset(_OLD_VOICE_BUFFER, 0, MAX_WAV_SIZE);
	//重置计数器
	_OLD_VOICE_TEMP_COUNT = 0;
	_OLD_VOICE_TOTAL_COUNT = 0;
	_COUNT = 0;
	//结束本次识别
	now_time(&nowTime);
	err_log("本次识别结束！当前时间:%s\n", nowTime);
	free(nowTime);
	return 0;
}

/*
 * 测试环境音量大小
 */
int test_envirment_volume(struct WaveHeader *hdr)
{
	int err, size, dir, avr = 0, sum = 0, tempi = 0;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int sampleRate = hdr->sample_rate;
	snd_pcm_uframes_t frames = 64;
	char *buffer;
	/* Open PCM device for recording (capture). */
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0);
	if (err)
	{
		err_log("Unable to open PCM device: %s\n", snd_strerror(err));
		return -1;
	}
	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);
	/* Fill it in with default values. */
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0)
	{
		err_log("snd_pcm_hw_params_any error,%s\n", snd_strerror(err));
		return -2;
	}
	/* ### Set the desired hardware parameters. ### */
	/* Interleaved mode */
	err = snd_pcm_hw_params_set_access(handle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err)
	{
		err_log("Error setting interleaved mode: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -3;
	}
	/* Signed 16-bit little-endian format */
	switch ((hdr->bits_per_sample) / 8)
	{
	case 1:
		err = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U8);
		break;
	case 2:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S16_LE);
		break;
	case 3:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S24_LE);
		break;
	default:
		err = snd_pcm_hw_params_set_format(handle, params,
				SND_PCM_FORMAT_S16_LE);
	}
	if (err)
	{
		err_log("Error setting format: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -4;
	}
	/* set channels (stereo) */
	err = snd_pcm_hw_params_set_channels(handle, params,
			hdr->number_of_channels);
	if (err)
	{
		err_log("Error setting channels: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -5;
	}
	/* there need 1600 */
	sampleRate = hdr->sample_rate;
	err = snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, &dir);
	if (err)
	{
		err_log("Error setting sampling rate (%d): %s\n", sampleRate,
				snd_strerror(err));
		snd_pcm_close(handle);
		return -6;
	}
	hdr->sample_rate = sampleRate;
	/* Set period size*/
	err = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
	if (err)
	{
		err_log("Error setting period size: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -7;
	}
	/* Write the parameters to the driver */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0)
	{
		err_log("Unable to set HW parameters: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -8;
	}
	/* Use a buffer large enough to hold one period */
	err = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	if (err)
	{
		err_log("Error retrieving period size: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		return -9;
	}
	size = frames * hdr->bits_per_sample / 8 * hdr->number_of_channels;
	buffer = (char *) malloc(size);
	if (!buffer)
	{
		err_log("Buffer error.\n");
		snd_pcm_close(handle);
		return -10;
	}
	err = snd_pcm_hw_params_get_period_time(params, &sampleRate, &dir);
	if (err)
	{
		err_log("Error retrieving period time: %s\n", snd_strerror(err));
		snd_pcm_close(handle);
		free(buffer);
		return -11;
	}
	for (int i = 0; i < TEST_ENVIRMENT_VOICE_TIME; i++)
	{
		err = snd_pcm_readi(handle, buffer, frames);
		if (err == -EPIPE)
		{
			/* EPIPE means overrun */
			err_log("Overrun occurred\n");
			snd_pcm_prepare(handle);
			break;
		}
		else if (err < 0)
		{
			err_log("Error from read: %s\n", snd_strerror(err));
			snd_pcm_close(handle);
			free(buffer);
			return -12;
		}
		else if (err != (int) frames)
		{
			err_log("short read, read %d frames\n", err);
			snd_pcm_close(handle);
			free(buffer);
			return -13;
		}
		int avrframe = pcm_db_average(buffer, size);
		//printf("Size:%d  avrframe:%d frames:%d\n",size,avrframe,frames);
		//消除异常数据
		if (avr == 32768 && tempi < REMOVE_ENTERFACE_TIME)
		{
			continue;
		}
		tempi++;
		if (tempi < REMOVE_ENTERFACE_TIME)
		{
			continue;
		}
		if (avrframe == 0)
		{
			err_log("Please check your mic!\n");
			avr = 0;
			break;
		}
		else
		{
			sum = sum + avrframe;
		}
		memset(buffer, 0, size);
	}
	avr = sum / TEST_ENVIRMENT_VOICE_TIME;
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	return avr;
}

/*
 * 测试环境，语音识别开始
 */
int init_record()
{
	int err, retry = 0;
	struct WaveHeader *hdr;
	hdr = generic_wav_header(SIMPLE_RATE, BIT_DEPTH, CHANNELS);
	int testtime = 0.008 * TEST_ENVIRMENT_VOICE_TIME;
	if (!hdr)
	{
		err_log("Error allocating WAV header.\n");
		return -1;
	}
	if (MP3_PLAY_METHOD)
	{
		err_log("语音播放方式：外置播放软件（sox）\n");
	}
	else
	{
		err_log("语音播放方式：内置播放程序（mpg123）\n");
	}
	err_log("测试环境音量大小中...大约需要%d秒\n", testtime);
	int avr = test_envirment_volume(hdr);
	err_log("环境音量大小(最高32767)：%d\n", avr);
	if (avr > 0 && avr <= 5000)
	{
		err_log("环境杂音等级：安静\n");
	}
	else if (avr > 5000 && avr <= 10000)
	{
		err_log("环境杂音等级：良好\n");
	}
	else if (avr > 10000 && avr <= 15000)
	{
		err_log("环境杂音等级：中等\n");
	}
	else if (avr > 15000 && avr <= 20000)
	{
		err_log("环境杂音等级：较吵\n");
	}
	else if (avr > 20000 && avr <= 25000)
	{
		err_log("环境杂音等级：嘈杂\n");
	}
	else if (avr > 25000)
	{
		err_log("你所在的环境太过于嘈杂，不建议运行程序！\n");
	}
	else
	{
		err_log("请检查麦克风是否插入或是否打开了麦克风静默！\n");
		free(hdr);
		return -1;
	}
	err_log("准备进入语音识别环境！\n");
	while (1)
	{
		err = start_record(hdr, avr);
		if (err)
		{
			err_log("启动语音识别环境失败!错误码：%d\n", err);
			retry++;
			if (retry > 3)
			{
				free(hdr);
				return -3;
			}
			err_log("第%d次重试！\n", retry);
		}
		else
		{
			retry = 0;
		}
	}
	return 0;
}

