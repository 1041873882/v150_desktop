
#ifndef __W_NT_INTERCOM_MONITOR_LABEL_H__
#define __W_NT_INTERCOM_MONITOR_LABEL_H__

#include "wNtWindow.h"

class wNtIntercomMonitorLabel : public wNtWindow {
public:
	wNtIntercomMonitorLabel();
	virtual ~wNtIntercomMonitorLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

	void doStart(void);
	void showPrompt(int type);

	void start(void);

private:
	mPixmap m_osd;
	mText m_prompt;

	int m_select;
};

#endif
