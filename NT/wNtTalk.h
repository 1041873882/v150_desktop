
#ifndef __W_NT_TALK_H__
#define __W_NT_TALK_H__

#include "wNtWindow.h"

class wNtTalk : public wNtWindow {
public:
	wNtTalk();
	virtual ~wNtTalk();

	void doEvent(mEvent *e);
	void doTimer(void);

	void host2id(const char *sid, char *s);
	void jpeg(const char *url);

private:
	mText m_host;
	mPixmap m_osd;

	int m_vo_st;
	int m_answer_st; //0:未接听状态 1:呼叫或已接听
	int m_type;

	time_t m_ts;
	time_t m_jpeg;
	time_t m_busy_ts;
};

#endif
