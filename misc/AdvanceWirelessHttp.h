#ifndef __ADVANCEWIRELESS_HTTP_H__
#define __ADVANCEWIRELESS_HTTP_H__

#include <stdio.h>
using namespace std;

class AdvanceWirelessHttp {
public:
	AdvanceWirelessHttp();
	~AdvanceWirelessHttp();

	enum {
		NONE = -1,
		REGISTER,
		ALARM
	};
	int send_message(int type, int io=-1, int sensor=-1);
};

extern AdvanceWirelessHttp awtek_http;

#endif
