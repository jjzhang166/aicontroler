/*
 * config.h
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

//=========================================================
/*
 * 百度语音识别API参数
 */
#define BAIDU_CUID "10210356"
#define BAIDU_APIKEY "6GhGZEfWavkb0dv2AiHlkluD"
#define BAIDU_SECRETKEY "5044cf86cfbde48cf9915a46489fea53"

/*
 * 图灵机器人API参数
 */
#define TULING_KEY "26a3d15c8f204f6fa2069d5d325a64b0"
#define TULING_UID "770702827"  //这个UID是自定义的一个ID，有了这个ID可以进行前后文关联

//=========================================================
/*
 * 是否保存百度返回的tts文件
 */
#define SAVE_TTS_VOICE_DATA 0

/*
 * 保存返回tts数据的文件名称
 */
#define SAVE_TTS_FILE_NAME "tts.mp3"

/*
 * 是否保存录音数据为WAV文件
 */
#define SAVE_RECORD_DATA 0

/*
 * 保存录音数据的文件名称
 */
#define SAVE_WAV_FILE_NAME "voice.wav"

/*
 * 默认播放设备
 */
#define DEFAULT_PALY_DEVICE "plughw:0,0"

/*
 * 默认录音设备
 */
#define DEFAULT_RECORD_DEVICE "plughw:0,0"

//=========================================================

/*
 * 百度语音合成之后的数据播放方法。１为使用外部播放器（sox）。其它则使用程序内置播放方式。
 * 设置这个的目的是解决在某些系统上面内置程序播放mp3数据出问题的bug。
 */
#define MP3_PLAY_METHOD 0

//=========================================================
/*
 * 百度语音识别所需要的基本参数，这个不要修改
 */
#define SIMPLE_RATE 16000
#define BIT_DEPTH 16
#define CHANNELS 1

//=========================================================

/*
 * 默认录音时间，超过这个时间后会跳出录音直接进行识别
 */
#define DEFAULT_RECORD_TIME 8

/*
 * 记录在录音之前的前多少帧的数据，解决声音识别延迟问题或者开始说话后未检测到录音的问题
 */
#define SAVE_LAST_FRAME 60

/*
 * 当检测到　环境音量>初始音量＋这个值 的时候，开始录音
 * 如果想要灵敏度高一点的话，就调低这个值。自行调整
 */
#define VOICE_THRESHOLD 400

/*
 * 录音过程中超过多少帧小于阀值，停止录音。更具语速来调整，不要过低或者过高
 */
#define VOICE_SILENCE_COUNT 150

/*
 * 连续多少帧大于阀值才开始录音
 */
#define VOICE_MAX_COUNT 20

/*
 * 环境音量测试次数
 */
#define TEST_ENVIRMENT_VOICE_TIME 600

/*
 * 在Nanopi上面，每次重新开始录音之后都会产生20-30次采样异常
 * 在这儿就设置了一个值，抛弃掉前面30次采样数据。不影响电脑和其他设备
 * 如果每次都是正常采样。可以将这个值设置为0
 */
#define REMOVE_ENTERFACE_TIME 30

//=========================================================

//GPIO
#define SPEECH_WORK_STATE_PIN 0      //录音指示灯
#define BDTTS_WORK_STATE_PIN 1       //语音识别工作指示灯

//=========================================================

#endif /* SRC_CONFIG_H_ */
