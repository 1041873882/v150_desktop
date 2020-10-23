
#include <unistd.h>
#include <pthread.h>
#include "sys.h"
#include "d600.h"
#include "sGpio.h"
#include "wTalk.h"
#include "wSecurityAlarmLabel.h"
#include "wNtSecurityAlarmLabel.h"
#include "SysSound.h"
#include "SysCaller.h"
#include "SysSecurity.h"

SysSecurity sSecurity;

int sys_alarm_delay[] = {0, 5, 15, 20, 25, 40, 60, 0, 0, 0, 0};

static void *dnake_security_thread(void *)
{
	sSecurity.do_process();
	return NULL;
}

SecurityZone::SecurityZone()
{
	defence = 0;
	type = 2;
	delay = 0;
	sensor = 0;
	mode = 0;
	have = 0;
	send = 0;
	ts = 0;
	memset(scene, 0, sizeof(scene));
}

int SecurityZone::alarm(void)
{
	if (have) {
		if (type == sSecurity.T_24H)
			return 1;
		else if (labs(time(NULL)-ts) >= sys_alarm_delay[delay])
			return 1;
	}
	return 0;
}

void SecurityZone::cancel(void)
{
	have = 0;
	send = 0;
}

void SysIpc::load(void)
{
	dxml p;
	p.loadFile("/dnake/cfg/ipc.xml");
	m_idx = p.getInt("/sys/max", 0);
	for(int i=0; i<m_idx; i++) {
		char s[128];
		sprintf(s, "/sys/r%d/name", i);
		m_name[i] = p.getText(s, "");
		sprintf(s, "/sys/r%d/url", i);
		m_url[i] = p.getText(s, "");
	}
}

void SysIpc::save(void)
{
	dxml p;
	p.setInt("/sys/max", m_idx);
	for(int i=0; i<m_idx; i++) {
		char s[128];
		sprintf(s, "/sys/r%d/name", i);
		p.setText(s, m_name[i].c_str());
		sprintf(s, "/sys/r%d/url", i);
		p.setText(s, m_url[i].c_str());
	}
	p.saveFile("/dnake/cfg/ipc.xml");
	system("sync");
}

SysSecurity::SysSecurity()
{
	sGpio io;
	io.start("gpio_para", "hooter");
	io.dir("gpio_para", "hooter", 1);
	//警笛当做电源
	io.set("gpio_para", "hooter", 1);

	io.start("gpio_para", "4051_in0");
	io.dir("gpio_para", "4051_in0", 0);
	io.start("gpio_para", "4051_in1");
	io.dir("gpio_para", "4051_in1", 0);
	io.start("gpio_para", "4051_out0");
	io.dir("gpio_para", "4051_out0", 1);
	io.start("gpio_para", "4051_out1");
	io.dir("gpio_para", "4051_out1", 1);
	io.start("gpio_para", "4051_out2");
	io.dir("gpio_para", "4051_out2", 1);

	for (int i=0; i<MAX_SECURITY_SZ; i++) {
		zone[i].scene[1] = 1;
	}
	zone[0].scene[2] = 1;
	zone[1].scene[2] = 1;
	zone[0].scene[3] = 1;
	zone[1].scene[3] = 1;
	zone[2].scene[3] = 1;
	zone[3].scene[3] = 1;

	m_ts = 0;
	m_defence = 0;
	m_delay = 100;
	for(int i=0; i<MAX_SECURITY_SZ; i++)
		m_io[i] = -1;

	dsound.ts = 0;
}

SysSecurity::~SysSecurity()
{
}

void SysSecurity::start(void)
{
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_security_thread, NULL) != 0)
		perror("pthread_create dnake_security_thread!\n");
}

static const int io_d_map[] = {0x02, 0x00, 0x00, 0x01};

int SysSecurity::read(int z)
{
	sGpio io;
	io.set("gpio_para", "4051_out2", z&0x01);
	io.set("gpio_para", "4051_out1", (z>>1)&0x01);
	io.set("gpio_para", "4051_out0", (z>>2)&0x01);
	usleep(2*1000);
	int v = io.get("gpio_para", "4051_in1");
	int v2 = io.get("gpio_para", "4051_in0");
	v = v | (v2<<1);
	return io_d_map[v];
}

//#define SECURITY_SUB_16		1

