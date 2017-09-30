/*
 * writewav.c
 *
 *  Created on: 2017年8月30日
 *      Author: cheetah
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include "config/config.h"
#include "record.h"

int write_wav_header(int fd, struct WaveHeader *hdr);

int write_wav_header(int fd, struct WaveHeader *hdr)
{
	if (!hdr)
		return -1;
	write(fd, &hdr->RIFF_marker, 4);
	write(fd, &hdr->file_size, 4);
	write(fd, &hdr->filetype_header, 4);
	write(fd, &hdr->format_marker, 4);
	write(fd, &hdr->data_header_length, 4);
	write(fd, &hdr->format_type, 2);
	write(fd, &hdr->number_of_channels, 2);
	write(fd, &hdr->sample_rate, 4);
	write(fd, &hdr->bytes_per_second, 4);
	write(fd, &hdr->bytes_per_frame, 2);
	write(fd, &hdr->bits_per_sample, 2);
	write(fd, "data", 4);
	uint32_t data_size = hdr->file_size + 8 - 44;
	write(fd, &data_size, 4);
	return 0;
}

int write_wav_file(char *filePath, char *data, size_t len)
{
	char *path = NULL;
	if (filePath == NULL || !strcmp(filePath, ""))
	{
		path = "vc.wav";
	}
	else
	{
		path = filePath;
	}
	remove(path);
	struct WaveHeader *hdr = generic_wav_header(base_set.simple_rate, base_set.bit_depth, base_set.channels);
	int filedesc = open(path, O_WRONLY | O_CREAT, 0644);
	hdr->file_size = len + 44 - 8;
	hdr->sample_rate = base_set.simple_rate;
	int err = write_wav_header(filedesc, hdr);
	if (err < 0)
	{
		err_log("Write wav data failed!");
	}
	write(filedesc, data, len);
	close(filedesc);
	return 0;
}

int write_get_file(char *filePath, char *data, size_t len)
{
	char *path = NULL;
	if (filePath == NULL || !strcmp(filePath, ""))
	{
		path = "temp";
	}
	else
	{
		path = filePath;
	}
	remove(path);
	int filedesc = open(path, O_WRONLY | O_CREAT, 0644);
	write(filedesc, data, len);
	close(filedesc);
	return 0;
}
