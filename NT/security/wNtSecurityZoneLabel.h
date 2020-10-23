
#ifndef __W_NT_SECURITY_ZONE_LABEL_H__
#define __W_NT_SECURITY_ZONE_LABEL_H__

#include "wNtWindow.h"

class wNtSecurityZoneLabel : public wNtWindow {
public:
	wNtSecurityZoneLabel();
	virtual ~wNtSecurityZoneLabel();

	void doEvent(mEvent *e);

	void doUp(void);
	void doDown(void);
	void showSelect(void);

private:
	mButton m_title;
	mPixmap m_bkg2;
	mText m_title2[5];
	mText m_data[8][5];
	int m_zone_s;
	int m_idx_s;
	int m_ok;
};

#endif
