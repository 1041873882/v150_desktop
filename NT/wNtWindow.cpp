
#include "wNtWindow.h"

wNtWindow::wNtWindow(const char *name) : mWindow(name)
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	for(int i=0; i<5; i++) {
		char s[128];
		sprintf(s, "btn%d", i);
		m_btn[i].setParent(this);
		m_btn[i].load(m_style, s);
	}
}

wNtWindow::~wNtWindow()
{
}

void wNtWindow::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
		switch(e->wParam) {
		case KEY_M_N1:
			m_btn[0].setFocus(1);
			break;
		case KEY_M_N2:
			m_btn[1].setFocus(1);
			break;
		case KEY_M_N3:
			m_btn[2].setFocus(1);
			break;
		case KEY_M_N4:
			m_btn[3].setFocus(1);
			break;
		case KEY_M_N5:
			m_btn[4].setFocus(1);
			break;
		}
	} else if (e->type == mEvent::KeyRelease) {
		switch(e->wParam) {
		case KEY_M_N1:
			m_btn[0].setFocus(0);
			break;
		case KEY_M_N2:
			m_btn[1].setFocus(0);
			break;
		case KEY_M_N3:
			m_btn[2].setFocus(0);
			break;
		case KEY_M_N4:
			m_btn[3].setFocus(0);
			break;
		case KEY_M_N5:
			m_btn[4].setFocus(0);
			break;
		}
	}
}

