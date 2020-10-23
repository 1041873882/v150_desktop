#include "SysSmart.h"
#include "wSmart.h"
#include "wSmartLightLabel.h"

wSmartLightLabel::wSmartLightLabel() : mWindow("smart/light")
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

	std::string ss = m_style.getText("/style/text/light");
	for(int i=0; i<8; i++) {
		m_light[i].setParent(this);
		sprintf(s, "%s%d", ss.c_str(), i+1);
		m_light[i].setText(s);
	}

	m_xoff = m_style.getInt("/style/xoff", 138);
	m_yoff = m_style.getInt("/style/yoff", 172);

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	this->select(0);
}

wSmartLightLabel::~wSmartLightLabel()
{
}

void wSmartLightLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for(int i=0; i<4; i++) {
			if (e->wParam == m_room[i].id()) {
				this->select(i);
				return;
			}
		}
		for(int i=0; i<8; i++) {
			if (e->wParam == m_light[i].id()) {
				if (smart.m_zone[m_select].light[i]) {
					smart.light(m_select, i, 0);
				} else {
					smart.light(m_select, i, 1);
				}
				return;
			}
		}
		if (e->wParam == m_exit.id()) {
			wSmart *w = new wSmart();
			w->show();
		}
	}
}

void wSmartLightLabel::doTimer(void)
{
	if (m_select < 0 || m_select > 3)
		return;
	for(int i=0; i<8; i++) {
		if (m_st[i] != smart.m_zone[m_select].light[i]) {
			m_st[i] = smart.m_zone[m_select].light[i];
			if (m_st[i])
				m_light[i].load(m_style, "on");
			else
				m_light[i].load(m_style, "off");
			int xx = (i%4)*m_xoff;
			int yy = (i/4)*m_yoff;
			m_light[i].move(m_light[i].x()+xx, m_light[i].y()+yy);
		}
	}
	smart.join(m_select);
}

void wSmartLightLabel::select(int n)
{
	if (m_select != n) {
		char s[128];
		sprintf(s, "r%d", m_select);
		m_room[m_select].load(m_style, s);
		sprintf(s, "r%d_b", n);
		m_room[n].load(m_style, s);
		m_select = n;

		for(int i=0; i<8; i++)
			m_st[i] = -1;
	}
}
