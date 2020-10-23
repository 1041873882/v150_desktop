
#ifndef __W_MESSAGE_H__
#define __W_MESSAGE_H__

#include "mWindow.h"
#include "mButton.h"
#include "mText.h"
#include "mScrollText.h"

class wMessage : public mWindow {
public:
	wMessage();
	virtual ~wMessage();

	void doEvent(mEvent *e);
	void doTimer(void);

	void showData(int n);
	void showMsg(int n);
	void showColor(int n);

private:
	mPixmap m_bkg;
	mText m_title[3];
	mButton m_data[6];
	mText m_index[6];
	mText m_context[6];
	mText m_date[6];
	mButton m_up;
	mButton m_down;
	mButton m_del;

	mPixmap m_view;
	mText m_view_ts;
	mScrollText m_view_msg;

	mButton m_exit;

	int m_first;
	int m_max;
	int m_select;

	int m_offset;
};

#endif
