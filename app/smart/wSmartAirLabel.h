
#ifndef __W_SMART_AIR_LABEL_H__
#define __W_SMART_AIR_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSmartAirLabel : public mWindow {
public:
	wSmartAirLabel();
	virtual ~wSmartAirLabel();

	void doEvent(mEvent *e);
	void select(int n);

private:
	mPixmap m_bkg;
	mButton m_room[6];
	mText m_index[2];
	mButton m_cold[2];
	mButton m_wet[2];
	mButton m_wind[2];
	mButton m_off[2];
	mButton m_exit;
	int m_select;
};

#endif
