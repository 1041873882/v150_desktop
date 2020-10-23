
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SysRs485.h"
#include "SysSmart.h"
#include "SmartIF.h"

void Smart700::scene(int mode)
{
	uint8_t d[256];
	d[0] = mode;
	this->tx(SCENE, d, 1);
	this->rx(SCENE, d, sizeof(d));
}

void Smart700::light(int z, int n, int onoff)
{
	uint8_t d[256];
	d[0] = z;
	d[1] = n;
	d[2] = onoff;
	this->tx(LIGHT, d, 3);
	this->rx(LIGHT, d, sizeof(d));
}

void Smart700::curtain(int z, int n, int onoff)
{
	uint8_t d[256];
	d[0] = z;
	d[1] = n;
	d[2] = onoff;
	this->tx(CURTAIN, d, 3);
	this->rx(CURTAIN, d, sizeof(d));
}

void Smart700::air(int z, int n, int onoff)
{
	uint8_t d[256];
	d[0] = z;
	d[1] = n;
	switch(onoff) {
	case 0:
		d[2] = 0x01;
		d[3] = 0x03;
		d[4] = 0xFF;
		d[5] = 0xFF;
		break;
	case 1:
		d[2] = 0x01;
		d[3] = 0x03;
		d[4] = 0x04;
		d[5] = 0xFF;
		break;
	case 2:
		d[2] = 0x01;
		d[3] = 0x00;
		d[4] = 0x03;
		d[5] = 0xFF;
		break;
	case 3:
		d[2] = 0x00;
		d[3] = 0xFF;
		d[4] = 0xFF;
		d[5] = 0xFF;
		break;
	}
	this->tx(AIR, d, 6);
	this->rx(AIR, d, sizeof(d));
}

void Smart700::qScene()
{
	static time_t ts = 0;
	if (labs(time(NULL)-ts) < 20)
		return;

	uint8_t d[256];
	this->tx(QSCENE, NULL, 0);
	if (this->rx(QSCENE, d, sizeof(d)) >= 1) {
		if (smart.m_scene != d[0]) {
			smart.m_scene = d[0];
			smart.sync(0);
		}
		ts = 0;
	} else
		ts = time(NULL);
}

void Smart700::qLight(int z, int n)
{
	static time_t ts[MAX_SMART_ZONE][8];
	if (labs(time(NULL)-ts[z][n]) < 20)
		return;

	uint8_t d[256];
	d[0] = z;
	d[1] = n;
	this->tx(QLIGHT, d, 2);
	if (this->rx(QLIGHT, d, sizeof(d)) >= 3) {
		if (smart.m_zone[z].light[n] != d[2]) {
			smart.m_zone[z].light[n] = d[2];
			smart.sync(z);
		}
		ts[z][n] = 0;
	} else
		ts[z][n] = time(NULL);
}

void Smart700::qCurtain(int z, int n)
{
	static time_t ts[MAX_SMART_ZONE][2];
	if (labs(time(NULL)-ts[z][n]) < 20)
		return;

	uint8_t d[256];
	d[0] = z;
	d[1] = n;
	this->tx(QCURTAIN, d, 2);
	if (this->rx(QCURTAIN, d, sizeof(d)) >= 3) {
		if (smart.m_zone[z].curtain[n] != d[2]) {
			smart.m_zone[z].curtain[n] = d[2];
			smart.sync(z);
		}
		ts[z][n] = 0;
	} else
		ts[z][n] = time(NULL);
}

void Smart700::qAir(int z, int n)
{
}

void Smart700::zigbee_mode(int mode)
{
	uint8_t d[256];
	d[0] = mode;
	this->tx(ZIGBEE_MODE, d, 1);
	this->rx(ZIGBEE_MODE, d, sizeof(d));
}

void Smart700::zigbee_reset(void)
{
	uint8_t d[256];
	this->tx(ZIGBEE_RESET, NULL, 0);
	this->rx(ZIGBEE_RESET, d, sizeof(d));
}

void Smart700::tx(uint8_t cmd, uint8_t *data, int length)
{
	uint8_t d[1024];
	int total = length+6;

	d[0] = 0xAE;
	d[1] = 0xD0;
	d[2] = total;
	d[3] = cmd;
	d[4] = 0x01;

	for(int i=0; i<length; i++)
		d[5+i] = data[i];

	int n = total-1;
	d[n] = 0;
	for(int i=0; i<n; i++)
		d[n] += d[i];
	rs485.tx(d, total);
}

int Smart700::rx(uint8_t cmd, uint8_t *data, int length)
{
	uint8_t d[4*1024];
	int idx = 0;
	int n = 6;

	do {
		uint8_t d2[256];
		int ret = rs485.rx(d2, 256, 40);
		if (ret > 0) {
			for(int i=0; i<ret; i++)
				d[idx+i] = d2[i];
			idx += ret;
			if (idx >= 6) {
				if (d[0] != 0xAE || d[1] != 0xD0 || d[3] != cmd || d[4] != 0x02) {
					idx = 0;
					continue;
				}

				int len = d[2];
				if (len <= idx) {
					uint8_t ck = 0;
					int sz = len-1;
					for(int i=0; i<sz; i++)
						ck += d[i];
					if (ck != d[sz]) {
						idx = 0;
						continue; //Ð£Ñé´íÎó
					}
					sz = len - 6;
					if (length < sz)
						sz = length;
					for(int i=0; i<sz; i++) {
						data[i] = d[5+i];
					}
					return sz;
				}
			}
		}
		n--;
	} while (n > 0);
	return -1;
}
