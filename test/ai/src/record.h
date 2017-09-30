/*
 * wav.h
 *
 *  Created on: 2017年8月31日
 *      Author: cheetah
 */

#ifndef SRC_RECORD_H_
#define SRC_RECORD_H_

#include "err_log.h"

struct WaveHeader
{
	char RIFF_marker[4];
	uint32_t file_size;
	char filetype_header[4];
	char format_marker[4];
	uint32_t data_header_length;
	uint16_t format_type;
	uint16_t number_of_channels;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t bytes_per_frame;
	uint16_t bits_per_sample;
};

struct WaveHeader *generic_wav_header(uint32_t sample_rate, uint16_t bit_depth,
		uint16_t channels);
int init_record();

#endif /* SRC_RECORD_H_ */
