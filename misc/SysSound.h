
#ifndef __SYS_SOUND_H__
#define __SYS_SOUND_H__

class SysSound {
public:
	SysSound();
	~SysSound();

	void load(void);

	void play(const char *url, int cycle);
	void press(void);
	void ringing(void);
	void ringback(void);
	void setup_ok(void);
	void setup_err(void);
	void passwd_err(void);
	void defence_on(void);
	void defence_off(void);
	void defence_delay(void);
	void bell(void);
	void alarm_delay(void);
	void alarm(void);
	void stop(void);

public:
	const char *m_press;
	const char *m_ringing;
	const char *m_ringback;
	const char *m_setup_ok;
	const char *m_setup_err;
	const char *m_passwd_err;
	const char *m_defence_on;
	const char *m_defence_off;
	const char *m_defence_delay;
	const char *m_bell;
	const char *m_alarm_delay;
	const char *m_alarm;
};

extern SysSound sound;

#endif
