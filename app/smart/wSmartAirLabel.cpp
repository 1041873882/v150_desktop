#include "wSmart.h"
#include "wSmartAirLabel.h"

wSmartAirLabel::wSmartAirLabel() : mWindow("smart/air")
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

	int offset = m_style.getInt("/style/offset", 186);
	for(int i=0; i<2; i++) {
		m_index[i].setParent(this);
		m_index[i].load(m_style, "index");
		m_index[i].move(m_index[i].x(), m_index[i].y()+i*offset);
		sprintf(s, "%d#", i+1);
		m_index[i].setText(s);

		m_cold[i].setParent(this);
		m_cold[i].load(m_style, "cold");
		m_cold[i].move(m_cold[i].x(), m_cold[i].y()+i*offset);
		m_wet[i].setParent(this);
		m_wet[i].load(m_style, "wet");
		m_wet[i].move(m_wet[i].x(), m_wet[i].y()+i*offset);
		m_wind[i].setParent(this);
		m_wind[i].load(m_style, "wind");
		m_wind[i].move(m_wind[i].x(), m_wind[i].y()+i*offset);
		m_off[i].setParent(this);
		m_off[i].load(m_style, "off");
		m_off[i].move(m_off[i].x(), m_off[i].y()+i*offset);
	}

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	this->select(0);
}

wSmartAirLabel::~wSmartAirLabel()
{
}

void wSmartAirLabel::doEvent(mEvent *e)
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
			if (e->wParam == m_cold[i].id()) {
				smart.air(m_select, i, 0);
				return;
			}
			if (e->wParam == m_wet[i].id()) {
				smart.air(m_select, i, 1);
				return;
			}
			if (e->wParam == m_wind[i].id()) {
				smart.air(m_select, i, 2);
				return;
			}
			if (e->wParam == m_off[i].id()) {
				smart.air(m_select, i, 3);
				return;
			}
		}

		if (e->wParam == m_exit.id()) {
			wSmart *w = new wSmart();
			w->show();
		}
	}
}

void wSmartAirLabel::select(int n)
{
	if (m_select != n) {
		char s[128];
		sprintf(s, "r%d", m_select);
		m_room[m_select].load(m_style, s);
		sprintf(s, "r%d_b", n);
		m_room[n].load(m_style, s);
		m_select = n;
	}
}
