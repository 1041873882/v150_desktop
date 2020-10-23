
#include <fcntl.h>
#include "SysSound.h"
#include "d600.h"
#include "sMisc.h"
#include "sys.h"
#include "sGpio.h"

#define SYS_CFG		"/dnake/cfg/sys.xml"

int sys_mode = SYS_NORMAL;
int sys_factory = 0; //����ģʽ

__sys sys;

__sys::__sys()
{
	mute.m_enable = 0;
	user.login = 0;
	admin.login = 0;

	int fd = ::open("/dev/urandom", 0);
	if (fd < 0) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		::srand(clock()+tv.tv_usec);
	} else {
		unsigned long seed;
		::read(fd, &seed, sizeof(seed));
		::srand(seed);
		::close(fd);
	}

	m_limit = 0;
	FILE *fp = fopen("/dnake/etc/limit", "r");
	if (fp) {
		fscanf(fp, "%d", &m_limit);
		fclose(fp);
	}

	sGpio io;
	io.start("gpio_para", "led0");
	io.dir("gpio_para", "led0", 1);
	io.set("gpio_para", "led0", 1);
}

__sys::~__sys()
{
}

void __sys::load(void)
{
	dxml p;
	bool ok = p.loadFile(SYS_CFG);

	user.passwd(p.getText("/sys/user/passwd", "1234"));
	user.tz(p.getText("/sys/user/tz", "+08:00"));
	user.language(p.getInt("/sys/user/language", 0));

	admin.passwd(p.getText("/sys/admin/passwd", "123456"));

	char s[32];
	sprintf(s, "BC:F8:11:%02X:%02X:%02X", (uint8_t)(rand()%15), (uint8_t)(rand()%255), (uint8_t)(rand()%255));
	net.mac(p.getText("/sys/network/mac", s));
	net.dhcp(p.getInt("/sys/network/dhcp", 0));
	net.ip(p.getText("/sys/network/ip", "192.168.68.90"));
	net.mask(p.getText("/sys/network/mask", "255.255.255.0"));
	net.gateway(p.getText("/sys/network/gateway", "192.168.68.1"));
	net.dns(p.getText("/sys/network/dns", "8.8.8.8"));
	net.ntp(p.getText("/sys/network/ntp", "pool.ntp.org"));

	talk.build(p.getInt("/sys/talk/building", 1));
	talk.unit(p.getInt("/sys/talk/unit", 1));
	talk.floor(p.getInt("/sys/talk/floor", 11));
	talk.family(p.getInt("/sys/talk/family", 11));
	talk.dcode(p.getInt("/sys/talk/dcode", 0));
	sprintf(s, "%d%04d", rand()%100, rand()%10000);
	talk.sync(p.getText("/sys/talk/sync", s));
	talk.server(p.getText("/sys/talk/server", "192.168.68.1"));
	talk.passwd(p.getText("/sys/talk/passwd", "123456"));
	talk.arp(p.getInt("/sys/talk/arp", 0));
	talk.answer(p.getInt("/sys/talk/answer", 0));

	sip.enable(p.getInt("/sys/sip/enable", 0));
	sip.proxy(p.getText("/sys/sip/proxy", "sip:192.168.68.1"));
	sip.realm(p.getText("/sys/sip/realm", "192.168.68.1"));
	sip.user(p.getText("/sys/sip/user", "100"));
	sip.passwd(p.getText("/sys/sip/passwd", "123456"));
	sip.host2id(p.getInt("/sys/sip/host2id", 1));
	sip.timeout(p.getInt("/sys/sip/timeout", 300));

	stun.ip(p.getText("/sys/stun/ip", "192.168.68.1"));
	stun.port(p.getInt("/sys/stun/port", 5060));

	volume.talk(p.getInt("/sys/volume/talk", 0));
	volume.music(p.getInt("/sys/volume/music", 0));

	payload.h264(p.getInt("/sys/payload/h264", 102));

	lcd.bright(p.getInt("/sys/lcd/bright", 204));

	dtmf.enable(p.getInt("/sys/dtmf/enable", 0));
	dtmf.data(p.getText("/sys/dtmf/data", "#"));

	quick.enable(p.getInt("/sys/quick/enable", 0));
	quick.url(p.getText("/sys/quick/url", "sip:911@192.168.12.40"));

	awtek.alarm_freq(p.getInt("/sys/awtek/alarm_freq", 1));
	awtek.url(0 ,p.getText("/sys/awtek/register_url", "http://s1.awtek-security.com.tw/"));
	awtek.url(1 ,p.getText("/sys/awtek/alarm_url", "http://s1.awtek-security.com.tw/"));
	awtek.id_code(p.getText("/sys/awtek/id_code", "1011111"));
	awtek.community_code(p.getText("/sys/awtek/community_code", "00001"));

	if (!ok) {
		this->save();
	}
	if (m_limit == 220) {
		user.language(1);
	}
	this->setHttpUser();
	this->setTZ();
	c600.setid();
}

