/*
 * httprequest.h
 *
 *  Created on: 2017年9月2日
 *      Author: cheetah
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <stdio.h>
#include <string.h>
#include "curl/curl.h"
#include "curl/easy.h"
#include "../err_log.h"

struct MemoryStruct
{
	char *memory;
	size_t size;
};

int http_get_request(char *url, char **result, size_t *memsize);
int http_post_request(char *url, char *data, char **result, size_t *memsize);
size_t call_back_request(void *contents, size_t size, size_t nmemb, void *userp);

#endif /* HTTPREQUEST_H_ */
