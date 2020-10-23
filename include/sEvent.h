
#ifndef __DNAKE_EVENT_H__
#define __DNAKE_EVENT_H__

#include "types.h"

class sEvent {
public:
	sEvent();
	~sEvent();

	int wakeup(void);
	int wait(int timeout);

private:
	int m_event[2];
};

#endif
