
#ifndef __W_SECURITY_DEFENCE_LABEL_H__
#define __W_SECURITY_DEFENCE_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSecurityDefenceLabel : public mWindow {
public:
	wSecurityDefenceLabel();
	virtual ~wSecurityDefenceLabel();

	void doEvent(mEvent *e);

	void showDefence(void);

private:
	mPixmap m_bkg;
	mButton m_out;
	mButton m_home;
	mButton m_sleep;
	mButton m_withdraw;
	mButton m_exit;
};

#endif
