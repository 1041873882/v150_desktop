
#include "sys.h"
#include "SysSound.h"
#include "wSetupRoomLabel.h"
#include "wSetupNetworkLabel.h"
#include "wSetupNormalLabel.h"
#include "wSetupVersionLabel.h"
#include "wSetupPasswdLabel.h"
#include "wMain.h"
#include "wSetup.h"

wSetup::wSetup() : mWindow("setup")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_misc.setParent(this);
	m_room.setParent(this);
	m_network.setParent(this);
	m_passwd.setParent(this);
	m_version.setParent(this);

	m_login_text.setParent(this);
	m_login_text.load(m_style, "login_text");
	m_login.setParent(this);
	m_login.load(m_style, "login");
	m_login.setMode(mEdit::PASSWD);
	m_login.setMax(10);

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_label = NULL;
	this->select(0);
}

wSetup::~wSetup()
{
	if (m_label) {
		delete m_label;
		m_label = NULL;
	}
}

static int factory_debug = 0;

void wSetup::select(int menu)
{
	m_misc.load(m_style, "misc");
	m_room.load(m_style, "room");
	m_network.load(m_style, "network");
	m_passwd.load(m_style, "passwd");
	m_version.load(m_style, "version");

	if (m_label) {
		delete m_label;
		m_label = NULL;
	}
	m_select = menu;

	m_login_text.hide();
	m_login.hide();

	switch(menu) {
	case 0:
		m_misc.load(m_style, "misc2");
		m_label = new wSetupNormalLabel();
		m_label->setParent(this);
		break;
	case 1:
		m_room.load(m_style, "room2");
		if (sys.admin.login) {
			m_label = new wSetupRoomLabel();
			m_label->setParent(this);
		} else {
			m_login_text.show();
			m_login.show();
		}
		break;
	case 2:
		m_network.load(m_style, "network2");
		if (sys.admin.login) {
			m_label = new wSetupNetworkLabel();
			m_label->setParent(this);
		} else {
			m_login_text.show();
			m_login.show();
		}
		break;
	case 3:
		m_passwd.load(m_style, "passwd2");
		m_label = new wSetupPasswdLabel();
		m_label->setParent(this);
		break;
	case 4:
		m_version.load(m_style, "version2");
		m_label = new wSetupVersionLabel();
		m_label->setParent(this);
		break;
	}

	if (menu == 4) { //连续按5下版本信息，进入生产模式
		factory_debug++;
		if (factory_debug >= 5) {
			if (sys_factory)
				sys_factory = 0;
			else
				sys_factory = 1;
			factory_debug = 0;
		}
	} else
		factory_debug = 0;
}

void wSetup::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_MENU) {
			wMain *w = new wMain();
			w->show();
		}
	}
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_misc.id()) {
			this->select(0);
		} else if (e->wParam == m_room.id()) {
			this->select(1);
		} else if (e->wParam == m_network.id()) {
			this->select(2);
		} else if (e->wParam == m_passwd.id()) {
			this->select(3);
		} else if (e->wParam == m_version.id()) {
			this->select(4);
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		}
	} else if (e->type == mEvent::EditEnd) {
		if (e->wParam == m_login.id() && strlen(m_login.text()) > 0) {
			if (!strcmp(sys.admin.passwd(), m_login.text())) {
				sys.admin.login = 1;
				this->select(m_select);
			} else {
				sys.admin.login = 0;
				sound.passwd_err();
			}
		}
	}
}
