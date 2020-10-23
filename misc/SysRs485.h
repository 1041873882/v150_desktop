#ifndef __SYS_RS485_H__
#define __SYS_RS485_H__

#include "sFifo.h"
#include "sGpio.h"
#include "AutoMutex.h"

class SysRs485 {
public:
	SysRs485();
	~SysRs485();

	int start(void);
	void process(void);
	int tx(uint8_t *data, int length);
	int rx(uint8_t *data, int length, int timeout);

public:
	int m_fd;
	pthread_mutex_t m_mutex;
};

extern SysRs485 rs485;

#endif
