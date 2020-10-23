
#include "sys.h"
#include "wNtMain.h"
#include "wNtIntercomCenterLabel.h"
#include "wNtIntercomMonitorLabel.h"
#include "wNtIntercomLoggerLabel.h"
#include "wNtIntercom.h"

wNtIntercom::wNtIntercom() : wNtWindow("intercom")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_talk_bkg.setParent(this);
	m_talk_bkg.load(m_style, "talk_bkg");
}

wNtIntercom::~wNtIntercom()
{
}

void wNtIntercom::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			wNtIntercomCenterLabel *w = new wNtIntercomCenterLabel();
			w->show();
		} else if (e->wParam == KEY_M_N2) {
			wNtIntercomMonitorLabel *w = new wNtIntercomMonitorLabel();
			w->show();
		} else if (e->wParam == KEY_M_N3) {
			wNtIntercomLoggerLabel *w = new wNtIntercomLoggerLabel();
			w->show();
		} else if (e->wParam == KEY_M_N5) {
			wNtMain *w = new wNtMain();
			w->show();
		}
	}
}
