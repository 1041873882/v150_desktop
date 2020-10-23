
#include <stdio.h>
#include <string.h>

#include "sys.h"
#include "SysSecurity.h"
#include "SysRs485.h"
#include "SysSmart.h"

SysSmart smart;

smart_elev_t sys_elev[3];

static void *dnake_smart_thread(void *)
{
	sleep(1);
	sSecurity.rs485_query();
	while (1) {
		rs485.process();
		smart.process();
		usleep(40*1000);
	}
	return NULL;
}

typedef struct {
	char cmd[128];
	int zone;
	int index;
	int onoff;
} smart_ctrl_t;

SysSmart::SysSmart()
{
	m_scene = 0;
	memset(m_zone, 0, sizeof(m_zone));
}

SysSmart::~SysSmart()
{
}

int SysSmart::start(void)
{
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_smart_thread, NULL) != 0)
		perror("pthread_create dnake_misc_thread!\n");
	return 0;
}

void SysSmart::scene(int mode)
{
	m_scene = mode;
	if (sys.talk.dcode() > 0) { // 副机操作
		dmsg req;
		dxml p;
		p.setText("/params/cmd", "scene");
		p.setInt("/params/mode", mode);
		req.request("/talk/slave/smart/control", p.data());
	} else {
		smart_ctrl_t d;
		strcpy(d.cmd, "scene");
		d.onoff = mode;
		m_fifo.put(&d, sizeof(d));
	}
}

void SysSmart::light(int z, int n, int onoff)
{
	if (z >= MAX_SMART_ZONE)
		return;

	m_zone[z].light[n] = onoff;
	if (sys.talk.dcode() > 0) { // 副机操作
		dmsg req;
		dxml p;
		p.setText("/params/cmd", "light");
		p.setInt("/params/module", z);
		p.setInt("/params/light", n);
		p.setInt("/params/onoff", onoff);
		req.request("/talk/slave/smart/control", p.data());
	} else {
		smart_ctrl_t d;
		strcpy(d.cmd, "light");
		d.zone = z;
		d.index = n;
		d.onoff = onoff;
		m_fifo.put(&d, sizeof(d));
	}
}

void SysSmart::curtain(int z, int n, int onoff)
{
	if (z >= MAX_SMART_ZONE)
		return;

	m_zone[z].curtain[n] = onoff;
	if (sys.talk.dcode() > 0) { // 副机操作
		dmsg req;
		dxml p;
		p.setText("/params/cmd", "curtain");
		p.setInt("/params/module", z);
		p.setInt("/params/index", n);
		p.setInt("/params/onoff", onoff);
		req.request("/talk/slave/smart/control", p.data());
	} else {
		smart_ctrl_t d;
		strcpy(d.cmd, "curtain");
		d.zone = z;
		d.index = n;
		d.onoff = onoff;
		m_fifo.put(&d, sizeof(d));
	}
}

void SysSmart::air(int z, int n, int onoff)
{
	if (z >= MAX_SMART_ZONE)
		return;

	if (sys.talk.dcode() > 0) { // 副机操作
		dmsg req;
		dxml p;
		p.setText("/params/cmd", "air");
		p.setInt("/params/module", z);
		p.setInt("/params/index", n);
		p.setInt("/params/onoff", onoff);
		req.request("/talk/slave/smart/control", p.data());
	} else {
		smart_ctrl_t d;
		strcpy(d.cmd, "air");
		d.zone = z;
		d.index = n;
		d.onoff = onoff;
		m_fifo.put(&d, sizeof(d));
	}
}

void SysSmart::join(int z)
{
	if (z >= MAX_SMART_ZONE)
		return;

	if (sys.talk.dcode() > 0) {
		dxml p;
		dmsg req;
		p.setText("/params/cmd", "join");
		p.setInt("/params/module", z);
		req.request("/talk/slave/smart/control", p.data());
	} else {
		m_zone[z].ts = time(NULL);
	}
}

