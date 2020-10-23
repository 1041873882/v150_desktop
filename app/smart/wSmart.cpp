
#include "wMain.h"
#include "wElev.h"
#include "wSmartSceneLabel.h"
#include "wSmartLightLabel.h"
#include "wSmartAirLabel.h"
#include "wSmartCurtainLabel.h"
#include "wSmart.h"

wSmart::wSmart() : mWindow("smart")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_scene.setParent(this);
	m_scene.load(m_style, "scene");
	m_light.setParent(this);
	m_light.load(m_style, "light");
	m_air.setParent(this);
	m_air.load(m_style, "air");
	m_curtain.setParent(this);
	m_curtain.load(m_style, "curtain");
	m_elevator.setParent(this);
	m_elevator.load(m_style, "elevator");

	for(int i=0; i<3; i++) {
		char s[128];
		sprintf(s, "zigbee_b%d", i);
		m_zigbee[i].setParent(this);
		m_zigbee[i].load(m_style, s);
		m_zigbee[i].hide();
	}

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_tv.tv_sec = 0;
}

wSmart::~wSmart()
{
}

#include "mFB.h"

void wSmart::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchRaw && fb.width() >= 800) {
		if (e->wParam) {
			if (m_tv.tv_sec == 0)
				gettimeofday(&m_tv, NULL);
		} else {
			if (m_tv.tv_sec) {
				struct timeval tv;
				gettimeofday(&tv, NULL);
				int ts = (tv.tv_sec-m_tv.tv_sec)*1000+(tv.tv_usec-m_tv.tv_usec)/1000;
				if (ts > 1000) {
					for(int i=0; i<3; i++)
						m_zigbee[i].show();
				}
				m_tv.tv_sec = 0;
			}
		}
	} else if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_zigbee[0].id()) {
			smart.ioctl.zigbee_mode(1);
			for(int i=0; i<3; i++)
				m_zigbee[i].hide();
		} else if (e->wParam == m_zigbee[1].id()) {
			smart.ioctl.zigbee_mode(0);
			for(int i=0; i<3; i++)
				m_zigbee[i].hide();
		} else if (e->wParam == m_zigbee[2].id()) {
			smart.ioctl.zigbee_reset();
			for(int i=0; i<3; i++)
				m_zigbee[i].hide();
		} else if (e->wParam == m_light.id()) {
			wSmartLightLabel *w = new wSmartLightLabel();
			w->show();
		} else if (e->wParam == m_air.id()) {
			wSmartAirLabel *w = new wSmartAirLabel();
			w->show();
		} else if (e->wParam == m_scene.id()) {
			wSmartSceneLabel *w = new wSmartSceneLabel();
			w->show();
		} else if (e->wParam == m_curtain.id()) {
			wSmartCurtainLabel *w = new wSmartCurtainLabel();
			w->show();
		} else if (e->wParam == m_elevator.id()) {
			wElev *w = new wElev();
			w->show();
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		}
	}
}
