
#ifndef __W_NT_INTERCOM_CENTER_LABEL_H__
#define __W_NT_INTERCOM_CENTER_LABEL_H__

#include "wNtWindow.h"

class wNtIntercomCenterLabel : public wNtWindow {
public:
	wNtIntercomCenterLabel();
	virtual ~wNtIntercomCenterLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

	void start(void);
	void stop(void);
	void doCenterNext(void);
	void showText(int type);

private:
	mButton m_title;
	mPixmap m_talk_bkg;
	mText m_prompt;
	int m_start;
};

#endif
