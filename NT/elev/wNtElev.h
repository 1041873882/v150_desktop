
#ifndef __W_NT_ELEV_H__
#define __W_NT_ELEV_H__

#include "wNtWindow.h"

class wNtElev : public wNtWindow {
public:
	wNtElev();
	virtual ~wNtElev();

	void appoint(int elev, int direct);
	void permit(void);
	void join(void);

	void doEvent(mEvent *e);
	void doTimer(void);

	void data(int index, int direct, int sign, const char *s);

private:
	mButton m_title;
	mPixmap m_elev;
	mText m_data;
	mPixmap m_up;
	mPixmap m_down;
	mText m_prompt;
	time_t m_ts;
};

#endif
