
#include "sys.h"
#include "SysSecurity.h"
#include "wNtSecurity.h"
#include "wNtSecurityZoneLabel.h"

wNtSecurityZoneLabel::wNtSecurityZoneLabel() :
    wNtWindow("security/zone")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");

	for(int i=0; i<5; i++) {
		char s[128];
		sprintf(s, "title_%d", i);
		m_title2[i].setParent(this);
		m_title2[i].load(m_style, s);
	}
	for(int n=0; n<8; n++) {
		char s[128];
		for(int i=0; i<5; i++) {
			sprintf(s, "title_%d", i);
			m_data[n][i].setParent(this);
			m_data[n][i].load(m_style, s);
			m_data[n][i].move(m_data[n][i].x(), m_data[n][i].y()+(n+1)*42);
		}
	}

	m_ok = 0;
	m_zone_s = 0;
	m_idx_s = 1;
	this->showSelect();
}

wNtSecurityZoneLabel::~wNtSecurityZoneLabel()
{
	if (m_ok) {
		sSecurity.save();
		sSecurity.sync_cfg(NULL);
	}
}

void wNtSecurityZoneLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			m_ok = 1;
			this->doUp();
			this->showSelect();
		} else if (e->wParam == KEY_M_N2) {
			m_ok = 1;
			this->doDown();
			this->showSelect();
		} else if (e->wParam == KEY_M_N3) {
			m_idx_s++;
			if (m_idx_s >= 5)
				m_idx_s = 1;
			this->showSelect();
		} else if (e->wParam == KEY_M_N4) {
			m_zone_s++;
			if (m_zone_s >= 8)
				m_zone_s = 0;
			this->showSelect();
		} else if (e->wParam == KEY_M_N5) {
			wNtSecurity *w = new wNtSecurity();
			w->show();
		}
	}
}

void wNtSecurityZoneLabel::doUp(void)
{
	int n = m_zone_s;
	switch (m_idx_s) {
	case 1:
		if (sSecurity.zone[n].type > 0)
			sSecurity.zone[n].type--;
		break;
	case 2:
		if (sSecurity.zone[n].mode > 0)
			sSecurity.zone[n].mode--;
		break;
	case 3:
		if (sSecurity.zone[n].delay > 0)
			sSecurity.zone[n].delay--;
		break;
	case 4:
		if (sSecurity.zone[n].sensor > 0)
			sSecurity.zone[n].sensor--;
		break;
	}
	if (sSecurity.zone[n].type != 0)
		sSecurity.zone[n].delay = 0;
}

void wNtSecurityZoneLabel::doDown(void)
{
	int n = m_zone_s;
	switch (m_idx_s) {
	case 1:
		if (sSecurity.zone[n].type < 2)
			sSecurity.zone[n].type++;
		break;
	case 2:
		if (sSecurity.zone[n].mode < 3)
			sSecurity.zone[n].mode++;
		break;
	case 3:
		if (sSecurity.zone[n].delay < 6)
			sSecurity.zone[n].delay++;
		break;
	case 4:
		if (sSecurity.zone[n].sensor < 6)
			sSecurity.zone[n].sensor++;
		break;
	}
	if (sSecurity.zone[n].type != 0)
		sSecurity.zone[n].delay = 0;
}

static const char *s_type[] = {"延时", "紧急", "24小时"};
static const char *s_mode[] = {"3C", "NO", "NC", "门铃"};
static const char *s_sensor[] = {"烟感", "煤气", "红外", "门磁", "窗磁", "紧急按钮", "水浸"};

void wNtSecurityZoneLabel::showSelect(void)
{
	for(int n=0; n<8; n++) {
		char s[128];
		sprintf(s, "%d", n+1);
		m_data[n][0].setText(s);

		m_data[n][1].setText(s_type[sSecurity.zone[n].type]);
		m_data[n][2].setText(s_mode[sSecurity.zone[n].mode]);
		sprintf(s, "%ds", sys_alarm_delay[sSecurity.zone[n].delay]);
		m_data[n][3].setText(s);
		m_data[n][4].setText(s_sensor[sSecurity.zone[n].sensor]);
		for(int i=1; i<5; i++) {
			if (n == m_zone_s && i == m_idx_s) {
				m_data[n][i].setColor(0xFF0000);
			} else {
				m_data[n][i].setColor(0xFFFFFF);
			}
		}
	}
}
