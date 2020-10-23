#ifndef __SYS_CALLER_H__
#define __SYS_CALLER_H__

#include <stdlib.h>
#include <string>

#include "dxml.h"
#include "dmsg.h"
#include "AutoMutex.h"
#include "mEvent.h"
#include "mWindow.h"

#define MAX_HOST2ID_SZ		16
#define MAX_SLAVE_SZ		10

typedef struct {
	char host[64];
	int build;
	int unit;
	int floor;
	int family;
} host2id_t;

class SysCaller {
public:
	SysCaller();
	~SysCaller();

	enum Type {
		NONE,
		QUERY,
		RINGING,
		CALLING,
		TALKING,
	};

	void query(const char *id);
	void start(const char *url);
	void calling(const char *id);
	void ringing(const char *id);
	void play(const char *id);
	void answer(void);
	void stop(void);
	void unlock(void);
	void mute(int index, int enable);
	int ts(void) { return (__ts(m_tv)/1000); };

	void m700(const char *url);
	void q600(const char *id);
	void s600(const char *id, const char *ip);
	void m600(const char *id, const char *ip);

	void host2id(host2id_t *id);
	host2id_t *host2id(const char *id);

	Type mode(void) { return m_mode; };

	//mode 0:已接听  1:未接听  2:开锁  3:呼叫  4:结束
	void logger(int mode);

	void slaves(dxml &p);
	void ui_process(void);

public:
	struct {
		const char *url;
		char __url[128];
	} sip;

	struct {
		const char *host;
		const char *ip;
		char __host[32];
		char __ip[32];
	} d600;

public:
	char m_id[128];
	Type m_mode;
	int m_proxy;
	int m_result;
	struct timeval m_tv;

	int m_window; // 0:无状态 1:显示对讲界面 2:显示主界面

	int m_mute[2];
	host2id_t *m_h2id[MAX_HOST2ID_SZ];
	int m_h2id_mark;

	std::string m_slaves[MAX_SLAVE_SZ];

	pthread_mutex_t m_mutex;
};

extern SysCaller sCaller;

extern mWindow *SysTalk;

#endif
