
#ifndef __W_SECURITY_SETUP_LABEL_H__
#define __W_SECURITY_SETUP_LABEL_H__

#include "mWindow.h"
#include "mButton.h"
#include "mEdit.h"

class wSecuritySetupLabel : public mWindow {
public:
	wSecuritySetupLabel();
	virtual ~wSecuritySetupLabel();

	void doEvent(mEvent *e);
	void save(void);

private:
	mPixmap m_bkg;
	mButton m_passwd;
	mText m_old_text;
	mEdit m_old;
	mText m_new_text;
	mEdit m_new;
	mText m_new2_text;
	mEdit m_new2;
	mButton m_ok;
	mButton m_exit;
};

#endif
