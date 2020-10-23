
#ifndef __W_SECURITY_IPC_LABEL_H__
#define __W_SECURITY_IPC_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSecurityIpcLabel : public mWindow {
public:
	wSecurityIpcLabel();
	virtual ~wSecurityIpcLabel();

	void doStart(void);
	void doStop(void);

	void doEvent(mEvent *e);
	void doTimer(void);

	void showData(void);

private:
	mPixmap m_bkg;
	mPixmap m_bkg2;
	mPixmap m_osd;
	mButton m_start;
	mButton m_stop;
	mButton m_left;
	mButton m_right;
	mButton m_data;
	mText m_prompt;
	mButton m_exit;

	int m_ts;
	int m_index; //±àºÅ
};

#endif
