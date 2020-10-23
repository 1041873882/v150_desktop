
#include "sys.h"
#include "MediaPlayer.h"
#include "SysSound.h"

SysSound sound;

SysSound::SysSound()
{
	m_press = "/dnake/sound/press.wav";
	m_ringing = "/dnake/sound/ring1.wav";
	m_ringback = "/dnake/sound/ringback.wav";
	m_setup_ok = "/dnake/sound/setup_ok.wav";
	m_setup_err = "/dnake/sound/setup_err.wav";
	m_passwd_err = "/dnake/sound/passwd_err.wav";
	m_defence_on = "/dnake/sound/defence_on.wav";
	m_defence_off = "/dnake/sound/defence_off.wav";
	m_defence_delay = "/dnake/sound/defence_delay.wav";
	m_alarm_delay = "/dnake/sound/alarm_delay.wav";
	m_alarm = "/dnake/sound/alarm.wav";
	m_bell = "/dnake/sound/bell.wav";
}

SysSound::~SysSound()
{
}

void SysSound::load(void)
{
	if (sys.user.language()) {
		//非简体中文
		m_setup_ok = "/dnake/sound/en/setup_ok.wav";
		m_setup_err = "/dnake/sound/en/passwd_err.wav";
		m_passwd_err = "/dnake/sound/en/passwd_err.wav";
		m_defence_on = "/dnake/sound/en/defence_on.wav";
		m_defence_off = "/dnake/sound/en/defence_off.wav";
		m_defence_delay = "/dnake/sound/en/defence_delay.wav";
		m_alarm_delay = "/dnake/sound/en/alarm_delay.wav";
	} else {
		m_setup_ok = "/dnake/sound/setup_ok.wav";
		m_setup_err = "/dnake/sound/setup_err.wav";
		m_passwd_err = "/dnake/sound/passwd_err.wav";
		m_defence_on = "/dnake/sound/defence_on.wav";
		m_defence_off = "/dnake/sound/defence_off.wav";
		m_defence_delay = "/dnake/sound/defence_delay.wav";
		m_alarm_delay = "/dnake/sound/alarm_delay.wav";
	}
}

void SysSound::play(const char *url, int cycle)
{
	player.start(url, cycle);
}

void SysSound::press(void)
{
	if (!player.used())
		player.start(m_press, 0);
}

void SysSound::ringing(void)
{
	player.start(m_ringing, 1);
}

void SysSound::ringback(void)
{
	player.start(m_ringback, 1);
}

void SysSound::setup_ok(void)
{
	player.start(m_setup_ok, 0);
}

void SysSound::setup_err(void)
{
	player.start(m_setup_err, 0);
}

void SysSound::passwd_err(void)
{
	player.start(m_passwd_err, 0);
}

void SysSound::defence_on(void)
{
	player.start(m_defence_on, 0);
}

void SysSound::defence_off(void)
{
	player.start(m_defence_off, 0);
}

void SysSound::defence_delay(void)
{
	player.start(m_defence_delay, 0);
}

void SysSound::bell(void)
{
	if (!player.used())
		player.start(m_bell, 0);
}

void SysSound::alarm_delay(void)
{
	player.start(m_alarm_delay, 0);
}

void SysSound::alarm(void)
{
	player.start(m_alarm, 1);
}

void SysSound::stop(void)
{
	player.stop();
}
