
#include "sys.h"
#include "SysSound.h"
#include "wNtMain.h"
#include "wNtSecurity.h"
#include "wNtSecurityLoginLabel.h"

wNtSecurityLoginLabel::wNtSecurityLoginLabel() :
    wNtWindow("security/login")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_passwd_text.setParent(this);
	m_passwd_text.load(m_style, "passwd_text");

	m_prompt[0].setParent(this);
	m_prompt[0].load(m_style, "prompt_text");
	m_prompt[1].setParent(this);
	m_prompt[1].load(m_style, "prompt_text2");

	m_passwd.setParent(this);
	m_passwd.load(m_style, "passwd");
	m_passwd.setMode(mEdit::PASSWD);
	m_passwd.setMax(10);
	m_switch = 0;
	m_btn[0].setText("1");
	m_btn[1].setText("2");
	m_btn[2].setText("3");
	m_btn[3].setText("4");
}

wNtSecurityLoginLabel::~wNtSecurityLoginLabel()
{
}

void wNtSecurityLoginLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			switch (m_switch) {
			case 0:
				m_passwd.key('1');
				break;
			case 1:
				m_passwd.key('5');
				break;
			case 2:
				m_passwd.key('9');
				break;
			}
		} else if (e->wParam == KEY_M_N2) {
			switch (m_switch) {
			case 0:
				m_passwd.key('2');
				break;
			case 1:
				m_passwd.key('6');
				break;
			case 2:
				m_passwd.key('0');
				break;
			}
		} else if (e->wParam == KEY_M_N3) {
			switch (m_switch) {
			case 0:
				m_passwd.key('3');
				break;
			case 1:
				m_passwd.key('7');
				break;
			case 2: //*
				wNtMain *w = new wNtMain();
				w->show();
				break;
			}
		} else if (e->wParam == KEY_M_N4) {
			switch (m_switch) {
			case 0:
				m_passwd.key('4');
				break;
			case 1:
				m_passwd.key('8');
				break;
			case 2: // #
				if (!this->isOK()) {
					sound.passwd_err();
					m_passwd.setText("");
				}
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
		if (this->isOK()) {
			sys.user.login = 1;
			wNtSecurity *w = new wNtSecurity();
			w->show();
		}
	}
}

int wNtSecurityLoginLabel::isOK(void)
{
	if (!strcmp(m_passwd.text(), sys.user.passwd()))
		return 1;
	return 0;
}