void SysSecurity::do_process(void)
{
	int io[8];

	this->load();
	m_ipc.load();

	sleep(5);

	while (1) {
		int err = 0;
		for(int i=0; i<8; i++) {
			io[i] = this->read(i);
		}
		for(int i=0; i<8; i++) {
			if (io[i] != this->read(i)) {
				err = 1;
				break;
			}
		}

		if (!err) {
			if (m_io[0] == -1) {
				for(int i=0; i<8; i++) {
					m_io[i] = io[i];
				}
			} else {
				for(int i=0; i<8; i++) {
					if (io[i] == m_io[i]) {
						io[i] = 0x10;
					}
				}
				if (sys.talk.dcode()) {
					//副分机报警直接同步到主分机
					int ok = 0;
					for(int i=0; i<8; i++) {
						if (io[i] != 0x10 && m_io[i] != io[i]) {
							ok = 1;
							break;
						}
					}
					if (ok) {
#ifdef SECURITY_SUB_16
						int io_s[MAX_SECURITY_SZ];
						for(int i=0; i<8; i++) {
							io_s[i] = 0x10; //前8防区不变

							//后8防区用副机状态
							if (io[i] != 0x10 && m_io[i] != io[i]) {
								io_s[8+i] = io[i];
							} else {
								io_s[8+i] = 0x10;
							}
						}
						this->sync_io(io_s, MAX_SECURITY_SZ);
#else
						this->sync_io(io, 8);
#endif
					}
				} else {
					this->process(io, 8);
				}

				for(int i=0; i<8; i++) {
					if (io[i] != 0x10) {
						m_io[i] = io[i];
					}
				}
			}
		}

		for(int i=0; i<MAX_SECURITY_SZ; i++) {
			if (zone[i].alarm() && zone[i].send) {
				zone[i].send = 0;
				this->cms_alarm(i);
			}
		}

		this->do_dsound();
		usleep(40*1000);
	}
}

void SysSecurity::process(int io[], int length)
{
	if (length > MAX_SECURITY_SZ)
		length = MAX_SECURITY_SZ;

	int sync_ok = 0;
	int bell = 0;
	for(int i=0; i<length; i++) {
		if (zone[i].mode == M_NO && io[i] == 0x01) //常开，io=1为正常状态
			io[i] = 0x00;
		else if (zone[i].mode == M_NC && io[i] == 0x02) //常闭，io=2为正常状态
			io[i] = 0x00;
	}

	for(int i=0; i<length; i++) {
		if (io[i] == 0x10)
			continue;

		if (io[i]) {
			sync_ok = 1;
			if (zone[i].mode == M_BELL) {
				if (io[i] == 0x02)
					bell = 1;
			} else if (zone[i].type == T_EMRG) {
				this->cms_alarm(i);
				sys.mute.m_enable = 0;
			} else {
				int ok = 0;
				if (zone[i].type == T_24H)
					ok = 1;
				else if (!dsound.ts && m_defence && zone[i].defence)
					ok = 1;
				if (ok) {
					dsound.ts = 0;
					sys.mute.m_enable = 0;
					if (!zone[i].have)
						zone[i].ts = time(NULL);
					zone[i].have = 1;
					zone[i].send = 1;
					m_ts = time(NULL);
				}
			}
		}
	}

	if (bell) {
		sound.bell();
	}
	if (sync_ok && !sys.talk.dcode()) {
		this->sync_io(io, length);
	}
}

void SysSecurity::defence(int val)
{
	int ok = (m_defence != val ? 1 : 0);

	m_defence = val;
	if (m_defence) {
		int n = m_defence;
		for (int i=0; i<MAX_SECURITY_SZ; i++)
			zone[i].defence = zone[i].scene[n-1];
	} else {
		for (int i=0; i<MAX_SECURITY_SZ; i++)
			zone[i].defence = 0;
	}
	if (ok) {
		this->save();
		if (val) {
			dsound.ts2 = 0;
			dsound.ts = time(NULL);
		} else {
			dsound.ts = 0;
			sound.defence_off();
		}
		this->cms_defence();
	}
}

void SysSecurity::withdraw(void)
{
	m_ts = 0;
	for (int i=0; i<MAX_SECURITY_SZ; i++)
		zone[i].cancel();
	c600.do_cancel();
	sSecurity.hooter(0);
	if (SysTalk == NULL && SysAlarm != NULL)
		sound.stop();
}

void SysSecurity::load(void)
{
	dxml p;
	int ok = p.loadFile("/dnake/cfg/security.xml");
	if (ok) {
		m_defence = p.getInt("/sys/defence", 0);
		m_delay = p.getInt("/sys/delay", 100);
		for(int i=0; i<MAX_SECURITY_SZ; i++) {
			char s[128];
			sprintf(s, "/sys/z%d/defence", i);
			zone[i].defence = p.getInt(s, 0);
			sprintf(s, "/sys/z%d/type", i);
			zone[i].type = p.getInt(s, 0);
			sprintf(s, "/sys/z%d/delay", i);
			zone[i].delay = p.getInt(s, 0);
			sprintf(s, "/sys/z%d/sensor", i);
			zone[i].sensor = p.getInt(s, 0);
			sprintf(s, "/sys/z%d/mode", i);
			zone[i].mode = p.getInt(s, 0);
			for(int j=0; j<4; j++) {
				sprintf(s, "/sys/z%d/s%d", i, j);
				zone[i].scene[j] = p.getInt(s, 0);
			}
		}
	} else {
		this->save();
	}
}

