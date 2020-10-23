
#include "sys.h"
#include "SysSound.h"
#include "wMain.h"
#include "wIntercom.h"
#include "wIntercomCallLabel.h"

wIntercomCallLabel::wIntercomCallLabel() : mWindow("intercom/call")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_pb.setParent(this);
	m_pb.load(m_style, "pb");
	m_pb.setEnable(0);
	m_line.setParent(this);
	m_line.load(m_style, "line");

	m_pb_s = -1;
	int n = 0;
	for(int i=0; i<MAX_SLAVE_SZ; i++) {
		char s[128];
		if (sys.talk.dcode() != i && sCaller.m_slaves[i].length() > 0) {
			m_pb_d[i].setParent(this);
			m_pb_d[i].load(m_style, "pb_data");
			sprintf(s, "%s%02d", m_style.getText("/style/text/name"), i+1);
			m_pb_d[i].setText(s);
			m_pb_d[i].move(m_pb_d[i].x(), m_pb_d[i].y()+n*m_pb_d[i].height());
			m_pb_url[i] = sCaller.m_slaves[i];
			n++;
		}
	}

	m_input.setParent(this);
	m_input.load(m_style, "input");
	m_input.setEnable(0);

	for(int i=0; i<12; i++) {
		char s[128];
		sprintf(s, "k%d", i);
		m_key[i].setParent(this);
		m_key[i].load(m_style, s);
	}

	m_build.setParent(this);
	m_build.load(m_style, "build");
	m_unit.setParent(this);
	m_unit.load(m_style, "unit");
	
	if (sys.talk.unit() == 0)
		m_unit.hide();

	m_center.setParent(this);
	m_center.load(m_style, "center");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	memset(m_text, 0, sizeof(m_text));
	m_idx = 0;
	m_mode = 0;
	m_start = 0;

	sCaller.stop();
}

wIntercomCallLabel::~wIntercomCallLabel()
{
}

void wIntercomCallLabel::showText(int type)
{
	char s[128];
	if (m_mode == 0) { //分机呼叫提示
		if (m_pb_s != -1) {
			strcpy(s, m_pb_d[m_pb_s].text());
		} else {
			if (m_idx > 0) {
				if (m_idx == 1) {
					sprintf(s, "%d-%s", m_data[0], m_text);
				} else if (m_idx == 2) {
					if (sys.talk.unit() == 0)
						sprintf(s, "%d-%s", m_data[0], m_text);
					else
						sprintf(s, "%d-%d-%s", m_data[0], m_data[1], m_text);
				}
			} else
				strcpy(s, m_text);
		}

		if (type == 1) {
			char s2[128];
			sprintf(s2, "%s %s", s, m_style.getText("/style/text/calling"));
			m_input.setText(s2);
		} else if (type == 2) {
			char s2[128];
			sprintf(s2, "%s %s", s, m_style.getText("/style/text/failed"));
			m_input.setText(s2);
		} else
			m_input.setText(s);
	} else if (m_mode == 1) { //管理机呼叫提示
		int n = atoi(sCaller.m_id)-10000;
		if (sys.quick.enable())
			n = 1;

		const char *t1 = m_style.getText("/style/text/center");
		if (type == 1) {
			const char *t2 = m_style.getText("/style/text/calling");
			sprintf(s, "%s%d %s", t1, n, t2);
		} else if (type == 2) {
			const char *t2 = m_style.getText("/style/text/failed");
			sprintf(s, "%s%d %s", t1, n, t2);
		} else
			sprintf(s, "%s%d", t1, n);
		m_input.setText(s);
	}
}

void wIntercomCallLabel::showPbText(void)
{
	for(int i=0; i<MAX_SLAVE_SZ; i++) {
		if (i == m_pb_s)
			m_pb_d[i].setColor(0xFF0000);
		else
			m_pb_d[i].setColor(0xFFFFFF);
	}
}

void wIntercomCallLabel::key(int key)
{
	int n = strlen(m_text);
	if (key == 10) { //呼叫
		this->doCall();
		return;
	} else if (key == 11) { //删除
		m_pb_s = -1;
		if (n > 0) {
			m_text[n-1] = 0;
		} else {
			if (m_idx == 1) {
				m_idx = 0;
				sprintf(m_text, "%d", m_data[0]);
			} else if (m_idx == 2) {
				if (sys.talk.unit() == 0) {
					m_idx = 0;
					sprintf(m_text, "%d", m_data[0]);
				} else {
					m_idx = 1;
					sprintf(m_text, "%d", m_data[1]);
				}
			}
		}
		this->showText(0);
	} else if (m_pb_s == -1) {
		if (!sys.sip.enable()) {
			if (m_idx == 0) {
				if (n >= 4) {
					m_idx = 1;
					m_data[0] = atoi(m_text);
					memset(m_text, 0, sizeof(m_text));
					if (sys.talk.unit() == 0) {
						m_data[1] = 0;
						m_idx = 2;
					}
					m_text[0] = '0'+key;
				} else {
					m_text[n++] = '0'+key;
					m_text[n] = 0;
				}
			} else if (m_idx == 1) {
				if (n >= 2) {
					m_idx = 2;
					m_data[1] = atoi(m_text);
					memset(m_text, 0, sizeof(m_text));
					m_text[0] = '0'+key;
				} else {
					m_text[n++] = '0'+key;
					m_text[n] = 0;
				}
			} else if (m_idx == 2) {
				if (n < 4) {
					m_text[n++] = '0'+key;
					m_text[n] = 0;
				}
			}
		} else {
			m_text[n++] = '0'+key;
			m_text[n] = 0;
		}
		this->showText(0);
	}
}

