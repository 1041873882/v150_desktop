
#ifndef __W_NT_SECURITY_LOGIN_LABEL_H__
#define __W_NT_SECURITY_LOGIN_LABEL_H__

#include "mEdit.h"
#include "wNtWindow.h"

class wNtSecurityLoginLabel : public wNtWindow {
public:
	wNtSecurityLoginLabel();
	virtual ~wNtSecurityLoginLabel();

	void doEvent(mEvent *e);

	int isOK(void);

private:
	mButton m_title;
	mText m_passwd_text;
	mEdit m_passwd;
	mText m_prompt[2];
	int m_switch;
};

#endif
