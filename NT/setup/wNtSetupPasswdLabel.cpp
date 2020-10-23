
#include "sys.h"
#include "SysSecurity.h"
#include "SysSound.h"
#include "wNtSetup.h"
#include "wNtSetupPasswdLabel.h"

wNtSetupPasswdLabel::wNtSetupPasswdLabel() :
    wNtWindow("setup/passwd")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt_text");

	for(int i=0; i<3; i++) {
		char s[128];
		m_passwd_text[i].setParent(this);
		sprintf(s, "passwd_text%d", i);
		m_passwd_text[i].load(m_style, s);
		m_passwd[i].setParent(this);
		sprintf(s, "passwd%d", i);
		m_passwd[i].load(m_style, s);
		m_passwd[i].setMode(mEdit::PASSWD);
		m_passwd[i].setMax(10);
	}

	m_select = 0;
	m_mode = 0;
	m_switch = 0;
	this->switchMode();
	this->showSelect();
}

wNtSetupPasswdLabel::~wNtSetupPasswdLabel()
{
}

void wNtSetupPasswdLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (m_mode == 1) {
		this->doMode2(e);
	} else {
		if (e->type == mEvent::KeyRelease) {
			if (e->wParam == KEY_M_N1) {
				if (m_select > 0) {
					m_select--;
					this->showSelect();
				}
			} else if (e->wParam == KEY_M_N2) {
				if (m_select < 2) {
					m_select++;
					this->showSelect();
				}
			} else if (e->wParam == KEY_M_N3) {
				m_mode = 1;
				this->switchMode();
			} else if (e->wParam == KEY_M_N4) {
				int ok = 0;
				if (!strcmp(m_passwd[0].text(), sys.user.passwd()) || !strcmp(m_passwd[0].text(), sys.admin.passwd())) {
					if (strlen(m_passwd[1].text()) > 0 && !strcmp(m_passwd[1].text(), m_passwd[2].text())) {
						ok = 1;
					}
				}
				if (ok) {
					sys.user.passwd(m_passwd[1].text());
	    				sys.save();
					sound.setup_ok();

					wNtSetup *w = new wNtSetup();
					w->show();
				} else
					sound.setup_err();
			} else if (e->wParam == KEY_M_N5) {
				wNtSetup *w = new wNtSetup();
				w->show();
			}
		}
	}
}

void wNtSetupPasswdLabel::switchMode(void)
{
	char s[128];
	if (m_mode == 1) {
		for(int i=0; i<5; i++) {
			sprintf(s, "btn%d_b", i);
			m_btn[i].load(m_style, s);
		}
		m_switch = 0;
		m_btn[0].setText("1");
		m_btn[1].setText("2");
		m_btn[2].setText("3");
		m_btn[3].setText("4");
	} else {
		for(int i=0; i<5; i++) {
			sprintf(s, "btn%d", i);
			m_btn[i].load(m_style, s);
		}
	}
}

void wNtSetupPasswdLabel::doMode2(mEvent *e)
{
	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			switch (m_switch) {
			case 0:
				m_passwd[m_select].key('1');
				break;
			case 1:
				m_passwd[m_select].key('5');
				break;
			case 2:
				m_passwd[m_select].key('9');
				break;
			}
		} else if (e->wParam == KEY_M_N2) {
			switch (m_switch) {
			case 0:
				m_passwd[m_select].key('2');
				break;
			case 1:
				m_passwd[m_select].key('6');
				break;
			case 2:
				m_passwd[m_select].key('0');
				break;
			}
		} else if (e->wParam == KEY_M_N3) {
			switch (m_switch) {
			case 0:
				m_passwd[m_select].key('3');
				break;
			case 1:
				m_passwd[m_select].key('7');
				break;
			case 2: //*
				m_passwd[m_select].setText("");
				break;
			}
		} else if (e->wParam == KEY_M_N4) {
			switch (m_switch) {
			case 0:
				m_passwd[m_select].key('4');
				break;
			case 1:
				m_passwd[m_select].key('8');
				break;
			case 2: // #
				m_mode = 0;
				this->switchMode();
				break;
			}
		} else if (e->wParam == KEY_M_N5) {
			if (m_switch == 0) {
				m_switch = 1;
				m_btn[0].setText("5");
				m_btn[1].setText("6");
				m_btn[2].setText("7");
				m_btn[3].setText("8");
			} else if (m_switch == 1) {
				m_switch = 2;
				m_btn[0].setText("9");
				m_btn[1].setText("0");
				m_btn[2].setText("*");
				m_btn[3].setText("#");
			} else {
				m_switch = 0;
				m_btn[0].setText("1");
				m_btn[1].setText("2");
				m_btn[2].setText("3");
				m_btn[3].setText("4");
			}
		}
	}
}

void wNtSetupPasswdLabel::showSelect(void)
{
	for(int i=0; i<3; i++) {
		if (m_select == i) {
			m_passwd_text[i].setColor(0xFF0000);
			m_passwd[i].setColor(0xFF0000);
		} else {
			m_passwd_text[i].setColor(0xFFFFFF);
			m_passwd[i].setColor(0x000000);
		}
	}
}
