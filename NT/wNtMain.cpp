
#include "sys.h"
#include "sMisc.h"
#include "SysLogger.h"
#include "SysSecurity.h"
#include "wNtIntercom.h"
#include "wNtElev.h"
#include "wNtMsg.h"
#include "wNtSecurity.h"
#include "wNtSecurityLoginLabel.h"
#include "wNtSetup.h"
#include "wNtMain.h"
#include "wNtIntercomCenterLabel.h"
#include "AdvanceWirelessHttp.h"

extern mWindow *SysMain;
extern int sys_ipwd_err;
extern std::string sys_ipwd_ip;
extern std::string sys_ipwd_mac;

wNtMain::wNtMain() : wNtWindow("main")
{
	SysMain = this;

	m_logo.setParent(this);
	m_logo.load(m_style, "logo");

	m_day.setParent(this);
	m_day.load(m_style, "day");
	m_week.setParent(this);
	m_week.load(m_style, "week");
	m_time.setParent(this);
	m_time.load(m_style, "time");

	int logo = 0;
	FILE *fp = fopen("/dnake/etc/logo", "r");
	if (fp) {
		fscanf(fp, "%d", &logo);
		fclose(fp);
	}
	if (!logo)
		m_logo.hide();

	m_bar_msg.setParent(this);
	m_bar_miss.setParent(this);
	m_bar_eth.setParent(this);
	m_bar_security.setParent(this);

	m_ip_err.setParent(this);
	m_ip_err.load(m_style, "ip_err");
	m_ip_err.hide();
	m_ip_err2.setParent(this);
	m_ip_err2.load(m_style, "ip_err2");
	m_ip_err2.hide();

	m_eth_st = -1;
	m_miss_st = -1;
	m_msg_st = -1;
	m_defence_st = -1;
	m_ts_st = 0;
	m_ip_st = -1;
}

wNtMain::~wNtMain()
{
	SysMain = NULL;
}

void wNtMain::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
	} else if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N3) {
			printf("wNtIntercomCenterLabel\n");
			wNtIntercomCenterLabel *w = new wNtIntercomCenterLabel();
			w->show();
		} else if (e->wParam == KEY_M_N5) {
			printf("httpWireless\n");
			sSecurity.start_alarm();
		}
	}
}

static const char *week[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

void wNtMain::loadToolbar(void)
{
	if (labs(time(NULL)-m_ts_st) >= 5) {
		m_ts_st = time(NULL);

		struct tm *tm = localtime(&m_ts_st);
		char s[128];
		sprintf(s, "%04d年 %02d月 %02d日", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
		m_day.setText(s);
		sprintf(s, "%02d:%02d", tm->tm_hour, tm->tm_min);
		m_time.setText(s);

		m_week.setText(week[tm->tm_wday]);
	}

	if (m_eth_st != ethtool_get_link()) {
		m_eth_st = ethtool_get_link();
		if (m_eth_st)
			m_bar_eth.load(m_style, "toolbar/eth2");
		else
			m_bar_eth.load(m_style, "toolbar/eth");
	}
	if (m_miss_st != sLogger.talk.m_NA) {
		m_miss_st = sLogger.talk.m_NA;
		if (m_miss_st)
			m_bar_miss.load(m_style, "toolbar/miss2");
		else
			m_bar_miss.load(m_style, "toolbar/miss");
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

void wNtMain::doTimer(void)
{
	wNtWindow::doTimer();
	this->loadToolbar();
}
