/*
 * mp3player.h
 *
 *  Created on: 2017年9月7日
 *      Author: cheetah
 */

#ifndef MP3PLAY_H_
#define MP3PLAY_H_

int mp3_player_from_mem(unsigned char *audiodata, int content_len);
int mp3_player_from_file(char *mp3path);

#endif /* MP3PLAY_H_ */
