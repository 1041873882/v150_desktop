
#include "sys.h"
#include "mFB.h"
#include "SysSound.h"
#include "SysCaller.h"
#include "SysSecurity.h"
#include "wMain.h"
#include "wSecurityAlarmLabel.h"

mWindow *SysAlarm = NULL;

wSecurityAlarmLabel::wSecurityAlarmLabel() : mWindow("security/alarm")
{
	SysAlarm = this;

	m_bkg.setParent(this);

	m_passwd_text.setParent(this);
	m_passwd_text.load(m_style, "passwd_text");
	m_passwd.setParent(this);
	m_passwd.load(m_style, "passwd");
	m_passwd.setMode(mEdit::PASSWD);

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");

	m_sensor.setParent(this);
	m_sensor.load(m_style, "sensor");
	m_sensor.setMultiLine(1);

	m_pts = 0;
	m_idx = 0;
	m_dts = 0;
	m_start = 0;
}

wSecurityAlarmLabel::~wSecurityAlarmLabel()
{
	SysAlarm = NULL;
	sSecurity.hooter(0);
}

void wSecurityAlarmLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	int ok = 0;
	if (e->type == mEvent::EditEnd && e->wParam == m_passwd.id())
		ok = 1;
	else if (e->type == mEvent::TouchEnd && e->wParam == m_ok.id())
		ok = 1;
	if (ok) {
		if (!strcmp(m_passwd.text(), sys.user.passwd())) {
			sound.stop();
			sSecurity.withdraw();
			sSecurity.defence(sSecurity.D_WITHDRAW);
			sSecurity.sync_cfg(NULL);
			wMain *w = new wMain();
			w->show();
		} else {
			sound.stop();
			sound.passwd_err();
			m_start = 0;
		}
	}
}

void wSecurityAlarmLabel::doTimer(void)
{
	mWindow::doTimer();

	if (labs(time(NULL)-m_pts) > 1) {
		m_pts = time(NULL);
		if (m_idx) {
			m_idx = 0;
			m_bkg.load(m_style, "alarm2");
		} else {
			m_idx = 1;
			m_bkg.load(m_style, "alarm");
		}

		int ok = 0;
		std::string ss;
		for(int i=0; i<MAX_SECURITY_SZ; i++) {
			if (sSecurity.zone[i].have) {
				char s[128], s2[128];
				sprintf(s2, "/style/text/sensor%d", sSecurity.zone[i].sensor);
				sprintf(s, "%d:%s ", i+1, m_style.getText(s2));
				ss += s;
				ok = 1;
			}
		}
		if (ok) {
			m_sensor.setText(ss);
		} else {
			wMain *w = new wMain();
			w->show();
		}
	}

	if (sSecurity.timeout() >= MAX_ALARM_TIMEOUT) {
		if (m_start) {
			sound.stop();
			sSecurity.hooter(0);
			m_start = 0;
		}
	} else {
		fb.enable(1);
		if (!m_start) {
			int ok = 0;
			for(int i=0; i<MAX_SECURITY_SZ; i++) {
				if (sSecurity.zone[i].alarm()) {
					ok = 1;
					break;
				}
			}
			if (!ok) {
				if (labs(time(NULL)-m_dts) >= 4) {
					m_dts = time(NULL);
					sound.alarm_delay();
				}
			} else {
				sSecurity.hooter(1);
				sound.alarm();
				m_start = 1;
			}
		}
	}
}
