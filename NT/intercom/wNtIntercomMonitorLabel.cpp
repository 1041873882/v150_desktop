
#include "sys.h"
#include "SysCaller.h"
#include "SysSecurity.h"
#include "mFB.h"
#include "wNtIntercom.h"
#include "wNtIntercomMonitorLabel.h"

wNtIntercomMonitorLabel::wNtIntercomMonitorLabel() : wNtWindow("intercom/monitor")
{
	m_select = 0;

	m_osd.setParent(this);
	m_osd.load(m_style, "osd");
	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	this->showPrompt(0);

	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	fb.osd(x, y, w, h);

	sCaller.stop();
}

wNtIntercomMonitorLabel::~wNtIntercomMonitorLabel()
{
	fb.osd(0, 0, 0, 0);

	if (SysAlarm && sCaller.mode() != sCaller.NONE) {
		//报警触发导致页面切换
		sCaller.stop();
	}
}

void wNtIntercomMonitorLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);
	if (e->type == mEventMonitor) {
		if (e->wParam) { //开始监视
			this->start();
			sCaller.play(NULL);
		} else { //监视结束
			if (sCaller.mode() != sCaller.NONE) {
				if (sCaller.mode() != sCaller.TALKING) {
					this->showPrompt(2);
				} else {
					this->showPrompt(3);
				}
			}
			sCaller.stop();
		}
	} else if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			if (m_select > 0) {
				m_select--;
				this->showPrompt(0);
			}
		} else if (e->wParam == KEY_M_N2) {
			if (m_select < 8) {
				m_select++;
				this->showPrompt(0);
			}
		} else if (e->wParam == KEY_M_N3) {
			if (sCaller.mode() != sCaller.NONE) {
				this->showPrompt(0);
				sCaller.stop();
			} else
				this->doStart();
		} else if (e->wParam == KEY_M_N4) {
			sCaller.unlock();
		} else if (e->wParam == KEY_M_N5) {
			sCaller.stop();
			wNtIntercom *w = new wNtIntercom();
			w->show();
		}
	}
}

void wNtIntercomMonitorLabel::doTimer(void)
{
	wNtWindow::doTimer();
	if (sCaller.mode() == sCaller.QUERY) {
		if (sCaller.sip.url) {
			sCaller.m700(sCaller.sip.url);
		} else if (sCaller.ts() > 1 && sCaller.d600.host) {
			sCaller.m600(sCaller.d600.host, sCaller.d600.ip);
		} else if (sCaller.ts() > 2) {
			this->showPrompt(2);
			sCaller.stop();
		}
	}
}

void wNtIntercomMonitorLabel::doStart(void)
{
	if (sCaller.mode() != sCaller.NONE)
		return;

	char s[128], s2[128];
	sprintf(s, "%d%02d99%02d", sys.talk.build(), sys.talk.unit(), m_select+1);
	sprintf(s2, "M%04d%02d%c0000", sys.talk.build(), sys.talk.unit(), '0'+m_select);

	sCaller.query(s);
	usleep(100*1000);
	sCaller.query(s);
	sCaller.q600(s2);
	this->showPrompt(1);
}

void wNtIntercomMonitorLabel::showPrompt(int type)
{
	char s[512];
	const char *ss = "";

	if (type == 1)
		ss = "正在监视";
	else if (type == 2)
		ss = "监视失败";
	else if (type == 3)
		ss = "监视结束";
	sprintf(s, "门口机%02d %s", m_select+1, ss);
	m_prompt.setText(s);
}

void wNtIntercomMonitorLabel::start(void)
{
	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;

	dmsg req;
	dxml p;
	p.setInt("/params/x", x);
	p.setInt("/params/y", y);
	p.setInt("/params/width", w);
	p.setInt("/params/height", h);
	req.request("/talk/vo_start", p.data());
	sCaller.mute(0, 1); //MIC静音
}
