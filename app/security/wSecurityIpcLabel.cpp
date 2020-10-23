
#include "sys.h"
#include "mFB.h"
#include "SysSecurity.h"
#include "wSecurity.h"
#include "wSecurityIpcLabel.h"

wSecurityIpcLabel::wSecurityIpcLabel() : mWindow("security/ipc")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

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
	m_data.setEnable(0);

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	fb.osd(x, y, w, h);

	m_ts = 0;
	m_index = 0;
	this->showData();
}

wSecurityIpcLabel::~wSecurityIpcLabel()
{
	dmsg req;
	req.request("/media/rtsp/stop", NULL);
	fb.osd(0, 0, 0, 0);
}

void wSecurityIpcLabel::showData(void)
{
	if (m_index < sSecurity.m_ipc.m_idx) {
		m_data.setText(sSecurity.m_ipc.m_name[m_index]);
	} else {
		m_data.setText("");
	}
}

void wSecurityIpcLabel::doStart(void)
{
	if (m_ts != 0 || m_index >= sSecurity.m_ipc.m_idx)
		return;

	dxml p, p2;
	dmsg req;
	p.setText("/params/url", sSecurity.m_ipc.m_url[m_index].c_str());
	req.request("/media/rtsp/play", p.data());

	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	p2.setInt("/params/x", x);
	p2.setInt("/params/y", y);
	p2.setInt("/params/w", w);
	p2.setInt("/params/h", h);
	req.request("/media/rtsp/screen", p2.data());

	m_ts = time(NULL);
}

void wSecurityIpcLabel::doStop(void)
{
	m_ts = 0;
	m_prompt.setText("");
	dmsg req;
	req.request("/media/rtsp/stop", NULL);
}

void wSecurityIpcLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_start.id()) {
			this->doStart();
		} else if (e->wParam == m_stop.id()) {
			this->doStop();
		} else if (e->wParam == m_left.id()) {
			if (m_index > 0) {
				m_index--;
				this->showData();
			}
		} else if (e->wParam == m_right.id()) {
			if (m_index+1 < sSecurity.m_ipc.m_idx) {
				m_index++;
				this->showData();
			}
		} else if (e->wParam == m_exit.id()) {
			wSecurity *w = new wSecurity();
			w->show();
		}
	}
}

void wSecurityIpcLabel::doTimer(void)
{
	mWindow::doTimer();

	if (m_ts && time(NULL) != m_ts) {
		dmsg req;
		if (req.request("/media/rtsp/length", NULL) == 200) {
			dxml p(req.body());
			int n = p.getInt("/params/length", 0);
			char s[128];
			sprintf(s, "%02d:%02d", n/60, n%60);
			m_prompt.setText(s);
			m_ts = time(NULL);
		} else {
			m_prompt.setText(m_style.getText("/style/text/failed"));
			m_ts = 0;
			req.request("/media/rtsp/stop", NULL);
		}
		fb.enable(1);
	}
}
