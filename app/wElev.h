
#ifndef __W_ELEV_H__
#define __W_ELEV_H__

#include "mWindow.h"
#include "mButton.h"

class wElev : public mWindow {
public:
	wElev();
	virtual ~wElev();

	void doEvent(mEvent *e);
	void doTimer(void);

	void appoint(int elev, int direct);
	void permit(void);
	void join(void);

	void data(uint8_t index, int direct, int sign, const char *s);

private:
	mPixmap m_bkg;
	mText m_data[2];
	mButton m_up[2];
	mButton m_down[2];
	mButton m_permit;
	mButton m_exit;
	time_t m_ts;

	int m_offset;
};

#endif
