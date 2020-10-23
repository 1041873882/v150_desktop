
#include "sys.h"
#include "SysSecurity.h"
#include "wNtSecurity.h"
#include "wNtSecurityDefenceLabel.h"

wNtSecurityDefenceLabel::wNtSecurityDefenceLabel() :
    wNtWindow("security/defence")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	this->showPrompt();
}

wNtSecurityDefenceLabel::~wNtSecurityDefenceLabel()
{
}

void wNtSecurityDefenceLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			sSecurity.defence(sSecurity.D_OUT);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == KEY_M_N2) {
			sSecurity.defence(sSecurity.D_HOME);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == KEY_M_N3) {
			sSecurity.defence(sSecurity.D_SLEEP);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == KEY_M_N4) {
			sSecurity.defence(sSecurity.D_WITHDRAW);
			sSecurity.sync_cfg(NULL);
		} else if (e->wParam == KEY_M_N5) {
			wNtSecurity *w = new wNtSecurity();
			w->show();
			return;
		}
		this->showPrompt();
	}
}

void wNtSecurityDefenceLabel::showPrompt(void)
{
	const char *st = "";
	switch (sSecurity.m_defence) {
	case 0:
		st = "撤防";
		break;
	case 1:
		st = "外出";
		break;
	case 2:
		st = "在家";
		break;
	case 3:
		st = "就寝";
		break;
	}
	char s[1024];
	sprintf(s, "当前状态: %s", st);
	m_prompt.setText(s);
}
