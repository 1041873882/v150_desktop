
#ifndef __W_INTERCOM_MONITOR_LABEL_H__
#define __W_INTERCOM_MONITOR_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wIntercomMonitorLabel : public mWindow {
public:
	wIntercomMonitorLabel();
	virtual ~wIntercomMonitorLabel();

	void start(void);
	void stop(void);

	void doStart(void);
	void doStop(void);

	void showData(void);
	void showPrompt(int st); // 0:空 1:监视中 2: 监视失败 3:监视结束

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mPixmap m_bkg2;
	mPixmap m_osd;
	mButton m_unlock;
	mButton m_start;
	mButton m_stop;
	mButton m_left;
	mButton m_right;
	mButton m_data;
	mText m_prompt;
	mButton m_exit;

	int m_mode; //0:大门口机 1:小门口机
	int m_index; //门口机编号
};

#endif
