
#ifndef __S_MISC_H__
#define	__S_MISC_H__

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "types.h"

/*获取eth0接口 IP地址*/
int local_if_addr(struct sockaddr_in *addr);

int __local_if_addr(const char *dev, struct sockaddr_in *addr);

/*获取eth0接口 MAC地址*/
int local_if_mac(const char *ifr, uint8_t *mac, uint8_t *mask);

/*获取网络连接状态 0:异常 1:正常*/
int ethtool_get_link(void);

void mkdir2(const char *url);

#endif
