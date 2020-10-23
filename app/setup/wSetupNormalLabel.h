
#ifndef __W_SETUP_NORMAL_LABEL_H__
#define __W_SETUP_NORMAL_LABEL_H__

#include "mWindow.h"
#include "mCombox.h"
#include "mButton.h"
#include "mSeekBar.h"
#include "mEdit.h"

class wSetupNormalLabel : public mWindow {
public:
	wSetupNormalLabel();
	virtual ~wSetupNormalLabel();

	void doEvent(mEvent *e);
	void save(void);

private:
	mText m_bright_text;
	mSeekBar m_bright;
	mText m_sound_text;
	mSeekBar m_sound;
	mText m_talk_text;
	mSeekBar m_talk;
	mText m_answer_text;
	mButton m_answer;
	mText m_lang_text;
	mCombox m_lang;
	mText m_tz_text;
	mEdit m_tz;

	int m_answer_st;

	mButton m_ok;
};

#endif
