/*
 * api.h
 *
 *  Created on: 2017年9月21日
 *      Author: cheetah
 */

#ifndef SRC_API_API_H_
#define SRC_API_API_H_

#include "../err_log.h"

//百度语音识别和语音合成
int bd_voice_recognition(char *data, int content_len, char **back);
int bd_voice_tts(char *tts, char **resultAudio);
//图灵机器人
int tl_reboot(char *input, char **output, size_t *outlen);

#endif /* SRC_API_API_H_ */
