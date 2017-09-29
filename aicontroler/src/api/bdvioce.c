/*
 * 百度语音识别SDK
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "api.h"
#include "../config.h"
#include "../base64/b64.h"
#include "../rectime.h"
#include "../writefile.h"
#include "../http/httprequest.h"
#include "../cjson/cJSON.h"
#include "../play/play.h"

#define MAX_BUFFER_SIZE 512
#define MAX_BODY_SIZE 1000000

time_t LAST_TIKEN_TIME;
char *TOKEN = NULL;

int get_baidu_api_token(char **token);
int getCmd(char *result, char **back);

/*
 * 获取token
 */
int get_baidu_api_token(char **token)
{
	char *result = (char *) malloc(1);
	char host[MAX_BUFFER_SIZE];
	size_t memsize;
	snprintf(host, sizeof(host),
			"openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s",
			BAIDU_APIKEY, BAIDU_SECRETKEY);
	int ret = http_get_request(host, &result, &memsize);
	if (ret < 0 || memsize == 0)
	{
		err_log("Get result failed!\n");
		return -1;
	}
	if (result != NULL)
	{
		cJSON *jsonData = cJSON_Parse(result);
		if (jsonData)
		{
			*token = cJSON_GetObjectItem(jsonData, "access_token")->valuestring;
		}
	}
	else
	{
		err_log("The host result data is null!\n");
		return -2;
	}
	LAST_TIKEN_TIME = start_time_s();
	free(result);
	return 0;
}

/*
 * 百度语音识别借口 数据上传方式为method1
 */
int bd_voice_recognition(char *data, int content_len, char **back)
{
	int retNum = 0;
	char host[MAX_BUFFER_SIZE];
	err_log("识别中...\n");
	//TOKEN为空或者token过期了才重新获取
	if ((TOKEN == NULL || !strcmp(TOKEN, ""))
			|| get_time_difference_s(LAST_TIKEN_TIME) > 86400)
	{
		int ret = get_baidu_api_token(&TOKEN);
		if (ret < 0)
		{
			err_log("Get token failed!\n");
			retNum = -1;
			return retNum;
		}
	}
	memset(host, 0, sizeof(host));
	snprintf(host, sizeof(host), "%s", "http://vop.baidu.com/server_api");
	//method 1
	char tmp[MAX_BUFFER_SIZE];
	memset(tmp, 0, sizeof(tmp));
	char body[MAX_BODY_SIZE];
	memset(body, 0, sizeof(body));
	char* decode_data = b64_encode((const unsigned char *) data, content_len);
	if (0 == strlen(decode_data))
	{
		err_log("base64 encoded data is empty.\n");
		retNum = -2;
		return retNum;
	}
	// 创建JSON Object
	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "format", "pcm");
	cJSON_AddNumberToObject(root, "rate", 16000);
	cJSON_AddNumberToObject(root, "channel", 1);
	cJSON_AddStringToObject(root, "token", TOKEN);
	cJSON_AddStringToObject(root, "cuid", BAIDU_CUID);
	cJSON_AddStringToObject(root, "speech", decode_data);
	cJSON_AddNumberToObject(root, "len", content_len);
	content_len = strlen(cJSON_PrintUnformatted(root));
	memcpy(body, cJSON_PrintUnformatted(root), content_len);
	CURL *curl;
	CURLcode res;
	struct curl_slist *headerlist = NULL;
	struct MemoryStruct chunk;
	chunk.memory = (char *) calloc(1, sizeof(char));
	chunk.size = 0;
	snprintf(tmp, sizeof(tmp), "%s",
			"Content-Type: application/json; charset=utf-8");
	headerlist = curl_slist_append(headerlist, tmp);
	snprintf(tmp, sizeof(tmp), "Content-Length: %d", content_len);
	headerlist = curl_slist_append(headerlist, tmp);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, host);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content_len);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_back_request);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&chunk);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		err_log("perform curl error:%d.\n", res);
		retNum = -3;
		return retNum;
	}
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);
	if (chunk.memory)
	{
		cJSON *jsonData = cJSON_Parse(chunk.memory);
		if (jsonData)
		{
			int errcode = cJSON_GetObjectItem(jsonData, "err_no")->valueint;
			if (errcode == 0)
			{
				char *resultContent = (char *) calloc(1, sizeof(char));
				cJSON *item = cJSON_GetObjectItem(jsonData, "result");
				if (item)
				{
					char *getdata = cJSON_GetArrayItem(item, 0)->valuestring;
					int ret = getCmd(getdata, &resultContent);
					if (ret < 0)
					{
						err_log("Get baidu speech rec content failed!\n");
						retNum = -4;
					}
					else
					{
						*back = (char *) calloc(strlen(resultContent) + 1,
								sizeof(char));
						strcpy(*back, resultContent);
					}
				}
			}
			else
			{
				char *errmsg =
						cJSON_GetObjectItem(jsonData, "err_msg")->valuestring;
				if (errmsg)
				{
					err_log("Baidu speech rec error:%s\n", errmsg);
				}
				else
				{
					err_log("Baidu speech rec error:no error data!\n");
				}
				retNum = -5;
			}
		}
		free(chunk.memory);
		chunk.size = 0;
	}
	curl_global_cleanup();
	cJSON_Delete(root);
	return retNum;
}

