
#ifndef __W_ALERT_H__
#define __W_ALERT_H__

#include "mWindow.h"
#include "mButton.h"
#include "mText.h"
#include "mEdit.h"

class wAlertPrompt : public mWindow {
public:
	wAlertPrompt();
	virtual ~wAlertPrompt();

	void doEvent(mEvent *e);
	void setTitle(const char *s);
	void setMessage(const char *s);

private:
	mPixmap m_bkg;
	mText m_title;
	mText m_text;
	mButton m_ok;
};

class wAlertLogin : public mWindow {
public:
	wAlertLogin();
	virtual ~wAlertLogin();

	void doEvent(mEvent *e);
	void setTitle(const char *s);
	void setMessage(const char *s);
	const char *passwd(void) { return m_passwd.text(); };

	void show(void);

private:
	mPixmap m_bkg;
	mText m_title;
	mText m_text;
	mEdit m_passwd;
	mButton m_ok;
	mButton m_cancel;
};

#endif
