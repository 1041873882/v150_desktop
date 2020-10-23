
#include "sys.h"
#include "SysCaller.h"
#include "SysSmart.h"
#include "wNtMain.h"
#include "wNtElev.h"

wNtElev::wNtElev() : wNtWindow("elev")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");

	m_elev.setParent(this);
	m_elev.load(m_style, "elev");

	m_data.load(m_style, "data");
	m_data.setParent(this);
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	char s[128];
	sprintf(s, "%d%02d9901", sys.talk.build(), sys.talk.unit());
	sCaller.query(s);
	m_ts = 0;
}

wNtElev::~wNtElev()
{
}

void wNtElev::appoint(int elev, int direct)
{
	if (sCaller.sip.url) {
		dxml p;
		dmsg req;
		p.setText("/params/to", sCaller.sip.url);
		p.setInt("/params/elev", elev);
		p.setInt("/params/direct", direct);
		p.setInt("/params/build", sys.talk.build());
		p.setInt("/params/unit", sys.talk.unit());
		p.setInt("/params/floor", sys.talk.floor());
		p.setInt("/params/family", sys.talk.family());
		req.request("/talk/elev/appoint", p.data());
	} else {
		char s[128];
		sprintf(s, "%d%02d9901", sys.talk.build(), sys.talk.unit());
		sCaller.query(s);
	}
}

void wNtElev::permit(void)
{
	if (sCaller.sip.url) {
		dxml p;
		dmsg req;
		p.setText("/params/to", sCaller.sip.url);
		p.setInt("/params/elev", 0);
		p.setInt("/params/build", sys.talk.build());
		p.setInt("/params/unit", sys.talk.unit());
		p.setInt("/params/floor", sys.talk.floor());
		p.setInt("/params/family", sys.talk.family());
		req.request("/talk/elev/permit", p.data());
	} else {
		char s[128];
		sprintf(s, "%d%02d9901", sys.talk.build(), sys.talk.unit());
		sCaller.query(s);
	}
}

void wNtElev::join(void)
{
	if (sCaller.sip.url) {
		dxml p;
		dmsg req;
		p.setText("/params/to", sCaller.sip.url);
		p.setText("/params/data/params/event_url", "/elev/join");
		req.request("/talk/sip/sendto", p.data());
	}
}

void wNtElev::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			m_up.load(m_style, "up2");
			this->appoint(0, 1);
		} else if (e->wParam == KEY_M_N2) {
			m_down.load(m_style, "down2");
			this->appoint(0, 2);
		} else if (e->wParam == KEY_M_N3) {
			m_prompt.setText("当前楼层已开放访问");
			this->permit();
		} else if (e->wParam == KEY_M_N5) {
			wNtMain *w = new wNtMain();
			w->show();
		}
	} else if (e->type == mEventElev) {
		int n = e->wParam;
		this->data(n, sys_elev[n].direct, sys_elev[n].sign, sys_elev[n].text.c_str());
	}
}

void wNtElev::doTimer(void)
{
	wNtWindow::doTimer();
	if (labs(time(NULL)-m_ts) > 2) {
		m_ts = time(NULL);
		this->join();
	}
}

void wNtElev::data(int index, int direct, int sign, const char *s)
{
	if (index != 0)
		return;

	char ss[128];
	for (int i=0; i<strlen(s); i++) {
		if (s[i] != '0') {
			s = s+i;
			break;
		}
	}
	if (sign == -1)
		sprintf(ss, "-%s", s);
	else
		strcpy(ss, s);
	m_data.setText(ss);
}

