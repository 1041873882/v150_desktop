
#ifndef __W_SMART_H__
#define __W_SMART_H__

#include "SysSmart.h"

#include "mWindow.h"
#include "mButton.h"

class wSmart : public mWindow {
public:
	wSmart();
	virtual ~wSmart();

	void doEvent(mEvent *e);

private:
	mPixmap m_bkg;
	mButton m_scene;
	mButton m_light;
	mButton m_air;
	mButton m_curtain;
	mButton m_elevator;
	mButton m_exit;
	mButton m_zigbee[3];

	struct timeval m_tv;
};

#endif
