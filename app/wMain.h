
#ifndef __W_MAIN_H__
#define __W_MAIN_H__

#include "mWindow.h"
#include "mButton.h"

class wMain : public mWindow {
public:
	wMain();
	virtual ~wMain();

	void doEvent(mEvent *e);
	void doTimer(void);

	void loadToolbar(void);

private:
	mPixmap m_bkg;
	mPixmap m_logo;

	mButton m_talk;
	mButton m_security;
	mButton m_smart;
	mButton m_apps;
	mButton m_setup;

	mPixmap m_calendar;
	mText m_day;
	mText m_time;
	mText m_ip_err;
	mText m_ip_err2;

	mButton m_bar_sos;
	mButton m_bar_mute;
	mButton m_bar_msg;
	mPixmap m_bar_eth;
	mPixmap m_bar_security;
	mPixmap m_bar_voip;

	uint32_t m_ts_st;
	int m_eth_st;
	int m_proxy_st;
	int m_mute_st;
	int m_msg_st;
	int m_defence_st;
	int m_ip_st;

	int m_point;
	int m_x, m_y;
	int m_x2, m_y2;
};

extern mWindow *SysMain;

#endif
