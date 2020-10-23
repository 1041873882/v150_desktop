#ifndef __SMART_IF_H__
#define __SMART_IF_H__

#include "types.h"

class SmartIF {
public:
	virtual void scene(int mode) {};
	virtual void light(int z, int n, int onoff) {};
	virtual void curtain(int z, int n, int onoff) {};
	virtual void air(int z, int n, int onoff) {};
	virtual void qScene() {};
	virtual void qLight(int z, int n) {};
	virtual void qCurtain(int z, int n) {};
	virtual void qAir(int z, int n) {};
};

class Smart700 : public SmartIF {
public:

	enum {
		SCENE = 0x01,
		LIGHT = 0x02,
		CURTAIN = 0x03,
		AIR = 0x04,
		ZIGBEE_MODE = 0x41,
		ZIGBEE_RESET = 0x42,

		QSCENE = 0x81,
		QLIGHT = 0x82,
		QCURTAIN = 0x83,
		QAIR = 0x84,
	};

	void scene(int mode);
	void light(int z, int n, int onoff);
	void curtain(int z, int n, int onoff);
	void air(int z, int n, int onoff);
	void qScene();
	void qLight(int z, int n);
	void qCurtain(int z, int n);
	void qAir(int z, int n);

	void zigbee_mode(int mode);
	void zigbee_reset(void);

	void tx(uint8_t cmd, uint8_t *data, int length);
	int rx(uint8_t cmd, uint8_t *data, int length);
};

#endif
