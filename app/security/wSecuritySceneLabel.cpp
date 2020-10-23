
#include "wSecurity.h"
#include "SysSound.h"
#include "SysSecurity.h"
#include "wSecuritySceneLabel.h"

static const int delay_map[] = {0, 30, 40, 60, 100, 300};

wSecuritySceneLabel::wSecuritySceneLabel() : mWindow("security/scene")
{
	char s[128];

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");

	m_delay_text.setParent(this);
	m_delay_text.load(m_style, "delay_text");
	m_delay.setParent(this);
	m_delay.load(m_style, "delay");
	for(int i=0; i<6; i++) {
		sprintf(s, "%ds", delay_map[i]);
		m_delay.insert(s);
		if (delay_map[i] == sSecurity.m_delay)
			m_delay.select(i);
	}

	for(int i=0; i<9; i++) {
		sprintf(s, "title%d", i);
		m_title[i].setParent(this);
		m_title[i].load(m_style, s);
	}
	for(int n=0; n<3; n++) {
		m_mode[n].setParent(this);
		m_mode[n].load(m_style, "mode");
		m_mode[n].move(m_mode[n].x(), m_mode[n].y()+n*m_mode[0].height());
		for(int i=0; i<8; i++) {
			sprintf(s, "s%d", i);
			m_scene[n][i].setParent(this);
			m_scene[n][i].load(m_style, s);
			m_scene[n][i].move(m_scene[n][i].x(), m_scene[n][i].y()+n*m_mode[0].height());
		}
	}

	m_offset = 0;
	this->loadScene();

	m_mode[0].setText(m_style.getText("/style/text/out"));
	m_mode[1].setText(m_style.getText("/style/text/home"));
	m_mode[2].setText(m_style.getText("/style/text/sleep"));

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
	m_exit.setParent(this);
	m_exit.load(m_style, "exit");
}

wSecuritySceneLabel::~wSecuritySceneLabel()
{
}

void wSecuritySceneLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for(int n=0; n<3; n++) {
			for(int i=0; i<8; i++) {
				if (e->wParam == m_scene[n][i].id()) {
					m_scene_st[n][i] = m_scene_st[n][i] ? 0 : 1;
					if (m_scene_st[n][i])
						m_scene[n][i].setText("√");
					else
						m_scene[n][i].setText("");
					break;
				}
			}
		}
		if (e->wParam == m_ok.id()) {
			this->save();
		} else if (e->wParam == m_exit.id()) {
			wSecurity *w = new wSecurity();
			w->show();
		} else if (e->wParam == m_up.id()) {
			if (m_offset) {
				for(int n = 0; n<3; n++) {
					for(int i=0; i<8; i++) {
						sSecurity.zone[i+m_offset].scene[n] = m_scene_st[n][i];
					}
				}
				m_offset = 0;
				this->loadScene();
			}
		} else if (e->wParam == m_down.id()) {
			if (m_offset == 0) {
				for(int n = 0; n<3; n++) {
					for(int i=0; i<8; i++) {
						sSecurity.zone[i+m_offset].scene[n] = m_scene_st[n][i];
					}
				}
				m_offset = 8;
				this->loadScene();
			}
		}
	}
}

void wSecuritySceneLabel::loadScene(void)
{
	char s[128];
	for(int i=1; i<9; i++) {
		sprintf(s, "%d", i+m_offset);
		m_title[i].setText(s);
	}
	for(int n=0; n<3; n++) {
		for(int i=0; i<8; i++) {
			sprintf(s, "s%d", i);
			m_scene_st[n][i] = sSecurity.zone[i+m_offset].scene[n];
			if (m_scene_st[n][i])
				m_scene[n][i].setText("√");
			else
				m_scene[n][i].setText("");
		}
	}
	m_up.hide();
	m_down.hide();
	if (m_offset)
		m_up.show();
	else
		m_down.show();
}

void wSecuritySceneLabel::save(void)
{
	sSecurity.m_delay = delay_map[m_delay.select()];
	for(int n = 0; n<3; n++) {
		for(int i=0; i<8; i++) {
			sSecurity.zone[i+m_offset].scene[n] = m_scene_st[n][i];
		}
	}
	sSecurity.save();
	sSecurity.sync_cfg(NULL);
	sound.setup_ok();
}
