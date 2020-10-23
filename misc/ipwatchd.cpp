#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/wait.h>
#include <libnet.h>
#include <pcap.h>
#include <pthread.h>
#include <linux/if_ether.h>

#include "types.h"
#include "sys.h"
#include "sMisc.h"

struct __arp_discover_t {
	uint16_t header;
	uint64_t id;
	uint32_t ip;
} __attribute__((packed));

typedef struct __arp_discover_t arp_discover_t;

typedef struct {
	const char *sip;
	const char *smac;
	const char *dip;
	const char *dmac;
	int op;
	uint8_t *payload;
	uint32_t payload_s;
} arp_args_t;

static int ipwd_error = 0;
static time_t ipwd_ts = 0;

//! Number of useless bytes in ARP packet
/*! Parsing of packets captured by libpcap on pseudo device "any" seems to be
 * slightly different than parsing of packets captured on one particular interface.
 * Instead of usual 14th byte addresses begin on 24th byte of ARP header.
 */
#define IPWD_ARP_HEADER_SIZE 24

//! Structure useful for parsing of individual addresses from packet
typedef struct
{
	u_int8_t arp_sha[ETH_ALEN];		/**< Source MAC address */
	u_int8_t arp_spa[4];			/**< Source IP address */
	u_int8_t arp_tha[ETH_ALEN];		/**< Destination MAC address */
	u_int8_t arp_tpa[4];			/**< Destination IP address */
} IPWD_S_ARP_HEADER;

pcap_t *h_pcap = NULL;

extern "C" char *ether_ntoa_r (const struct ether_addr *addr, char * buf);
extern "C" char * ether_ntoa (const struct ether_addr *addr);
extern "C" struct ether_addr *ether_aton_r (const char *asc, struct ether_addr * addr);
extern "C" struct ether_addr *ether_aton (const char *asc);

//! Generates ARP packet with libnet1
/*!
 * \param dev Name of the device
 * \param p_sip Source IP address
 * \param p_smac Source MAC address
 * \param p_dip Destination IP address
 * \param p_dmac Destination MAC address
 * \param opcode ARPOP_REQUEST or ARPOP_REPLY
 * \return IPWD_RV_SUCCESS if successful IPWD_RV_ERROR otherwise 
 */
int ipwd_genarp(char *dev, arp_args_t *args)
{

	struct in_addr sip, dip;
	struct ether_addr smac, dmac;

	/* Convert source IP address */
	if (inet_aton(args->sip, &sip) == 0) {
		fprintf (stderr, "Unable to convert source IP address %s\n", args->sip);
		return -1;
	}

	/* Convert destination IP address */
	if (inet_aton(args->dip, &dip) == 0)
	{
		fprintf (stderr, "Unable to convert destination IP address %s\n", args->dip);
		return -1;
	}

	/* Convert source MAC address */
	if (ether_aton_r(args->smac, &smac) == NULL)
	{
		fprintf (stderr, "Unable to convert source MAC address %s\n", args->smac);
		return -1;
	}

	/* Convert destination MAC address */
	if (ether_aton_r(args->dmac, &dmac) == NULL)
	{
		fprintf (stderr, "Unable to convert destination MAC address %s\n", args->dmac);
		return -1;
	}

	/* Set opcode once again. Just in case.. */
	if (args->op != ARPOP_REQUEST)
	{
		args->op = ARPOP_REPLY;
	}

	libnet_t *h_net = NULL;
	char errbuf[LIBNET_ERRBUF_SIZE];

	/* Initialize libnet */
	h_net = libnet_init(LIBNET_LINK_ADV, dev, errbuf);
	if (h_net == NULL)
	{
		fprintf (stderr, "Unable to initialize libnet1 - %s\n", errbuf);
		return -1;
	}

	/* Gratuitous ARP request will be created if destination MAC address is broadcast.
	 * GARP requests from Windows and OpenBSD have destination MAC in ARP header
	 * always set to 00:00:00:00:00:00 so we will do the same thing */
	if ((strcmp(args->dmac, "ff:ff:ff:ff:ff:ff") == 0) && (args->op == ARPOP_REQUEST))
	{
		struct ether_addr nullmac;
		const char *null_mac = "00:00:00:00:00:00";

		/* Convert null MAC address */
		if (ether_aton_r (null_mac, &nullmac) == NULL)
		{
			fprintf (stderr, "Unable to convert destination MAC address for gratuitous ARP request\n");
			libnet_destroy (h_net);
			return -1;
		}

		/* Build ARP header for gratuitous ARP packet */
		libnet_ptag_t arp = 0;
		arp = libnet_build_arp (ARPHRD_ETHER,
					ETHERTYPE_IP,
					6,
					4,
					args->op,
					(u_int8_t *) & smac,
					(u_int8_t *) & sip,
					(u_int8_t *) & nullmac,
					(u_int8_t *) & dip,
					args->payload,
					args->payload_s,
					h_net,
					arp );
		if (arp == -1) {
			fprintf (stderr, "Unable to build ARP header: %s\n", libnet_geterror (h_net));
			libnet_destroy (h_net);
			return -1;
		}

	} else {

		/* Build ARP header for normal ARP packet */
		libnet_ptag_t arp = 0;
		arp = libnet_build_arp (ARPHRD_ETHER,
					ETHERTYPE_IP,
					6,
					4,
					args->op,
					(u_int8_t *) & smac,
					(u_int8_t *) & sip,
					(u_int8_t *) & dmac,
					(u_int8_t *) & dip,
					args->payload,
					args->payload_s,
					h_net,
					arp);
		if (arp == -1) {
			fprintf (stderr, "Unable to build ARP header: %s\n", libnet_geterror (h_net));
			libnet_destroy (h_net);
			return -1;
		}

	}

	/* Build ethernet header */
	libnet_ptag_t ether = 0;
	ether = libnet_build_ethernet((u_int8_t *) & dmac, (u_int8_t *) & smac, ETHERTYPE_ARP, NULL, 0, h_net, ether);
	if (ether == -1)
	{
		fprintf (stderr, "Unable to build ethernet header: %s\n", libnet_geterror (h_net));
		libnet_destroy(h_net);
		return -1;
	}

	/* Send packet */
	int c = libnet_write(h_net);
	if (c == -1)
	{
		fprintf(stderr, "Unable to send packet: %s\n",	libnet_geterror (h_net));
		libnet_destroy(h_net);
		return -1;
	} else {
//		fprintf (stderr, "Packet with size of %d bytes sent\n", c);
	}

	libnet_destroy (h_net);

	return 0;
}

