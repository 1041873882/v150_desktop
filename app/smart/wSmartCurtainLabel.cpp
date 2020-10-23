#include "SysSmart.h"
#include "wSmart.h"
#include "wSmartCurtainLabel.h"

wSmartCurtainLabel::wSmartCurtainLabel() : mWindow("smart/curtain")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	char s[128];
	for(int i=0; i<6; i++) {
		sprintf(s, "r%d", i);
		m_room[i].setParent(this);
		m_room[i].load(m_style, s);
	}
	m_select = 1;

	m_offset = m_style.getInt("/style/offset", 186);
	for(int i=0; i<2; i++) {
		m_index[i].setParent(this);
		m_index[i].load(m_style, "index");
		m_index[i].move(m_index[i].x(), m_index[i].y()+i*m_offset);
		sprintf(s, "%d#", i+1);
		m_index[i].setText(s);

		m_close[i].setParent(this);
		m_open[i].setParent(this);
		m_pause[i].setParent(this);
	}

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	this->select(0);
}

wSmartCurtainLabel::~wSmartCurtainLabel()
{
}

void wSmartCurtainLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for(int i=0; i<4; i++) {
			if (e->wParam == m_room[i].id()) {
				this->select(i);
				return;
			}
		}
		for(int i=0; i<2; i++) {
			if (e->wParam == m_close[i].id()) {
				smart.curtain(m_select, i, 0);
			} else if (e->wParam == m_open[i].id()) {
				smart.curtain(m_select, i, 1);
			} else if (e->wParam == m_pause[i].id()) {
				smart.curtain(m_select, i, 2);
			}
		}
		if (e->wParam == m_exit.id()) {
			wSmart *w = new wSmart();
			w->show();
		}
	}
}

void wSmartCurtainLabel::doTimer(void)
{
	if (m_select < 0 || m_select > 3)
		return;
	for(int i=0; i<2; i++) {
		if (m_st[i] != smart.m_zone[m_select].curtain[i]) {
			m_st[i] = smart.m_zone[m_select].curtain[i];
			if (m_st[i] == 0) {
				m_close[i].load(m_style, "close2");
				m_open[i].load(m_style, "open");
				m_pause[i].load(m_style, "pause");
			} else if (m_st[i] == 1) {
				m_close[i].load(m_style, "close");
				m_open[i].load(m_style, "open2");
				m_pause[i].load(m_style, "pause");
			} else {
				m_close[i].load(m_style, "close");
				m_open[i].load(m_style, "open");
				m_pause[i].load(m_style, "pause2");
			}
			m_close[i].move(m_close[i].x(), m_close[i].y()+i*m_offset);
			m_open[i].move(m_open[i].x(), m_open[i].y()+i*m_offset);
			m_pause[i].move(m_pause[i].x(), m_pause[i].y()+i*m_offset);
		}
	}
	smart.join(m_select);
}

void wSmartCurtainLabel::select(int n)
{
	if (m_select != n) {
		char s[128];
		sprintf(s, "r%d", m_select);
		m_room[m_select].load(m_style, s);
		sprintf(s, "r%d_b", n);
		m_room[n].load(m_style, s);
		m_select = n;
		for(int i=0; i<2; i++)
			m_st[i] = -1;
	}
}
