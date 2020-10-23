#ifndef __SYS_LOGGER_H__
#define __SYS_LOGGER_H__

#include <string>
#include "sys.h"

#define MAX_LOGGER_SZ	18

class TalkLogger {
public:
	TalkLogger() { m_sz = 0; };

	void load(void);
	void save(void);
	void insert(int type);
	void remove(int n);

	enum {
		IN = 1, //呼入
		OUT = 2, //呼出
		NA = 3, //未接
	};

	struct {
		std::string id;
		int type; // 1:呼入 2:呼出 3:未接
		int done; // 0:未读 1:已读
		int format; //DNAKE格式
		int timeout;
		time_t ts;
	} data[MAX_LOGGER_SZ];

public:
	int m_sz;
	int m_NA; //有未接听记录
};

#define MAX_JPEG_SZ	8

class JpegLogger {
public:
	JpegLogger() { m_sz = 0; };

	void load(void);
	void save(void);
	void insert(time_t ts, const char *url);
	void remove(time_t ts);
	const char *url(time_t ts);

	struct {
		std::string url;
		time_t ts;
	} data[MAX_JPEG_SZ];

public:
	int m_sz;
};

class MsgLogger {
public:
	MsgLogger() { m_sz = 0; m_have = 0; };

	void load(void);
	void save(void);
	void insert(const char *text, time_t ts);
	void remove(int n);
	void done(int n);

	struct {
		std::string text;
		int done; //0:未读 1:已读
		time_t ts;
	} data[MAX_LOGGER_SZ];

public:
	int m_sz;
	int m_have;
};

class SysLogger {
public:
	SysLogger();
	~SysLogger();

	const char *dir(void);
	const char *jpeg_dir(void);
	void load(void);

public:
	TalkLogger talk;
	JpegLogger jpeg;
	MsgLogger msg;
};

extern SysLogger sLogger;

#endif
