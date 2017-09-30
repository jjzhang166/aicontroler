/*
 * writefile.h
 *
 *  Created on: 2017年9月2日
 *      Author: cheetah
 */

#ifndef WRITEFILE_H_
#define WRITEFILE_H_

#include <stdio.h>
#include "err_log.h"

int write_wav_file(char *filePath, char *data, size_t len);
int write_get_file(char *filePath, char *data, size_t len);

#endif /* WRITEFILE_H_ */
