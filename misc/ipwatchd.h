
#ifndef __IPWATCHD_H__
#define __IPWATCHD_H__

void ipwd_arp_discover(const char *id);

void ipwd_process(void);

int ipwd_start(void);

#endif
