
#include "sys.h"
#include "SysSound.h"
#include "SysSecurity.h"
#include "wMain.h"
#include "wSecurityDefenceLabel.h"
#include "wSecurityZoneLabel.h"
#include "wSecuritySceneLabel.h"
#include "wSecuritySetupLabel.h"
#include "wSecurityIpcLabel.h"
#include "wSecurity.h"

wSecurity::wSecurity() : mWindow("security")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_defence.setParent(this);
	m_defence.load(m_style, "defence");
	m_ipc.setParent(this);
	m_ipc.load(m_style, "ipc");
	m_zone.setParent(this);
	m_zone.load(m_style, "zone");
	m_scene.setParent(this);
	m_scene.load(m_style, "scene");
	m_setup.setParent(this);
	m_setup.load(m_style, "setup");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_select = 0;
	m_login.setParent(this);
	m_login.setTitle(m_style.getText("/style/text/login"));
	m_login.hide();
}

wSecurity::~wSecurity()
{
}

void wSecurity::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_defence.id()) {
			if (!sys.user.login && sSecurity.m_defence) {
				m_select = 0;
				m_login.show();
			} else {
				wSecurityDefenceLabel *w = new wSecurityDefenceLabel();
				w->show();
			}
		} else if (e->wParam == m_scene.id()) {
			if (sys.user.login) {
				wSecuritySceneLabel *w = new wSecuritySceneLabel();
				w->show();
			} else {
				m_select = 1;
				m_login.show();
			}
		} else if (e->wParam == m_zone.id()) {
			if (sys.user.login) {
				wSecurityZoneLabel *w = new wSecurityZoneLabel();
				w->show();
			} else {
				m_login.show();
				m_select = 2;
			}
		} else if (e->wParam == m_ipc.id()) {
			wSecurityIpcLabel *w = new wSecurityIpcLabel();
			w->show();
		} else if (e->wParam == m_setup.id()) {
			wSecuritySetupLabel *w = new wSecuritySetupLabel();
			w->show();
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		}
	}
	if (e->type == mEvent::AlertEnd && e->wParam == m_login.id()) {
		if (!strcmp(m_login.passwd(), sys.user.passwd())) {
			sys.user.login = 1;
			if (m_select == 0) {
				wSecurityDefenceLabel *w = new wSecurityDefenceLabel();
				w->show();
			} else if (m_select == 1) {
				wSecuritySceneLabel *w = new wSecuritySceneLabel();
				w->show();
			} else if (m_select == 2) {
				wSecurityZoneLabel *w = new wSecurityZoneLabel();
				w->show();
			}
		} else
			sound.passwd_err();
	}
}

void wSecurity::doTimer(void)
{
	mWindow::doTimer();
}
