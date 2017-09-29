/*
 * rebot.c
 *
 *  Created on: 2017年9月1日
 *      Author: cheetah
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../config.h"
#include "../http/httprequest.h"
#include "../cjson/cJSON.h"
#include "api.h"

/*
 * 图灵机器人
 * 更具input的问题，输出相关答案
 * 参数input 输入； **result 输入答案存放地址；*outlen 答案数据长度
 */
int tl_reboot(char *input, char **result, size_t *outlen)
{
	if (input == NULL || !strcmp(input, ""))
	{
		err_log("Tuling input is empty!\n");
		return -1;
	}
	char *url = TULING_API_URL;
	char *post = (char *) calloc(1, sizeof(char));
	size_t postsize = 0;
	//post的数据长度
	int postDataLen = 18 + strlen(TULING_KEY) + strlen(TULING_UID)
			+ strlen(input);
	char *postdata = (char *) malloc(postDataLen + 1);
	snprintf(postdata, postDataLen, "key=%s&info=%s&userid=%s", TULING_KEY,
			input, TULING_UID);
	//通过HTTP POST的方式提交数据
	int ret = http_post_request(url, postdata, &post, &postsize);
	if (ret < 0)
	{
		err_log("Post tuling data error!\n");
		return -1;
	}
	if ((post == NULL || !strcmp(post, "")) || postsize == 0)
	{
		err_log("Tuling result data is empty!\n");
		return -2;
	}
	cJSON *jsonData = cJSON_Parse(post);
	if (jsonData)
	{
		char *text = cJSON_GetObjectItem(jsonData, "text")->valuestring;
		int textLen = strlen(text);
		*result = (char *) calloc(textLen + 1, sizeof(char));
		strcpy(*result, text);
		*outlen = textLen;
	}
	else
	{
		char *text = "小白没有获取到相关内容哦";
		int textLen = strlen(text);
		*result = (char *) calloc(textLen + 1, sizeof(char));
		strcpy(*result, text);
		*outlen = textLen;
	}
	free(postdata);
	free(post);
	return 0;
}
