#ifndef __D600_H__
#define __D600_H__

#include "d600_msg.h"

class __c600 {
public:
	__c600();
	~__c600();

	void setid(void);
	int start(void);
	void process(void);
	void rx(void);
	int tx(void *data, int length);
	void do_msg(d600_msg_t *msg);
	void do_query(const char *id);
	void do_query_ack(void *data);
	void do_heartbeat(void);
	void do_heartbeat_ack(d600_msg_t *msg);

	void do_alarm(int z);
	void do_cancel(void);

public:
	int m_socket;
	char m_id[32];
	int m_timeout;
	time_t m_ts;
};

extern __c600 c600;

#endif
