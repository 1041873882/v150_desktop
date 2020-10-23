
#include "mFB.h"
#include "wAlert.h"

wAlertPrompt::wAlertPrompt() : mWindow("misc/alert/prompt")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_text.setParent(this);
	m_text.load(m_style, "text");

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");

	m_x = (fb.width()-m_bkg.width())/2;
	m_y = (fb.height()-m_bkg.height())/2;
	m_width = m_bkg.width();
	m_height = m_bkg.height();

	this->setLevel(500);
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i]) {
			m_subs[i]->move(m_subs[i]->x()+m_x, m_subs[i]->y()+m_y);
			m_subs[i]->setLevel(501);
		}
	}
}

wAlertPrompt::~wAlertPrompt()
{
}

void wAlertPrompt::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->hide();
		}
	}
}

void wAlertPrompt::setTitle(const char *s)
{
	m_title.setText(s);
}

void wAlertPrompt::setMessage(const char *s)
{
	m_text.setText(s);
}

wAlertLogin::wAlertLogin() : mWindow("misc/alert/login")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_text.setParent(this);
	m_text.load(m_style, "text");
	m_passwd.setParent(this);
	m_passwd.load(m_style, "passwd");
	m_passwd.setMode(mEdit::PASSWD);

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
	m_cancel.setParent(this);
	m_cancel.load(m_style, "cancel");

	m_x = (fb.width()-m_bkg.width())/2;
	m_y = (fb.height()-m_bkg.height())/2;
	m_width = m_bkg.width();
	m_height = m_bkg.height();

	this->setLevel(500);
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i]) {
			m_subs[i]->move(m_subs[i]->x()+m_x, m_subs[i]->y()+m_y);
			m_subs[i]->setLevel(501);
		}
	}
}

wAlertLogin::~wAlertLogin()
{
}

void wAlertLogin::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->hide();
			mEvent e(mEvent::AlertEnd);
			e.wParam = (uint32_t)this;
			mEvent::post(&e);
		} else if (e->wParam == m_cancel.id())
			this->hide();
	}
}

void wAlertLogin::setTitle(const char *s)
{
	m_title.setText(s);
}

void wAlertLogin::show(void)
{
	m_passwd.setText("");
	mWindow::show();
}
