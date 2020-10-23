
#ifndef __W_NT_INTERCOM_H__
#define __W_NT_INTERCOM_H__

#include "wNtWindow.h"

class wNtIntercom : public wNtWindow {
public:
	wNtIntercom();
	virtual ~wNtIntercom();

	void doEvent(mEvent *e);

private:
	mButton m_title;
	mPixmap m_talk_bkg;
};

#endif
