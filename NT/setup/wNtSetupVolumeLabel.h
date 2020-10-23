
#ifndef __W_NT_SETUP_VOLUME_H__
#define __W_NT_SETUP_VOLUME_H__

#include "mSeekBar.h"
#include "wNtWindow.h"

class wNtSetupVolumeLabel : public wNtWindow {
public:
	wNtSetupVolumeLabel();
	virtual ~wNtSetupVolumeLabel();

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg2;
	mButton m_title;
	mButton m_music_text;
	mSeekBar m_music;
};

#endif