void __sys::save(void)
{
	dxml p;
	p.setText("/sys/user/passwd", user.passwd());
	p.setText("/sys/user/tz", user.tz());
	p.setInt("/sys/user/language", user.language());

	p.setText("/sys/admin/passwd", admin.passwd());

	p.setText("/sys/network/mac", net.mac());
	p.setInt("/sys/network/dhcp", net.dhcp());
	p.setText("/sys/network/ip", net.ip());
	p.setText("/sys/network/mask", net.mask());
	p.setText("/sys/network/gateway", net.gateway());
	p.setText("/sys/network/dns", net.dns());
	p.setText("/sys/network/ntp", net.ntp());

	p.setInt("/sys/talk/building", talk.build());
	p.setInt("/sys/talk/unit", talk.unit());
	p.setInt("/sys/talk/floor", talk.floor());
	p.setInt("/sys/talk/family", talk.family());
	p.setInt("/sys/talk/dcode", talk.dcode());
	p.setText("/sys/talk/sync", talk.sync());
	p.setText("/sys/talk/server", talk.server());
	p.setText("/sys/talk/passwd", talk.passwd());
	p.setInt("/sys/talk/arp", talk.arp());
	p.setInt("/sys/talk/answer", talk.answer());

	p.setInt("/sys/sip/enable", sip.enable());
	p.setText("/sys/sip/proxy", sip.proxy());
	p.setText("/sys/sip/realm", sip.realm());
	p.setText("/sys/sip/user", sip.user());
	p.setText("/sys/sip/passwd", sip.passwd());
	p.setInt("/sys/sip/host2id", sip.host2id());
	p.setInt("/sys/sip/timeout", sip.timeout());

	p.setText("/sys/stun/ip", stun.ip());
	p.setInt("/sys/stun/port", stun.port());

	p.setInt("/sys/volume/talk", volume.talk());
	p.setInt("/sys/volume/music", volume.music());

	p.setInt("/sys/payload/h264", payload.h264());

	p.setInt("/sys/lcd/bright", lcd.bright());

	p.setInt("/sys/dtmf/enable", dtmf.enable());
	p.setText("/sys/dtmf/data", dtmf.data());

	p.setInt("/sys/quick/enable", quick.enable());
	p.setText("/sys/quick/url", quick.url());

	p.setInt("/sys/awtek/alarm_freq", awtek.alarm_freq());
	p.setText("/sys/awtek/register_url", awtek.url(0));
	p.setText("/sys/awtek/alarm_url", awtek.url(1));
	p.setText("/sys/awtek/id_code", awtek.id_code());
	p.setText("/sys/awtek/community_code", awtek.community_code());

	p.saveFile(SYS_CFG);
	system("sync");

	this->setHttpUser();
	this->setTZ();
	c600.setid();
	sound.load();

	dmsg req;
	req.request("/talk/setid", NULL);
}

