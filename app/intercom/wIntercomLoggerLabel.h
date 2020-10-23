
#ifndef __W_INTERCOM_LOGGER_LABEL_H__
#define __W_INTERCOM_LOGGER_LABEL_H__

#include "mWindow.h"
#include "mButton.h"
#include "mText.h"

#define MAX_LOGGER_LINE		6

class wIntercomLoggerLabel : public mWindow {
public:
	wIntercomLoggerLabel();
	virtual ~wIntercomLoggerLabel();

	void showData(int n);
	void showPrompt(int type, const char *id);

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mText m_title[4];
	mButton m_data[MAX_LOGGER_LINE];
	mPixmap m_type[MAX_LOGGER_LINE];
	mText m_index[MAX_LOGGER_LINE];
	mText m_host[MAX_LOGGER_LINE];
	mText m_date[MAX_LOGGER_LINE];
	mText m_timeout[MAX_LOGGER_LINE];
	mPixmap m_jpeg[MAX_LOGGER_LINE];

	mButton m_prompt;

	mButton m_call;
	mButton m_up;
	mButton m_down;
	mButton m_del;
	mButton m_exit;

	mButton m_jpeg_view;

	int m_first;
	int m_max;
	int m_select;
	int m_offset;
	int m_start;
	time_t m_ts;
};

#endif
