
#ifndef __W_TALK_H__
#define __W_TALK_H__

#include "mWindow.h"
#include "mButton.h"

class wTalk : public mWindow {
public:
	wTalk();
	virtual ~wTalk();

	void doEvent(mEvent *e);
	void doTimer(void);

	void host2id(const char *sid, char *s);
	void jpeg(const char *url);

	int doElevVisit(const char *host);

private:
	mPixmap m_bkg;
	mButton m_answer;
	mButton m_unlock;
	mButton m_mic;
	mButton m_keyboard;
	mPixmap m_osd;
	mText m_host;

	mButton m_dtmf_text;
	mButton m_dtmf[12];
	std::string m_dtmf_key;

	time_t m_ts;

	int m_vo_st;
	int m_answer_st; //0:Î´½ÓÌý×´Ì¬ 1:ºô½Ð»òÒÑ½ÓÌý
	int m_type;

	time_t m_jpeg;
	time_t m_busy_ts;
};

#endif
