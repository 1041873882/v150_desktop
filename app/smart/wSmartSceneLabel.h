
#ifndef __W_SMART_SCENE_LABEL_H__
#define __W_SMART_SCENE_LABEL_H__

#include "mWindow.h"
#include "mButton.h"

class wSmartSceneLabel : public mWindow {
public:
	wSmartSceneLabel();
	virtual ~wSmartSceneLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mButton m_home;
	mButton m_out;
	mButton m_dinner;
	mButton m_party;
	mButton m_sleep;
	mButton m_exit;

	int m_scene;
};

#endif
