
#include "sys.h"
#include "SysSecurity.h"
#include "wNtSecurity.h"
#include "wNtSecuritySceneLabel.h"

static const char *scene[3] = {"外出", "在家", "就寝"};

wNtSecuritySceneLabel::wNtSecuritySceneLabel() :
    wNtWindow("security/scene")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
	for(int i=0; i<8; i++) {
		char s[64];
		sprintf(s, "%d", i+1);
		m_title2[i].setParent(this);
		m_title2[i].load(m_style, "title2");
		m_title2[i].move(m_title2[0].x()+i*60, m_title2[0].y());
		m_title2[i].setText(s);
	}
	for(int i=0; i<3; i++) {
		m_scene[i].setParent(this);
		m_scene[i].load(m_style, "scene");
		m_scene[i].move(m_scene[0].x(), m_scene[0].y()+i*49);
		m_scene[i].setText(scene[i]);
		for(int j=0; j<8; j++) {
			m_zone[i][j].setParent(this);
			m_zone[i][j].load(m_style, "zone");
			m_zone[i][j].move(m_zone[0][0].x()+j*60, m_zone[0][0].y()+i*49);
		}
	}
	m_ok = 0;
	m_scene_s = 0;
	m_zone_s = 0;
	this->showSelect();
}

wNtSecuritySceneLabel::~wNtSecuritySceneLabel()
{
	if (m_ok) {
		sSecurity.save();
		sSecurity.sync_cfg(NULL);
	}
}

void wNtSecuritySceneLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			m_ok = 1;
			sSecurity.zone[m_zone_s].scene[m_scene_s] = 1;
			this->showSelect();
		} else if (e->wParam == KEY_M_N2) {
			m_ok = 1;
			sSecurity.zone[m_zone_s].scene[m_scene_s] = 0;
			this->showSelect();
		} else if (e->wParam == KEY_M_N3) {
			m_zone_s++;
			if (m_zone_s >= 8)
				m_zone_s = 0;
			this->showSelect();
		} else if (e->wParam == KEY_M_N4) {
			m_scene_s++;
			if (m_scene_s >= 3)
				m_scene_s = 0;
			this->showSelect();
		} else if (e->wParam == KEY_M_N5) {
			wNtSecurity *w = new wNtSecurity();
			w->show();
		}
	}
}

void wNtSecuritySceneLabel::showSelect(void)
{
	for(int i=0; i<3; i++) {
		for(int j=0; j<8; j++) {
			if (m_scene_s == i && m_zone_s == j)
				m_zone[i][j].setColor(0xFF0000);
			else
				m_zone[i][j].setColor(0xFFFFFF);
			if (sSecurity.zone[j].scene[i])
				m_zone[i][j].setText("√");
			else
				m_zone[i][j].setText("--");
		}
	}
}
