#ifndef __SYS_SECURITY_H__
#define __SYS_SECURITY_H__

#include <string>
#include "mWindow.h"

#define MAX_SECURITY_SZ		(16)
#define MAX_ALARM_TIMEOUT	(300)

class SecurityZone {
public:
	SecurityZone();

	int alarm(void);
	void cancel(void);

public:
	int defence;
	int type;
	int delay;
	int sensor;
	int mode;
	int scene[4];

	int have;
	int send;
	time_t ts;
};

#define MAX_IPC_SZ	10

class SysIpc {
public:
	SysIpc() { m_idx = 0; };
	~SysIpc() {};

	void load(void);
	void save(void);

public:
	int m_idx;
	std::string m_name[MAX_IPC_SZ];
	std::string m_url[MAX_IPC_SZ];
};

class SysSecurity {
public:
	SysSecurity();
	~SysSecurity();

	enum {
		D_WITHDRAW = 0,
		D_OUT,
		D_HOME,
		D_SLEEP
	};

	enum {
		T_NONE = 0, //��ͨ
		T_EMRG, //����
		T_24H, //24Сʱ
	};

	enum {
		M_3C = 0,
		M_NO,
		M_NC,
		M_BELL
	};

	struct {
		time_t ts;
		time_t ts2;
	} dsound;

	void do_process(void);
	void start(void);
	int  start_alarm(void);  //YBH 2020/10/26
	void cancel(void);
	int read(int z); // 0: ���� 1:�Ͽ� 2:�պ� 0x10: �ޱ仯
	void process(int io[], int length);

	void defence(int val);
	void withdraw(void);

	void load(void);
	void save(void);
	void hooter(int onoff);
	int timeout(void) { return labs(time(NULL)-m_ts); };

	void sync_load(dxml &p);
	void sync_cfg(const char *url);
	void sync_io(int io[], int length);

	void do_dsound(void);
	void cms_defence(void);
	void cms_alarm(int z);
	void cms_broadcast(int z);

	void ui_process(void);
	void do_sos(void);
	void rs485_query(void);
	void rs485_io(uint8_t *d, int sz);

public:
	int m_io[MAX_SECURITY_SZ];
	int m_defence;
	int m_delay;

	uint8_t m_rs485_io[MAX_SECURITY_SZ];

	time_t m_ts;
	SecurityZone zone[MAX_SECURITY_SZ];

	SysIpc m_ipc;
private:
	int retry_times;
	int alarm_running;
	time_t send_time;
};

extern int sys_alarm_delay[];

extern SysSecurity sSecurity;

extern mWindow *SysAlarm;

#endif
