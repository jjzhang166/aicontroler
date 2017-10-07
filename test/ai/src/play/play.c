#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <endian.h>
#include <mpg123.h>
#include <unistd.h>

#include "play.h"
#include "../err_log.h"
#include "../config/config.h"

int mp3_player_from_mem(unsigned char *audiodata, int content_len)
{
	int err, retval, channels = 1, buffer_size = 1152;
	long int rate = 16000;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	size_t lretval;
	mpg123_handle *m;
	snd_pcm_uframes_t frames = 1152;
	unsigned char * buffer;
	// init mpg123, decode first chunk and set audio output according to that.
	int ret = mpg123_init();
	if (ret < 0)
	{
		err_log("MPG123 init failed!\n");
		return -3;
	}
	m = mpg123_new(NULL, &retval);
	if (!m)
	{
		err_log("Unable to init libmpg123: %s\n",
				mpg123_plain_strerror(retval));
		return -4;
	}
	if (mpg123_param(m, MPG123_VERBOSE, 2, 0) != MPG123_OK) //解码设置
	{
		err_log("Set mpg123_param failed!\n", content_len);
		return -5;
	}
	if (mpg123_open_feed(m) != MPG123_OK)
	{
		err_log("Unable to open datastream!Len:%d\n", content_len);
		return -6;
	}
	retval = mpg123_decode(m, audiodata, content_len, NULL, 0, &lretval);	//解码
	if (retval == MPG123_NEW_FORMAT)
	{
		if (mpg123_getformat(m, &rate, &channels, &retval) != MPG123_OK)
		{
			err_log("Error trying to parse stream!\n");
			return -7;
		}
	}
	else
	{
		err_log("[GET FORMAT]No new format rules? (%s) Ayeeee!\n",
				mpg123_plain_strerror(retval));
		return -8;
	}
	// Open PCM device for playback
	if ((err = snd_pcm_open(&playback_handle, base_set.default_play_device,
			SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		err_log("ERROR: Cannot open pcm device. %s\n", snd_strerror(err));
		return -9;
	}
	// Allocate hardware parameters
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		err_log("ERROR: Cannot allocate hardware parameters. %s\n",
				snd_strerror(err));
		return -10;
	}
	// Initialize parameters with default values
	if ((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
	{
		err_log("ERROR: Cannot initialize hardware parameters. %s\n",
				snd_strerror(err));
		return -11;
	}
	// Setting hardware parameters
	if ((err = snd_pcm_hw_params_set_access(playback_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		err_log("ERROR: Cannot set interleaved mode. %s\n", snd_strerror(err));
		return -12;
	}
	if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0)
	{
		err_log("ERROR: Cannot set PCM format. %s\n", snd_strerror(err));
		return -13;
	}
	if ((err = snd_pcm_hw_params_set_channels_near(playback_handle, hw_params,
			(unsigned int *) &channels)) < 0)
	{
		err_log("ERROR: Cannot set number of channels. %s\n",
				snd_strerror(err));
		return -14;
	}
	if ((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params,
			(unsigned int *) &rate, 0)) < 0)
	{
		err_log("ERROR: Cannot set plyabck rate. %s\n", snd_strerror(err));
		return -15;
	}
	if ((err = snd_pcm_hw_params_set_period_size_near(playback_handle,
			hw_params, &frames, 0)) < 0)
	{
		err_log("cannot set period size (%s)\n", snd_strerror(err));
		return -16;
	}
	if ((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
	{
		err_log("ERROR: Cannot set hardware parameters. %s\n",
				snd_strerror(err));
		return -17;
	}
	if ((err = snd_pcm_hw_params_get_channels(hw_params,
			(unsigned int *) &channels)) < 0)
	{
		err_log("Playback ERROR: Can't get channel number. %s\n",
				snd_strerror(err));
		return -18;
	}
	if ((err = snd_pcm_hw_params_get_rate(hw_params, (unsigned int *) &rate, 0))
			< 0)
	{
		err_log("ERROR: Cannot get rate. %s\n", snd_strerror(err));
		return -19;
	}
	// Free paraemeters
	snd_pcm_hw_params_free(hw_params);
	if ((err = snd_pcm_prepare(playback_handle)) < 0)
	{
		err_log("Cannont prepare audio interface for use (%s)\n",
				snd_strerror(err));
		return -20;
	}
	// Create buffer
	buffer_size = mpg123_outblock(m);
	buffer = (unsigned char *) calloc(buffer_size, sizeof(char));
	int underrun = 0;
	while (1)
	{
		memset(buffer, 0, buffer_size);
		retval = mpg123_decode(m, NULL, 0, buffer, buffer_size, &lretval);	//解码
		if (retval == MPG123_NEW_FORMAT)
		{
			snd_pcm_drain(playback_handle);	// let it play up until here in the former frequency
			if (mpg123_getformat(m, &rate, &channels, &retval) != MPG123_OK)
			{
				err_log("Error while changing stream bitrate/audio format.\n");
				break;
			}
			if ((err = snd_pcm_hw_params_set_rate_near(playback_handle,
					hw_params, (unsigned int *) &rate, 0)) < 0)
			{
				err_log("Could not set sample rate to %d (%s)\n", (int) rate,
						snd_strerror(err));
				break;
			}
			if ((err = snd_pcm_hw_params_set_channels(playback_handle,
					hw_params, channels)) < 0)
			{
				err_log("Could not set channel count to %d (%s)\n", channels,
						snd_strerror(err));
				break;
			}
			if ((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
			{
				err_log("Cannot set parameters (%s)\n", snd_strerror(err));
				break;
			}
		}
		else if (retval == MPG123_OK)
		{
			if (lretval)
			{
				int ret = snd_pcm_writei(playback_handle, buffer,
						(lretval / (channels << 1)));
				if (ret == -EPIPE)
				{
					/* EPIPE means underrun */
					err_log("An underrun occurred.error code %d\n", ret);
					//完成硬件参数设置，使设备准备好
					snd_pcm_prepare(playback_handle);
					underrun++;
					if (underrun < 4)
					{
						continue;
					}
					else
					{
						break;
					}
				}
				else if (ret == -EBADFD)
				{
					err_log("PCM is not in the right state.error code %d\n",
							ret);
					break;
				}
				else if (ret == -ESTRPIPE)
				{
					err_log("A suspend event occurred.error code %d\n", ret);
					break;
				}
				else if (ret < 0)
				{
					err_log("error from writei: %s\n", snd_strerror(ret));
					break;
				}
			}
			else
			{
				break;
			}
		}
		else if (retval == MPG123_DONE)
		{
			break;
		}
		else if (retval == MPG123_NEED_MORE)
		{
			break;
		}
		else
		{
			err_log("[DECODE]No new format rules? (%s) Ayeeee!\n",
					mpg123_plain_strerror(retval));
			break;
		}
	}
	snd_pcm_drain(playback_handle);
	snd_pcm_close(playback_handle);
	mpg123_close(m);
	mpg123_delete(m);
	mpg123_exit();
	free(buffer);
	return 0;
}

/*
 * 使用外部播放器播放存放的mp3文件
 */
int mp3_player_from_file(char *mp3path)
{
	if (mp3path == NULL || !strcmp(mp3path, ""))
	{
		err_log("MP3 path is empty!\n");
		return -1;
	}
	char *cmd = "play -q ";
	char *play = (char *) calloc(strlen(cmd) + strlen(mp3path) + 1,
			sizeof(char));
	sprintf(play, "%s%s", cmd, mp3path);
	system(play);
	free(play);
	return 0;
}
