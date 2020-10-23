
#ifndef __W_SECURITY_H__
#define __W_SECURITY_H__

#include "mWindow.h"
#include "mButton.h"
#include "wAlert.h"

class wSecurity : public mWindow {
public:
	wSecurity();
	virtual ~wSecurity();

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mButton m_defence;
	mButton m_ipc;
	mButton m_zone;
	mButton m_scene;
	mButton m_setup;
	mButton m_exit;

	int m_select;
	wAlertLogin m_login;
};

#endif
