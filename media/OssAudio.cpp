
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "asoundlib.h"
#include "alsa_audio.h"
#include "soundcard.h"
#include "OssAudio.h"

OssAudio::OssAudio()
{
	m_fd = -1;
	m_rate = 8000;
	m_channels = 1;
}

OssAudio::~OssAudio()
{
	this->close();
}

void OssAudio::set(int rate, int channels, int rw)
{
	m_rate = rate;
	m_channels = channels;
	m_rw = rw;
}

int OssAudio::open(void)
{
	m_fd = ::open("/dev/dsp", O_WRONLY);
	if (m_fd < 0) {
		fprintf(stderr, "alsa_dsp::open open /dev/dsp err!\n");
		return -1;
	}

	int val = AFMT_S16_LE;
	::ioctl(m_fd, SNDCTL_DSP_SETFMT, &val);
	::ioctl(m_fd, SNDCTL_DSP_CHANNELS, &m_channels);
	::ioctl(m_fd, SNDCTL_DSP_SPEED, &m_rate);
	int fm = 0x0A + ((m_rate-1)/8000);
	::ioctl(m_fd, SNDCTL_DSP_SETFRAGMENT, &fm);
	return 0;
}

void OssAudio::close(void)
{
	if (m_fd > 0) {
		::close(m_fd);
		m_fd = -1;
	}
}

int OssAudio::read(void *data, int length)
{
	return 0;
}

int OssAudio::write(void *data, int length)
{
	if (m_fd < 0)
		return -1;
	return ::write(m_fd, data, length);
}

void OssAudio::volume(int val)
{
	struct mixer *m = mixer_open(0);
	if (m) {
		struct mixer_ctl * ctl;
		if ((ctl = mixer_get_ctl_by_name(m, "Master Playback Volume")) != NULL)
			mixer_ctl_set_value(ctl, 0, val);
		if ((ctl = mixer_get_ctl_by_name(m, "Line Volume")) != NULL)
			mixer_ctl_set_value(ctl, 0, (val/(float)0x3F)*0x1F);
		mixer_close(m);
	}
}

void OssAudio::control(int val)
{
	struct mixer *m = mixer_open(0);
	if (m) {
		struct mixer_ctl *ctl;
		if ((ctl = mixer_get_ctl_by_name(m, "Speaker Control")) != NULL) {
			mixer_ctl_set_value(ctl, 0, val);
		}
		mixer_close(m);
	}
}
