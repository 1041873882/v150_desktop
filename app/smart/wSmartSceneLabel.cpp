
#include "wSmart.h"
#include "wSmartSceneLabel.h"

wSmartSceneLabel::wSmartSceneLabel() : mWindow("smart/scene")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_home.setParent(this);
	m_out.setParent(this);
	m_dinner.setParent(this);
	m_party.setParent(this);
	m_sleep.setParent(this);

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_scene = -1;
}

wSmartSceneLabel::~wSmartSceneLabel()
{
}

void wSmartSceneLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_home.id()) {
			smart.scene(0);
		} else if (e->wParam == m_out.id()) {
			smart.scene(1);
		} else if (e->wParam == m_dinner.id()) {
			smart.scene(2);
		} else if (e->wParam == m_party.id()) {
			smart.scene(3);
		} else if (e->wParam == m_sleep.id()) {
			smart.scene(4);
		} else if (e->wParam == m_exit.id()) {
			wSmart *w = new wSmart();
			w->show();
		}
	}
}

void wSmartSceneLabel::doTimer(void)
{
	if (m_scene != smart.m_scene) {
		m_scene = smart.m_scene;

		m_home.load(m_style, "home");
		m_out.load(m_style, "out");
		m_dinner.load(m_style, "dinner");
		m_party.load(m_style, "party");
		m_sleep.load(m_style, "sleep");
		switch(smart.m_scene) {
		case 0:
			m_home.load(m_style, "home2");
			break;
		case 1:
			m_out.load(m_style, "out2");
			break;
		case 2:
			m_dinner.load(m_style, "dinner2");
			break;
		case 3:
			m_party.load(m_style, "party2");
			break;
		case 4:
			m_sleep.load(m_style, "sleep2");
			break;
		}
	}
	smart.join(0);
}