//! Gets the IP and MAC addresses of specified device in human readable form
/*!
 * Based on examples from: http://english.geekpage.jp/programming/linux-network/
 * \param p_dev Name of the device (i.e. eth0)
 * \param p_ip Pointer to string where the IP address should be stored
 * \param p_mac Pointer to string where the MAC address should be stored
 * \return IPWD_RV_SUCCESS if successful IPWD_RV_ERROR otherwise
 */
int __ipwd_devinfo(const char *p_dev, char *p_ip, char *p_mac)
{
	/* Create UDP socket */
	int sock = -1;
	char *p_dev_mac = NULL;
	char *p_dev_ip = NULL;
	struct ifreq ifr;

	sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		fprintf (stderr, "Could not open socket\n");
		return -1;
	}

	ifr.ifr_addr.sa_family = AF_INET;
	strcpy(ifr.ifr_name, p_dev);

	/* Get IP address of interface */
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
//		fprintf (stderr, "Could not retrieve IP address of the device \"%s\"\n", p_dev);
		goto err;
	}

	if (p_ip) {
		if ((p_dev_ip = inet_ntoa (((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr)) == NULL) {
			fprintf (stderr, "Could not convert IP address of the device \"%s\"\n", p_dev);
			goto err;
		}

		strcpy(p_ip, p_dev_ip);
	}

	if (p_mac) {
		/* Get MAC address of interface */
		if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
			fprintf (stderr, "Could not retrieve IP address of the device \"%s\"\n", p_dev);
			goto err;
		}

		if ((p_dev_mac = ether_ntoa ((const struct ether_addr *) &ifr.ifr_hwaddr.sa_data[0])) == NULL) {
			fprintf (stderr, "Could not convert IP address of the device \"%s\"\n", p_dev);
			goto err;
		}

		strcpy(p_mac, p_dev_mac);
	}

	/* Close socket */
	close(sock);

//	fprintf(stderr, "Device info: %s %s-%s\n", p_dev, p_ip, p_mac);

	return 0;

err:
	close(sock);
	return -1;
}

int ipwd_devinfo(const char *p_dev, char *p_ip, char *p_mac)
{
	int ret = __ipwd_devinfo(p_dev, p_ip, p_mac);
	if (ret)
		ret = __ipwd_devinfo("wlan0", p_ip, p_mac);
	return ret;
}

