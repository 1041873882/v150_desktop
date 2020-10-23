
#ifndef __W_SMART_CURTAIN_LABEL_H__
#define __W_SMART_CURTAIN_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSmartCurtainLabel : public mWindow {
public:
	wSmartCurtainLabel();
	virtual ~wSmartCurtainLabel();

	void doEvent(mEvent *e);
	void doTimer(void);
	void select(int n);

private:
	mPixmap m_bkg;
	mButton m_room[6];
	mText m_index[2];
	mButton m_close[2];
	mButton m_open[2];
	mButton m_pause[2];
	mButton m_exit;
	int m_select;
	int m_st[2];

	int m_offset;
};

#endif
