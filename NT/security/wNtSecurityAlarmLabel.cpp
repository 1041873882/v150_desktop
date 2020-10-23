
#include "sys.h"
#include "SysSound.h"
#include "SysSecurity.h"
#include "mFB.h"
#include "wNtMain.h"
#include "wNtSecurityAlarmLabel.h"

extern mWindow *SysAlarm;

wNtSecurityAlarmLabel::wNtSecurityAlarmLabel() :
    wNtWindow("security/alarm")
{
	SysAlarm = this;

	m_title.setParent(this);
	m_title.load(m_style, "title");

	m_alarm[0].setParent(this);
	m_alarm[0].load(m_style, "alarm");
	m_alarm[1].setParent(this);
	m_alarm[1].load(m_style, "alarm2");
	m_alarm[1].hide();

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt_text");

	m_sensor.setParent(this);
	m_sensor.load(m_style, "sensor");
	m_sensor.setMultiLine(1);

	m_passwd_text.setParent(this);
	m_passwd_text.load(m_style, "passwd_text");
	m_passwd.setParent(this);
	m_passwd.load(m_style, "passwd");
	m_passwd.setMode(mEdit::PASSWD);
	m_passwd.setMax(10);
	m_switch = 0;
	m_btn[0].setText("1");
	m_btn[1].setText("2");
	m_btn[2].setText("3");
	m_btn[3].setText("4");

	m_ts = time(NULL);
	m_idx = 0;
	m_dts = 0;
	m_start = 0;
}

wNtSecurityAlarmLabel::~wNtSecurityAlarmLabel()
{
	SysAlarm = NULL;
	sSecurity.hooter(0);
}

void wNtSecurityAlarmLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			switch (m_switch) {
			case 0:
				m_passwd.key('1');
				break;
			case 1:
				m_passwd.key('5');
				break;
			case 2:
				m_passwd.key('9');
				break;
			}
		} else if (e->wParam == KEY_M_N2) {
			switch (m_switch) {
			case 0:
				m_passwd.key('2');
				break;
			case 1:
				m_passwd.key('6');
				break;
			case 2:
				m_passwd.key('0');
				break;
			}
		} else if (e->wParam == KEY_M_N3) {
			switch (m_switch) {
			case 0:
				m_passwd.key('3');
				break;
			case 1:
				m_passwd.key('7');
				break;
			case 2: //*
				m_passwd.setText("");
				break;
			}
		} else if (e->wParam == KEY_M_N4) {
			switch (m_switch) {
			case 0:
				m_passwd.key('4');
				break;
			case 1:
				m_passwd.key('8');
				break;
			case 2: // #
				m_passwd.setText("");
				sound.stop();
				sound.passwd_err();
				m_start = 0;
				break;
			}
		} else if (e->wParam == KEY_M_N5) {
			if (m_switch == 0) {
				m_switch = 1;
				m_btn[0].setText("5");
				m_btn[1].setText("6");
				m_btn[2].setText("7");
				m_btn[3].setText("8");
			} else if (m_switch == 1) {
				m_switch = 2;
				m_btn[0].setText("9");
				m_btn[1].setText("0");
				m_btn[2].setText("*");
				m_btn[3].setText("#");
			} else {
				m_switch = 0;
				m_btn[0].setText("1");
				m_btn[1].setText("2");
				m_btn[2].setText("3");
				m_btn[3].setText("4");
			}
		}
		if (!strcmp(m_passwd.text(), sys.user.passwd())) {
			sound.stop();
			sSecurity.withdraw();
			sSecurity.defence(sSecurity.D_WITHDRAW);
			sSecurity.sync_cfg(NULL);

			wNtMain *w = new wNtMain();
			w->show();
		}
	}
}

void wNtSecurityAlarmLabel::doTimer(void)
{
	wNtWindow::doTimer();

	if (time(NULL) != m_ts) {
		m_ts = time(NULL);
		m_idx = m_idx ? 0 : 1;
		if (m_idx) {
			m_alarm[0].hide();
			m_alarm[1].show();
		} else {
			m_alarm[0].show();
			m_alarm[1].hide();
		}

		int have = 0;
		std::string ss;
		for(int i=0; i<MAX_SECURITY_SZ; i++) {
			if (sSecurity.zone[i].have) {
				char s[128], s2[128];
				sprintf(s2, "/style/text/sensor%d", sSecurity.zone[i].sensor);
				sprintf(s, "%d:%s ", i+1, m_style.getText(s2));
				ss += s;
				have = 1;
			}
		}
		if (have) {
			m_sensor.setText(ss);

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
		} else {
			wNtMain *w = new wNtMain();
			w->show();
		}
	}
}
