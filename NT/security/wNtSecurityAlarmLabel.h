
#ifndef __W_NT_SECURITY_ALARM_LABEL_H__
#define __W_NT_SECURITY_ALARM_LABEL_H__

#include "mEdit.h"
#include "wNtWindow.h"

class wNtSecurityAlarmLabel : public wNtWindow {
public:
	wNtSecurityAlarmLabel();
	virtual ~wNtSecurityAlarmLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mButton m_title;
	mPixmap m_alarm[2];
	mText m_passwd_text;
	mEdit m_passwd;
	mText m_sensor;
	mText m_prompt;
	int m_switch;

	time_t m_ts;
	int m_idx;

	time_t m_dts;
	int m_start;
};

#endif