void SysSecurity::save(void)
{
	dxml p;
	p.setInt("/sys/defence", m_defence);
	p.setInt("/sys/delay", m_delay);
	for (int i=0; i<MAX_SECURITY_SZ; i++) {
		char s[128];
		sprintf(s, "/sys/z%d/defence", i);
		p.setInt(s, zone[i].defence);
		sprintf(s, "/sys/z%d/type", i);
		p.setInt(s, zone[i].type);
		sprintf(s, "/sys/z%d/delay", i);
		p.setInt(s, zone[i].delay);
		sprintf(s, "/sys/z%d/sensor", i);
		p.setInt(s, zone[i].sensor);
		sprintf(s, "/sys/z%d/mode", i);
		p.setInt(s, zone[i].mode);
		for(int j=0; j<4; j++) {
			sprintf(s, "/sys/z%d/s%d", i, j);
			p.setInt(s, zone[i].scene[j]);
		}
	}
	p.saveFile("/dnake/cfg/security.xml");
	system("sync");
}

void SysSecurity::hooter(int onoff)
{
#if 0
	sGpio io;
	io.set("gpio_para", "hooter", onoff);
#endif
}

void SysSecurity::sync_load(dxml &p)
{
	char s[128];
	for (int i=0; i<MAX_SECURITY_SZ; i++) {
		sprintf(s, "/params/zone%d/defence", i);
		zone[i].defence = p.getInt(s, zone[i].defence);
		sprintf(s, "/params/zone%d/type", i);
		zone[i].type = p.getInt(s, zone[i].type);
		sprintf(s, "/params/zone%d/delay", i);
		zone[i].delay = p.getInt(s, zone[i].delay);
		sprintf(s, "/params/zone%d/sensor", i);
		zone[i].sensor = p.getInt(s, zone[i].sensor);
		sprintf(s, "/params/zone%d/mode", i);
		zone[i].mode = p.getInt(s, zone[i].mode);
		for (int j = 0; j < 4; j++) {
			sprintf(s, "/params/zone%d/scene%d", i, j);
			zone[i].scene[j] = p.getInt(s, zone[i].scene[j]);
		}
	}
	if (p.getInt("/params/defence", -1) != -1) {
		m_delay = p.getInt("/params/timeout", m_delay);
		int st = p.getInt("/params/defence", m_defence);
		if (st == 0)
			this->withdraw();
		this->defence(st);
		this->save();

		if (sys.talk.dcode() == 0)
			sSecurity.sync_cfg(NULL);
	}
}

void SysSecurity::sync_cfg(const char *url)
{
	dmsg req;
	int max = 2;
	for (int n=0; n<MAX_SECURITY_SZ; n += max) {
		dxml p;

		if (url)
			p.setText("/params/slave_url", url);
		
		if (n+max >= MAX_SECURITY_SZ) {
			p.setInt("/params/defence", m_defence);
			p.setInt("/params/timeout", m_delay);
		}

		for (int i=0; i<max; i++) {
			char s[128];
			sprintf(s, "/params/zone%d/defence", n+i);
			p.setInt(s, zone[n+i].defence);
			sprintf(s, "/params/zone%d/type", n+i);
			p.setInt(s, zone[n+i].type);
			sprintf(s, "/params/zone%d/delay", n+i);
			p.setInt(s, zone[n+i].delay);
			sprintf(s, "/params/zone%d/sensor", n+i);
			p.setInt(s, zone[n+i].sensor);
			sprintf(s, "/params/zone%d/mode", n+i);
			p.setInt(s, zone[n+i].mode);
			for (int j=0; j<4; j++) {
				sprintf(s, "/params/zone%d/scene%d", n+i, j);
				p.setInt(s, zone[n+i].scene[j]);
			}
		}
		req.request("/talk/slave/security", p.data());
	}
}

void SysSecurity::sync_io(int io[], int length)
{
	dxml p;
	for (int i=0; i<length; i++) {
		char s[128];
		sprintf(s, "/params/io%d", i);
		p.setInt(s, io[i]);
	}
	dmsg req;
	req.request("/talk/slave/mcu_io", p.data());
}

void SysSecurity::do_dsound(void)
{
	if (dsound.ts == 0)
		return;
	if (labs(time(NULL)-dsound.ts) >= m_delay) {
		dsound.ts = 0;
		sound.defence_on();
		return;
	}
	if (labs(time(NULL)-dsound.ts2) >= 5) {
		dsound.ts2 = time(NULL);
		sound.defence_delay();
	}
}

