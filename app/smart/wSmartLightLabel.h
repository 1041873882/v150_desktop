
#ifndef __W_SMART_LIGHT_LABEL_H__
#define __W_SMART_LIGHT_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSmartLightLabel : public mWindow {
public:
	wSmartLightLabel();
	virtual ~wSmartLightLabel();

	void doEvent(mEvent *e);
	void doTimer(void);
	void select(int n);

private:
	mPixmap m_bkg;
	mButton m_room[6];
	mButton m_light[8];
	mButton m_exit;
	int m_select;

	int m_xoff, m_yoff;

	int m_st[8];
};

#endif
