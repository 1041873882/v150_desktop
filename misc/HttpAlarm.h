#ifndef __HTTP_ALARM_H__
#define __HTTP_ALARM_H__

#include <string>
#include "wNtWindow.h"

class HttpAlarm {
public:
	HttpAlarm();
	~HttpAlarm();

	void start(void);
	// void end(void);
	// int  start_alarm(void);
	// void cancel(void);
	void do_process(void);
private:
	pthread_t pid;
	int retry_times;
};

extern HttpAlarm hAlarm;

#endif
