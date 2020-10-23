
#include "sys.h"
#include "wNtMain.h"
#include "wNtSecurityDefenceLabel.h"
#include "wNtSecurityZoneLabel.h"
#include "wNtSecuritySceneLabel.h"
#include "wNtSecurity.h"

wNtSecurity::wNtSecurity() : wNtWindow("security")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
}

wNtSecurity::~wNtSecurity()
{
}

void wNtSecurity::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			wNtSecurityDefenceLabel *w = new wNtSecurityDefenceLabel();
			w->show();
		} else if (e->wParam == KEY_M_N2) {
			wNtSecurityZoneLabel *w = new wNtSecurityZoneLabel();
			w->show();
		} else if (e->wParam == KEY_M_N3) {
			wNtSecuritySceneLabel *w = new wNtSecuritySceneLabel();
			w->show();
		} else if (e->wParam == KEY_M_N4) {
		} else if (e->wParam == KEY_M_N5) {
			wNtMain *w = new wNtMain();
			w->show();
		}
	}
}
