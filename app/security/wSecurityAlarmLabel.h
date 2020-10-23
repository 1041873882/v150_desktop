
#ifndef __W_SECURITY_ALARM_LABEL_H__
#define __W_SECURITY_ALARM_LABEL_H__

#include "mWindow.h"
#include "mButton.h"
#include "mEdit.h"

class wSecurityAlarmLabel : public mWindow {
public:
	wSecurityAlarmLabel();
	virtual ~wSecurityAlarmLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mText m_sensor;
	mText m_passwd_text;
	mEdit m_passwd;
	mButton m_ok;

	time_t m_pts;
	int m_idx;

	time_t m_dts;
	int m_start;
};

#endif
