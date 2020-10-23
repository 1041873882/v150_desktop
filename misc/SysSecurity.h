#ifndef __SYS_SECURITY_H__
#define __SYS_SECURITY_H__

#include <string>

class SysSecurity {
public:
	SysSecurity();
	~SysSecurity();

	void start(void);
	int  start_alarm(void);
	void cancel(void);
	void do_process(void);
private:
	int retry_times;
	int alarm_running;
	time_t send_time;
};

extern SysSecurity sSecurity;

#endif
