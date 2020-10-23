
#include "sys.h"
#include "SysSound.h"
#include "wSecurity.h"
#include "wSecuritySetupLabel.h"

wSecuritySetupLabel::wSecuritySetupLabel() : mWindow("security/setup")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_passwd.setParent(this);
	m_passwd.load(m_style, "passwd2");

	m_old_text.setParent(this);
	m_old_text.load(m_style, "old_text");
	m_old.setParent(this);
	m_old.load(m_style, "old");
	m_old.setMode(mEdit::PASSWD);
	m_old.setMax(10);

	m_new_text.setParent(this);
	m_new_text.load(m_style, "new_text");
	m_new.setParent(this);
	m_new.load(m_style, "new");
	m_new.setMode(mEdit::PASSWD);
	m_new.setMax(10);

	m_new2_text.setParent(this);
	m_new2_text.load(m_style, "new2_text");
	m_new2.setParent(this);
	m_new2.load(m_style, "new2");
	m_new2.setMode(mEdit::PASSWD);
	m_new2.setMax(10);

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
}

wSecuritySetupLabel::~wSecuritySetupLabel()
{
}

void wSecuritySetupLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->save();
		} else if (e->wParam == m_exit.id()) {
			wSecurity *w = new wSecurity();
			w->show();
		}
	}
}

void wSecuritySetupLabel::save(void)
{
	const char *old = m_old.text();
	const char *n = m_new.text();
	const char *n2 = m_new2.text();
	if (strlen(old) > 0 && strlen(n) > 0 && strlen(n2) > 0 &&
	    (!strcmp(old, sys.user.passwd()) || !strcmp(old, sys.admin.passwd())) &&
	    !strcmp(n, n2)) {
	    	sys.user.passwd(n);
	    	sys.save();
		sound.setup_ok();
	} else
		sound.setup_err();
}
