#ifndef __SYS_SMART_H__
#define __SYS_SMART_H__

#include <string>
#include "types.h"
#include "dxml.h"
#include "sFifo.h"
#include "SmartIF.h"

#define MAX_SMART_ZONE		(4)

typedef struct {
	int direct;
	int sign;
	std::string text;
} smart_elev_t;

class SysSmart {
public:
	SysSmart();
	~SysSmart();

	int start(void);

	void scene(int mode);
	void light(int z, int n, int onoff);
	void curtain(int z, int n, int onoff);
	void air(int z, int n, int onoff);
	void join(int z);
	void sync(int z);
	void rSync(int z);

	void rControl(dxml &p);
	void rData(dxml &p);

	void sControl(void);
	void process(void);

	Smart700 ioctl;

	struct {
		int light[8];
		int curtain[2];
		int mark; //状态同步标志
		time_t ts;
	} m_zone[MAX_SMART_ZONE];

	int m_scene;

	sFifo m_fifo;
};

extern SysSmart smart;

extern smart_elev_t sys_elev[3];

#endif
