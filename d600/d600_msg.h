
#ifndef __D600_MSG_H__
#define __D600_MSG_H__

#include "types.h"

#define D600_PORT		8300

struct __d600_msg_t {
	char cid[6];
	uint8_t cmd;
	uint8_t type;
	char host[20];

	union {
		struct {
			uint8_t mac[6];
			uint8_t defence;
			uint8_t sp;
			uint8_t modules;
			uint8_t info[4];
		} __attribute__((packed)) alarm_s;

		struct {
			uint8_t defence;
			uint8_t modules;
		} __attribute__((packed)) alarm_c;

		struct {
			uint16_t type;
			uint32_t seq;
			uint16_t length;
			uint8_t data[200];
		} __attribute__((packed)) msg;

		struct {
			uint8_t mac[6];
			uint8_t defence;
			uint8_t modules;
			uint8_t zone[80];
		} __attribute__((packed)) report;

		struct {
			uint16_t timeout;
			uint8_t date[7];
			int8_t weather[3];
		} __attribute__((packed)) report_ack;

		struct {
			uint8_t type;
		} __attribute__((packed)) passwd;

		struct {
			uint32_t seq;
			uint32_t total;
			uint32_t total_card;
			uint32_t max;
			uint32_t idx;
			struct {
				uint32_t card;
				uint8_t host[20];
			} __attribute__((packed)) data[50];
		} __attribute__((packed)) card_auth;

		struct {
			uint16_t max;
			struct {
				uint32_t card;
				uint8_t date[7];
			} __attribute__((packed)) data[8];
		} __attribute__((packed)) card_report;
	} d;
} __attribute__((packed));

typedef struct __d600_msg_t d600_msg_t;

struct __D600_Query_t {
	char cid[6];
	uint8_t cmd;
	uint8_t type;
	char host[20];
	uint32_t h_addr;
	char to[20];
	uint32_t t_addr;
} __attribute__((packed));

struct __D600_QueryAck_t {
	char cid[6];
	uint8_t cmd;
	uint8_t type;
	char host[20];
	uint32_t h_addr;

	uint8_t ips;
	struct {
		char name[20];
		uint32_t addr;
	} __attribute__((packed)) zone[8];
} __attribute__((packed));

typedef struct __D600_Query_t D600_Query_t;
typedef struct __D600_QueryAck_t D600_QueryAck_t;

#define D600_PKG_LEN  512

#endif