void SysSmart::rControl(dxml &p)
{
	const char *cmd = p.getText("/params/cmd");
	if (cmd == NULL)
		return;
	if (!strcmp(cmd, "scene")) {
		int m = p.getInt("/params/mode", 0);
		smart.scene(m);
	} else if (!strcmp(cmd, "light")) {
		int m = p.getInt("/params/module", 0);
		int light = p.getInt("/params/light", 0);
		int onoff = p.getInt("/params/onoff", 0);
		smart.light(m, light, onoff);
	} else if (!strcmp(cmd, "curtain")) {
		int m = p.getInt("/params/module", 0);
		int n = p.getInt("/params/index", 0);
		int onoff = p.getInt("/params/onoff", 0);
		smart.curtain(m, n, onoff);
	} else if (!strcmp(cmd, "air")) {
		int m = p.getInt("/params/module", 0);
		int idx = p.getInt("/params/index", 0);
		int onoff = p.getInt("/params/onoff", 0);
		smart.air(m, idx, onoff);
	} else if (!strcmp(cmd, "join")) {
		int m = p.getInt("/params/module", 0);
		smart.join(m);
	}
}

void SysSmart::rData(dxml &p)
{
	int z = p.getInt("/params/zone", 0);
	if (z < MAX_SMART_ZONE) {
		m_scene = p.getInt("/params/scene", m_scene);
		for (int i=0; i<8; i++) {
			char s[128];
			sprintf(s, "/params/light/s%d", i);
			m_zone[z].light[i] = p.getInt(s, m_zone[z].light[i]);
		}
		m_zone[z].curtain[0] = p.getInt("/params/curtain/s0", m_zone[z].curtain[0]);
		m_zone[z].curtain[1] = p.getInt("/params/curtain/s1", m_zone[z].curtain[1]);
	}
}

void SysSmart::sync(int z)
{
	if (z < MAX_SMART_ZONE)
		m_zone[z].mark = 1;
}

void SysSmart::rSync(int z)
{
	char s[128];
	dmsg req;
	dxml p;

	if (z == 0)
		p.setInt("/params/scene", m_scene);
	p.setInt("/params/zone", z);
	for(int i=0; i<8; i++) {
		sprintf(s, "/params/light/s%d", i);
		p.setInt(s, m_zone[z].light[i]);
	}
	for(int i=0; i<2; i++) {
		sprintf(s, "/params/curtain/s%d", i);
		p.setInt(s, m_zone[z].curtain[i]);
	}
	req.request("/talk/slave/smart/data", p.data());
}

void SysSmart::sControl(void)
{
	smart_ctrl_t d;
	while (m_fifo.get(&d, sizeof(d)) > 0) {
		if (!strcmp(d.cmd, "scene")) {
			ioctl.scene(d.onoff);
			this->sync(0);
		} else if (!strcmp(d.cmd, "light")) {
			ioctl.light(d.zone, d.index, d.onoff);
			this->sync(d.zone);
		} else if (!strcmp(d.cmd, "curtain")) {
			ioctl.curtain(d.zone, d.index, d.onoff);
			this->sync(d.zone);
		} else if (!strcmp(d.cmd, "air")) {
			ioctl.air(d.zone, d.index, d.onoff);
		}
	}
}

void SysSmart::process(void)
{
	static time_t ts = 0;

	this->sControl();
	if (ts != time(NULL)) {
		for(int i=0; i<MAX_SMART_ZONE; i++) {
			if (labs(time(NULL)-m_zone[i].ts) < 10) {
				if (i == 0) {
					this->sControl();
					ioctl.qScene();
				}

				for(int n=0; n<8; n++) {
					this->sControl();
					ioctl.qLight(i, n);
				}
				for(int n=0; n<2; n++) {
					this->sControl();
					ioctl.qCurtain(i, n);
				}
			}
		}
		for(int i=0; i<MAX_SMART_ZONE; i++) {
			if (m_zone[i].mark) {
				m_zone[i].mark = 0;
				this->rSync(i);
			}
		}
		ts = time(NULL);
	}
}
