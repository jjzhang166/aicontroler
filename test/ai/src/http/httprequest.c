#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "httprequest.h"

size_t call_back_request(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *) userp;
	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL)
	{
		err_log("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}

int http_get_request(char *url, char **result, size_t *memsize)
{
	if (url == NULL || !strcmp(url, ""))
	{
		err_log("Request url cannot null!\n");
		return -1;
	}
	struct MemoryStruct chunk;
	chunk.memory = (char *) calloc(1, sizeof(char));
	chunk.size = 0;
	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, call_back_request);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void * )&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK)
	{
		err_log("curl_easy_perform() get failed: %s\n",
				curl_easy_strerror(res));
		return -2;
	}
	curl_easy_cleanup(curl_handle);
	if (chunk.memory)
	{
		*result = (char*) calloc(chunk.size + 1, sizeof(char));
		if (*result != NULL)
		{
			memcpy(*result, chunk.memory, chunk.size);
		}
		*memsize = chunk.size;
		free(chunk.memory);
		chunk.size = 0;
	}
	curl_global_cleanup();
	return 0;
}

int http_post_request(char *url, char *data, char **result, size_t *memsize)
{
	if (url == NULL || !strcmp(url, ""))
	{
		err_log("Request url cannot null!\n");
		return -1;
	}
	struct MemoryStruct chunk;
	chunk.memory = (char *) calloc(1, sizeof(char));
	chunk.size = 0;
	CURLcode res;
	CURL *curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url); //url地址
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); //post参数
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_back_request); //对返回的数据进行操作的函数地址
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&chunk); //这是write_data的第四个参数值
	curl_easy_setopt(curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		err_log("curl_easy_perform() get failed: %s\n",
				curl_easy_strerror(res));
		return -2;
	}
	curl_easy_cleanup(curl);
	if (chunk.memory)
	{
		*result = (char*) calloc(chunk.size + 1, sizeof(char));
		if (*result != NULL)
		{
			memcpy(*result, chunk.memory, chunk.size);
		}
		*memsize = chunk.size;
		free(chunk.memory);
		chunk.size = 0;
	}
	curl_global_cleanup();
	return 0;
}
