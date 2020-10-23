
#ifndef __W_SETUP_PASSWD_LABEL_H__
#define __W_SETUP_PASSWD_LABEL_H__

#include "mWindow.h"
#include "mEdit.h"

class wSetupPasswdLabel : public mWindow {
public:
	wSetupPasswdLabel();
	virtual ~wSetupPasswdLabel();

	void doEvent(mEvent *e);
	void save(void);

private:
	mText m_old_text;
	mEdit m_old;
	mText m_new_text;
	mEdit m_new;
	mText m_new2_text;
	mEdit m_new2;
	mButton m_ok;
};

#endif
