
#ifndef __SGPIO_H__
#define __SGPIO_H__

#include "types.h"

/*
* IO需要在sys_config.fex文件中进行定义如：
* romfs\lichee\tools\pack\chips\sun8iw8p1\configs\800x480\sys_config.fex
*
* GPIO控制调用顺序 start->dir->set/get
* 具体参考应用代码
*/

class sGpio {
public:
	sGpio();
	~sGpio();

	int start(const char *main, const char *sub);
	int stop(const char *main, const char *sub);

	// val: 0->输入 1->输出
	int dir(const char *main, const char *sub, int val);
	// val: 0->低电平
	int set(const char *main, const char *sub, int val);
	//返回值 0->低电平 1->高电平
	int get(const char *main, const char *sub);
	//ldo电源控制 val: 0->禁用 其他->电源电压
	int ldo(const char *name, int val);
};

//标准I2C接口
class i2c_dev {
public:
	i2c_dev();
	~i2c_dev();

	int open(const char *dev);
	void close(void);
	void addr(uint8_t addr) { m_addr = addr; };

	int transfer(uint8_t flags, uint8_t *data, uint8_t length);
	uint8_t read8(uint8_t offset);
	int read8(uint8_t offset, uint8_t *data, int length);
	int write8(uint8_t offset, uint8_t data);
	int write8(uint8_t offset, uint8_t *data, int length);
	int read16(uint16_t offset, uint8_t *data, size_t length);
	int write16(uint16_t offset, uint8_t *data, size_t length);

private:
	int m_i2c;
	uint8_t m_addr;
};

//模拟I2C接口
class i2c_dev2 {
public:
	i2c_dev2();
	~i2c_dev2();

	void start_bit(void);
	void stop_bit(void);
	void send_byte(uint8_t d);
	uint8_t read_byte(void);
	int read_ack(void);
	void send_ack(int ack);

	void set_sda(const char *key);
	void set_scl(const char *key);
	void sda(int val);
	void scl(int val);

	int open(const char *dev);
	void close(void);
	void addr(uint8_t addr) { m_addr = addr; };

	uint8_t read8(uint8_t offset);
	int read8(uint8_t offset, uint8_t *data, int length);
	int write8(uint8_t offset, uint8_t data);
	int write8(uint8_t offset, uint8_t *data, int length);

private:
	sGpio m_gpio;
	uint8_t m_addr;

	char *m_sda;
	char *m_scl;
};

#endif
