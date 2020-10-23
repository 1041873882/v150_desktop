
#include "sys.h"
#include "sMisc.h"
#include "SysCaller.h"
#include "SysLogger.h"
#include "SysSecurity.h"
#include "wElev.h"
#include "wSetup.h"
#include "wIntercom.h"
#include "wMessage.h"
#include "wSecurity.h"
#include "wCalibrate.h"
#include "wIntercomCallLabel.h"
#include "wIntercomMonitorLabel.h"
#include "wSmart.h"
#include "wMain.h"

mWindow *SysMain = NULL;

int sys_ipwd_err = 0;
std::string sys_ipwd_ip = "";
std::string sys_ipwd_mac = "";

void sys_ipwd_result(int result, const char *ip, const char *mac)
{
	sys_ipwd_err = result;
	sys_ipwd_ip = ip;
	sys_ipwd_mac = mac;
	if (sys_ipwd_err)
		fb.enable(1);
}

wMain::wMain() : mWindow("main")
{
	SysMain = this;

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_logo.setParent(this);
	m_logo.load(m_style, "logo");

	if (!sys.logo()) {
		m_logo.hide();
	}

	m_talk.setParent(this);
	m_talk.load(m_style, "talk");
	m_security.setParent(this);
	m_security.load(m_style, "security");

	m_smart.setParent(this);

	if (sys.flash() != 0) {
		m_smart.load(m_style, "smart");
	} else {
		m_smart.load(m_style, "elev");
	}

	m_apps.setParent(this);
	m_apps.load(m_style, "apps");
	m_setup.setParent(this);
	m_setup.load(m_style, "setup");

	m_calendar.setParent(this);
	m_calendar.load(m_style, "calendar");
	m_day.setParent(this);
	m_day.load(m_style, "day");
	m_time.setParent(this);
	m_time.load(m_style, "time");

	m_bar_sos.setParent(this);
	m_bar_sos.load(m_style, "toolbar/sos");

	m_bar_mute.setParent(this);
	m_bar_msg.setParent(this);
	m_bar_eth.setParent(this);
	m_bar_security.setParent(this);
	m_bar_voip.setParent(this);

	m_ip_err.setParent(this);
	m_ip_err.load(m_style, "ip_err");
	m_ip_err.hide();
	m_ip_err2.setParent(this);
	m_ip_err2.load(m_style, "ip_err2");
	m_ip_err2.hide();

	m_ts_st = -1;
	m_eth_st = -1;
	m_proxy_st = -1;
	m_mute_st = -1;
	m_msg_st = -1;
	m_defence_st = -1;
	m_ip_st = -1;

	m_point = 0;
	m_x = 10000;
	m_y = 10000;
	m_x2 = 0;
	m_y2 = 0;

	this->loadToolbar();
}

wMain::~wMain()
{
	SysMain = NULL;
}

