
#include "wSecurity.h"
#include "SysSound.h"
#include "SysSecurity.h"
#include "wSecurityZoneLabel.h"

wSecurityZoneLabel::wSecurityZoneLabel() : mWindow("security/zone")
{
	char s[128];

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");

	for(int i=0; i<5; i++) {
		sprintf(s, "title%d", i);
		m_title[i].setParent(this);
		m_title[i].load(m_style, s);
	}

	for(int i=0; i<8; i++) {
		m_zone[i].setParent(this);
		m_zone[i].load(m_style, "zone");
		m_zone[i].move(m_zone[i].x(), m_zone[i].y()+i*m_zone[0].height());

		m_type[i].setParent(this);
		m_type[i].load(m_style, "type");
		m_type[i].move(m_type[i].x(), m_type[i].y()+i*m_zone[0].height());

		m_mode[i].setParent(this);
		m_mode[i].load(m_style, "mode");
		m_mode[i].move(m_mode[i].x(), m_mode[i].y()+i*m_zone[0].height());

		m_delay[i].setParent(this);
		m_delay[i].load(m_style, "delay");
		m_delay[i].move(m_delay[i].x(), m_delay[i].y()+i*m_zone[0].height());

		m_sensor[i].setParent(this);
		m_sensor[i].load(m_style, "sensor");
		m_sensor[i].move(m_sensor[i].x(), m_sensor[i].y()+i*m_zone[0].height());
	}
	for(int i=0; i<3; i++) {
		sprintf(s, "/style/text/type%d", i);
		for(int j=0; j<8; j++)
			m_type[j].insert(m_style.getText(s));
	}
	for(int i=0; i<4; i++) {
		sprintf(s, "/style/text/mode%d", i);
		for(int j=0; j<8; j++)
			m_mode[j].insert(m_style.getText(s));
	}
	for(int i=0; i<8; i++) {
		for(int j=0; j<7; j++) {
			sprintf(s, "%ds", sys_alarm_delay[j]);
			m_delay[i].insert(s);
		}
	}
	for(int i=0; i<7; i++) {
		sprintf(s, "/style/text/sensor%d", i);
		for(int j=0; j<8; j++)
			m_sensor[j].insert(m_style.getText(s));
	}
	m_offset = 0;
	this->loadZone();

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
}

wSecurityZoneLabel::~wSecurityZoneLabel()
{
}

void wSecurityZoneLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->save();
		} else if (e->wParam == m_exit.id()) {
			wSecurity *w = new wSecurity();
			w->show();
		} else if (e->wParam == m_up.id()) {
			if (m_offset) {
				for(int i=0; i<8; i++) {
					sSecurity.zone[i+m_offset].type = m_type[i].select();
					sSecurity.zone[i+m_offset].mode = m_mode[i].select();
					sSecurity.zone[i+m_offset].delay = m_delay[i].select();
					sSecurity.zone[i+m_offset].sensor = m_sensor[i].select();
				}
				m_offset = 0;
				this->loadZone();
			}
		} else if (e->wParam == m_down.id()) {
			if (m_offset == 0) {
				for(int i=0; i<8; i++) {
					sSecurity.zone[i+m_offset].type = m_type[i].select();
					sSecurity.zone[i+m_offset].mode = m_mode[i].select();
					sSecurity.zone[i+m_offset].delay = m_delay[i].select();
					sSecurity.zone[i+m_offset].sensor = m_sensor[i].select();
				}
				m_offset = 8;
				this->loadZone();
			}
		}
	} else {
		for(int i=0; i<8; i++) {
			if (e->wParam == m_type[i].id()) {
				if (m_type[i].select()) {
					m_delay[i].setEnable(0);
					m_delay[i].select(0);
				} else {
					m_delay[i].setEnable(1);
				}
				break;
			}
		}
	}
}

void wSecurityZoneLabel::loadZone(void)
{
	char s[128];
	for(int i=0; i<8; i++) {
		sprintf(s, "%d", i+1+m_offset);
		m_zone[i].setText(s);
		m_type[i].select(sSecurity.zone[i+m_offset].type);
		m_mode[i].select(sSecurity.zone[i+m_offset].mode);
		m_sensor[i].select(sSecurity.zone[i+m_offset].sensor);
		if (sSecurity.zone[i+m_offset].type == sSecurity.T_NONE) {
			m_delay[i].setEnable(1);
			m_delay[i].select(sSecurity.zone[i+m_offset].delay);
		} else {
			m_delay[i].setEnable(0);
			m_delay[i].select(0);
		}
	}
	m_up.hide();
	m_down.hide();
	if (m_offset)
		m_up.show();
	else
		m_down.show();
}

void wSecurityZoneLabel::save(void)
{
	for(int i=0; i<8; i++) {
		sSecurity.zone[i+m_offset].type = m_type[i].select();
		sSecurity.zone[i+m_offset].mode = m_mode[i].select();
		sSecurity.zone[i+m_offset].delay = m_delay[i].select();
		sSecurity.zone[i+m_offset].sensor = m_sensor[i].select();
	}
	sSecurity.save();
	sSecurity.sync_cfg(NULL);
	sound.setup_ok();
}