void SysSecurity::cms_defence(void)
{
	if (sys.talk.dcode())
		return;

	dmsg req;
	dxml p;
	p.setText("/params/event_url", "/msg/alarm/defence");
	p.setInt("/params/defence", m_defence);
	p.setInt("/params/total", MAX_SECURITY_SZ);
	for(int i=0; i<MAX_SECURITY_SZ; i++) {
		char s[128];
		sprintf(s, "/params/io%d", i);
		p.setInt(s, zone[i].defence);
	}
	req.request("/talk/center/to", p.data());
}

void SysSecurity::cms_alarm(int z)
{
	if (sys.talk.dcode() || z >= MAX_SECURITY_SZ)
		return;

	dmsg req;
	dxml p;
	p.setText("/params/event_url", "/msg/alarm/trigger");
	p.setInt("/params/zone", z);
	p.setInt("/params/data", 1);
	req.request("/talk/center/to", p.data()); // 700协议报警上报

	c600.do_alarm(z);
	this->cms_broadcast(z);
}

void SysSecurity::cms_broadcast(int z)
{
	dxml p;
	p.setText("/event/broadcast_url", "security");
	p.setInt("/event/data/io", z);
	p.setInt("/event/data/type", zone[z].type);
	p.setInt("/event/data/sensor", zone[z].sensor);
	p.setInt("/event/talk/build", sys.talk.build());
	p.setInt("/event/talk/unit", sys.talk.unit());
	p.setInt("/event/talk/floor", sys.talk.floor());
	p.setInt("/event/talk/family", sys.talk.family());

	dmsg req;
	req.request("/talk/broadcast/data", p.data());
}

//UI线程处理，不允许挂起
void SysSecurity::ui_process(void)
{
	if (SysAlarm == NULL && SysTalk == NULL) {
		int have = 0;
		for(int i=0; i<MAX_SECURITY_SZ; i++) {
			if (zone[i].have) {
				have = 1;
				break;
			}
		}
		if (have) {
			if (sys.m_limit == 220) {
				wNtSecurityAlarmLabel *w = new wNtSecurityAlarmLabel();
				w->show();
			} else {
				wSecurityAlarmLabel *w = new wSecurityAlarmLabel();
				w->show();
			}
		}
	}
}

void SysSecurity::do_sos(void)
{
	for (int i=0; i<MAX_SECURITY_SZ; i++) {
		dmsg req;
		dxml p;
		p.setText("/params/event_url", "/msg/alarm/trigger");
		p.setInt("/params/zone", i);
		p.setInt("/params/data", 1);
		req.request("/talk/center/to", p.data()); // 700协议报警上报

		c600.do_alarm(i);
		this->cms_broadcast(i);
	}
}

#include "SysRs485.h"

static const uint8_t hs_cmd_al[] = {0x85, 0x02};

void SysSecurity::rs485_query(void)
{
	uint8_t tx[256], *data = NULL;
	uint16_t ck = 0;
	int bus = 0x01;
	int addr = 0x02;
	int cmd = 0x8008;
	int length = 0;

	tx[0] = 0x55;
	tx[1] = 0xAA;
	tx[2] = length+10;
	tx[3] = (bus >> 8) & 0xFF;
	tx[4] = bus & 0xFF;
	tx[5] = addr;
	tx[6] = (cmd >> 8) & 0xFF;
	tx[7] = cmd & 0xFF;

	for(int i=0; i<8; i++)
		ck += tx[i];

	for(int i=0; i<length; i++) {
		tx[8+i] = data[i];
		ck += data[i];
	}
	ck ^= 0x55AA;
	ck = htons(ck);
	memcpy(tx+8+length, &ck, 2);

	rs485.tx(tx, length+10);
}

void SysSecurity::rs485_io(uint8_t *d, int sz)
{
	uint8_t data[256], *p = data;
	memcpy(data, d, sz);
	int len = data[2];
	if (len > sz) {
		int r = rs485.rx(data+sz, sizeof(data)-sz, 100);
		if (r > 0)
			sz += r;
	}

	uint16_t s = 0;
	for(int i=0; i<len-2; i++)
		s += p[i];
	s ^= 0x55AA;
	s = htons(s);
	if (memcmp(&s, &p[len-2], 2)) {
		fprintf(stderr, "SysSecurity::rs485_io checksum err!\n");
		return;
	}

	if (memcmp(p+6, hs_cmd_al, sizeof(hs_cmd_al)) == 0) { //报警命令
		static int rs485_ok = 0;

		p += 9; //报警数据 16个
		if (rs485_ok) {
			int st[MAX_SECURITY_SZ];
			for(int i=0; i<MAX_SECURITY_SZ; i++) {
				if (p[i] == m_rs485_io[i])
					st[i] = 0x10;
				else
					st[i] = p[i];
			}
			this->process(st, MAX_SECURITY_SZ);
		} else
			rs485_ok = 1;
		memcpy(m_rs485_io, p, 16);
	}
}
