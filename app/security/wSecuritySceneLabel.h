
#ifndef __W_SECURITY_SCENE_LABEL_H__
#define __W_SECURITY_SCENE_LABEL_H__

#include "mWindow.h"
#include "mButton.h"
#include "mCombox.h"

class wSecuritySceneLabel : public mWindow {
public:
	wSecuritySceneLabel();
	virtual ~wSecuritySceneLabel();

	void doEvent(mEvent *e);
	void loadScene(void);
	void save(void);

private:
	mPixmap m_bkg;
	mText m_delay_text;
	mCombox m_delay;
	mText m_title[9];
	mText m_mode[3];
	mButton m_scene[3][8];
	mButton m_ok;
	mButton m_exit;
	mButton m_up;
	mButton m_down;

	int m_scene_st[3][8];
	int m_offset;
};

#endif
