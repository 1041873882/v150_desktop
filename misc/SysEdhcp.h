
#ifndef __SYS_E_DHCP_H__
#define __SYS_E_DHCP_H__

class SysEdhcp {
public:
	SysEdhcp();
	~SysEdhcp();

	int start(void);
	void process(void);
	void discover(void);
	void save(dxml &p);

public:
	int m_socket;
	time_t m_ts;
};

extern SysEdhcp eDhcp;

#endif
