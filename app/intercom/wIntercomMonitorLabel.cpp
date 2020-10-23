
#include "sys.h"
#include "SysCaller.h"
#include "SysSecurity.h"
#include "mFB.h"
#include "wMain.h"
#include "wIntercom.h"
#include "wIntercomMonitorLabel.h"

wIntercomMonitorLabel::wIntercomMonitorLabel() : mWindow("intercom/monitor")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

	m_unlock.setParent(this);
	m_unlock.load(m_style, "unlock");
	m_start.setParent(this);
	m_start.load(m_style, "start");
	m_stop.setParent(this);
	m_stop.load(m_style, "stop");
	m_left.setParent(this);
	m_left.load(m_style, "left");
	m_right.setParent(this);
	m_right.load(m_style, "right");
	m_data.setParent(this);
	m_data.load(m_style, "data");
	char s[128];
	sprintf(s, "%s1", m_style.getText("/style/text/panel"));
	m_data.setText(s);

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	fb.osd(x, y, w, h);

	m_mode = 0;
	m_index = 0;
	sCaller.stop();
}

wIntercomMonitorLabel::~wIntercomMonitorLabel()
{
	fb.osd(0, 0, 0, 0);

	if (SysAlarm && sCaller.mode() != sCaller.NONE) {
		//报警触发导致页面切换
		sCaller.stop();
	}
}

void wIntercomMonitorLabel::start(void)
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

void wIntercomMonitorLabel::stop(void)
{
	if (sCaller.mode() == sCaller.NONE)
		return;

	if (sCaller.mode() != sCaller.TALKING) {
		this->showPrompt(2);
	} else {
		this->showPrompt(3);
	}
}

void wIntercomMonitorLabel::doStart(void)
{
	if (sCaller.mode() != sCaller.NONE)
		return;

	char s[128], s2[128];
	if (m_mode == 0) {
		sprintf(s, "%d%02d99%02d", sys.talk.build(), sys.talk.unit(), m_index+1);
		sprintf(s2, "M%04d%02d%c0000", sys.talk.build(), sys.talk.unit(), '0'+m_index);
	} else {
		sprintf(s, "%d%04d%02d%02d%02d", m_index+1, sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family());
		sprintf(s2, "H%04d%02d%02d%02d%c", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family(), '0'+m_index);
	}
	sCaller.query(s);
	usleep(100*1000);
	sCaller.q600(s2);
	this->showPrompt(1);
}

void wIntercomMonitorLabel::doStop(void)
{
	this->showPrompt(0);
	sCaller.stop();
}

void wIntercomMonitorLabel::showData(void)
{
	char s[128];
	if (m_mode)
		sprintf(s, "%s%d", m_style.getText("/style/text/person"), m_index+1);
	else
		sprintf(s, "%s%d", m_style.getText("/style/text/panel"), m_index+1);
	m_data.setText(s);
}

void wIntercomMonitorLabel::showPrompt(int st)
{
	const char *s;
	if (st == 0)
		s = "";
	else if (st == 1)
		s = m_style.getText("/style/text/start");
	else if (st == 2)
		s = m_style.getText("/style/text/failed");
	else
		s = m_style.getText("/style/text/stop");
	m_prompt.setText(s);
}

void wIntercomMonitorLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEventMonitor) {
		if (e->wParam) { //开始监视
			this->start();
			sCaller.play(NULL);
		} else { //监视结束
			this->stop();
			sCaller.stop();
		}
	} else if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_MONITOR) {
			sCaller.stop();
			wMain *w = new wMain();
			w->show();
		} else if (e->wParam == KEY_M_UNLOCK)
			sCaller.unlock();
	}
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_start.id()) {
			this->doStart();
		} else if (e->wParam == m_stop.id()) {
			this->doStop();
		} else if (e->wParam == m_left.id()) {
			if (m_index > 0)
				m_index--;
			this->showData();
		} else if (e->wParam == m_right.id()) {
			if (m_index < 8)
				m_index++;
			this->showData();
		} else if (e->wParam == m_data.id()) {
			m_mode = m_mode ? 0 : 1;
			this->showData();
		} else if (e->wParam == m_unlock.id()) {
			sCaller.unlock();
		} else if (e->wParam == m_exit.id()) {
			sCaller.stop();
			wIntercom *w = new wIntercom();
			w->show();
		}
	}
}

void wIntercomMonitorLabel::doTimer(void)
{
	if (sCaller.mode() == sCaller.QUERY) {
		if (sCaller.sip.url) {
			sCaller.m700(sCaller.sip.url);
		} else if (sCaller.ts() >= 1 && sCaller.d600.host) {
			sCaller.m600(sCaller.d600.host, sCaller.d600.ip);
		} else if (sCaller.ts() >= 2) {
			this->showPrompt(2);
			sCaller.stop();
		}
	}
	mWindow::doTimer();
}