void wMain::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEventHook) {
		if (e->wParam == 0) {
			wIntercomCallLabel *w = new wIntercomCallLabel();
			w->show();
			return;
		}
	} else if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_talk.id()) {
			wIntercom *w = new wIntercom();
			w->show();
		} else if (e->wParam == m_security.id()) {
			wSecurity *w = new wSecurity();
			w->show();
		} else if (e->wParam == m_smart.id()) {
			if (sys.flash() != 0) {
				wSmart *w = new wSmart();
				w->show();
			} else {
				wElev *w = new wElev();
				w->show();
			}
		} else if (e->wParam == m_apps.id() || e->wParam == m_bar_msg.id()) {
			wMessage *w = new wMessage();
			w->show();
		} else if (e->wParam == m_setup.id()) {
			wSetup *w = new wSetup();
			w->show();
		} else if (e->wParam == m_bar_mute.id()) {
			if (sys.mute.m_enable) {
				sys.mute.m_enable = 0;
			} else {
				sys.mute.m_enable = 1;
				sys.mute.m_ts = time(NULL);
			}
		} else if (e->wParam == m_bar_sos.id()) {
			//紧急求助SOS
			sSecurity.do_sos();
		}
	} else if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CALL) {
			wIntercomCallLabel *w = new wIntercomCallLabel();
			w->show();
			w->doCenter();
		} else if (e->wParam == KEY_M_MONITOR) {
			wIntercomMonitorLabel *w = new wIntercomMonitorLabel();
			w->show();
			w->doStart();
		}
	} else if (e->type == mEvent::TouchRaw) {
		int m = e->wParam;
		int x = e->lParam&0xFFFF;
		int y = (e->lParam>>16)&0xFFFF;
		if (m == 0) {
			int ok = 0;
			if (m_point > 20) {
				if (m_x2-m_x > 2400 && m_y2-m_y < 300) {
					ok = 1;
				} else if (m_y2-m_y > 2400 && m_x2-m_x < 300) {
					ok = 1;
				}
			}
			m_point = 0;
			m_x = 10000;
			m_y = 10000;
			m_x2 = 0;
			m_y2 = 0;
			if (ok) {
				wCalibrate *w = new wCalibrate();
				w->show();
			}
		} else {
			m_point++;
			if (m_x > x)
				m_x = x;
			if (m_y > y)
				m_y = y;
			if (m_x2 < x)
				m_x2 = x;
			if (m_y2 < y)
				m_y2 = y;
		}
	}
}

void wMain::loadToolbar(void)
{
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	if (m_ts_st != tm->tm_min) {
		char s[128];
		sprintf(s, "%02d", tm->tm_mday);
		m_day.setText(s);
		sprintf(s, "%02d:%02d", tm->tm_hour, tm->tm_min);
		m_time.setText(s);
		m_ts_st = tm->tm_min;
	}
	if (m_eth_st != ethtool_get_link()) {
		m_eth_st = ethtool_get_link();
		if (m_eth_st)
			m_bar_eth.load(m_style, "toolbar/eth2");
		else
			m_bar_eth.load(m_style, "toolbar/eth");
	}
	if (m_proxy_st != sCaller.m_proxy) {
		m_proxy_st = sCaller.m_proxy;
		if (m_proxy_st)
			m_bar_voip.load(m_style, "toolbar/voip2");
		else
			m_bar_voip.load(m_style, "toolbar/voip");
	}
	if (m_mute_st != sys.mute.m_enable) {
		m_mute_st = sys.mute.m_enable;
		if (m_mute_st)
			m_bar_mute.load(m_style, "toolbar/mute2");
		else
			m_bar_mute.load(m_style, "toolbar/mute");
	}
	if (m_msg_st != sLogger.msg.m_have) {
		m_msg_st = sLogger.msg.m_have;
		if (m_msg_st)
			m_bar_msg.load(m_style, "toolbar/msg2");
		else
			m_bar_msg.load(m_style, "toolbar/msg");
	}
	if (m_defence_st != sSecurity.m_defence) {
		m_defence_st = sSecurity.m_defence;
		if (m_defence_st)
			m_bar_security.load(m_style, "toolbar/security2");
		else
			m_bar_security.load(m_style, "toolbar/security");
	}
	if (m_ip_st != sys_ipwd_err) {
		m_ip_st = sys_ipwd_err;
		if (m_ip_st) {
			char s[1024];
			sprintf(s, "%s %s", m_style.getText("/style/text/ip_err"), sys_ipwd_ip.c_str());
			m_ip_err.setText(s);
			m_ip_err.show();
			sprintf(s, "MAC: %s", sys_ipwd_mac.c_str());
			m_ip_err2.setText(s);
			m_ip_err2.show();
		} else {
			m_ip_err.setText("");
			m_ip_err.hide();
			m_ip_err2.setText("");
			m_ip_err2.hide();
		}
	}
}

void wMain::doTimer(void)
{
	mWindow::doTimer();
	this->loadToolbar();
}
