
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "sMisc.h"
#include "types.h"
#include "sys.h"
#include "SysNtp.h"

#define JAN_1970   2208988800.0 /* 1970 - 1900 in seconds */

typedef struct __ntp_t {
	uint8_t mode : 3; // NTP mode
	uint8_t versionNumber : 3; // SNTP version number
	uint8_t leapIndicator : 2; // Leap second indicator

	uint8_t stratum; // Stratum level of local clock
	int8_t poll; // Poll interval
	int8_t precision; // Precision (seconds to nearest power of 2)
	uint32_t root_delay; // Root delay between local machine and server
	uint32_t root_dispersion; // Root dispersion (maximum error)
	uint32_t ref_identifier; // Reference clock identifier
	uint32_t ref_ts_secs; // Reference timestamp (in seconds)
	uint32_t ref_ts_fraq; // Reference timestamp (fractions)
	uint32_t orig_ts_secs; // Origination timestamp (in seconds)
	uint32_t orig_ts_fraq; // Origination timestamp (fractions)
	uint32_t recv_ts_secs; // Time at which request arrived at sender (seconds)
	uint32_t recv_ts_fraq; // Time at which request arrived at sender (fractions)
	uint32_t tx_ts_secs; // Time at which request left sender (seconds)
	uint32_t tx_ts_fraq; // Time at which request left sender (fractions)
} __attribute__((packed)) ntp_t;

static unsigned int __inet_addr(const char *hostname)
{
	struct hostent * host = gethostbyname(hostname);
	if (host == NULL) {
		herror(hostname);
		goto err;
	}
	if (host->h_length != 4) {
		fprintf(stderr, "host->h_length:%d\n", host->h_length);
		goto err;
	}
	unsigned int addr;
	memcpy(&addr, host->h_addr_list[0], 4);
	return addr;
err:
	return inet_addr(hostname);
}

static int domain_resolver(const char *domain, char* ipaddr)
{
	if (!domain || !ipaddr) {
		return -1;
	}
	struct hostent* host = gethostbyname(domain);
	if (!host) {
		return -1;
	}
	strncpy(ipaddr, inet_ntoa(*(struct in_addr*)host->h_addr), 16);
	return 0;
}

static int IpValueOK(const char *p, const char *p2)
{
	char s[32];

	memset(s, 0, sizeof(s));
	memcpy(s, p, p2-p);
	if (atoi(s) > 255) {
		return 0;
	}
	return 1;
}

static int isValidIp(const char *ip)
{
	int c = 0;
	const char *p = ip;

	while (*ip) {
		if (*ip == '.') {
			if (IpValueOK(p, ip) == 0) {
				return 0;
			}
			p = ip+1;
			if (*(ip+1) != '\0') {
				c++;
			}
		}
		ip++;
	}
	if (IpValueOK(p, ip) == 0) {
		return 0;
	}
	return (c == 3) ? 1 : 0;
}

static int do_ntp(const char *host, int port)
{
	int result  = -1;
	char ip[64] = {0};

	if (!host || host[0] == '\0') {
		return -1;
	}
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		return -1;
	}
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if (isValidIp(host)) {
		strncpy(ip, host, strlen(host));
	} else if (domain_resolver(host, ip) != 0) {
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = __inet_addr(ip);

	ntp_t pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.versionNumber = 3; // NTP Version 3
	pkt.mode = 3; // NTP Client
	pkt.orig_ts_secs = htonl(JAN_1970);

	// 只发送一次较容易失败
	for (int i = 0; i < 3; i++) {
		sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&addr, len);
	}

	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(s, &rfds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	select(s+1, &rfds, NULL, NULL, &tv);
	if (FD_ISSET(s, &rfds)) {
		int ret = recvfrom(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&addr, &len);
		if (ret > 0) {
			time_t sec = ntohl(pkt.tx_ts_secs) - JAN_1970;
			if (((uint8_t *)&pkt.tx_ts_fraq)[0] & 0x80) {
				sec++;
			}
			if (labs(time(NULL)-sec) > 3) {
				tv.tv_sec = sec;
				tv.tv_usec = 0;
				settimeofday(&tv, NULL);
				//system("hwclock -u -w");
			}
			result = 0;
		}
	}
	close(s);
	return result;
}

#define NTP_TIMEOUT	(5*60)
void sys_ntp_process(void)
{
	int force_update = 0;
	static time_t ts = time(NULL)+10-NTP_TIMEOUT;
	if (!ethtool_get_link()) {
		return;
	}
	if (sys.net.ntp_force()) {
		force_update = 1;
		sys.net.ntp_force(0);
	}
	if (force_update || ((time(NULL)-ts) >= NTP_TIMEOUT)) {
		if (do_ntp(sys.talk.server(), 9123)) {
			do_ntp(sys.net.ntp(), 123);
		}
		ts = time(NULL);
	}
}
