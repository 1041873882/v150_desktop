
#ifndef __W_INTERCOM_H__
#define __W_INTERCOM_H__

#include "mWindow.h"
#include "mButton.h"

class wIntercom : public mWindow {
public:
	wIntercom();
	virtual ~wIntercom();

	void doEvent(mEvent *e);

private:
	mPixmap m_bkg;
	mButton m_call;
	mButton m_monitor;
	mButton m_logger;
	mButton m_exit;
};

#endif
