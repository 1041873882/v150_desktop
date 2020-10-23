#include <unistd.h>
#include <pthread.h>
#include "sys.h"
#include "HttpAlarm.h"
#include "AdvanceWirelessHttp.h"

static void *dnake_security_thread(void *view)
{
	HttpAlarm * hAlarm = (HttpAlarm *)view;
	hAlarm->do_process();
	return NULL;
}

HttpAlarm::HttpAlarm()
{
	retry_times = 0;
	this->start();
}

HttpAlarm::~HttpAlarm()
{
}

void HttpAlarm::start(void)
{
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_security_thread, this) != 0)
		perror("pthread_create dnake_security_thread!\n");
}

// void HttpAlarm::end(void)
// {
// 	void *ret = NULL;
// 	retry_times = 0;
//     pthread_join(pid, &ret);
//     printf("thread 3 exit code %d\n", (int)ret);
// }

void HttpAlarm::do_process(void)
{
	retry_times = 5;
	time_t send_time = 0;
	while (retry_times) {
		if (time(NULL) - send_time >= (sys.awtek.alarm_freq() * 10)) {
			//awtek_http.send_message(AdvanceWirelessHttp::ALARM, -1, 5);
			awtek_http.send_message(AdvanceWirelessHttp::ALARM, -1, 5);
			printf("send_message OK\n");
			send_time = time(NULL);	
			retry_times--;				
		}
		usleep(40*1000);
	}
}