void ipwatchd_garp(void)
{
	char ip[64], mac[24];
	arp_args_t args;

	/* Get actual IP and MAC address of interface */
	if (__ipwd_devinfo("eth0", ip, mac)) {
		//fprintf (stderr, "Unable to get IP and MAC address of eth0\n");
		return;
	}

	args.sip = ip;
	args.smac = mac;
	args.dip = ip;
	args.dmac = "ff:ff:ff:ff:ff:ff";
	args.op = ARPOP_REQUEST;
	args.payload = NULL;
	args.payload_s = 0;

	ipwd_genarp((char *)"eth0", &args);
}

void ipwd_arp_discover(const char *id)
{
	char ip[64], mac[24], dip[64];
	arp_args_t args;
	arp_discover_t d;

	/* Get actual IP and MAC address of interface */
	__ipwd_devinfo("eth0", ip, mac);

	d.header = 0x51 | (0x43<<8);
	d.id = atoll(id);
	d.ip = inet_addr(ip);

	args.sip = ip;
	args.smac = mac;

	uint8_t __mac[6], __mask[4];
	local_if_mac("eth0", __mac, __mask);

	uint8_t *p = (uint8_t *)&d.ip;
	if (sys.talk.arp() && __mask[2] == 0 && __mask[3] == 0) {
		uint8_t data;
		char s[16];

		memcpy(s, id+strlen(id)-4, 2);
		s[2] = 0;
		data = (atoi(s)%0x0F)+1;
		data = (data<<4) | (p[2] & 0x0F);

		sprintf(dip, "%d.%d.%d.%d", p[0], p[1], data, p[3]);
	} else
		sprintf(dip, "%d.%d.%d.%d", p[0], p[1], p[2], 254);

	args.dip = dip;
	args.dmac = "ff:ff:ff:ff:ff:ff";
	args.op = ARPOP_REQUEST;
	args.payload = (uint8_t *)&d;
	args.payload_s = sizeof(d);

	ipwd_genarp((char *)"eth0", &args);
}

static void ipwd_arp_query(const char *ip, const char *id)
{
	dmsg req;
	dxml p;
	p.setText("/params/ip", ip);
	p.setText("/params/id", id);
	req.request("/talk/arp/query", p.data());
}

static void ipwd_arp_query2(const char *ip, int data)
{
	dmsg req;
	dxml p;
	p.setText("/params/ip", ip);
	p.setInt("/params/data", data);
	req.request("/talk/arp/query2", p.data());
}

void ipwd_process(void)
{
	if (sys_ipwd_err) {
		if (labs(time(NULL)-ipwd_ts) >= 10) {
			ipwd_ts = time(NULL);
			ipwd_error++;
			if (ipwd_error >= 3) {
				sys_ipwd_result(0, "", "");
				ipwd_error = 0;
			}
			ipwatchd_garp();
		}
	}
}

static int sys_ipwd_wait = 0;

void ipwd_force_wait(int val)
{
	sys_ipwd_wait = val;
}

//! Callback for "pcap_loop" with standard parameters. Called when ARP packet is received (detection of conflict is done here).
/*! 
 * \param args Last parameter of pcap_loop
 * \param header Packet header
 * \param packet Packet data
 */
