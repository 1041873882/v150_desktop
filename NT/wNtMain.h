
#ifndef __W_NT_MAIN_H__
#define __W_NT_MAIN_H__

#include "wNtWindow.h"

class wNtMain : public wNtWindow {
public:
	wNtMain();
	virtual ~wNtMain();

	void doEvent(mEvent *e);
	void doTimer(void);

	void loadToolbar(void);

private:
	mPixmap m_logo;

	mPixmap m_calendar;
	mText m_day;
	mText m_week;
	mText m_time;
	mText m_ip_err;
	mText m_ip_err2;

	mPixmap m_bar_msg;
	mPixmap m_bar_miss;
	mPixmap m_bar_eth;
	mPixmap m_bar_security;

	int m_eth_st;
	int m_miss_st;
	int m_msg_st;
	int m_defence_st;
	time_t m_ts_st;
	int m_ip_st;
};

#endif
