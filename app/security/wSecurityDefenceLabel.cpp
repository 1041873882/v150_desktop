
#include "wSecurity.h"
#include "SysSecurity.h"
#include "wSecurityDefenceLabel.h"

wSecurityDefenceLabel::wSecurityDefenceLabel() : mWindow("security/defence")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_out.setParent(this);
	m_home.setParent(this);
	m_sleep.setParent(this);
	m_withdraw.setParent(this);

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	this->showDefence();
}

wSecurityDefenceLabel::~wSecurityDefenceLabel()
{
}

void wSecurityDefenceLabel::showDefence(void)
{
	m_out.load(m_style, "out");
	m_home.load(m_style, "home");
	m_sleep.load(m_style, "sleep");
	m_withdraw.load(m_style, "withdraw");

	switch (sSecurity.m_defence) {
	case 0:
		m_withdraw.load(m_style, "withdraw2");
		break;
	case 1:
		m_out.load(m_style, "out2");
		break;
	case 2:
		m_home.load(m_style, "home2");
		break;
	case 3:
		m_sleep.load(m_style, "sleep2");
		break;
	}
}

void wSecurityDefenceLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_exit.id()) {
			wSecurity *w = new wSecurity();
			w->show();
			return;
		} else if (e->wParam == m_out.id()) {
			sSecurity.defence(sSecurity.D_OUT);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == m_home.id()) {
			sSecurity.defence(sSecurity.D_HOME);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == m_sleep.id()) {
			sSecurity.defence(sSecurity.D_SLEEP);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == m_withdraw.id()) {
			sSecurity.defence(sSecurity.D_WITHDRAW);
			sSecurity.sync_cfg(NULL);
		}
		this->showDefence();
	}
}
