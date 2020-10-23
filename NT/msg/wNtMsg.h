
#ifndef __W_NT_MSG_H__
#define __W_NT_MSG_H__

#include "mScrollText.h"
#include "wNtWindow.h"

class wNtMsg : public wNtWindow {
public:
	wNtMsg();
	virtual ~wNtMsg();

	void doEvent(mEvent *e);
	void doTimer(void);

	void showData(int n);
	void showSelect(void);
	void showText(void);

private:
	mButton m_title;
	mButton m_data[8];
	mText m_context[8];
	mText m_date[8];

	mPixmap m_text_bkg;
	mText m_text_date;
	mScrollText m_text_data;

	int m_first;
	int m_max;
	int m_select;
	int m_mode; //0:列表 1:查看
};

#endif
