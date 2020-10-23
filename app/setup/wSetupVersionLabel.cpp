
#include "sys.h"
#include "dxml.h"
#include "dmsg.h"
#include "sMisc.h"
#include "wSetupVersionLabel.h"

wSetupVersionLabel::wSetupVersionLabel() : mWindow("setup/version")
{
	char s[128];

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
		m_ip.setText("0.0.0.0");
	else
		m_ip.setText(inet_ntoa(addr.sin_addr));

	m_mac_text.setParent(this);
	m_mac_text.load(m_style, "mac_text");
	m_mac.setParent(this);
	m_mac.load(m_style, "mac");
	m_mac.setText(sys.net.mac());
}

wSetupVersionLabel::~wSetupVersionLabel()
{
}