void ipwd_analyse(u_char * args, const struct pcap_pkthdr *header, const u_char * packet)
{
	if (sys_ipwd_wait)
		sleep(2);

	/* Get addresses from packet */
	IPWD_S_ARP_HEADER *arpaddr;
	arpaddr = (IPWD_S_ARP_HEADER *) (packet + IPWD_ARP_HEADER_SIZE);
	unsigned int addr;

	/* Source IP address */
	char rcv_sip[16];
	char *p_rcv_sip = NULL;

	memcpy(&addr, arpaddr->arp_spa, 4);
	if ((p_rcv_sip = inet_ntoa(*((struct in_addr *) &addr))) == NULL) {
		fprintf(stderr, "Could not get source IP address from packet\n");
		return;
	}
	strcpy(rcv_sip, p_rcv_sip);

	/* Source MAC address */
	char rcv_smac[18];
	char *p_rcv_smac = NULL;

	if ((p_rcv_smac = ether_ntoa((const struct ether_addr *) &arpaddr->arp_sha)) == NULL) {
		fprintf(stderr, "Could not get source MAC address from packet\n");
		return;
	}
	strcpy(rcv_smac, p_rcv_smac);

	/* Destination IP address */
	char rcv_dip[16];
	char *p_rcv_dip = NULL;

	memcpy(&addr, arpaddr->arp_tpa, 4);
	if ((p_rcv_dip = inet_ntoa(*((struct in_addr *) &addr))) == NULL) {
		fprintf(stderr, "Could not get destination IP address from packet\n");
		return;
	}
	strcpy(rcv_dip, p_rcv_dip);

	/* Destination MAC address */
	char rcv_dmac[18];
	char *p_rcv_dmac = NULL;

	if ((p_rcv_dmac = ether_ntoa((const struct ether_addr *) &arpaddr->arp_tha)) == NULL) {
		fprintf(stderr, "Could not get destination MAC address from packet\n");
		return;
	}

	strcpy(rcv_dmac, p_rcv_dmac);

	char ip[64] = {0}, mac[24] = {0};
	char ip2[64] = {0}, mac2[24] = {0};
	char wip[64] = {0}, wmac[24] = {0};
	/* Get actual IP and MAC address of interface */
	__ipwd_devinfo("eth0", ip, mac);
	__ipwd_devinfo("eth1", ip2, mac2);
	__ipwd_devinfo("wlan0", wip, wmac);

	if (!strcmp(rcv_smac, mac2) || !strcmp(rcv_smac, wmac))
		return;

	//printf("recv: %s %s\n", rcv_sip, rcv_smac);

	/* Check if received packet causes conflict with IP address of this interface */
	if (!strcmp(rcv_sip, ip) && strcmp(rcv_smac, mac) && strcmp(rcv_smac, mac2) && strcmp(rcv_smac, wmac)) {
		//IP³åÍ» ETH
		LOGE("MAC address %s causes IP conflict with address %s set on interface %s - passive mode - reply not sent\n", rcv_smac, ip, "eth0");
		sys_ipwd_result(1, ip, rcv_smac);
		ipwd_ts = time(NULL);
		ipwd_error = 0;
	} else if (!strcmp(rcv_smac, mac) && strcmp(rcv_sip, ip) && strcmp(rcv_sip, ip2) && strcmp(rcv_sip, wip)) {
		LOGE("MAC Conflict: S:%s-%s D:%s-%s\n", rcv_sip, rcv_smac, rcv_dip, rcv_dmac);
		//MAC³åÍ»
		sys.resetMac();
		sys.save();
		sys.setEthMac();
		ipwatchd_garp();
	} else {
		arp_discover_t d;
		char id[64], s[64];

		memcpy(&d, packet+44, sizeof(d));
		if (d.header == (0x51 | (0x43<<8))) {
			uint8_t *p = (uint8_t *)&d.ip;

			sprintf(id, "%lld", d.id);
			sprintf(s, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

			ipwd_arp_query(s, id);
			LOGI("arp query for: %s %s %s\n", id, ip, rcv_dip);
		} else if (sys.talk.arp()) {
			uint8_t __mac[6], __mask[4];
			if (!local_if_mac("eth0", __mac, __mask) && __mask[2] == 0 && __mask[3] == 0) {
				uint8_t *p = (uint8_t *)&addr;
				uint8_t data = p[2] >> 4;
				if (data) {
					sprintf(s, "%d.%d.%d.%d", p[0], p[1], p[2]&0x0F, p[3]);
					ipwd_arp_query2(s, data);
				}
			}
		}
	}
}

static void *sys_ipwd_thread(void *)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;

	sleep(10);

	h_pcap = pcap_open_live(NULL, BUFSIZ, 0, 0, errbuf);
	if (h_pcap == NULL) {
		LOGE("Unable to create packet capture object - %s\n", errbuf);
		return NULL;
	}

	/* Compile packet capture filter - only ARP packets will be captured */
	if (pcap_compile(h_pcap, &fp, (char *)"arp", 0, 0) == -1) {
		LOGE("Unable to compile packet capture filter - %s\n", pcap_geterr (h_pcap));
		return NULL;
	}

	/* Set packet capture filter */
	if (pcap_setfilter(h_pcap, &fp) == -1) {
		LOGE("Unable to set packet capture filter - %s\n", pcap_geterr (h_pcap));
		return NULL;
	}
	pcap_freecode(&fp);

	ipwatchd_garp();

	/* Loop until SIGTERM or any error destroys pcap object */
	pcap_loop(h_pcap, -1, ipwd_analyse, NULL);

	return h_pcap;
}

int ipwd_start(void)
{
	pthread_t pid;
	if (pthread_create(&pid, NULL, sys_ipwd_thread, NULL) != 0)
		perror("ipwatchd_start pthread_create\n");
	return 0;
}
