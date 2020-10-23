
#ifndef __ALSA_H__
#define __ALSA_H__

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include "types.h"

class OssAudio {
public:
	OssAudio();
	~OssAudio();

	int fd(void) { return m_fd; };
	void set(int rate, int channels, int rw);
	int open(void);
	void close(void);
	int read(void *data, int length);
	int write(void *data, int length);

	static void volume(int val); // 0x00-0x3F
	static void control(int val);

public:
	int m_fd;
	int m_rate;
	int m_channels;
	int m_rw;

	int m_period_sz;
	int m_start;
};

#endif
