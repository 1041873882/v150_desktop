
#include "wMain.h"
#include "wIntercomCallLabel.h"
#include "wIntercomMonitorLabel.h"
#include "wIntercomLoggerLabel.h"
#include "wIntercom.h"

wIntercom::wIntercom() : mWindow("intercom")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_call.setParent(this);
	m_call.load(m_style, "call");
	m_monitor.setParent(this);
	m_monitor.load(m_style, "monitor");
	m_logger.setParent(this);
	m_logger.load(m_style, "logger");
	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
}

wIntercom::~wIntercom()
{
}

void wIntercom::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_call.id()) {
			wIntercomCallLabel *w = new wIntercomCallLabel();
			w->show();
		} else if (e->wParam == m_monitor.id()) {
			wIntercomMonitorLabel *w = new wIntercomMonitorLabel();
			w->show();
		} else if (e->wParam == m_logger.id()) {
			wIntercomLoggerLabel *w = new wIntercomLoggerLabel();
			w->show();
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		}
	}
}
