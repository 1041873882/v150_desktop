
#ifndef __W_SETUP_H__
#define __W_SETUP_H__

#include "mWindow.h"
#include "mButton.h"
#include "mEdit.h"

class wSetup : public mWindow {
public:
	wSetup();
	virtual ~wSetup();

	void select(int menu);

	void doEvent(mEvent *e);

private:
	mWindow *m_label;

	mPixmap m_bkg;
	mButton m_misc;
	mButton m_room;
	mButton m_network;
	mButton m_passwd;
	mButton m_version;

	mText m_login_text;
	mEdit m_login;

	int m_select;

	mButton m_exit;
};

#endif