void wIntercomCallLabel::doCall(void)
{
	int n = strlen(m_text);
	if ((m_pb_s == -1 && n == 0) || sCaller.mode() != sCaller.NONE)
		return;

	m_start = 1;
	m_mode = 0;
	if (m_pb_s != -1) {
		sCaller.start(m_pb_url[m_pb_s].c_str());
		this->showText(1);
	} else if (sys.sip.enable()) {
		char s[128];
		strcpy(s, m_text);
		if (m_idx == 0 && n <= 4)
			sprintf(s, "%d%02d%04d", sys.talk.build(), sys.talk.unit(), atoi(m_text));
		else if (m_idx == 2)
			sprintf(s, "%d%02d%04d", m_data[0], m_data[1], atoi(m_text));
		sCaller.query(s);
		this->showText(1);
	} else {
		if (n <= 4) {
			if (m_idx == 0) {
				m_data[0] = sys.talk.build();
				m_data[1] = sys.talk.unit();
				m_idx = 2;
			}
			if (m_idx == 2) {
				char s[128];
				sprintf(s, "%d%02d%04d", m_data[0], m_data[1], atoi(m_text));
				sCaller.query(s);
				usleep(100*1000);
				sCaller.query(s);
				sprintf(s, "S%04d%02d%04d0", m_data[0], m_data[1], atoi(m_text));
				sCaller.q600(s);
				this->showText(1);
			}
		} else {
			sCaller.query(m_text);
			this->showText(1);
		}
	}
}

void wIntercomCallLabel::doCenter(void)
{
	if (sCaller.mode() != sCaller.NONE) {
		return;
	}
	m_pb_s = -1;
	m_start = 1;
	m_mode = 1;
	if (sys.quick.enable()) {
		sCaller.start(sys.quick.url());
	} else {
		sCaller.query("10001");
		usleep(100*1000);
		sCaller.query("10001");
		sCaller.q600("Z0001");
	}
	this->showText(1);
}

void wIntercomCallLabel::doCenterNext(void)
{
	if (sys.quick.enable()) {
		this->doStop();
		return;
	}

	int id = atoi(sCaller.m_id);
	if (id < 10005) {
		char s[128];
		sprintf(s, "%d", id+1);
		sCaller.query(s);
		usleep(100*1000);
		sCaller.query(s);
		sprintf(s, "Z%04d", (id+1)-10000);
		sCaller.q600(s);
		this->showText(1);
	} else {
		this->doStop();
	}
}

void wIntercomCallLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEventHook) {
		if (e->wParam) {
			//挂机
			sCaller.stop();
			wMain *w = new wMain();
			w->show();
		}
	} else if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_CALL) {
			sCaller.stop();
			wMain *w = new wMain();
			w->show();
		}
	} else if (e->type == mEvent::TouchEnd) {
		for(int i=0; i<MAX_SLAVE_SZ; i++) {
			if (e->wParam == m_pb_d[i].id()) {
				m_pb_s = i;
				m_idx = 0;
				memset(m_text, 0, sizeof(m_text));
				this->showText(0);
				break;
			}
		}

		for(int i=0; i<12; i++) {
			if (e->wParam == m_key[i].id()) {
				this->key(i);
				break;
			}
		}
		if (e->wParam == m_build.id()) {
			int n = strlen(m_text);
			if (m_idx == 0 && n > 0 && n <= 4) {
				m_idx = 1;
				m_data[0] = atoi(m_text);
				memset(m_text, 0, sizeof(m_text));
				this->showText(0);
			}
		} else if (e->wParam == m_unit.id()) {
			if (m_idx == 1 && strlen(m_text)) {
				m_idx = 2;
				m_data[1] = atoi(m_text);
				memset(m_text, 0, sizeof(m_text));
				this->showText(0);
			}
		} else if (e->wParam == m_center.id()) {
			this->doCenter();
		} else if (e->wParam == m_exit.id()) {
			sCaller.stop();
			wIntercom *w = new wIntercom();
			w->show();
		}

		this->showPbText();
	}
}

void wIntercomCallLabel::doTimer(void)
{
	mWindow::doTimer();
	if (m_start)
		this->doCallTimer();
}

void wIntercomCallLabel::doCallTimer(void)
{
	if (m_mode == 1) {
		//呼叫管理中心处理
		if (sCaller.mode() == sCaller.NONE) {
			//呼叫被结束强制呼叫下一个
			this->doCenterNext();
		} else if (sCaller.mode() == sCaller.QUERY) {
			if (sCaller.sip.url) {
				sCaller.start(sCaller.sip.url);
			} else if (sCaller.ts() > 1 && sCaller.d600.host) {
				sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
			} else if (sCaller.ts() > 2) {
				this->doCenterNext();
			}
		} else if (sCaller.mode() == sCaller.CALLING) {
			if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
				this->doCenterNext();
			}
		}
	} else {
		if (sCaller.mode() == sCaller.NONE) {
			this->doStop();
		} else if (sCaller.mode() == sCaller.QUERY) {
			if (sCaller.sip.url) {
				sCaller.start(sCaller.sip.url);
			} else if (sCaller.ts() > 1 && sCaller.d600.host) {
				sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
			} else if (sCaller.ts() > 2) {
				if (m_idx == 0 && sys.sip.enable() && sCaller.m_proxy) {
					char s[512];
					sprintf(s, "sip:%s@%s", m_text, sys.sip.m_proxy+4);
					sCaller.start(s);
				} else 
					this->doStop();
			}
		} else if (sCaller.mode() == sCaller.CALLING) {
			if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
				this->doStop();
			}
		}
	}
}

void wIntercomCallLabel::doStop(void)
{
	m_start = 0;
	m_mode = 0;
	sCaller.stop();
	this->showText(2);
}
