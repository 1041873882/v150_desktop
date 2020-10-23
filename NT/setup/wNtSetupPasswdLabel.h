
#ifndef __W_NT_SETUP_PASSWD_LABEL_H__
#define __W_NT_SETUP_PASSWD_LABEL_H__

#include "mButton.h"
#include "mEdit.h"
#include "wNtWindow.h"

class wNtSetupPasswdLabel : public wNtWindow {
public:
	wNtSetupPasswdLabel();
	virtual ~wNtSetupPasswdLabel();

	void doEvent(mEvent *e);

	void switchMode(void);
	void doMode2(mEvent *e);
	void showSelect(void);

private:
	mButton m_title;
	mPixmap m_bkg2;
	mText m_passwd_text[3];
	mEdit m_passwd[3];
	mText m_prompt;

	int m_mode;
	int m_switch;
	int m_select;
};

#endif
