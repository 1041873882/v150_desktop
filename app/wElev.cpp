
#include "sys.h"
#include "SysCaller.h"
#include "wSmart.h"
#include "wMain.h"
#include "wElev.h"

wElev::wElev() : mWindow("elev")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_offset = m_style.getInt("/style/offset", 440);
	for(int i=0; i<2; i++) {
		m_data[i].setParent(this);
		m_data[i].load(m_style, "data");
		m_data[i].move(m_data[i].x()+i*m_offset, m_data[i].y());

		m_up[i].setParent(this);
		m_up[i].load(m_style, "up");
		m_up[i].move(m_up[i].x()+i*m_offset, m_up[i].y());

		m_down[i].setParent(this);
		m_down[i].load(m_style, "down");
		m_down[i].move(m_down[i].x()+i*m_offset, m_down[i].y());
	}

	m_permit.setParent(this);
	m_permit.load(m_style, "permit");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	char s[128];
	sprintf(s, "%d%02d9901", sys.talk.build(), sys.talk.unit());
	sCaller.query(s);

	m_ts = 0;
}

wElev::~wElev()
{
}

void wElev::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for(int i=0; i<2; i++) {
			if (e->wParam == m_up[i].id()) {
				this->appoint(i, 1);
				return;
			} else if (e->wParam == m_down[i].id()) {
				this->appoint(i, 2);
				return;
			}
		}
		if (e->wParam == m_permit.id()) {
			this->permit();
			return;
		}

		if (e->wParam == m_exit.id()) {
			if (sys.flash() != 0) {
				wSmart *w = new wSmart();
				w->show();
			} else {
				wMain *w = new wMain();
				w->show();
			}
		}
	} else if (e->type == mEventElev) {
		int n = e->wParam;
		this->data(n, sys_elev[n].direct, sys_elev[n].sign, sys_elev[n].text.c_str());
	}
}

void wElev::doTimer(void)
{
	if (labs(time(NULL)-m_ts) > 2) {
		m_ts = time(NULL);
		this->join();
	}
}

void wElev::appoint(int elev, int direct)
{
	if (direct == 1) {
		m_up[elev].load(m_style, "up2");
		m_up[elev].move(m_up[elev].x()+elev*m_offset, m_up[elev].y());
	} else if (direct == 2) {
		m_down[elev].load(m_style, "down2");
		m_down[elev].move(m_down[elev].x()+elev*m_offset, m_down[elev].y());
	}

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

void wElev::permit(void)
{
	m_permit.load(m_style, "permit2");
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

void wElev::join(void)
{
	if (sCaller.sip.url) {
		dxml p;
		dmsg req;
		p.setText("/params/to", sCaller.sip.url);
		p.setText("/params/data/params/event_url", "/elev/join");
		req.request("/talk/sip/sendto", p.data());
	}
}

void wElev::data(uint8_t index, int direct, int sign, const char *s)
{
	if (index < 2) {
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
		m_data[index].setText(ss);
	}
}