/*
 * 百度文字转语音
 */
int bd_voice_tts(char *tts, char **resultAudio)
{
	err_log("文字转语音...\n");
	int testCode = strlen("汉");  //测试一个汉子的字节数
	if (strlen(tts) > 125 * testCode)
	{  //baidu　tts限定１００个字符以内，但是实际测试是可以到１２５左右的。
		tts = "这已经超出我的应答极限啦！";
		err_log("回复超出限制，应答：%s\n", tts);
	}
	char host[MAX_BUFFER_SIZE];
	char *result = (char *) calloc(1, sizeof(char));
	size_t memsize = 0;
	int ret = 0;
	if (tts == NULL || !strcmp(tts, ""))
	{
		err_log("Tts input is empty!\n");
		return 0;
	}
	//TOKEN为空或者token过期了才重新获取
	if ((TOKEN == NULL || !strcmp(TOKEN, ""))
			|| get_time_difference_s(LAST_TIKEN_TIME) > 86400)
	{
		int ret = get_baidu_api_token(&TOKEN);
		if (ret < 0)
		{
			err_log("Get token failed!\n");
			return -1;
		}
	}
	snprintf(host, sizeof(host),
			"tsn.baidu.com/text2audio?tex=%s&lan=zh&cuid=%s&ctp=1&tok=%s", tts,
			BAIDU_CUID, TOKEN);
	ret = http_get_request(host, &result, &memsize);
	if (ret < 0 || memsize == 0)
	{
		err_log("Get tts result failed!\n");
		return -1;
	}
	if (result == NULL || !strcmp(result, "") || memsize == 0)
	{
		err_log("Tts result is empty!\n");
		return -1;
	}
	*resultAudio = (char *) calloc(memsize + 1, sizeof(char));
	memcpy(*resultAudio, result, memsize);

	err_log("应答中...\n");
	if (MP3_PLAY_METHOD == 1)
	{

		ret = write_get_file(SAVE_TTS_FILE_NAME, *resultAudio, memsize);
		if (ret < 0)
		{
			err_log("Write file failed!\n");
		}
		ret = mp3_player_from_file(SAVE_TTS_FILE_NAME);
		if (ret < 0)
		{
			err_log("Play mp3 from file failed!\n");
		}
	}
	else
	{
		ret = mp3_player_from_mem((unsigned char *) *resultAudio, memsize);
		if (ret < 0)
		{
			err_log("MP3 play failed!\n");
		}
		if (SAVE_TTS_VOICE_DATA)  //是否保存音频文件
		{
			ret = write_get_file(SAVE_TTS_FILE_NAME, *resultAudio, memsize);
			if (ret < 0)
			{
				err_log("Write file failed!\n");
			}
		}
	}
	memset(host, 0, MAX_BUFFER_SIZE);
	free(result);
	return 0;
}

/*
 * 去掉百度语音识别之后的最后一个中文符号
 */
int getCmd(char *result, char **back)
{
	if (result == NULL || !strcmp(result, ""))
	{
		return -1;
	}
	int testCode = strlen("汉");  //测试一个汉子所需要的字节数。ＵＴＦ－８想需要３个字节
	int len = strlen(result) - testCode;
	char get[len + 1];
	strncpy(get, result, len);
	get[len] = '\0';
	*back = (char *) calloc(len + 1, sizeof(char));
	strcpy(*back, get);
	return 0;
}

