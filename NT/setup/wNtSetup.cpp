
#include "sys.h"
#include "sMisc.h"
#include "wNtMain.h"
#include "wNtSetupVolumeLabel.h"
#include "wNtSetupPasswdLabel.h"
#include "wNtSetup.h"

wNtSetup::wNtSetup() : wNtWindow("setup")
{
	char s[128];

	m_title.setParent(this);
	m_title.load(m_style, "title");

	m_fw_text.setParent(this);
	m_fw_text.load(m_style, "fw_text");
	m_fw.setParent(this);
	m_fw.load(m_style, "fw");
	dmsg req;
	req.request("/talk/version", NULL);
	dxml p(req.body());
	m_fw.setText(p.getText("/params/version"));

	m_ui_text.setParent(this);
	m_ui_text.load(m_style, "ui_text");
	m_ui.setParent(this);
	m_ui.load(m_style, "ui");
	sys.version(s);
	m_ui.setText(s);

	m_ip_text.setParent(this);
	m_ip_text.load(m_style, "ip_text");
	m_ip.setParent(this);
	m_ip.load(m_style, "ip");
	struct sockaddr_in addr;
	if (local_if_addr(&addr))
		sprintf(s, "%s", "0.0.0.0");
	else
		sprintf(s, "%s", inet_ntoa(addr.sin_addr));
	m_ip.setText(s);

	m_gw_text.setParent(this);
	m_gw_text.load(m_style, "gw_text");
	m_gw.setParent(this);
	m_gw.load(m_style, "gw");
	m_gw.setText(sys.net.gateway());

	m_mac_text.setParent(this);
	m_mac_text.load(m_style, "mac_text");
	m_mac.setParent(this);
	m_mac.load(m_style, "mac");
	m_mac.setText(sys.net.mac());

	m_room_text.setParent(this);
	m_room_text.load(m_style, "room_text");
	m_room.setParent(this);
	m_room.load(m_style, "room");
	sprintf(s, "%d-%d-%d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family());
	m_room.setText(s);
}

wNtSetup::~wNtSetup()
{
}

void wNtSetup::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			wNtSetupVolumeLabel *w = new wNtSetupVolumeLabel();
			w->show();
		} else if (e->wParam == KEY_M_N2) {
			wNtSetupPasswdLabel *w = new wNtSetupPasswdLabel();
			w->show();
		} else if (e->wParam == KEY_M_N3) {
		} else if (e->wParam == KEY_M_N5) {
			wNtMain *w = new wNtMain();
			w->show();
		}
	}
}
