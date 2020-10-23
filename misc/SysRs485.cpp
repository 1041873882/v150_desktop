
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <strings.h>
#include <sys/time.h>

#include "sys.h"
#include "SysSecurity.h"
#include "SysRs485.h"

SysRs485 rs485;

SysRs485::SysRs485()
{
	m_fd = -1;

	sGpio io;
	io.start("gpio_para", "rs485_de");
	io.dir("gpio_para", "rs485_de", 1);
	io.set("gpio_para", "rs485_de", 0);

	pthread_mutex_init(&m_mutex, NULL);
}

SysRs485::~SysRs485()
{
}

int SysRs485::start(void)
{
	m_fd = ::open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_fd < 0) {
		fprintf(stderr, "SysRs485::start open /dev/ttyS1 err.\n");
		return -1;
	}

	struct termios opt;
	fcntl(m_fd, F_SETFL, 0);
	bzero(&opt, sizeof(opt));
	opt.c_cflag |= CLOCAL | CREAD;
	opt.c_cflag &= ~CSIZE;
	opt.c_cflag |= CS8;
	opt.c_cflag &= ~PARENB;
	opt.c_cflag &= ~CSTOPB;
	opt.c_cc[VTIME] = 0;
	opt.c_cc[VMIN]  = 0;

	cfsetispeed(&opt, B9600);
	cfsetospeed(&opt, B9600);

	tcflush(m_fd, TCIFLUSH);
	tcsetattr(m_fd, TCSANOW, &opt);

	return 0;
}

static const uint8_t rs485_ex_io[] = {0x55, 0xAA};

void SysRs485::process(void)
{
	if (sys.flash() != 0) {
		uint8_t d[256];
		int r = this->rx(d, sizeof(d), 0);
		if (r > 0) {
			if (memcmp(d, rs485_ex_io, sizeof(rs485_ex_io)) == 0) {
				sSecurity.rs485_io(d, r);
			}
		}
	}
}

int SysRs485::tx(uint8_t *data, int length)
{
	AutoMutex m(&m_mutex);

	if (m_fd < 0 || data == NULL)
		return -1;

#if 1
	fprintf(stderr, "--> ");
	for (int i=0; i<length; i++)
		fprintf(stderr, "%02X ", data[i]);
	fprintf(stderr, "\n");
#endif

	sGpio de;
	de.set("gpio_para", "rs485_de", 1);

	int r = ::write(m_fd, data, length);
	uint16_t n = 0;
	while (++n) {
		int lsr = 0;
		if (ioctl(m_fd, TIOCSERGETLSR, &lsr) < 0)
			break;
		if (lsr & TIOCSER_TEMT) {
			break;
		}
	}

	de.set("gpio_para", "rs485_de", 0);
	tcflush(m_fd, TCIFLUSH); //Çå¿Õ»º³åÀ¬»øÊý¾Ý

	return r;
}

int SysRs485::rx(uint8_t *data, int length, int timeout)
{
	AutoMutex m(&m_mutex);

	if (m_fd < 0 || data == NULL)
		return -1;

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(m_fd, &rfds);
	tv.tv_sec = timeout/1000;
	tv.tv_usec = timeout*1000;
	select(m_fd+1, &rfds, NULL, NULL, &tv);
	if (FD_ISSET(m_fd, &rfds)) {
		int r = ::read(m_fd, data, length);
		if (r > 0) {
#if 1
			fprintf(stderr, "<-- ");
			for (int i=0; i<r; i++)
				fprintf(stderr, "%02X ", data[i]);
			fprintf(stderr, "\n");
#endif
		}
		return r;
	}
	return -1;
}
