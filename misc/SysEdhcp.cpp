
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sys.h"
#include "SysEdhcp.h"

#define EDHCP_PORT	8420
#define EDHCP_TIMEOUT	(5*60)

SysEdhcp eDhcp;

SysEdhcp::SysEdhcp()
{
	m_ts = time(NULL)-EDHCP_TIMEOUT+5;
}

SysEdhcp::~SysEdhcp()
{
}

int SysEdhcp::start(void)
{
	if((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("SysEdhcp::start socket\n");
		return -1;
	}

	int val = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));

	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(EDHCP_PORT);
	if(bind(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("SysEdhcp::start bind\n");
		return -1;
	}

	return 0;
}

void SysEdhcp::process(void)
{
	if (m_socket < 0)
		return;

	if (labs(time(NULL)-m_ts) >= EDHCP_TIMEOUT) {
		m_ts = time(NULL);
		this->discover();
	}

	fd_set rfds;
	struct timeval tv;
	FD_ZERO(&rfds);
	FD_SET(m_socket, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	select(m_socket+1, &rfds, NULL, NULL, &tv);

	if (FD_ISSET(m_socket, &rfds)) {
		char data[2*1024];
		struct sockaddr_in addr;
		socklen_t sz = sizeof(addr);
		int r = recvfrom(m_socket, data, sizeof(data)-1, 0, (struct sockaddr *)&addr, &sz);
		if (r > 0) {
			data[r] = 0;
			dxml p(data);
			const char *op = p.getText("/dhcp/op");
			const char *mac = p.getText("/dhcp/mac");
			if (op && mac && !strcasecmp(op, "ack") && !strcasecmp(mac, sys.net.mac())) {
				p.setText("/dhcp/data/server", inet_ntoa(addr.sin_addr));
				this->save(p);
			}
		}
	}
}

void SysEdhcp::discover(void)
{
	dxml p;
	p.setText("/dhcp/event", "/discover");
	p.setText("/dhcp/op", "req");
	p.setText("/dhcp/mac", sys.net.mac());

	const char *xml = p.data();

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	addr.sin_port = htons(EDHCP_PORT);
	::sendto(m_socket, xml, strlen(xml), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
}

void SysEdhcp::save(dxml &p)
{
	if (p.getInt("/dhcp/data/tid", 0) != 1) //非分机设备
		return;

	const char *user = p.getText("/dhcp/data/user");
	int n = strlen(user);
	char b[8], u[8], f[8], r[8];
	memcpy(r, user+n-2, 2); r[2] = 0;
	memcpy(f, user+n-4, 2); f[2] = 0;
	memcpy(u, user+n-6, 2); u[2] = 0;
	memcpy(b, user, n-6); b[n-6] = 0;

	int ok = 0;
	if (sys.talk.build() != atoi(b)) {
		sys.talk.build(atoi(b));
		ok = 1;
	}
	if (sys.talk.unit() != atoi(u)) {
		sys.talk.unit(atoi(u));
		ok = 1;
	}
	if (sys.talk.floor() != atoi(f)) {
		sys.talk.floor(atoi(f));
		ok = 1;
	}
	if (sys.talk.family() != atoi(r)) {
		sys.talk.family(atoi(r));
		ok = 1;
	}

	const char *server = p.getText("/dhcp/data/server");
	if (server && strcmp(server, sys.talk.server())) {
		sys.talk.server(server);
		ok = 1;
	}
	const char *passwd = p.getText("/dhcp/data/passwd");
	if (passwd && strcmp(passwd, sys.talk.passwd())) {
		sys.talk.passwd(passwd);
		ok = 1;
	}
	int dcode = p.getInt("/dhcp/data/idx", 0);
	if (dcode != sys.talk.dcode()) {
		sys.talk.dcode(dcode);
		ok = 1;
	}

	const char *net_ip = p.getText("/dhcp/data/lan/ip");
	if (net_ip && strcmp(net_ip, sys.net.ip())) {
		sys.net.ip(net_ip);
		ok = 1;
	}
	const char *net_mask = p.getText("/dhcp/data/lan/mask");
	if (net_mask && strcmp(net_mask, sys.net.mask())) {
		sys.net.mask(net_mask);
		ok = 1;
	}
	const char *net_gateway = p.getText("/dhcp/data/lan/gateway");
	if (net_gateway && strcmp(net_gateway, sys.net.gateway())) {
		sys.net.gateway(net_gateway);
		ok = 1;
	}
	const char *net_dns = p.getText("/dhcp/data/lan/dns");
	if (net_dns && strcmp(net_dns, sys.net.dns())) {
		sys.net.dns(net_dns);
		ok = 1;
	}

	int sip_enable = p.getInt("/dhcp/data/sip/enable", 0);
	if (sip_enable != sys.sip.enable()) {
		sys.sip.enable(sip_enable);
		ok = 1;
	}
	if (sip_enable) {
		const char *sip_proxy = p.getText("/dhcp/data/sip/proxy");
		if (sip_proxy && strcmp(sip_proxy, sys.sip.proxy())) {
			sys.sip.proxy(sip_proxy);
			ok = 1;
		}
		const char *sip_realm = p.getText("/dhcp/data/sip/realm");
		if (sip_realm && strcmp(sip_realm, sys.sip.realm())) {
			sys.sip.realm(sip_realm);
			ok = 1;
		}
		const char *sip_user = p.getText("/dhcp/data/sip/user");
		if (sip_user && strcmp(sip_user, sys.sip.user())) {
			sys.sip.user(sip_user);
			ok = 1;
		}
		const char *sip_passwd = p.getText("/dhcp/data/sip/passwd");
		if (sip_passwd && strcmp(sip_passwd, sys.sip.passwd())) {
			sys.sip.passwd(sip_passwd);
			ok = 1;
		}
	}

	char s[128];
	sprintf(s, "%d%02d%02d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family());
	sys.talk.sync(s);

	if (ok) {
		LOGI("SysEdhcp::save have different settings!\n");
		LOGI("%s\n", p.data());
		sys.save();
		sys.setEthIp();
	}
}