static const char *gdate_table[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

static const char *GCC_DATE(void)
{
	static char s[128];
	char m[32];
	int y, d;

	sscanf(__DATE__, "%s %d %d", m, &d, &y);
	for (int i=0; i<12; i++) {
		if (!strcasecmp(m, gdate_table[i])) {
			sprintf(s, "%04d%02d%02d", y, i+1, d);
			return s;
		}
	}
	return "err";
}

void __sys::setEthIp(void)
{
	char s[128];
	system("route del default");
	sprintf(s, "ifconfig eth0 %s netmask %s up", net.ip(), net.mask());
	system(s);
	sprintf(s, "route add default gw %s dev eth0", net.gateway());
	system(s);

	FILE *fp = fopen("/etc/resolv.conf", "w+");
	if (fp) {
		fprintf(fp, "nameserver %s\n", net.dns());
		fprintf(fp, "nameserver %s\n", "114.114.114.114");
		fclose(fp);
	}
}

void __sys::setEthMac(void)
{
	char s[128];
	system("ifconfig eth0 down");
	sprintf(s, "ifconfig eth0 hw ether %s", net.mac());
	system(s);
	system("ifconfig eth0 up");
	usleep(100*1000);
	this->setEthIp();
}

void __sys::setHttpUser(void)
{
	FILE *fp = fopen("/var/etc/httppasswd", "w+");
	if (fp) {
		fprintf(fp, "%s:%s\n", "admin", admin.passwd());
		fprintf(fp, "%s:%s\n", "special", admin.passwd());
		fprintf(fp, "%s:%s\n", "user", user.passwd());
		fclose(fp);
	}
	fp = fopen("/var/etc/language", "w+");
	if (fp) {
		fprintf(fp, "%s\n", user.language() ? "EN" : "CHS");
		fclose(fp);
	}
}

void __sys::setTZ(void)
{
	char tz[64];
	const char *p = user.tz();
	if (*p == '-')
		sprintf(tz, "GMT+%s", p+1);
	else
		sprintf(tz, "GMT-%s", p+1);
	setenv("TZ", tz, 1);
}

void __sys::resetMac(void)
{
	char s[32];
	sprintf(s, "BC:F8:11:%02X:%02X:%02X", (uint8_t)(rand()%15), (uint8_t)(rand()%255), (uint8_t)(rand()%255));
	net.mac(s);
}

void __sys::version(char *s)
{
	sprintf(s, "%d.%d.%d %s %s", SYS_MAJOR, SYS_MINOR, SYS_MINOR2, GCC_DATE(), this->flash() ? "16M" : "8M");
}

int __sys::flash(void)
{
	static int mtd = -1; // 0:8M 1:16M
	if (mtd < 0) {
		FILE *fp = fopen("/proc/mtd", "r");
		if (fp) {
			char s[1024];
			fread(s, 1, sizeof(s), fp);
			fclose(fp);

			if (strstr(s, "NorFlash 8MB")) {
				mtd = 0;
			} else if (strstr(s, "NorFlash 16MB")) {
				mtd = 1;
			} else if (strstr(s, "NorFlash 32MB")) {
				mtd = 2;
			}
			if (mtd != -1)
				return mtd;
		}

		mtd = 0;
		fp = fopen("/proc/partitions", "r");
		if (fp) {
			char m[4][256];
			while (!feof(fp)) {
				fscanf(fp, "%s %s %s %s\n", m[0], m[1], m[2], m[3]);
				if (atoi(m[2]) > 6*1024) { //dnake��������6MB
					mtd = 1;
				}
			}
			fclose(fp);
		}
	}
	return mtd;
}

const char *__sys::style(void)
{
	if (user.language() == LANG_EN)
		return "style_en.xml";
	else if (user.language() == LANG_CHT)
		return "style_cht.xml";
	else if (user.language() == LANG_RUS)
		return "style_rus.xml";
	else if (user.language() == LANG_PL)
		return "style_pl.xml";
	else if (user.language() == LANG_FR)
		return "style_fr.xml";
	else if (user.language() == LANG_TUR)
		return "style_tur.xml";
	else if (user.language() == LANG_SLO)
		return "style_slo.xml";
	return "style.xml";
}

int __sys::logo(void)
{
	dxml p;
	p.loadFile("/dnake/data/sys_logo.xml");
	int logo = p.getInt("/sys/logo", -1);
	if (logo == -1) {
		int val = 0;
		FILE *fp = fopen("/dnake/etc/logo", "r");
		if (fp) {
			fscanf(fp, "%d", &val);
			fclose(fp);
		}
		logo = val;
	}
	return logo;
}

void __sys::logo(int val)
{
	dxml p;
	p.setInt("/sys/logo", val);
	p.saveFile("/dnake/data/sys_logo.xml");
	system("sync");
}

static int VmRSS(void)
{
	int rss = 0;
	char s[2*1024];
	sprintf(s, "/proc/%d/status", getpid());
	FILE *fp = fopen(s, "r");
	if (fp) {
		int r = fread(s, 1, sizeof(s)-1, fp);
		if (r > 0) {
			s[r] = 0;
			char *p = strstr(s, "VmRSS:");
			if (p) {
				p += strlen("VmRSS:");
				rss = atoi(p);
			}
		}
		fclose(fp);
	}
	return rss;
}

void __sys::process(void)
{
	static time_t ts = 0;
	if (time(NULL) != ts) {
		if (VmRSS() >= 12*1024) //�ڴ�ռ��̫��
			exit(-1);
		ts = time(NULL);
	}
}
