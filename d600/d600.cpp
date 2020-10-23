
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "AutoMutex.h"
#include "sMisc.h"

#include "sys.h"
#include "SysNtp.h"
#include "SysLogger.h"
#include "SysSecurity.h"
#include "d600.h"

__c600 c600;

static void *dnake_c600_thread(void *)
{
	c600.process();
	return NULL;
}

__c600::__c600()
{
	strcpy(m_id, "0000");
	m_timeout = 10;
	m_ts = time(NULL);
}

__c600::~__c600()
{
}

void __c600::setid(void)
{
	if (sys_mode == SYS_PANEL) {
		if (sys.panel.mode() == 0) //单元门口机
			sprintf(m_id, "M%04d%02d%c0000", sys.talk.build(), sys.talk.unit(), '0'+(sys.panel.index()-1));
		else if (sys.panel.mode() == 1) //围墙机
			sprintf(m_id, "W00%02d", sys.panel.index());
		else //小门口机
			sprintf(m_id, "H%04d%02d%02d%02d%d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family(), sys.panel.index()-1);
	} else
		sprintf(m_id, "S%04d%02d%02d%02d%c", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family(), '0'+sys.talk.dcode());
}

int __c600::start(void)
{
	if((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("__d600::start socket\n");
		return -1;
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(D600_PORT);
	if(bind(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("__d600::start bind\n");
		return -1;
	}

	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_c600_thread, NULL) != 0)
		perror("pthread_create dnake_c600_thread!\n");

	return 0;
}

void __c600::process(void)
{
	fd_set rfds;
	struct timeval tv;
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(m_socket, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 100*1000;
		select(m_socket+1, &rfds, NULL, NULL, &tv);

		if (FD_ISSET(m_socket, &rfds))
			this->rx();
		if (labs(time(NULL)-m_ts) >= m_timeout) {
			this->do_heartbeat();
			m_ts = time(NULL);
		}

		sys_ntp_process();
	}
}

void __c600::rx(void)
{
	struct sockaddr_in addr;
	socklen_t sz = sizeof(struct sockaddr_in);

	uint8_t data[2*1024];
	memset(data, 0, sizeof(data));
	int ret = recvfrom(m_socket, data, sizeof(data), 0, (struct sockaddr *)&addr, &sz);
	if (ret <= 0)
		return;

	d600_msg_t *msg = (d600_msg_t *)data;
	if (msg->type == 1) {
		msg->type = 2;
		if (msg->cmd != 55)
			this->tx(msg, D600_PKG_LEN);

		switch (msg->cmd) {
		case 3:
			this->do_msg(msg);
			break;
		}
	} else {
		switch (msg->cmd) {
		case 4:
			this->do_heartbeat_ack(msg);
			break;
		case 155:
			this->do_query_ack(msg);
			break;
		}
	}
}

int __c600::tx(void *data, int length)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(sys.talk.server());
	addr.sin_port = htons(D600_PORT);
	return sendto(m_socket, data, length, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
}

void __c600::do_msg(d600_msg_t *msg)
{
	static uint32_t seq = -1;
	static struct timeval tv = {0, 0};

	if (seq == msg->d.msg.seq)
		return;

	struct timeval tv2;
	gettimeofday(&tv2, NULL);
	int ts = (tv2.tv_sec-tv.tv_sec)*1000+(tv2.tv_usec-tv.tv_usec)/1000;
	if (abs(ts) < 300)
		return;
	tv = tv2;

	seq = msg->d.msg.seq;
	msg->d.msg.data[msg->d.msg.length] = 0;

	sLogger.msg.insert((char *)msg->d.msg.data, time(NULL));
}

void __c600::do_query(const char *id)
{
	uint8_t data[512];
	D600_Query_t *q;

	memset(data, 0, sizeof (data));
	q = (D600_Query_t *)data;
	memcpy(q->cid, "XXXCID", 6);
	q->cmd = 155;
	q->type = 1;
	struct sockaddr_in addr;
	local_if_addr(&addr);
	memcpy(q->host, m_id, 20);
	memcpy(&q->h_addr, &addr.sin_addr.s_addr, 4);
	memcpy(q->to, id, 20);

	this->tx(data, sizeof(data));
}

void __c600::do_query_ack(void *data)
{
	D600_QueryAck_t *ack = (D600_QueryAck_t *)data;
	struct sockaddr_in addr;
	char s[128];

	dmsg req;
	dxml p;
	p.setInt("/params/ips", ack->ips);
	for (int i=0; i<ack->ips; i++) {
		ack->zone[i].name[19] = 0;
		sprintf(s, "/params/name%d", i);
		p.setText(s, ack->zone[i].name);
		memcpy(&addr.sin_addr.s_addr, &ack->zone[i].addr, 4);
		sprintf(s, "/params/ip%d", i);
		p.setText(s, inet_ntoa(addr.sin_addr));
	}
	req.request("/talk/center/query", p.data());
}

void __c600::do_heartbeat(void)
{
	d600_msg_t *msg;
	uint8_t pkg[512];

	memset(pkg, 0, sizeof (pkg));
	msg = (d600_msg_t *)pkg;
	memcpy(msg->cid, "XXXCID", 6);
	msg->cmd = 4;
	msg->type = 1;
	memcpy(msg->host, m_id, 20);
	local_if_mac("eth0", msg->d.alarm_s.mac, NULL);
	msg->d.report.defence = sSecurity.m_defence;
	msg->d.report.modules = 1;
	memset(msg->d.report.zone, 0, sizeof(msg->d.report.zone));
	for (int i=0; i<8; i++)
		msg->d.report.zone[i*10+2] = sSecurity.zone[i].defence;
	this->tx(pkg, sizeof(pkg));
}

void __c600::do_heartbeat_ack(d600_msg_t *msg)
{
	m_timeout = msg->d.report_ack.timeout-1;
	if (m_timeout < 5)
		m_timeout = 5;

	struct tm tm;
	memset(&tm, 0x00, sizeof(struct tm));
	tm.tm_year = msg->d.report_ack.date[0]*256+msg->d.report_ack.date[1]-1900;
	tm.tm_mon = msg->d.report_ack.date[2]-1;
	tm.tm_mday = msg->d.report_ack.date[3];
	tm.tm_hour = msg->d.report_ack.date[4];
	tm.tm_min = msg->d.report_ack.date[5];
	tm.tm_sec = msg->d.report_ack.date[6];

	struct timeval tv;
	tv.tv_sec = mktime(&tm);
	if (labs(time(NULL)-tv.tv_sec) > 5) {
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);
		system("busybox hwclock -u -w");
	}
}

void __c600::do_alarm(int z)
{
	d600_msg_t *msg;
	uint8_t pkg[512];

	memset(pkg, 0, sizeof (pkg));
	msg = (d600_msg_t *)pkg;
	memcpy(msg->cid, "XXXCID", 6);
	msg->cmd = 1;
	msg->type = 1;
	memcpy(msg->host, m_id, 20);
	local_if_mac("eth0", msg->d.alarm_s.mac, NULL);
	msg->d.alarm_s.defence = sSecurity.m_defence;
	msg->d.alarm_s.sp = 0;
	msg->d.alarm_s.modules = 1;
	msg->d.alarm_s.info[0] = 0x00;
	msg->d.alarm_s.info[1] = 0x00;
	if (z > 8) {
		msg->d.alarm_s.info[1] |= 1<<(z-8);
	} else
		msg->d.alarm_s.info[0] = 1<<z;
	this->tx(pkg, sizeof(pkg));
}

void __c600::do_cancel(void)
{
	uint8_t pkg[512];
	d600_msg_t *msg = (d600_msg_t *)pkg;

	memset(pkg, 0, sizeof(pkg));
	memcpy(msg->cid, "XXXCID", 6);
	msg->cmd = 2;
	msg->type = 1;
	memcpy(msg->host, m_id, 20);
	msg->d.alarm_c.defence = sSecurity.m_defence;
	msg->d.alarm_c.modules = 1;
	this->tx(pkg, sizeof(pkg));
}
