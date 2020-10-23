
#ifndef __W_SECURITY_ZONE_LABEL_H__
#define __W_SECURITY_ZONE_LABEL_H__

#include "mWindow.h"
#include "mButton.h"
#include "mCombox.h"

class wSecurityZoneLabel : public mWindow {
public:
	wSecurityZoneLabel();
	virtual ~wSecurityZoneLabel();

	void doEvent(mEvent *e);
	void loadZone(void);
	void saveZone(void);
	void save(void);

private:
	mPixmap m_bkg;
	mText m_title[5];
	mText m_zone[8];
	mCombox m_type[8];
	mCombox m_mode[8];
	mCombox m_delay[8];
	mCombox m_sensor[8];
	mButton m_ok;
	mButton m_exit;
	mButton m_up;
	mButton m_down;
	int m_offset;
};

#endif
