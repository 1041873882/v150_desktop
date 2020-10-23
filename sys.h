
#ifndef __SYS_H__
#define __SYS_H__

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <string>

#include "types.h"
#include "dxml.h"
#include "dmsg.h"

#define SYS_MAJOR			1
#define SYS_MINOR			0
#define SYS_MINOR2			2

enum {
	SYS_NORMAL = 0, //�ֻ�ģʽ
	SYS_PANEL, //�ſڻ�ģʽ
};

enum {
	LANG_CHS = 0,
	LANG_EN,
	LANG_CHT,
	LANG_PL,
	LANG_FR,
	LANG_TUR,
	LANG_SLO,
	LANG_RUS,
};

class __sys {
public:
	__sys();
	~__sys();

	void load(void);
	void save(void);

	class __mute {
	public:
		int m_enable;
		time_t m_ts;
	} mute;

	class __user {
	public:
		int login;
		char m_passwd[16];
		int m_language;
		std::string m_tz; //ʱ��

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void tz(const char *val) { m_tz = val; };
		const char *tz(void) { return m_tz.c_str(); };
		void language(int val) { m_language = val; };
		int language(void) { return m_language; };
	} user;

	class __admin {
	public:
		int login;
		char m_passwd[16];

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
	} admin;

	class __net {
	public:
		char m_mac[20];
		int m_dhcp;
		int m_force;
		char m_ip[20];
		char m_mask[20];
		char m_gateway[20];
		char m_dns[20];
		char m_ntp[64];

		void mac(const char *val) { if (val) strcpy(m_mac, val); };
		const char *mac(void) { return m_mac; };
		void dhcp(int val) { m_dhcp = val; };
		int dhcp(void) { return m_dhcp; };
		void ip(const char *val) { if (val) strcpy(m_ip, val); };
		const char *ip(void) { return m_ip; };
		void mask(const char *val) { if (val) strcpy(m_mask, val); };
		const char *mask(void) { return m_mask; };
		void gateway(const char *val) { if (val) strcpy(m_gateway, val); };
		const char *gateway(void) { return m_gateway; };
		void dns(const char *val) { if (val) strcpy(m_dns, val); };
		const char *dns(void) { return m_dns; };
		void ntp(const char *val) { if (val) strcpy(m_ntp, val); };
		const char *ntp(void) { return m_ntp; };
		void ntp_force(int val) { m_force = val; };
		int  ntp_force(void) { return m_force; };
	} net;

	class __talk {
	public:
		int m_build; //����
		int m_unit; //��Ԫ��
		int m_floor; //¥���
		int m_family; //������
		int m_dcode; //�豸��
		char m_sync[16]; //ͬ����
		char m_server[32];
		char m_passwd[32];
		int m_arp;
		int m_answer; //�Զ�����

		void build(int val) { m_build = val; };
		int build(void) { return m_build; };
		void unit(int val) { m_unit = val; };
		int unit(void) { return m_unit; };
		void floor(int val) { m_floor = val; };
		int floor(void) { return m_floor; };
		void family(int val) { m_family = val; };
		int family(void) { return m_family; };
		void dcode(int val) { m_dcode = val; };
		int dcode(void) { return m_dcode; };
		void sync(const char *val) { if (val) strcpy(m_sync, val); };
		const char *sync(void) { return m_sync; };
		void server(const char *val) { if (val) strcpy(m_server, val); };
		const char *server(void) { return m_server; };
		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void arp(int val) { m_arp = val; };
		int arp(void) { return m_arp; };
		void answer(int val) { m_answer = val; };
		int answer(void) { return m_answer; };
	} talk;

	class __sip {
	public:
		int m_enable;
		char m_proxy[64];
		char m_realm[32];
		char m_user[32];
		char m_passwd[32];
		int m_timeout; //ͨ��ʱ��
		int m_host2id;

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void proxy(const char *val) { if (val) strcpy(m_proxy, val); };
		const char *proxy(void) { return m_proxy; };
		void realm(const char *val) { if (val) strcpy(m_realm, val); };
		const char *realm(void) { return m_realm; };
		void user(const char *val) { if (val) strcpy(m_user, val); };
		const char *user(void) { return m_user; };
		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void host2id(int val) { m_host2id = val; };
		int host2id(void) { return m_host2id; };
		void timeout(int val) { m_timeout = val; };
		int timeout(void) { return m_timeout; };
	} sip;

	class __stun {
	public:
		char m_ip[20];
		int m_port;

		void ip(const char *val) { if (val) strcpy(m_ip, val); };
		const char *ip(void) { return m_ip; };
		void port(int val) { m_port = val; };
		int port(void) { return m_port; };
	} stun;

	class __panel {
	public:
		__panel() { m_mode = 0; m_index = 1; };

		int m_mode;
		int m_index;

		void mode(int val) { m_mode = val; };
		int mode(void) { return m_mode; };
		void index(int val) { m_index = val; };
		int index(void) { return m_index; };
	} panel;

	class __volume {
	public:
		int m_talk;
		int m_music;

		void talk(int val) { m_talk = val; };
		int talk(void) { return m_talk; };
		void music(int val) { m_music = val; };
		int music(void) { return m_music; };
	} volume;

	class __payload {
	public:
		int m_h264;

		void h264(int val) { m_h264 = val; };
		int h264(void) { return m_h264; };
	} payload;

	class __lcd {
	public:
		int m_bright;
		void bright(int val) { m_bright = val; };
		int bright(void) { return m_bright; };
	} lcd;

	class __dtmf {
	public:
		int m_enable;
		char m_data[32];

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void data(const char *val) { if (val) strcpy(m_data, val); };
		const char *data(void) { return m_data; };
	} dtmf;

	class __quick {
	public:
		int m_enable;
		char m_url[128];

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void url(const char *val) { if (val) strcpy(m_url, val); };
		const char *url(void) { return m_url; };
	} quick;

	class __awtek {
	public:
		int m_alarm_freq;
		char m_register_url[64];
		char m_alarm_url[64];
		char m_id_code[64];
		char m_community_code[254];
		
		void alarm_freq(int val) { m_alarm_freq = val; };
		int alarm_freq(void) { return m_alarm_freq; };
		void url(int x, const char *val) { if (val) x ? strcpy(m_alarm_url, val) : strcpy(m_register_url, val); };
		const char *url(int x) { return x ? m_alarm_url : m_register_url;};
		void id_code(const char *val) { if (val) strcpy(m_id_code, val); };
		const char *id_code(void) { return m_id_code; };
		void community_code(const char *val) { if (val) strcpy(m_community_code, val); };
		const char *community_code(void) { return m_community_code; };
	} awtek;

	void setEthIp(void);
	void setEthMac(void);
	void setHttpUser(void);
	void setTZ(void);
	void resetMac(void);

	void version(char *s);
	int flash(void);
	const char *style(void);

	int logo(void);
	void logo(int val);

	void process(void);

public:
	int m_limit;
};

extern __sys sys;

extern int sys_ipwd_err;

extern int sys_mode;

extern int sys_factory;

int ui_msg_init(void);

void sys_ipwd_result(int result, const char *ip, const char *mac);

#endif
