
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sys.h"
#include "SysSound.h"
#include "wSetupNetworkLabel.h"

wSetupNetworkLabel::wSetupNetworkLabel() : mWindow("setup/network")
{
	m_ip_text.setParent(this);
	m_ip_text.load(m_style, "ip_text");
	m_ip.setParent(this);
	m_ip.load(m_style, "ip");
	m_ip.setText(sys.net.ip());
	m_ip.setMode(mEdit::IP);

	m_mask_text.setParent(this);
	m_mask_text.load(m_style, "mask_text");
	m_mask.setParent(this);
	m_mask.load(m_style, "mask");
	m_mask.setText(sys.net.mask());
	m_mask.setMode(mEdit::IP);

	m_gateway_text.setParent(this);
	m_gateway_text.load(m_style, "gateway_text");
	m_gateway.setParent(this);
	m_gateway.load(m_style, "gateway");
	m_gateway.setText(sys.net.gateway());
	m_gateway.setMode(mEdit::IP);

	m_dns_text.setParent(this);
	m_dns_text.load(m_style, "dns_text");
	m_dns.setParent(this);
	m_dns.load(m_style, "dns");
	m_dns.setText(sys.net.dns());
	m_dns.setMode(mEdit::IP);

	m_server_text.setParent(this);
	m_server_text.load(m_style, "server_text");
	m_server.setParent(this);
	m_server.load(m_style, "server");
	m_server.setText(sys.talk.server());
	m_server.setMode(mEdit::IP);

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
}

wSetupNetworkLabel::~wSetupNetworkLabel()
{
}

void wSetupNetworkLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->save();
		}
	}
}

static inline void IpStrip(char *ip) //IP去前导0
{
	char *p = ip;
	int dot = 1;

	while (*p) {
		if (*p == '0') {
			if (dot == 1 && *(p+1) != '.' && *(p+1) != 0)
				strcpy(p, p+1);
		} else if (*p == '.')
			dot = 1;
		else
			dot = 0;
		p++;
	}
}

static inline int IpValueOK(const char *p, const char *p2)
{
	char s[32];

	memset(s, 0, sizeof(s));
	memcpy(s, p, p2-p);
	if (atoi(s) > 255)
		return 1;
	return 0;
}

static inline int IpValided(const char *ip)
{
	int c = 0;
	const char *p = ip;

	while (*ip) {
		if (*ip == '.') {
			if (IpValueOK(p, ip))
				return 0;
			p = ip+1;
			if (*(ip+1) != '\0')
				c++;
		}
		ip++;
	}
	if (IpValueOK(p, ip))
		return 0;

	return (c == 3) ? 1 : 0;
}

static inline int IpValided2(const char *ip, const char *mask, const char *gateway)
{
	in_addr_t _ip, _mask, _gateway;

	_ip = inet_addr(ip);
	_mask = inet_addr(mask);
	_gateway = inet_addr(gateway);

	if ((_gateway & 0xFF000000) == 0x00 || (_gateway & 0xFF000000) == 0xFF000000)
		return 0;
	if ((_ip & 0xFF000000) == 0xFF000000) //禁止IP为255
		return 0;
	if ((_ip & _mask) == (_gateway & _mask))
		return 1;
	return 0;
}

void wSetupNetworkLabel::save(void)
{
	char ip[32], mask[32], gateway[32], dns[32], server[32];
	strcpy(ip, m_ip.text());
	strcpy(mask, m_mask.text());
	strcpy(gateway, m_gateway.text());
	strcpy(dns, m_dns.text());
	strcpy(server, m_server.text());

	IpStrip(ip);
	IpStrip(mask);
	IpStrip(gateway);
	IpStrip(dns);
	IpStrip(server);

	if (IpValided(ip) && IpValided(mask) && IpValided(gateway) &&
	    IpValided(dns) && IpValided(server) && IpValided2(ip, mask, gateway)) {
		sys.net.ip(ip);
		sys.net.mask(mask);
		sys.net.gateway(gateway);
		sys.net.dns(dns);
		sys.talk.server(server);
		sys.save();
		sound.setup_ok();
		sys.setEthIp();
	} else
		sound.setup_err();
}
