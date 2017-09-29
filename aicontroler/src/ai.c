/*
 * ai.c
 * 作用：开始进行语音识别，对识别后的内容进行处理
 *
 *  Created on: 2017年9月3日
 *      Author: cheetah
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "ai.h"
#include "api/api.h"
#include "control.h"

int _START_EXIT = 0;
int _OPEN_VOICE = 1;
int _BD_TTS_TIME = 0;
int _TL_RCV_TIME = 0;
int find_cmd_local(char *cmdstr);
int process_cmd(int local, char **result);

/*
 * 将需要的指令填到这里面格式为编号+指令内容,例：001退出系统
 * 然后在process_cmd中进行处理指令
 */
char *cmdlist[] = { "001退出系统", "002退出程序", "003关闭程序", "004确定", "005关闭语音",
		"006不要回复", "007关闭回复", "008打开语音", "009开启语音", "010开启回复", "011是的",
		"012打开电灯", "013开灯", "014开一下灯", "015点亮电灯", "016关闭电灯", "017关灯", "018关一下灯",
		"019熄灭电灯", "020开电灯", "021关电灯" };

/*
 * 还有种写法是下面这种 序号.指令层次+指令
 * 比如 001.0退出系统 表示0层，不会继续匹配指令
 *     001.1打开 表示1层指令，会进行拼接二级指令，依次类推。当下一级没有指令之后进行匹配输入指令。
 *     012.2客厅 表示二级指令。让上级指令进行拼接。比如通过1级指令拼接之后就是打开客厅....依次类推就是打开客厅电灯，或者打开客厅冰箱
 * 遗留问题就是我没想好怎么处理多指令，所以没写这个算法了。有时间和兴趣可以想想。
 *
char *cmdlist[] = { "001.0退出系统", "002.0退出程序", "003.0关闭程序", "004.0确定", "005.0关闭语音",
		"006.0不要回复", "007.0关闭回复", "008.0打开语音", "009.0开启语音", "010.0开启回复", "011.0是的",
		"012.1打开","013.2卧室","014.2客厅","015.3电灯","016.1" };
 */

int speech_record(char *buffer, int count)
{
	err_log("录音完成，进入识别阶段！\n");
	write_bdtts_light_state(1);
	if (count != 0 && (buffer != NULL || !strcmp(buffer, "")))
	{
		char *result = (char *) calloc(1, sizeof(char));
		size_t resultCount = 0;
		//百度语音识别
		int ret = bd_voice_recognition(buffer, count, &result);
		if (ret < 0)
		{
			err_log("语音识别错误！错误：%d\n", ret);
		}
		else if (result == NULL || !strcmp(result, ""))
		{
			err_log("未能识别到任何内容！\n");
		}
		else
		{
			err_log("语音识别内容：%s\n", result);
			//先查找本地指令
			int ret = find_cmd_local(result);
			if (ret == 0)
			{
				//未找到，那么使用图灵机器人应答
				ret = tl_reboot(result, &result, &resultCount);
				if (ret < 0)
				{
					err_log("图灵机器人智能应答获取失败！\n");
					free(result);
					return -1;
				}
				//如果前一个指令是退出程序，但是现在的指令不是确认指令，重置准备退出状态为０
				if (_START_EXIT)
				{
					_START_EXIT = 0;
				}
				err_log("图灵机器人应答：%s\n", result);
			}
			else
			{
				process_cmd(ret, &result);
				err_log("命令执行应答：%s\n", result);
				if (!_OPEN_VOICE)
				{  //当关闭回复之后，回复命令执行语音
					ret = bd_voice_tts(result, &result);
					if (ret < 0)
					{
						err_log("文字转语音失败！\n");
						free(result);
						return -1;
					}
				}
			}
			if (_OPEN_VOICE)
			{
				ret = bd_voice_tts(result, &result);
				if (ret < 0)
				{
					err_log("文字转语音失败！\n");
					free(result);
					return -1;
				}
			}
		}
		free(result);
	}
	else
	{
		err_log("音频数据为空！\n");
		return -3;
	}
	write_bdtts_light_state(0);
	return 0;
}

/*
 * 处理指令
 */
int process_cmd(int local, char **result)
{
	char *back;
	if (local == 0 || *result == NULL)
	{
		return -1;
	}
	//正对退出指令
	if (_START_EXIT && (local == 4 || local == 11))
	{
		err_log("程序已退出!\n");
		exit(0);
	}
	else if (_START_EXIT && (local != 1 || local != 2 || local != 3))
	{
		_START_EXIT = 0;
	}
	switch (local)
	{
	case 1:  //退出程序
	case 2:
	case 3:
		_START_EXIT = 1;
		back = "确定退出语音识别程序吗？";
		break;
	case 5:  //关闭语音回复
	case 6:
	case 7:
		_OPEN_VOICE = 0;
		back = "已经为您关闭语音回复！";
		break;
	case 8:  //开启语音回复
	case 9:
	case 10:
		_OPEN_VOICE = 1;
		back = "已经为您开启语音回复！";
		break;
	case 12:  //开灯
	case 13:
	case 14:
	case 15:
	case 20:
		write_bedroom_light_state(1);
		back = "已经为您打开电灯！";
		break;
	case 16:  //关灯
	case 17:
	case 18:
	case 19:
	case 21:
		write_bedroom_light_state(0);
		back = "已经为您关闭电灯！";
		break;
	default:  //未识别到内容
		back = "本机没法识别你提交的参数所表达的意思！";
		break;
	}
	*result = (char *) calloc(strlen(back) + 1, sizeof(char));
	strcpy(*result, back);
	return 0;
}

/*
 * 匹配语音识别之后的内容是是否有符合设置的内容
 * return 为指令数组的中的位置。第一个位置为１
 */
int find_cmd_local(char *cmdstr)
{
	if (cmdstr == NULL || !strcmp(cmdstr, ""))
	{
		return 0;
	}
	int cmdCount = sizeof(cmdlist) / sizeof(cmdlist[1]);
	for (int i = 0; i < cmdCount; i++)
	{
		char *tempCmd = (char *) calloc(strlen(cmdlist[i] + 1), sizeof(char));
		strcpy(tempCmd, cmdlist[i] + 3);  //拷贝指令，原指令指针位置加3
		for (int j = 0; j < strlen(cmdstr); j++)
		{
			if (strlen(cmdstr) - j < strlen(tempCmd))
			{
				break;
			}
			else
			{
				int ret = strncmp(&cmdstr[j], tempCmd, strlen(tempCmd));
				if (ret == 0)
				{
					int back = i + 1;
					i = cmdCount;
					free(tempCmd);
					return back;
				}
			}
		}
		free(tempCmd);
	}
	return 0;
}
