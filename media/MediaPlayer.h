#ifndef __MEDIA_PLAYER_H__
#define __MEDIA_PLAYER_H__

#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <stdint.h>
#include <semaphore.h>

#include "libavformat/avformat.h"
#include "libavcodec/audioconvert.h"

#include "AutoMutex.h"
#include "sFifo.h"
#include "OssAudio.h"

typedef struct {
	char url[128];
	int cycle;
} player_list_t;

class MediaPlayer {
public:
	MediaPlayer();
	~MediaPlayer();

	int open(const char *url);
	void close(void);

	void start(const char *url, int cycle);
	void stop(void);
	int decode(void);
	void playing(player_list_t *list);
	int used(void) { return (m_ctx != NULL ? 1 : 0); };

	void process(void);

private:
	AVFormatContext *m_ctx;
	AVAudioConvert *m_convert;
	OssAudio m_dev;
	int m_cycle;

	sFifo m_list;
	int m_stop;

	sem_t m_wait;
	pthread_mutex_t m_mutex;
};

extern MediaPlayer player;

#endif
