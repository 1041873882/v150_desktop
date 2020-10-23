
#ifndef __W_NT_SECURITY_SCENE_LABEL_H__
#define __W_NT_SECURITY_SCENE_LABEL_H__

#include "wNtWindow.h"

class wNtSecuritySceneLabel : public wNtWindow {
public:
	wNtSecuritySceneLabel();
	virtual ~wNtSecuritySceneLabel();

	void doEvent(mEvent *e);

	void showSelect(void);

private:
	mButton m_title;
	mPixmap m_bkg2;
	mText m_title2[8];
	mText m_scene[3];
	mText m_zone[3][8];

	int m_scene_s;
	int m_zone_s;
	int m_ok;
};

#endif
